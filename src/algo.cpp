#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <thread>
#include <chrono>

#include "algo.h"

using namespace std;

extern vector<int> target;
extern int delay;
extern bool running;

namespace algo {
  class BubbleSort : public IAlgo {
  public:
    void run(){
      bool swapped = true;
      while(swapped){
        swapped = false;
        for(size_t i = 1; i < target.size(); i++){
          if(target[i-1] > target[i]){
            swap(target[i-1], target[i]);
            swapped = true;
          }

          this_thread::sleep_for(chrono::microseconds(delay));
          if(!running) return;
        }
      }
    }
  };

  // Initialization stuff
  map<string, IAlgo*> algos;
  void reg(string name, IAlgo* func){
    algos[name] = func;
  }
  void init(){
    reg("Bubble Sort", new BubbleSort());
  }
  void deinit(){
    for(pair<string, IAlgo*> a : algos)
      delete a.second;
  }
  void run(string name){
    printf("Running %s with a delay of %dµs\n", name.c_str(), delay);
    algos[name]->run();
    printf("Done\n");
  }
}