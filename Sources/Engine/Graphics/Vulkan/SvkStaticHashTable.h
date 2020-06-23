/* Copyright (c) 2020 Sultim Tsyrendashiev
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

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
  // Get value by its key
  T &Get(INDEX key);
  // Try to find value by its key, returns nullptr if not found
  T *TryGet(INDEX key);
  void Delete(INDEX key);
  void Clear();
  // Apply function to each element in each bucket
  void Map(void (*func) (T&));

private:
  bool FindElement(INDEX key, INDEX &outBucketIndex, INDEX &outElemIndex);
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

#ifndef NDEBUG
  auto *sps = TryGet(key);
  ASSERTMSG(sps == nullptr, "Adding an element with a key that already exist in a hash table.");
#endif // !NDEBUG

  
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
  bool found = FindElement(key, bucketIndex, index);
  ASSERTMSG(found, "SvkHashTable: Can't find element with specified key");

  return ht_Buckets[bucketIndex][index].Value;
}

template<class T>
inline T *SvkStaticHashTable<T>::TryGet(INDEX key)
{
  ASSERT(ht_Buckets != nullptr && ht_BucketCount != 0);

  INDEX bucketIndex, index;
  bool found = FindElement(key, bucketIndex, index);

  if (found)
  {
    return &ht_Buckets[bucketIndex][index].Value;
  }
  else
  {
    return nullptr;
  }
}

template<class T>
inline void SvkStaticHashTable<T>::Delete(INDEX key)
{
  ASSERT(ht_Buckets != nullptr && ht_BucketCount != 0);

  INDEX bucketIndex, index;
  bool found = FindElement(key, bucketIndex, index);
  ASSERTMSG(found, "SvkHashTable: Can't find element with specified key");

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
inline bool SvkStaticHashTable<T>::FindElement(INDEX key, INDEX &outBucketIndex, INDEX &outElemIndex)
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
      return true;
    }
  }

  return false;
}
#endif

