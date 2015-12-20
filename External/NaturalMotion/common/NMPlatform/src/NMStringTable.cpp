// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMStringTable.h"
#include "NMPlatform/NMHash.h"
#include "NMPlatform/NMMathUtils.h"
#include <string>
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

// comparison function given to std::bsearch
static int compareUint32(const void* a, const void * b)
{
  NMP_ASSERT(a && b);
  if ((*(uint32_t*)a) < (*(uint32_t*)b))
  {
    return -1;
  }
  if ((*(uint32_t*)a) > (*(uint32_t*)b))
  {
    return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
IDMappedStringTable* IDMappedStringTable::init(
  NMP::Memory::Resource& resource,
  uint32_t               numEntrys,
  const uint32_t*        ids,
  const uint32_t*        offsets,
  const char*            data,
  uint32_t               dataLength)
{
  IDMappedStringTable* result = initResourcePointers(resource, numEntrys, dataLength);

  // Copy the data in.
  NMP::Memory::memcpy((void*)result->m_IDs, ids, sizeof(uint32_t) * numEntrys);
  NMP::Memory::memcpy((void*)result->m_Offsets, offsets, sizeof(uint32_t) * numEntrys);
  NMP::Memory::memcpy((void*)result->m_Data, data, sizeof(char) * dataLength);

  result->buildHashTable();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
IDMappedStringTable* IDMappedStringTable::init(
  NMP::Memory::Resource& resource,
  uint32_t               numEntrys,
  const uint32_t*        ids,
  const char* const*           strings)
{
  // Find out how much data space we need
  uint32_t dataLength = 0;
  for (uint32_t i = 0; i < numEntrys; i++)
  {
    dataLength += (uint32_t)NMP_STRLEN(strings[i]) + 1;
  }

  IDMappedStringTable* result = initResourcePointers(resource, numEntrys, dataLength);

  // Copy the data in.
  NMP::Memory::memcpy((void*)result->m_IDs, ids, sizeof(uint32_t) * numEntrys);

  uint32_t currentOffset = 0;
  for (uint32_t i = 0; i < numEntrys; i++)
  {
    result->m_Offsets[i] = currentOffset;
    uint32_t currLength = (uint32_t)NMP_STRLEN(strings[i]) + 1;
    NMP_ASSERT(currLength <= (dataLength - currentOffset));
    NMP_STRNCPY_S((char*)result->m_Data + currentOffset, currLength, strings[i]);
    currentOffset += currLength;
  }

  result->buildHashTable();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
IDMappedStringTable* IDMappedStringTable::initResourcePointers(
  NMP::Memory::Resource& resource,
  uint32_t               numEntrys,
  uint32_t               dataLength)
{
  // String table class itself.
  NMP::Memory::Format format(sizeof(IDMappedStringTable), NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  IDMappedStringTable* result = (IDMappedStringTable*) resource.ptr;
  resource.increment(format);

  // Setup the debug string table.
  result->m_NumEntrys = numEntrys;
  result->m_DataLength = dataLength;

  // IDs array.
  format = NMP::Memory::Format(sizeof(uint32_t) * numEntrys, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_IDs = (uint32_t*) resource.ptr;
  resource.increment(format);

  // offsets array.
  format = NMP::Memory::Format(sizeof(uint32_t) * numEntrys, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_Offsets = (uint32_t*) resource.ptr;
  resource.increment(format);

  // Hash array.
  format = NMP::Memory::Format(sizeof(uint32_t) * numEntrys, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_HashTable = (uint32_t*) resource.ptr;
  resource.increment(format);

  // String data.
  format = NMP::Memory::Format(sizeof(char) * dataLength, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_Data = (char*) resource.ptr;
  resource.increment(format);

  return result;
}

void IDMappedStringTable::buildHashTable()
{
  for (uint32_t i = 0; i < m_NumEntrys; ++i)
  {
    m_HashTable[i] = hashStringCRC32(getEntryString(i));
  }

  sortByHash();
}

void IDMappedStringTable::sortByHash()
{
  for (uint32_t i = 0; i < m_NumEntrys; ++i)
  {
    uint32_t nextMinEntry = i;
    for (uint32_t j = i+1; j < m_NumEntrys; ++j)
    {
      if (m_HashTable[j] < m_HashTable[nextMinEntry])
      {
        nextMinEntry = j;
      }
    }
    swapEntry(i, nextMinEntry);
  }
}

void IDMappedStringTable::swapEntry(uint32_t a, uint32_t b)
{
  if (a == b)
  {
    return;
  }

  uint32_t temp;
  // swap ID
  temp = m_IDs[a];
  m_IDs[a] = m_IDs[b];
  m_IDs[b] = temp;

  // swap offset
  temp = m_Offsets[a];
  m_Offsets[a] = m_Offsets[b];
  m_Offsets[b] = temp;

  // swap hash
  temp = m_HashTable[a];
  m_HashTable[a] = m_HashTable[b];
  m_HashTable[b] = temp;
}


//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format IDMappedStringTable::getMemoryRequirements(uint32_t numEntries, uint32_t dataLength)
{
  NMP::Memory::Format result(NMP::Memory::align(sizeof(IDMappedStringTable), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  // reserve space for the ID to index, offset for index, and string hash tables
  result.size += numEntries * 3 * sizeof(uint32_t);

  // reserve space for the string data itself
  result.size += sizeof(char) * dataLength;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format IDMappedStringTable::getMemoryRequirements(uint32_t numEntries, const char* const* strings)
{
  // Get the data length
  uint32_t dataLength = 0;
  for (uint32_t i = 0; i < numEntries; i++)
  {
    dataLength += (uint32_t)NMP_STRLEN(strings[i]) + 1;
  }

  return getMemoryRequirements(numEntries, dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format IDMappedStringTable::getInstanceMemoryRequirements() const
{
  return getMemoryRequirements(m_NumEntrys, m_DataLength);
}

//----------------------------------------------------------------------------------------------------------------------
const char* IDMappedStringTable::getStringForID(uint32_t id) const
{
  // linear search for id. (return longest string)
  const char* currStr = NULL;
  size_t len = 0;
  for (uint32_t i = 0; i < m_NumEntrys; i++)
  {
    if (m_IDs[i] == id)
    {
      size_t strSize = NMP_STRLEN(getEntryString(i));
      if(strSize >= len)
      {
        len = strSize;
        currStr = getEntryString(i);
      }
    }
  }
  return currStr;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t IDMappedStringTable::getIDForString(const char* stringName) const
{
  // hash input
  uint32_t inputHash = hashStringCRC32(stringName);
  // binary search hash entries
  uint32_t* pFoundHash = (uint32_t*)std::bsearch(&inputHash, m_HashTable, m_NumEntrys, sizeof(uint32_t), compareUint32);

  if (pFoundHash == NULL)
  {
    return NMP_STRING_NOT_FOUND;
  }

  uint32_t index = (uint32_t)(pFoundHash - m_HashTable);

  // in case of hash collision rewind to first match
  while (index != 0 && m_HashTable[index - 1] == inputHash)
  {
    --index;
  }

  // verify string match
  while (index < m_NumEntrys && m_HashTable[index] == inputHash)
  {
    const char* currString = getEntryString(index);
    if (!NMP_STRCMP(stringName, currString))
    {
      return getEntryID(index);
    }
    ++index;
  }
  return NMP_STRING_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t IDMappedStringTable::getIDForStringN(const char* stringName, uint32_t numCharsToCompare) const
{
  for (uint32_t i = 0; i < m_NumEntrys; ++i)
  {
    const char* currString = getEntryString(i);
    if (!NMP_STRNCMP(currString, stringName, numCharsToCompare))
    {
      return getEntryID(i);
    }
  }
  return NMP_STRING_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t IDMappedStringTable::findNumEntriesForString(
  const char* stringName) const
{
  // hash input
  uint32_t inputHash = hashStringCRC32(stringName);
  // binary search hash entries
  uint32_t* pFoundHash = (uint32_t*)std::bsearch(&inputHash, m_HashTable, m_NumEntrys, sizeof(uint32_t), compareUint32);

  if (pFoundHash == NULL)
  {
    return 0;
  }

  uint32_t index = (uint32_t)(pFoundHash - m_HashTable);

  // in case of hash collision rewind to first match
  while (index != 0 && m_HashTable[index - 1] == inputHash)
  {
    --index;
  }

  // verify string match
  uint32_t count = 0;
  while (index < m_NumEntrys && m_HashTable[index] == inputHash)
  {
    const char* currString = getEntryString(index);
    if (!NMP_STRCMP(stringName, currString))
    {
      ++count;
    }
    ++index;
  }
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t IDMappedStringTable::getEntryID(uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_NumEntrys);
  return m_IDs[entryIndex];
}

//----------------------------------------------------------------------------------------------------------------------
const char* IDMappedStringTable::getEntryString(uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_NumEntrys);
  uint32_t offset = m_Offsets[entryIndex];
  return &(m_Data[offset]);
}

//----------------------------------------------------------------------------------------------------------------------
void IDMappedStringTable::getIDs( uint32_t* dst, uint32_t maxNum ) const
{
  const uint32_t numRequestsToCopy = NMP::minimum(m_NumEntrys, maxNum);
  for(uint32_t i = 0; i < numRequestsToCopy; ++i)
  {
    dst[i] = getEntryID(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void IDMappedStringTable::relocate()
{
  // String table class itself.
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  ptr = (void*)(((uint8_t*)ptr) + sizeof(IDMappedStringTable));

  // IDs array.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_IDs = (uint32_t*) ptr;
  ptr = (void*)(((uint8_t*)ptr) + (sizeof(uint32_t) * m_NumEntrys));

  // offsets array.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_Offsets = (uint32_t*) ptr;
  ptr = (void*)(((uint8_t*)ptr) + (sizeof(uint32_t) * m_NumEntrys));

  // hash array.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_HashTable = (uint32_t*) ptr;
  ptr = (void*)(((uint8_t*)ptr) + (sizeof(uint32_t) * m_NumEntrys));

  // String data.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_Data = (char*) ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void IDMappedStringTable::locate()
{
  REFIX_SWAP_PTR(uint32_t, m_IDs);
  REFIX_SWAP_PTR(uint32_t, m_Offsets);
  REFIX_SWAP_PTR(uint32_t, m_HashTable);
  REFIX_SWAP_PTR(const char, m_Data);

  NMP::endianSwap(m_NumEntrys);
  NMP::endianSwap(m_DataLength);

  // Endian swap the table entry array
  NMP::endianSwapArray((uint32_t*)m_IDs, m_NumEntrys);
  NMP::endianSwapArray((uint32_t*)m_Offsets, m_NumEntrys);
  NMP::endianSwapArray((uint32_t*)m_HashTable, m_NumEntrys);
}

//----------------------------------------------------------------------------------------------------------------------
void IDMappedStringTable::dislocate()
{
  // Endian swap the table entry array
  NMP::endianSwapArray((uint32_t*)m_IDs, m_NumEntrys);
  NMP::endianSwapArray((uint32_t*)m_Offsets, m_NumEntrys);
  NMP::endianSwapArray((uint32_t*)m_HashTable, m_NumEntrys);

  NMP::endianSwap(m_DataLength);
  NMP::endianSwap(m_NumEntrys);

  UNFIX_SWAP_PTR(const char, m_Data);
  UNFIX_SWAP_PTR(uint32_t, m_IDs);
  UNFIX_SWAP_PTR(uint32_t, m_Offsets);
  UNFIX_SWAP_PTR(uint32_t, m_HashTable);
}

//----------------------------------------------------------------------------------------------------------------------
OrderedStringTable* OrderedStringTable::init(
  NMP::Memory::Resource& resource,
  uint32_t               numEntrys,
  const uint32_t*        offsets,
  const char*            data,
  uint32_t               dataLength)
{
  OrderedStringTable* result = initResourcePointers(resource, numEntrys, dataLength);

  // Copy the data in.
  NMP::Memory::memcpy(result->m_Offsets, offsets, sizeof(uint32_t) * numEntrys);
  NMP::Memory::memcpy((void*)result->m_Data, data, sizeof(char) * dataLength);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
OrderedStringTable* OrderedStringTable::init(
  NMP::Memory::Resource& resource,
  uint32_t               numEntrys,
  const char* const*     strings)
{
  uint32_t dataLength = 0;

  // Find out how much data space we need
  for (uint32_t i = 0; i < numEntrys; i++)
  {
    dataLength += (uint32_t)NMP_STRLEN(strings[i]) + 1;
  }

  OrderedStringTable* result = initResourcePointers(resource, numEntrys, dataLength);

  // Copy the data in.
  uint32_t currentOffset = 0;
  for (uint32_t i = 0; i < numEntrys; i++)
  {
    result->m_Offsets[i] = currentOffset;
    uint32_t currLength = (uint32_t)NMP_STRLEN(strings[i]) + 1;
    NMP_ASSERT(currLength <= (dataLength - currentOffset));
    NMP_STRNCPY_S((char*)result->m_Data + currentOffset, currLength, strings[i]);
    currentOffset += currLength;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
OrderedStringTable* OrderedStringTable::initResourcePointers(
  NMP::Memory::Resource& resource,
  uint32_t               numEntrys,
  uint32_t               dataLength)
{
  // String table class itself.
  NMP::Memory::Format format(sizeof(OrderedStringTable), NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  OrderedStringTable* result = (OrderedStringTable*) resource.ptr;
  resource.increment(format);

  // Setup the debug string table.
  result->m_NumEntrys = numEntrys;
  result->m_DataLength = dataLength;

  // Offsets array.
  format = NMP::Memory::Format(sizeof(uint32_t) * numEntrys, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_Offsets = (uint32_t*) resource.ptr;
  resource.increment(format);

  // String data.
  format = NMP::Memory::Format(sizeof(char) * dataLength, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_Data = (char*) resource.ptr;
  resource.increment(format);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format OrderedStringTable::getMemoryRequirements(uint32_t numEntries, uint32_t dataLength)
{
  NMP::Memory::Format result(NMP::Memory::align(sizeof(OrderedStringTable), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  // reserve space for the offset table
  result.size += numEntries * sizeof(uint32_t);

  // reserve space for the string data itself
  result.size += sizeof(char) * dataLength;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format OrderedStringTable::getMemoryRequirements(uint32_t numEntries, const char* const* strings)
{
  // Get the data length
  uint32_t dataLength = 0;
  for (uint32_t i = 0; i < numEntries; i++)
  {
    dataLength += (uint32_t)NMP_STRLEN(strings[i]) + 1;
  }

  return getMemoryRequirements(numEntries, dataLength);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format OrderedStringTable::getInstanceMemoryRequirements() const
{
  NMP::Memory::Format result(NMP::Memory::align(sizeof(OrderedStringTable), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  // reserve space for the offset table
  result.size += m_NumEntrys * sizeof(uint32_t);

  // reserve space for the string data itself
  result.size += m_DataLength * sizeof(char);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* OrderedStringTable::getStringForID(uint32_t id) const
{
  return getEntryString(id);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OrderedStringTable::getEntryID(uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_NumEntrys);
  return entryIndex;
}

//----------------------------------------------------------------------------------------------------------------------
const char* OrderedStringTable::getEntryString(uint32_t entryIndex) const
{
  NMP_ASSERT(entryIndex < m_NumEntrys)
  uint32_t offset = m_Offsets[entryIndex];
  return &(m_Data[offset]);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OrderedStringTable::getIDForString(const char* stringName) const
{
  for (uint32_t currEntry = 0; currEntry < m_NumEntrys; currEntry++)
  {
    const char* currString = getEntryString(currEntry);

    if (!NMP_STRCMP(stringName, currString))
    {
      return currEntry;
    }
  }
  return NMP_STRING_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OrderedStringTable::getIDForStringN(const char* stringName, uint32_t numCharsToCompare) const
{
  for (uint32_t currEntry = 0; currEntry < m_NumEntrys; ++currEntry)
  {
    const char* currString = getEntryString(currEntry);
    if (!NMP_STRNCMP(currString, stringName, numCharsToCompare))
    {
      return currEntry;
    }
  }
  return NMP_STRING_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t OrderedStringTable::findNumEntriesForString(const char* stringName) const
{
  uint32_t count = 0;
  for (uint32_t currEntry = 0; currEntry < m_NumEntrys; currEntry++)
  {
    const char* currString = getEntryString(currEntry);
    if (!NMP_STRCMP(stringName, currString))
    {
      ++count;
    }
  }
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
void OrderedStringTable::getIDs( uint32_t* dst, uint32_t maxNum ) const
{
  const uint32_t numRequestsToCopy = NMP::minimum(m_NumEntrys, maxNum);
  for(uint32_t i = 0; i < numRequestsToCopy; ++i)
  {
    dst[i] = i;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void OrderedStringTable::relocate()
{
  // String table class itself.
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  ptr = (void*)(((uint8_t*)ptr) + sizeof(OrderedStringTable));

  // Offsets array.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_Offsets = (uint32_t*) ptr;
  ptr = (void*)(((uint8_t*)ptr) + (sizeof(uint32_t) * m_NumEntrys));

  // String data.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_Data = (char*) ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void OrderedStringTable::locate()
{
  REFIX_SWAP_PTR(uint32_t, m_Offsets);
  REFIX_SWAP_PTR(const char, m_Data);

  NMP::endianSwap(m_NumEntrys);
  NMP::endianSwap(m_DataLength);

  // Endian swap the offset table
  NMP::endianSwapArray(m_Offsets, m_NumEntrys);
}

//----------------------------------------------------------------------------------------------------------------------
void OrderedStringTable::dislocate()
{
  // Endian swap the offset table
  NMP::endianSwapArray(m_Offsets, m_NumEntrys);

  NMP::endianSwap(m_NumEntrys);
  NMP::endianSwap(m_DataLength);

  UNFIX_SWAP_PTR(uint32_t, m_Offsets);
  UNFIX_SWAP_PTR(const char, m_Data);
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
