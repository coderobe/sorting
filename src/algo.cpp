#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "algo.h"

using namespace std;

extern vector<algo::TraceableAtom<int>> target;
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
        }
        if(minimum != current){
          swap(target[current], target[minimum]);
        }
      }
    }
  };

  class MonkeySort : public IAlgo {
  private:
    bool isSorted() {
      size_t size = target.size();
      for (size_t i = 0; i < size-1; i++) {
        if (target[i] > target[i+1]) return false;
      }
      return true;
    }
  public:
    void run(){
      size_t size = target.size();
      std::srand(std::time(nullptr));
      while(!isSorted()) {
        int idx1 = rand() % size;
        int idx2 = rand() % size;
        swap(target[idx1], target[idx2]);
      }
    }
  };

  class InsertionSort : public IAlgo {
  public:
    void run(){
      size_t size = target.size();
      for(size_t i = 1; i < size; i++) {
        int val = target[i];
        int j = i;
        while(j > 0 && target[j-1] > val) {
          target[j] = target[j-1];
          j--;
        }
        target[j] = val;
      }
    }
  };
  class HeapSort : public IAlgo {
  private:
    void siftDown(size_t start, size_t end){
      size_t root = start;
      while (2*root+1 <= end){
        size_t child = 2*root+1;
        size_t toswap = root;
        if (target[toswap] < target[child]){
          toswap = child;
        }
        if (child+1 <= end && target[toswap] < target[child+1]){
          toswap = child + 1;
        }
        if (toswap == root){
          return;
        }
        else {
          swap(target[root], target[toswap]);
          root = toswap;
        }
      }
    };
  public:
    void run(){
      size_t size = target.size();
      size_t start = (size-2)/2;
      while (start > 0) {
        siftDown(start, size-1);
        start--;
      }
      size_t end = size - 1;
      while (end > 0){
        swap(target[end], target[0]);
        end--;
        siftDown(0,end);
      }
    }
  };

class CombSort : public IAlgo {
  public:
    void run(){
      size_t gap = target.size();
      float shrink = 1.3;
      bool sorted = false;
      while(!sorted) {
        gap /= shrink;
        if (gap <= 1) {
          gap = 1;
          sorted = true;
        }

        int i = 0;
        while(i+gap < target.size()) {
          if (target[i] > target[i+gap]) {
            swap(target[i], target[i+gap]);
            sorted = false;
          }
          i++;
        }
      }
    }

  };

  class GnomeSort : public IAlgo{
  public:
    void run(){
      size_t i = 0;
      size_t size = target.size();
      while(i < size){
        if (i == 0){
          i++;
        }
        if (target[i] >= target[i-1]){
          i++;
        }
        else{
          swap(target[i], target[i -1]);
          i--;
        }
      }
      return;
    }
  };
  // Utility stuff
  map<string, IAlgo*> algos;
  template <typename T> void swap(T &a, T &b){
    T temp = a;
    a = b;
    b = temp;
    if(!running) throw InterruptedException();
  }
  void reg(string name, IAlgo* func){
    algos[name] = func;
  }
  void init(){
    reg("Bubble Sort", new BubbleSort());
    reg("Cocktail Shaker Sort", new CocktailShakerSort());
    reg("Selection Sort", new SelectionSort());
    reg("Monkey Sort", new MonkeySort());
    reg("Insertion Sort", new InsertionSort());
    reg("Comb Sort", new CombSort());
    reg("Heap Sort", new HeapSort());
    reg("Gnome Sort", new GnomeSort());
  }
  void deinit(){
    for(pair<string, IAlgo*> a : algos)
      delete a.second;
  }
  void run(string name){
    printf("Running %s\n", name.c_str());
    algos[name]->run();
    printf("Done\n");
  }
}
