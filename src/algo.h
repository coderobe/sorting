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
    T _a;
    mutable mutex mx;

    operator T() {
      lock_guard<mutex> lock(mx);
      return _a;
    }

    TraceableAtom() : _a() {
    }

    TraceableAtom(const atomic<T>& a) {
      _a = a.load();      
    }

    TraceableAtom(T& a) {
      _a = a;      
    }

    TraceableAtom(const TraceableAtom& other) {
      lock_guard<mutex> lock(other.mx);
      _a = other._a;
    }

    TraceableAtom& operator=(const TraceableAtom& other){
      T value;
      {
        lock_guard<mutex> lock(other.mx);
        value = other._a;
      }
      lock_guard<mutex> lock(mx);
      _a = value;
      return *this;
    }

    TraceableAtom& operator=(T& other){
      lock_guard<mutex> lock(mx);
      _a = other;
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

