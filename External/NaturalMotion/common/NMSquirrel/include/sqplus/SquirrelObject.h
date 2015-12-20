//----------------------------------------------------------------------------------------------------------------------
/// \file SquirrelObject.h
/// Based on the SquirrelObject class written by Alberto Demichelis in his DXSquirrel example bindings
/// and the further work of John Schultz in sqp.
/// \author Alberto Demichelis
//----------------------------------------------------------------------------------------------------------------------
#ifndef _SQPLUS_SQUIRRELOBJECT_H_
#define _SQPLUS_SQUIRRELOBJECT_H_

#include <sqplus/Interface.h>
#include <squirrel/squirrel.h>

namespace sqp
{
class SQPLUS_API ClassTypeBase;

//----------------------------------------------------------------------------------------------------------------------
/// \brief wrapper object for manipulating some of the more complex squirrel types, table, arrays or class instances
//----------------------------------------------------------------------------------------------------------------------
class SQPLUS_API SquirrelObject
{
  friend class SquirrelVM;
public:
  /// \brief default constructor
  SquirrelObject();

  /// \brief Constructor initialises with a vm
  SquirrelObject(HSQUIRRELVM vm);

  /// \brief Copy constructor maintains another reference to the same object as the rhs one.
  SquirrelObject(const SquirrelObject& rhs);

  /// \brief Destructor removes reference to the HSQOBJECT. 
  ~SquirrelObject();

  /// \brief Releases any existing reference, sets this HSQOBJECT to the assigning object's
  /// side HSQOBJECT and then adds a reference to it with this SquirrelObject's associated 
  /// \link #HSQUIRRELVM vm\endlink. If this object has no vm then the vm of the assigning
  /// object on the right of the equals operator is used.
  const SquirrelObject& operator = (const SquirrelObject& rhs);

  /// \brief Attaches this object to an object on the stack at the location specified by idx.
  void AttachToStackObject(SQInteger idx, SQRESULT *pResult = 0);

  /// \brief Attaches this object to an object on the stack at the location specified by idx.
  void AttachToStackObject(HSQUIRRELVM vm, SQInteger idx, SQRESULT *pResult = 0);

  /// \brief Clones the array, class instance or table the object references.
  SquirrelObject Clone() const;

  HSQUIRRELVM GetVMPtr() const;

  /// \brief Gets the handle to the underlying squirrel object.
  HSQOBJECT GetHandle() const;

  /// \brief Checks if the object is null.
  bool IsNull() const;

  /// \brief Checks if the object is a float or an integer.
  bool IsNumber() const;

  /// \brief Checks if the object is an instance of a particular bound class type.
  /// \code
  /// SquirrelObject obj;
  /// ...
  /// if (obj.IsInstanceType<MyClass>())
  /// {
  ///   MyClass *ptr = obj.ToInstanceNoTypeCheck<MyClass>();
  /// }
  /// \endcode
  /// \note MyClass must already be bound using DECLARE_INSTANCE_TYPE(MyClass); and sqp::ClassDef<MyClass>.
  template<typename T>
  bool IsInstanceType() const;

  /// \brief Returns the \link #SQObjectType type\endlink of the object.
  SQObjectType GetType() const;

  /// \brief Returns the size of the object depending on what type it is. The length of an array, 
  /// the number of slots in a table, the length of a string or the size of userdata.
  SQInteger GetSize() const;

  /// \brief Returns the object as a bool.
  bool ToBool();

  /// \brief Returns the object as an integer.
  SQInteger ToInteger();

  /// \brief Returns the object as a float.
  SQFloat ToFloat();

  /// \brief Returns the object as a string.
  const SQChar *ToString();

  /// \brief Returns the object as a specifically bound class type, if the object is not an instance
  /// of the desired type it will return null.
  template<typename T>
  T *ToInstance();

  /// \brief Returns the object as a specifically bound class type without checking the typetag of
  /// the instance, use in conjunction with IsInstanceType. This function will return null if the
  /// object is not of type OT_INSTANCE but it will also return an invalid pointer to an instance
  /// of type T if the object is of type OT_INSTANCE but not the correct class type.
  template<typename T>
  T *ToInstanceNoTypeCheck();

  /// \brief If the object is an instance of a class, sets the associated user pointer.
  bool SetInstanceUP(SQUserPointer instance);

  /// \brief If the object is an instance of a class, gets the associated user pointer. The optional
  /// typetag argument is used to check if the instance is of the desired type, if not the operation
  /// fails.
  SQUserPointer GetInstanceUP(SQUserPointer typetag = 0);

  bool NewUserData(const SQChar *key, SQUnsignedInteger size, SQUserPointer *typetag = 0);
  bool GetUserData(const SQChar *key, SQUserPointer *data, SQUserPointer *typetag = 0);
  bool RawGetUserData(const SQChar *key, SQUserPointer *data, SQUserPointer *typetag = 0);

  SQRESULT GetTypeTag(SQUserPointer *typeTag) const;

  const SQChar *GetTypeName(const SQChar *key) const;
  const SQChar *GetTypeName(SQInteger key) const;
  const SQChar *GetTypeName() const;

  /// \brief If the object is an array, appends a boolean to value to the end.
  bool ArrayAppend(bool value);
  bool ArrayAppend(SQInteger value);
  bool ArrayAppend(SQFloat value);
  bool ArrayAppend(const SQChar *value);
  bool ArrayAppend(const SquirrelObject& value);

  SquirrelObject ArrayPop(bool retval);
  bool ArrayResize(SQInteger size);
  bool ArrayReverse();

  bool BeginIteration();
  bool Next(SquirrelObject &key, SquirrelObject &value);
  void EndIteration();

  bool SetDelegate(SquirrelObject& object);
  SquirrelObject GetDelegate();

  bool Exists(SQInteger key) const;
  bool Exists(const SQChar *key) const;
  bool Exists(const SquirrelObject& key) const;

  SquirrelObject CreateTable(SQInteger key);
  SquirrelObject CreateTable(const SQChar *key);
  SquirrelObject CreateTable(const SquirrelObject& key);

  bool SetValue(SQInteger key, bool value);
  bool SetValue(SQInteger key, SQInteger value);
  bool SetValue(SQInteger key, SQFloat value);
  bool SetValue(SQInteger key, const SQChar *value);
  bool SetValue(SQInteger key, const SquirrelObject& value);
  template<typename T>
  bool SetValue(SQInteger key, const T *value);
  template<typename T>
  bool SetValue(SQInteger key, const T &value);

  bool SetValue(const SQChar *key, bool value);
  bool SetValue(const SQChar *key, SQInteger value);
  bool SetValue(const SQChar *key, SQFloat value);
  bool SetValue(const SQChar *key, const SQChar *value);
  bool SetValue(const SQChar *key, const SquirrelObject& value);
  template<typename T>
  bool SetValue(const SQChar *key, const T *value);
  template<typename T>
  bool SetValue(const SQChar *key, const T &value);
  
  bool SetValue(const SquirrelObject& key, bool value);
  bool SetValue(const SquirrelObject& key, SQInteger value);
  bool SetValue(const SquirrelObject& key, SQFloat value);
  bool SetValue(const SquirrelObject& key, const SQChar *value);
  bool SetValue(const SquirrelObject& key, const SquirrelObject& value);
  template<typename T>
  bool SetValue(const SquirrelObject& key, const T *value);
  template<typename T>
  bool SetValue(const SquirrelObject& key, const T &value);

  bool GetBool(SQInteger key) const;
  int32_t GetInteger32(SQInteger key) const;
  SQInteger GetInteger(SQInteger key) const;
  SQFloat GetFloat(SQInteger key) const;
  const SQChar *GetString(SQInteger key) const;
  SquirrelObject GetValue(SQInteger key) const;

  bool GetBool(const SQChar *key) const;
  int32_t GetInteger32(const SQChar *key) const;
  SQInteger GetInteger(const SQChar *key) const;
  SQFloat GetFloat(const SQChar *key) const;
  const SQChar *GetString(const SQChar *key) const;
  SquirrelObject GetValue(const SQChar *key) const;

  bool GetBool(const SquirrelObject& key) const;
  int32_t GetInteger32(const SquirrelObject& key) const;
  SQInteger GetInteger(const SquirrelObject& key) const;
  SQFloat GetFloat(const SquirrelObject& key) const;
  const SQChar *GetString(const SquirrelObject& key) const;
  SquirrelObject GetValue(const SquirrelObject& key) const;

private:
  HSQUIRRELVM  m_vm;
  HSQOBJECT    m_obj;

private:

  bool GetSlot(const SQInteger key) const;
  bool GetSlot(const SQChar *key) const;
  bool GetSlot(const SquirrelObject& key) const;
  bool RawGetSlot(const SQInteger key) const;
  bool RawGetSlot(const SQChar *key) const;
  bool RawGetSlot(const SquirrelObject& key) const;
};

//----------------------------------------------------------------------------------------------------------------------
}

#include "sqplus/SquirrelObject.inl"

#endif // _SQPLUS_SQUIRRELOBJECT_H_
