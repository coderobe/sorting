#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

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

  class CocktailShakerSort : public IAlgo {
  public:
    void run(){
      bool swapped = true;
      while(swapped){
        { // forwards
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

        if(!swapped) return;

        { // backwards
          swapped = false;
          for(size_t i = target.size()-2; i > 0; i--){
            if(target[i] > target[i+1]){
              swap(target[i], target[i+1]);
              swapped = true;
            }

            this_thread::sleep_for(chrono::microseconds(delay));
            if(!running) return;
          }
        }
      }
    }
  };

  class SelectionSort : public IAlgo {
  public:
    void run(){
      size_t size = target.size();
      for(size_t current = 0; current < size; current++){
        size_t minimum = current;
        for(size_t candidate = current+1; candidate < size; candidate++){
          if(target[candidate] < target[minimum]){
            minimum = candidate;
          }

          this_thread::sleep_for(chrono::microseconds(delay));
          if(!running) return;
        }
        if(minimum != current){
          swap(target[current], target[minimum]);
        }
      }
    }
  };

  class MonkeySort : public IAlgo {
  public:
    bool isSorted() {
      size_t size = target.size();
      for (size_t i = 0; i < size-1; i++) {
        if (target[i] > target[i+1]) return false;
      }
      return true;
    }
    void run(){
      size_t size = target.size();
      std::srand(std::time(nullptr));
      while(!isSorted()) {
        int idx1 = rand() % size;
        int idx2 = rand() % size;
        swap(target[idx1], target[idx2]);
        this_thread::sleep_for(chrono::microseconds(delay));
        if(!running) return;
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
    reg("Cocktail Shaker Sort", new CocktailShakerSort());
    reg("Selection Sort", new SelectionSort());
    reg("Monkey Sort", new MonkeySort());
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
