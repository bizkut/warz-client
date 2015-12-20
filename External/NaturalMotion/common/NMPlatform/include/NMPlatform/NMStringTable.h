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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_STRINGTABLE_H
#define NM_STRINGTABLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

#define NMP_STRING_NOT_FOUND 0xFFFFFFFF

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::IDMappedStringTable
/// \brief A simple ID to string map.
/// \details Supports arbitrarily assigned IDs, so getting string from ID is O(N). Getting ID from string is O(log(N))
/// \ingroup NaturalMotionPlatform
///
/// String entries must be null terminated.
//----------------------------------------------------------------------------------------------------------------------
class IDMappedStringTable
{
public:
  /// \brief initialise a string table in the supplied memory.
  ///
  /// Increments resource pointer.
  static IDMappedStringTable* init(
    NMP::Memory::Resource& resource,
    uint32_t               numEntrys,
    const uint32_t*        ids,
    const uint32_t*        offsets,
    const char*            data,
    uint32_t               dataLength);

  /// \brief initialise a string table in the supplied memory.
  ///
  /// Increments resource pointer.
  static IDMappedStringTable* init(
    NMP::Memory::Resource& resource,
    uint32_t               numEntrys,
    const uint32_t*        ids,
    const char* const*     strings);
 
  /// \brief Calculate the memory required to create an OrderedStringTable from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries, uint32_t dataLength);

  /// \brief Calculate the memory required to create an OrderedStringTable from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries, const char* const* strings);

  /// \brief Get the memory requirements of this instance.
  NMP::Memory::Format getInstanceMemoryRequirements() const;

  /// \brief Given an ID, return the string corresponding to it in the map.
  /// \return The string corresponding to the ID, or NULL on failure
  const char* getStringForID(uint32_t id) const;

  /// \brief Given a string, return the stored entry ID of the entry string that fully corresponds to it in the map.
  /// \return The entry ID for the string, or NMP_STRING_NOT_FOUND.
  ///
  /// Note: The strings must be identical.
  uint32_t getIDForString(const char* stringName) const;        ///< Must be null terminated.

  /// \brief Given a string, return the stored entry ID of the first found entry string whose first numCharsToCompare
  ///        are the same as the search string.
  /// \return The entry ID for the string, or NMP_STRING_NOT_FOUND.
  ///
  /// Note: The entry string can be longer then the provided string
  uint32_t getIDForStringN(
    const char* stringName,               ///< Must be null terminated.
    uint32_t    numCharsToCompare         ///< Only compare this many characters.
    ) const;

  /// \brief Find the number of entries which are identical to the search string.
  uint32_t findNumEntriesForString(
    const char* stringName) const;        ///< Must be null terminated.
  
  /// \brief Find the number of entries whos first numCharsToCompare are the same as the search string.
  uint32_t findNumEntriesForStringN(
    const char* stringName,               ///< Must be null terminated.
    uint32_t    numCharsToCompare         ///< Only compare this many characters.
    ) const;

  /// \brief Get the ID stored at the given entry index.
  uint32_t getEntryID(uint32_t entryIndex) const;

  /// \brief Get the string stored at the given entry index.
  const char* getEntryString(uint32_t entryIndex) const;

  void locate();
  void dislocate();
  void relocate();

  uint32_t getNumEntries() const { return m_NumEntrys; }
  uint32_t getDataLength() const { return m_DataLength; }
  void getIDs(uint32_t* dst, uint32_t maxNum) const;
  bool getOffsets(uint32_t* dst, uint32_t maxNum) const;
  const char* getData() const { return m_Data; }

protected:
  static IDMappedStringTable* initResourcePointers(NMP::Memory::Resource& resource, uint32_t numEntrys, uint32_t dataLength);
  void buildHashTable();
  void sortByHash();
  void swapEntry(uint32_t a, uint32_t b);

private:
  uint32_t     m_NumEntrys;
  uint32_t     m_DataLength;
  uint32_t*    m_IDs;
  uint32_t*    m_Offsets;
  uint32_t*    m_HashTable;
  const char*  m_Data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::OrderedStringTable
/// \brief A simple ID to string map.
/// \details Assumes assumes each string's ID matches the string's zero-based position in the list,
/// so string look up by ID is O(1). Getting ID from string is O(N)
/// \todo test then phase out validation wrappers
/// \ingroup NaturalMotionPlatform
///
/// String entries must be null terminated.
//----------------------------------------------------------------------------------------------------------------------
class OrderedStringTable
{
public:

  /// \brief initialise a string table in the supplied memory.
  ///
  /// Increments resource pointer.
  static OrderedStringTable* init(
    NMP::Memory::Resource& resource,
    uint32_t               numEntrys,
    const uint32_t*        offsets,
    const char*            data,
    uint32_t               dataLength);

  /// \brief initialise a string table in the supplied memory.
  ///
  /// Increments resource pointer.
  static OrderedStringTable* init(
    NMP::Memory::Resource& resource,
    uint32_t               numEntrys,
    const char* const*     strings);

  /// \brief Calculate the memory required to create a OrderedStringTable from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries, uint32_t dataLength);

  /// \brief Calculate the memory required to create a OrderedStringTable from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries, const char* const* strings);

  /// \brief Get the memory requirements of this instance.
  NMP::Memory::Format getInstanceMemoryRequirements() const;

  /// \brief Given an ID, return the string corresponding to it in the map.
  /// \return The string corresponding to the ID, or NULL on failure
  const char* getStringForID(uint32_t id) const;

  /// \brief Given a string, return the stored entry ID of the entry string that fully corresponds to it in the map.
  /// \return The entry ID for the string, or NMP_STRING_NOT_FOUND.
  ///
  /// Note: The strings must be identical.
  uint32_t getIDForString(
    const char* stringName) const;        ///< Must be null terminated.

  /// \brief Given a string, return the stored entry ID of the first found entry string whose first numCharsToCompare
  ///        are the same as the search string.
  /// \return The entry ID for the string, or NMP_STRING_NOT_FOUND.
  ///
  /// Note: The entry string can be longer then the provided string
  uint32_t getIDForStringN(
    const char* stringName,               ///< Must be null terminated.
    uint32_t    numCharsToCompare         ///< Only compare this many characters.
    ) const;

  /// \brief Find the number of entries which are identical to the search string.
  uint32_t findNumEntriesForString(
    const char* stringName) const;        ///< Must be null terminated.

  /// \brief Find the number of entries whos first numCharsToCompare are the same as the search string.
  uint32_t findNumEntriesForStringN(
    const char* stringName,               ///< Must be null terminated.
    uint32_t    numCharsToCompare         ///< Only compare this many characters.
    ) const;

  /// \brief Get the ID stored at the given entry index. Calling this on OrderedStringTables is pointless.
  uint32_t getEntryID(uint32_t entryIndex) const;

  /// \brief Get the string stored at the given entry index.
  const char* getEntryString(uint32_t entryIndex) const;

  void locate();
  void dislocate();
  void relocate();

  uint32_t getNumEntries() const { return m_NumEntrys; }
  uint32_t getDataLength() const { return m_DataLength; }
  const uint32_t* getOffsets() const { return m_Offsets; }
  const char* getData() const { return m_Data; }
  void getIDs(uint32_t* dst, uint32_t maxNum) const;

protected:
  static OrderedStringTable* initResourcePointers(NMP::Memory::Resource& resource, uint32_t numEntrys, uint32_t dataLength);

private:
  uint32_t    m_NumEntrys;
  uint32_t    m_DataLength;
  uint32_t*   m_Offsets;
  const char* m_Data;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_STRINGTABLE_H
//----------------------------------------------------------------------------------------------------------------------
