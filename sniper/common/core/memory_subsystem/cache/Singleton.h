#ifndef SINGLETON_H
#define SINGLETON_H
#include <fstream>
#include<list>
#include<vector>
#include "fixed_types.h"
#include "cache.h"
#include <string>
#include <array>
#include <map>
using namespace std;
class AccessAdd
{
public:
};
class Singleton
{
    
private:
    list<String>::iterator itor;
    ifstream optFIle;
    fstream optOutpt;
    list<String> futureList;
    static Singleton *local_instance;
    Singleton();
    int now=1;
public:
    int L1hit=0;
    int L1miss=0;
    int L2hit=0;
    int L2miss=0;
    map<String,Cache*> cacheMap;
    void cleanOptOut();
    void optOutWrite(String str);
    static Singleton *getInstance();
    static String getName(String str);
    static IntPtr getAddr(String);
    void next();
    String getValue();
    list<String>::iterator getItor();
    list<String>::iterator getEnd();
    int getPos();
    void showHit();
};
#endif /* CACHE_SET_LRU_H */
