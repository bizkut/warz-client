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
#if (!defined(NMTL_STRING_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_STRING_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include <assert.h>
#ifndef NMTL_NO_SENTRY
  #include "nmtl/pod_vector.h"
#endif

#ifndef NMTL_TEMPLATE_EXPORT
  #define NMTL_TEMPLATE_EXPORT
#endif
#ifndef NMTL_NAMESPACE_OVERRIDE
  #define NMTL_NAMESPACE nmtl
#else
  #define NMTL_NAMESPACE NMTL_NAMESPACE_OVERRIDE
#endif

#ifndef NMTL_INDEX
  #define NMTL_INDEX
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMTL_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMTL::wstring
/// \brief std::wstring replacement that works safely over DLL boundaries
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename value_type>
class NMTL_TEMPLATE_EXPORT basic_string :
  protected pod_vector<value_type>
{
public:
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef pod_vector<value_type> base_type;
  typedef typename base_type::const_iterator const_iterator;
  typedef typename base_type::iterator iterator;

  static const size_type npos; // generic bad/missing length/position

  /// \brief  assign from a string
  /// \param  to_copy - the string to assign
  /// \return *this
  NMTL_INLINE basic_string<value_type>& operator=(const basic_string<value_type>& to_copy)
  {

    return *this;
  }

  /// \brief  assign from a char* string
  /// \param  __s - the string to assign
  /// \return *this
  NMTL_INLINE basic_string<value_type>& operator=(const value_type* __s)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  assign from a wchar_t
  /// \param  __s - the value to assign
  /// \return *this
  NMTL_INLINE basic_string<value_type>& operator=(value_type __c)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _Ptr - the value to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(const value_type* _Ptr)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _Ptr - the value to append
  /// \param  _Count - the number of chars to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(const value_type* _Ptr, size_type _Count)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _Str - the value to append
  /// \param  _Count - the number of chars to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(const basic_string<value_type>& _Str, NMTL_INDEX size_type _Off, size_type _Count)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _Str - the value to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(const basic_string<value_type>& _Str)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _Count - the number of chars to append
  /// \param  _Ch - the char to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(size_type _Count, value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _First - the start of the string to append
  /// \param  _Last - the end of the string to append
  /// \return *this
  template<typename InputIterator>
  NMTL_INLINE basic_string<value_type>& append(InputIterator _First, InputIterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _First - the start of the string to append
  /// \param  _Last - the end of the string to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(const_pointer _First, const_pointer _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  appends the value to the end of the string
  /// \param  _First - the start of the string to append
  /// \param  _Last - the end of the string to append
  /// \return *this
  NMTL_INLINE basic_string<value_type>& append(const_iterator _First, const_iterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns a reference to the character at the specified offset
  /// \param  _Off - the offset of the character to return
  /// \return the requested character
  NMTL_INLINE const_reference at(NMTL_INDEX size_type _Off) const
  {
    NMTL_UNIMPLEMENTED;
    static value_type v = 0;
    return v;
  }

  /// \brief  returns a reference to the character at the specified offset
  /// \param  _Off - the offset of the character to return
  /// \return the requested character
  NMTL_INLINE reference at(NMTL_INDEX size_type _Off)
  {
    NMTL_UNIMPLEMENTED;
    static value_type v = 0;
    return v;
  }

  /// \brief  returns the string as a C-string
  /// \return the C-string
  NMTL_INLINE const value_type* c_str() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns the capacity of the string
  /// \return the string capacity
  NMTL_INLINE size_type capacity() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  empties the string
  NMTL_INLINE void clear()
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  does a case sensitive comparison with the input string
  /// \param  _Str - the input string
  /// \return the lexicographical comparison
  NMTL_INLINE int compare(const basic_string<value_type>& _Str) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  does a case sensitive comparison with the input string
  /// \param  _Str - the input string
  /// \return the lexicographical comparison
  NMTL_INLINE int compare(NMTL_INDEX size_type _Pos1, size_type _Num1, const basic_string<value_type>& _Str) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  does a case sensitive comparison with the input string
  /// \param  _Str - the input string
  /// \return the lexicographical comparison
  NMTL_INLINE int compare(NMTL_INDEX size_type _Pos1, size_type _Num1, const basic_string<value_type>& _Str, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  does a case sensitive comparison with the input string
  /// \param  _Str - the input string
  /// \return the lexicographical comparison
  NMTL_INLINE int compare(const value_type* _Ptr) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  does a case sensitive comparison with the input string
  /// \param  _Str - the input string
  /// \return the lexicographical comparison
  NMTL_INLINE int compare(NMTL_INDEX size_type _Pos1, size_type _Num1, const value_type* _Ptr) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  does a case sensitive comparison with the input string
  /// \param  _Str - the input string
  /// \return the lexicographical comparison
  NMTL_INLINE int compare(NMTL_INDEX size_type _Pos1, size_type _Num1, const value_type* _Ptr, size_type _Num2) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  copies the string data into this string
  /// \param  _Ptr - the string data
  /// \param  _Count - the number of chars to copy
  /// \param  _Off - the offset to copy
  NMTL_INLINE size_type copy(value_type* _Ptr, size_type _Count, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns the internal data buffer
  /// \return the string data
  NMTL_INLINE const value_type* data() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns true if the string is empty
  /// \return true if the string is empty
  NMTL_INLINE bool empty() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  erases the string data between the specified range
  /// \param  _Firt - start of the string data to erase
  /// \param  _Last - end of the string data to erase
  NMTL_INLINE iterator erase(iterator _First, iterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  erases the specified element
  /// \param  _It - the element to erase
  /// \return the new iterator location
  NMTL_INLINE iterator erase(iterator _It)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  erases the specified number of chars from the specified location
  /// \param  _Pos - the position to erase from
  /// \param  _Count - the number of elements to erase
  NMTL_INLINE basic_string<value_type>& erase(NMTL_INDEX size_type _Pos = 0, size_type _Count = npos)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a forward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  /// \param  _Ch - the character to locate
  /// \param  _Off - the position to start searching from
  /// \return The index of the first character of the substring searched for when successful;
  ///         otherwise npos.
  NMTL_INLINE size_type find(value_type _Ch, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a forward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  /// \param  _Ptr - the string to locate
  /// \param  _Off - the offset to start the search
  /// \return The index of the first character of the substring searched for when successful;
  ///         otherwise npos.
  NMTL_INLINE size_type find(const value_type* _Ptr, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a forward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  /// \param  _Ptr - the string to locate
  /// \param  _Off - the offset to start the search
  /// \param  _Count - the number of elements to find
  /// \return The index of the first character of the substring searched for when successful;
  ///         otherwise npos.
  NMTL_INLINE size_type find(const value_type* _Ptr, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a forward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  /// \param  _Str - the string to locate
  /// \param  _Off - the position to start searching from
  /// \return The index of the first character of the substring searched for when successful;
  ///         otherwise npos.
  NMTL_INLINE size_type find(const basic_string<value_type>& _Str, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is not an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_not_of(value_type _Ch, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is not an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_not_of(const value_type* _Ptr, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is not an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_not_of(const value_type* _Ptr, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is not an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_not_of(const basic_string<value_type>& _Str, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_of(value_type _Ch, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_of(const value_type* _Ptr, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_of(const value_type* _Ptr, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the first character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character to find
  /// \param  _Off - the characters offset
  /// \return the location
  NMTL_INLINE size_type find_first_of(const basic_string<value_type>& _Str, size_type _Off = 0) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is not any element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_not_of(value_type _Ch, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is not any element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_not_of(const value_type* _Ptr, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is not any element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_not_of(const value_type* _Ptr, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is not any element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_not_of(const basic_string<value_type>& _Str, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_of(value_type _Ch, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_of(const value_type* _Ptr, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_of(const value_type* _Ptr, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches through a string for the last character that is an element of
  ///         a specified string.
  /// \param  _Ch - the character
  /// \param  _Off - the start of the search location
  /// \return the location
  NMTL_INLINE size_type find_last_of(const basic_string<value_type>& _Str, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE basic_string<value_type>& insert(NMTL_INDEX size_type _P0, const value_type* _Ptr)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE basic_string<value_type>& insert(NMTL_INDEX size_type _P0, const value_type* _Ptr, size_type _Count)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE basic_string<value_type>& insert(NMTL_INDEX size_type _P0, const basic_string<value_type>& _Str)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE basic_string<value_type>& insert(NMTL_INDEX size_type _P0, const basic_string<value_type>& _Str, size_type _Off, size_type _Count)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE basic_string<value_type>& insert(NMTL_INDEX size_type _P0, size_type _Count, value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE iterator insert(iterator _It)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE iterator insert(iterator _It, value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  template<typename InputIterator>
  NMTL_INLINE void insert(iterator _It, InputIterator _First, InputIterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE void insert(iterator _It, size_type _Count, value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE void insert(iterator _It, const_pointer _First, const_pointer _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  inserts the specified string into the specified string location
  /// \param  _P0 - the position to insert the string
  /// \param  _Ptr - the string data
  /// \return *this
  NMTL_INLINE void insert(iterator _It, const_iterator _First, const_iterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns the length of the string
  /// \return the str length
  NMTL_INLINE size_type length() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns the capacity for the string
  /// \return the current capacity
  NMTL_INLINE size_type max_size() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  adds an element to the back of the string
  NMTL_INLINE void push_back(value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(NMTL_INDEX size_type _Pos1, size_type _Num1, const value_type* _Ptr)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(NMTL_INDEX size_type _Pos1, size_type _Num1, const basic_string<value_type>& _Str)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(NMTL_INDEX size_type _Pos1, size_type _Num1, const value_type* _Ptr, size_type _Num2)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(NMTL_INDEX size_type _Pos1, size_type _Num1, const basic_string<value_type>& _Str, size_type _Pos2, size_type _Num2)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(NMTL_INDEX size_type _Pos1, size_type _Num1, size_type _Count, value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, const value_type* _Ptr)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, const basic_string<value_type>& _Str)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, const value_type* _Ptr, size_type _Num2)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, size_type _Num2, value_type _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  template<typename InputIterator>
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, InputIterator _First, InputIterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, const_pointer _First, const_pointer _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  replaces a set of characters within the string
  /// \param  _Pos1 -
  NMTL_INLINE basic_string<value_type>& replace(iterator _First0, iterator _Last0, const_iterator _First, const_iterator _Last)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  reserves a specific amount of capacity
  NMTL_INLINE void reserve(size_type _Count = 0)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  resizes the string
  NMTL_INLINE void resize(size_type _Count)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }
  /// \brief  resizes the string
  NMTL_INLINE void resize(size_type _Count, const value_type& _Ch)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a backward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  NMTL_INLINE NMTL_INDEX size_type rfind(value_type _Ch, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a backward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  NMTL_INLINE NMTL_INDEX size_type rfind(const value_type* _Ptr, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a backward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  NMTL_INLINE NMTL_INDEX size_type rfind(const value_type* _Ptr, size_type _Off, size_type _Count) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  Searches a string in a backward direction for the first occurrence of a
  ///         substring that matches a specified sequence of characters.
  NMTL_INLINE NMTL_INDEX size_type rfind(const basic_string<value_type>& _Str, size_type _Off = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns the size of the string
  NMTL_INLINE size_type size() const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  returns a substring
  NMTL_INLINE basic_string<value_type> substr(size_type _Off = 0, size_type _Count = npos) const
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

  /// \brief  swaps the two strings
  NMTL_INLINE void swap(basic_string<value_type>& _Str)
  {
    NMTL_UNIMPLEMENTED;
    return *this;
  }

};

/// \brief  specialised for char
typedef basic_string<char> string;

/// \brief  specialised for wchar_t
typedef basic_string<wchar_t> wstring;

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_STRING_H
