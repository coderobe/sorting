#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <string>
#include <mutex>
#include <map>
#include <atomic>
#include <iostream>
#include <functional>

namespace algo {
  class InterruptedException : virtual public std::exception {};
  class IAlgo {
  public:
    virtual ~IAlgo() {};
    virtual void run() = 0;
  };

  template <typename T> struct TraceableAtom {
    std::atomic<T> _a;
    std::vector<std::function<void(TraceableAtom&)>> cb_read;
    std::vector<std::function<void(TraceableAtom&)>> cb_write;
    
    operator T() {
      for(std::function<void(TraceableAtom&)>& fun : cb_read) fun(*this);
      return _a.load();
    }

    T without_cb() {
      return _a.load();
    }

    TraceableAtom() {
    }

    TraceableAtom(const std::atomic<T>& a) {
      _a.store(a.load());      
    }

    TraceableAtom(T& a) {
      _a.store(a);      
    }

    TraceableAtom(const TraceableAtom& other) {
      _a.store(other._a);
    }

    TraceableAtom& operator=(const TraceableAtom& other){
      for(std::function<void(TraceableAtom&)>& fun : cb_write) fun(*this);
      _a.store(other._a.load());
      return *this;
    }

    TraceableAtom& operator=(T& other){
      for(std::function<void(TraceableAtom&)>& fun : cb_write) fun(*this);
      _a.store(other._a.load());
      return *this;
    }
  };

  extern std::map<std::string, IAlgo*> algos;
  void add(std::string name, IAlgo* func);
  void init();
  void deinit();
  void run(std::string name);
  template <typename T> void swap(T &a, T &b);
}

#endif

