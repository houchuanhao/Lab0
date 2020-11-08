#include "cache_set_opt.h"
#include "log.h"
#include "stats.h"
#include "Singleton.h"

// Implements OPT replacement, optionally augmented with Query-Based Selection [Jaleel et al., MICRO'10]
list<IntPtr> CacheSetOPT::initFutureTag(){// 替换前从now开始
   Singleton *single=Singleton::getInstance();
   list<String>::iterator itor=single->getItor();
   int sum=0;
   printf("11111 setindex %d\n",setIndex);
   for(int j=1;j<=50;j++){
      printf(" j:%d  \n",j);
      if(itor==single->getEnd()){
         printf("end\n");
         break;
      }
      sum++;
      int current=single->getPos();
      String addrStr=*itor;
      IntPtr addr=Singleton::getAddr(addrStr);
      IntPtr tag;
      UInt32 set_index;
      cache->splitAddress(addr, tag, set_index);
      printf("got set_index %d \n",set_index);
      if (setIndex==set_index){
         j++;
         for (UInt32 i = 0; i < m_associativity; i++){
            if(m_cache_block_info_array[i]->isValid()){ 
               if(m_cache_block_info_array[i]->getTag()==tag){
                  if(m_opt_bits[i]!=unknown&&m_opt_bits[i]<single->getPos()){
                     m_opt_bits[i]=current+j;
                  }
               }

            }
         }
      }
      itor++;
   }
   list<IntPtr> p;
   return p;
}
void CacheSetOPT::updateFutureTage(){
   Singleton *single=Singleton::getInstance();
   list<String>::iterator itor=single->getItor();
   for(int j=1;j<=ahead&&itor!=single->getEnd();itor++){
      int current=single->getPos();
      String addrStr=*itor;
      IntPtr addr=Singleton::getAddr(addrStr);
      IntPtr tag;
      UInt32 set_index;
      cache->splitAddress(addr, tag, set_index);
      if (setIndex==set_index){
         j++;
         if(j==ahead){
            for (UInt32 i = 0; i < m_associativity; i++){
               if(m_cache_block_info_array[i]->isValid()){ 
                  if(m_cache_block_info_array[i]->getTag()==tag){
                     if(m_opt_bits[i]!=unknown&&m_opt_bits[i]<single->getPos()){
                        m_opt_bits[i]=current+j;
                     }
                  }
               }
            }
         }
      }
      //printf("%d updatej\n",j);
   }
   printf("UPDATE OK \n");
}

CacheSetOPT::CacheSetOPT(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, CacheSetInfoOPT* set_info, UInt8 num_attempts,CacheBase * bcache,int cacheIndex)
   : CacheSet(cache_type, associativity, blocksize)
   , m_num_attempts(num_attempts)
   , m_set_info(set_info)
{
   setIndex=cacheIndex;
   cache=bcache;
   printf("set cacheName %s num_attempts %d \n",bcache->getName().c_str(),num_attempts);
   //printf("opt cache_type: %d associativity %x ,blocksize %x ***************LRU\n",cache_type,associativity,blocksize);
   m_opt_bits = new int[m_associativity];
   printf("m_opt_bits\n");
   for (int i = 0; i < m_associativity; i++)
      m_opt_bits[i] = i;
   printf("begin initTag \n");
   initFutureTag();
   printf("cacheSet aaaaaaaaaaaa\n");
}

CacheSetOPT::~CacheSetOPT()
{
   delete [] m_opt_bits;
}
UInt32
CacheSetOPT::getReplacementIndex(CacheCntlr *cntlr)
{  

   // 更新最后一个
   int maxpos=0;
   int maxvalue=0;
   updateFutureTage();
   for (UInt32 i = 0; i < m_associativity; i++)
   {

      if (!m_cache_block_info_array[i]->isValid())
      {
         
         // Mark our newly-inserted line as most-recently used
         //moveToMRU(i);
         // 还没有被用过，可直接返回，且不需要移除
         //printf("------------getReplacementIndex: %d-------------\n",i);
         return i;
      }


      if(m_opt_bits[i]==unknown){
         return i;
      }

      if(m_opt_bits[i]>maxvalue){
         maxpos=i;
         maxvalue=m_opt_bits[i];
      }

   }
   return maxpos;




   Singleton *single=Singleton::getInstance();
   IntPtr currentAddr=Singleton::getAddr(single->getValue());

   IntPtr tag;
   UInt32 set_index;
   cache->splitAddress(currentAddr, tag, set_index);

    printf("currentAddr:%lx currentTag %lx,current_SetIndex \n",currentAddr,tag,set_index);


   // First try to find an invalid block
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         
         // Mark our newly-inserted line as most-recently used
         moveToMRU(i);
         // 还没有被用过，可直接返回，且不需要移除
         //printf("------------getReplacementIndex: %d-------------\n",i);
         return i;
      }
   }

   // Make m_num_attemps attempts at evicting the block at OPT position
   // inclusive/noclusive,LRU不用考虑
   for(UInt8 attempt = 0; attempt < m_num_attempts; ++attempt)
   {
      UInt32 index = 0;
      UInt8 max_bits = 0;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_opt_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_opt_bits[i];
         }
      }
      LOG_ASSERT_ERROR(index < m_associativity, "Error Finding OPT bits");

      bool qbs_reject = false;
      if (attempt < m_num_attempts - 1)
      {
         LOG_ASSERT_ERROR(cntlr != NULL, "CacheCntlr == NULL, QBS can only be used when cntlr is passed in");
         qbs_reject = cntlr->isInLowerLevelCache(m_cache_block_info_array[index]);
      }

      if (qbs_reject)
      {
         // Block is contained in lower-level cache, and we have more tries remaining.
         // Move this block to MRU and try again
         moveToMRU(index);
         cntlr->incrementQBSLookupCost();
         continue;
      }
      else
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(index);
         m_set_info->incrementAttempt(attempt);
         //printf("------------getReplacementIndex_xixxxxxx: %d-------------\n",index);
         return index;
      }
   }

   LOG_PRINT_ERROR("Should not reach here");
}

void
CacheSetOPT::updateReplacementIndex(UInt32 accessed_index)
{
   return;
   // 在这里执行lookahead
   m_set_info->increment(m_opt_bits[accessed_index]);
   moveToMRU(accessed_index);
}

void
CacheSetOPT::moveToMRU(UInt32 accessed_index)
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_opt_bits[i] < m_opt_bits[accessed_index])
         m_opt_bits[i] ++;
   }
   m_opt_bits[accessed_index] = 0;
}

CacheSetInfoOPT::CacheSetInfoOPT(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts)
   : m_associativity(associativity)
   , m_attempts(NULL)
{
   m_access = new UInt64[m_associativity];
   for(UInt32 i = 0; i < m_associativity; ++i)
   {
      m_access[i] = 0;
      registerStatsMetric(name, core_id, String("access-mru-")+itostr(i), &m_access[i]);
   }

   if (num_attempts > 1)
   {
      m_attempts = new UInt64[num_attempts];
      for(UInt32 i = 0; i < num_attempts; ++i)
      {
         m_attempts[i] = 0;
         registerStatsMetric(name, core_id, String("qbs-attempt-")+itostr(i), &m_attempts[i]);
      }
   }
};

CacheSetInfoOPT::~CacheSetInfoOPT()
{
   delete [] m_access;
   if (m_attempts)
      delete [] m_attempts;
}
