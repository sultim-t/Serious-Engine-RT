#ifndef SE_INCL_SVKSTATICHASHTABLE_H
#define SE_INCL_SVKSTATICHASHTABLE_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/Memory.h>
#include <Engine/Templates/StaticStackArray.cpp>

template <class T>
class SvkStaticHashTable
{
private:
  struct SvkBucketElement
  {
    INDEX   Key;
    T       Value;

    SvkBucketElement()
    {
      Key = 0;
    }
  };

  CStaticStackArray<SvkBucketElement>   *ht_Buckets;
  INDEX                                 ht_BucketCount;

  INDEX (*hashFunction) (INDEX key);

public:
  SvkStaticHashTable();
  // Bucket size must be as small as possible as linear search is applied to it
  void New(INDEX bucketCount = 128, INDEX defaultBucketSize = 32);
  void SetHashFunction(INDEX(*func) (INDEX key));
  bool IsAllocated();

  // Value will be copied to hash table
  void Add(INDEX key, const T &value);
  T &Get(INDEX key);
  void Delete(INDEX key);
  void Clear();
  // Apply function to each element in each bucket
  void Map(void (*func) (T&));

private:
  void FindElement(INDEX key, INDEX &outBucketIndex, INDEX &outElemIndex);
};



template<class T>
inline SvkStaticHashTable<T>::SvkStaticHashTable()
{
  ht_Buckets = nullptr;
  ht_BucketCount = 0;
  hashFunction = nullptr;
}

template<class T>
inline void SvkStaticHashTable<T>::New(INDEX bucketCount, INDEX defaultBucketSize)
{
  ASSERT(ht_Buckets == nullptr && ht_BucketCount == 0);

  ht_Buckets = new CStaticStackArray<SvkBucketElement>[bucketCount];
  ht_BucketCount = bucketCount;

  for (INDEX i = 0; i < bucketCount; i++)
  {
    ht_Buckets[i].New(defaultBucketSize);
  }
}

template<class T>
inline void SvkStaticHashTable<T>::SetHashFunction(INDEX(*func)(INDEX key))
{
  hashFunction = func;
}

template<class T>
inline bool SvkStaticHashTable<T>::IsAllocated()
{
  return ht_Buckets != nullptr;
}

template<class T>
inline void SvkStaticHashTable<T>::Add(INDEX key, const T &value)
{
  ASSERT(ht_Buckets != nullptr && ht_BucketCount != 0);
  
  INDEX hash = hashFunction == nullptr ? key : hashFunction(key);
  INDEX bucketIndex = hash % ht_BucketCount;

  auto &added = ht_Buckets[bucketIndex].Push();
  added.Key = key;
  added.Value = value;
}

template<class T>
inline T &SvkStaticHashTable<T>::Get(INDEX key)
{
  ASSERT(ht_Buckets != nullptr && ht_BucketCount != 0);

  INDEX bucketIndex, index;
  FindElement(key, bucketIndex, index);

  return ht_Buckets[bucketIndex][index].Value;
}

template<class T>
inline void SvkStaticHashTable<T>::Delete(INDEX key)
{
  ASSERT(ht_Buckets != nullptr && ht_BucketCount != 0);

  INDEX bucketIndex, index;
  FindElement(key, bucketIndex, index);

  auto &bucket = ht_Buckets[bucketIndex];

  // replace deleted with last
  bucket[index] = bucket[bucket.Count() - 1];
  bucket.Pop();
}

template<class T>
inline void SvkStaticHashTable<T>::Clear()
{
  if (ht_Buckets == nullptr)
  {
    return;
  }

  for (INDEX i = 0; i < ht_BucketCount; i++)
  {
    ht_Buckets[i].Clear();
  }

  delete[] ht_Buckets;

  ht_Buckets = nullptr;
  ht_BucketCount = 0;
}

template<class T>
inline void SvkStaticHashTable<T>::Map(void(*func)(T &))
{
  if (ht_Buckets == nullptr)
  {
    return;
  }

  for (INDEX i = 0; i < ht_BucketCount; i++)
  {
    auto &bucket = ht_Buckets[i];
    for (INDEX j = 0; j < bucket.Count(); j++)
    {
      func(bucket[j].Value);
    }
  }
}

template<class T>
inline void SvkStaticHashTable<T>::FindElement(INDEX key, INDEX &outBucketIndex, INDEX &outElemIndex)
{
  INDEX hash = hashFunction == nullptr ? key : hashFunction(key);
  INDEX bucketIndex = hash % ht_BucketCount;
  auto &bucket = ht_Buckets[bucketIndex];

  // find index, slow
  for (INDEX i = 0; i < bucket.Count(); i++)
  {
    if (bucket[i].Key == key)
    {
      outElemIndex = i;
      outBucketIndex = bucketIndex;
      return;
    }
  }

  ASSERTALWAYS("SvkHashTable: Can't find element with specified key");
}
#endif

