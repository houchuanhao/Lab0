#include"Singleton.h"
#include <array>
Singleton * Singleton::local_instance = NULL;
Singleton::Singleton(){
    optFIle.open(optfilePathIn);
    char line[100];
    int sum=0;
    assert(optFIle.is_open());
    if(optFIle.is_open())
    {
        while(!optFIle.eof())
        {
            optFIle.getline(line,1024);
            futureList.push_back(line);
            // 按行读入，如果读单词用 in>>value;
        }
    }
    
    futureList.pop_back();
    
    /*
    itor=futureList.begin();
    list<String>::iterator itor;
    while (itor!=futureList.end())
    {
        getAddr(*itor);
        cout<<*itor++<<endl;
    }
    */
    optFIle.close();
    printf("read OK sum: %d \n",futureList.size());
}
IntPtr Singleton::getAddr(String str){
    IntPtr inp;
    int pos=str.find(' ')+1;  
    str=str.substr(pos);
    char *ptr;
    inp = strtoul(str.c_str(), &ptr, 10);
    return inp;
}


Singleton *Singleton::getInstance()
{
    if (local_instance == NULL)
    {
        local_instance = new Singleton();
    }
    return local_instance;
}