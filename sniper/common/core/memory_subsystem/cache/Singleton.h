#ifndef SINGLETON_H
#define SINGLETON_H
#include <fstream>
#include<list>
#include<vector>
#include "fixed_types.h"
#include "cache.h"
#include <string>
using namespace std;
class AccessAdd
{
public:
};
class Singleton
{
    static IntPtr getAddr(String);
private:
    ifstream optFIle;
    list<String> futureList;
    static Singleton *local_instance;
    Singleton();

public:
    static Singleton *getInstance();
   
};
#endif /* CACHE_SET_LRU_H */
