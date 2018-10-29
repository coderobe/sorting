#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <thread>
#include <random>
#include <mutex>
#include <chrono>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include "algo.h"

#if defined(__APPLE__)
    #include <objc/message.h>       // Required for: objc_msgsend(), sel_registerName()
    #define GLFW_EXPOSE_NATIVE_NSGL
    #include <GLFW/glfw3native.h>   // Required for: glfwGetNSGLContext()
#endif

using namespace std;

static GLFWwindow *win;
int width = 0, height = 0;
struct nk_context *ctx;

auto rng = default_random_engine {};
vector<thread*> threads;
char* windowname;
int elements = 200;
int read_delay = 100;
int write_delay = 500;
string last_action = "nothing";
string last_time = "0";
atomic<size_t> read_count = 0;
atomic<size_t> write_count = 0;
vector<algo::TraceableAtom<int>> target;
bool running = false;
vector<const char*> algo_vec;
int algo_current = 0;
mutex vector_busy_mutex;

nk_color color_red = nk_rgba(255, 0, 0, 128);
nk_color color_green = nk_rgba(0, 255, 0, 128);
nk_color color_blue = nk_rgba(0, 0, 255, 128);
nk_color color_default = color_green;

void fill_targets(){
  try{
    // clear
    printf("Clearing vector\n");
    target.erase(target.begin(), target.end());

    // fill
    printf("Seeding next run\n");
    for(int i = 1; i <= elements; i++){
      lock_guard<mutex> lock(vector_busy_mutex);

      target.push_back(i);
      target.back().cb_write.push_back([](algo::TraceableAtom<int>& atom){
        last_action = "write";
        write_count++;
        this_thread::sleep_for(chrono::microseconds(read_delay));
        if(!running) throw algo::InterruptedException();
      });
      target.back().cb_read.push_back([](algo::TraceableAtom<int>& atom){
        last_action = "read";
        read_count++;
        this_thread::sleep_for(chrono::microseconds(write_delay));
        if(!running) throw algo::InterruptedException();
      });

      if(!running) return;
    }

    // shuffle
    printf("Shuffling\n");
    srand(time(nullptr));
    for(int i = elements-1; i > 0; i--){
      int irand = rng() % (i+1);
      algo::swap(target[irand], target[i]);
      if(!running) return;
    }

    printf("Resetting results\n");
    write_count = 0;
    read_count = 0;

    // sort
    printf("Running\n");
    chrono::high_resolution_clock::time_point time_start = chrono::high_resolution_clock::now();
    algo::run(std::string(algo_vec[algo_current]));
    chrono::high_resolution_clock::time_point time_end = chrono::high_resolution_clock::now();
    size_t time_duration = chrono::duration_cast<chrono::microseconds>(time_end - time_start).count();
    printf("Took %ldµs\n", time_duration);
    last_time = to_string(time_duration);
  }catch(algo::InterruptedException& e) {
    printf("Interrupted\n");
  }

  running = false;
}

void render(){
  glfwSetErrorCallback([](int e, const char *d){
    fprintf(stderr, "[GLFW] Error %d: %s\n", e, d);
  });

  if(!glfwInit()){
    fprintf(stderr, "[GLFW] failed to init!\n");
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowname, NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwGetWindowSize(win, &width, &height);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glewExperimental = 1;
  if(glewInit() != GLEW_OK){
    fprintf(stderr, "Failed to setup GLEW\n");
    exit(1);
  }

  ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
  {
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
  }

#if __APPLE__
  // Fix black screen on macOS Mojave, need to call some natives to force the window to update
  glfwPollEvents();
  objc_msgSend(glfwGetNSGLContext(win), sel_registerName("update"));
#endif

  while(!glfwWindowShouldClose(win)){
    glfwPollEvents();
    nk_glfw3_new_frame();

    int width_border = 1;
    int width_settings = 200;
    int width_chart = width-width_settings-width_border*2;

    if(nk_begin(ctx, "Settings", nk_rect(0, 0, width_settings, height), NK_WINDOW_TITLE|NK_WINDOW_BORDER)){
      nk_layout_row_dynamic(ctx, 25, 1);
      if(nk_button_label(ctx, running ? "Cancel" : "Start")){
        running = !running;
        if(running)
          threads.push_back(new thread(fill_targets));
      }

      nk_layout_row_dynamic(ctx, 25, 1);
      algo_current = nk_combo(ctx, &algo_vec[0], algo_vec.size(), algo_current, 25, nk_vec2(200, 200));

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_property_int(ctx, "Elements:", 0, &elements, 4096, 100, 2);

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_property_int(ctx, "Write Delay (µs):", 0, &write_delay, 1000, 100, 1);

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_property_int(ctx, "Read Delay (µs):", 0, &read_delay, 1000, 100, 1);

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, (string("Last action: ") + last_action).c_str(), NK_TEXT_LEFT);

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, (string("Last time: ") + last_time + "µs").c_str(), NK_TEXT_LEFT);

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, (string("Total writes: ") + to_string(write_count)).c_str(), NK_TEXT_LEFT);

      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, (string("Total reads: ") + to_string(read_count)).c_str(), NK_TEXT_LEFT);
    }
    nk_end(ctx);

    if(nk_begin(ctx, "Chart", nk_rect(width_settings+width_border*2, 0, width_chart, height), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_ROM)){
      nk_layout_row_static(ctx, height-55, width_chart-30, 1);
      nk_chart_begin_colored(ctx, NK_CHART_COLUMN, color_green, color_red, target.size(), 0, target.size());
      {
        lock_guard<mutex> lock(vector_busy_mutex);
        for(size_t i = 0; i < target.size(); i++){
          nk_chart_push(ctx, target[i].without_cb());
        }
      }
      nk_chart_end(ctx);
    }
    nk_end(ctx);

    glfwGetWindowSize(win, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    nk_glfw3_render(NK_ANTI_ALIASING_OFF, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glfwSwapBuffers(win);
  }

  nk_glfw3_shutdown();
  glfwTerminate();
}

int main(int argc, char** argv){
  algo::init();

  for(pair<string, algo::IAlgo*> e : algo::algos){
    char* copy = strdup(e.first.c_str());
    printf("Found algo %s\n", copy);
    algo_vec.push_back(copy);
  }

  windowname = argv[0];

  render();

  while(!threads.empty()){
    thread* t = threads.front();
    threads.erase(threads.begin());
    threads.shrink_to_fit();
    if(t->joinable()){
      t->join();
    }
    delete t;
  }

  for(const char* e : algo_vec){
    free(const_cast<char*>(e));
  }

  algo::deinit();

  return 0;
}

