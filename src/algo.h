#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <string>
#include <mutex>
#include <map>

using namespace std;

namespace algo {
  class IAlgo{
  public:
    virtual ~IAlgo() {};
    virtual void run() = 0;
  };

  extern map<string, IAlgo*> algos;
  void add(string name, IAlgo* func);
  void init();
  void deinit();
  void run(string name);
}

#endif