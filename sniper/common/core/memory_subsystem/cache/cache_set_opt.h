#ifndef CACHE_SET_OPT_H
#define CACHE_SET_OPT_H

#include "cache_set.h"
#include <list>
#define ahead 50
#define unknown INT32_MAX
class CacheSetInfoOPT : public CacheSetInfo
{
   public:
      CacheSetInfoOPT(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts);
      virtual ~CacheSetInfoOPT();
      void increment(UInt32 index)
      {
         LOG_ASSERT_ERROR(index < m_associativity, "Index(%d) >= Associativity(%d)", index, m_associativity);
         ++m_access[index];
      }
      void incrementAttempt(UInt8 attempt)
      {
         if (m_attempts)
            ++m_attempts[attempt];
         else
            LOG_ASSERT_ERROR(attempt == 0, "No place to store attempt# histogram but attempt != 0");
      }
   private:
      const UInt32 m_associativity;
      UInt64* m_access;
      UInt64* m_attempts;
};

class CacheSetOPT : public CacheSet
{
   public:
      CacheBase *cache;
      int setIndex;
      std::list<IntPtr> initFutureTag();
      void updateFutureTage();
      CacheSetOPT(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, CacheSetInfoOPT* set_info, UInt8 num_attempts,CacheBase * bcache,int cacheIndex);
      virtual ~CacheSetOPT();

      virtual UInt32 getReplacementIndex(CacheCntlr *cntlr);
      void updateReplacementIndex(UInt32 accessed_index);

   protected:
      
      const UInt8 m_num_attempts;
      int* m_opt_bits;
      CacheSetInfoOPT* m_set_info;
      void moveToMRU(UInt32 accessed_index);
};

#endif /* CACHE_SET_OPT_H */
