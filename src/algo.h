#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <atomic>
#include <iostream>

using namespace std;

namespace algo {
  class InterruptedException : virtual public std::exception {};
  class IAlgo {
  public:
    virtual ~IAlgo() {};
    virtual void run() = 0;
  };

  template <typename T>
  struct TraceableAtom {
    atomic<T> _a;
    
    operator T() {
      return _a.load();
    }

    TraceableAtom() {
    }

    TraceableAtom(const atomic<T>& a) {
      _a.store(a.load());      
    }

    TraceableAtom(T& a) {
      _a.store(a);      
    }

    TraceableAtom(const TraceableAtom& other) {
      _a.store(other._a);
    }

    TraceableAtom& operator=(const TraceableAtom& other){
      _a.store(other._a.load());
      return *this;
    }

    TraceableAtom& operator=(T& other){
      _a.store(other._a.load());
      return *this;
    }
  };

  extern map<string, IAlgo*> algos;
  void add(string name, IAlgo* func);
  void init();
  void deinit();
  void run(string name);
  template <typename T>
  void swap(T &a, T &b);
}

#endif

