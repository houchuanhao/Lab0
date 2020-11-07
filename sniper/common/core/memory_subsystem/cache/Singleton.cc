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
    itor=futureList.begin();
    futureList.pop_back();
    /*
    list<String>::iterator itor1;
    itor1=futureList.begin();
    while (itor1!=futureList.end())
    {
        getAddr(*itor1);
        cout<<"   name:"<<getName(*itor1)<<endl;
        cout<<*itor1++<<endl;
    }
*/
    optFIle.close();
    printf("read Infile OK sum: %ld \n",futureList.size());
}
IntPtr Singleton::getAddr(String str){
    IntPtr inp;
    int pos=str.find(' ')+1;  
    str=str.substr(pos);
    char *ptr;
    inp = strtoul(str.c_str(), &ptr, 10);
    return inp;
}
String Singleton::getName(String str){
    IntPtr inp;
    int pos=str.find(' ');  
    String substr=str.substr(0,pos);
    return substr;
}

Singleton *Singleton::getInstance()
{
    if (local_instance == NULL)
    {
        local_instance = new Singleton();
    }
    return local_instance;
}
void Singleton::cleanOptOut(){
    optOutpt.open(optfilePathOut,ios::out);
    //optOutpt<<"0 0"<<endl;
    printf("clean optout\n");
    optOutpt.close();
}
void Singleton::optOutWrite(String str){

}