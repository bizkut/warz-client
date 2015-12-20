/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: JAPA
#ifndef Navigation_BaseSystem_H
#define Navigation_BaseSystem_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_System.h"
#include "gwnavruntime/kernel/SF_Log.h"
#include "gwnavruntime/basesystem/gamewarekey.h"
#include "gwnavruntime/basesystem/defaultlog.h"
#include "gwnavruntime/basesystem/checkheaderlibdesync.h"

namespace Kaim
{

class PerfCounterManager;
class IPerfMarkerInterface;

/// The BaseSystemConfig class contains a set of objects and configuration parameters that are required in order
/// to initialize the BaseSystem. The information maintained by the BaseSystemConfig determines how the BaseSystem
/// carries out basic operations common to many Gameware Navigation runtime components, such as allocating memory,
/// logging messages, etc.
/// When you initialize Gameware Navigation for use in your game, you typically create an instance of BaseSystemConfig
/// on the stack, and customize the behavior of the subsystems managed by the BaseSystem by modifying the class
/// members of the BaseSystemConfig.
/// Depending on the kind of sub-system you intend to customize, you may have to write your own derivation of a class
/// supplied in the SDK, create a new instance of your class on the heap, and/or set the relevant member of the
/// BaseSystemConfig to point to your new object. NOTE: the BaseSystem does not assume the responsibility for
/// the lifespan of the object. You must ensure the pointer remains valid until the BaseSystem is terminated, and
/// you must destroy the object after it is no longer needed.
/// For example, to customize the message logging system, you need to write your own implementation of the BaseLog class,
/// create an instance of your class, and set the #m_log member to point to your new instance.
/// When you have set up the BaseSystemConfig with all the customizations that you need, pass it in a call to
/// the BaseSystem::Init() method.
/// Once the BaseSystem has been successfully initialized, you can destroy the BaseSystemConfig or let it pass out of
/// scope.
class BaseSystemConfig
{
public:
	BaseSystemConfig()
		: m_defaultLogToMode(DefaultLog::LogToAll)
		, m_log(KY_NULL)
		, m_perfMarkerInterface(KY_NULL)
		, m_sysAlloc(KY_NULL)
		, m_rootHeapDesc(KY_NULL)
	{}

public:
	/// Provides the gameware key.
	GamewareKeyConfig m_gamewareKeyConfig;

	/// Specifies the output locations in which the BaseSystem will print debug messages when using its default Log: that is,
	/// when you do not override #m_log with a custom Log object. If you do override #m_log, this value is not used. 
	DefaultLog::LogToMode m_defaultLogToMode;

	/// Provides an instance of class that derives from Log, which Gameware Navigation components will invoke in order to
	/// log debug messages, warnings and errors.
	/// If you do not customize this pointer, the BaseSystem will create and use an instance of the DefaultLog class
	/// internally, which provides a basic default set of logging outputs. 
	BaseLog* m_log; 

	/// Provides an instance of a custom class that implements IPerfMarkerInterface. Gameware Navigation components notify
	/// this object of counters and events; you can forward those notifications to a performance profiling system.
	/// If you do not customize this pointer, you will not be able to retrieve performance profiling data. 
	IPerfMarkerInterface* m_perfMarkerInterface;

	// memory allocation
	/// Provides an instance of a class that implements the SysAlloc interface, which will be invoked any time a Gameware Navigation
	/// component needs to allocate, free or re-allocate memory. To customize Gameware Navigation memory management, write your own
	/// class that derives from SysAlloc, create an instance of your class on the heap, and set this member to point to your
	/// instance.
	/// If you do not customize this pointer, the BaseSystem will use an instance of the default SysAllocMalloc class. See
	/// the HeapMH_SysAllocMalloc.h file for its platform-specific implementations. 
	SysAllocBase* m_sysAlloc;
	MemoryHeap::HeapDesc* m_rootHeapDesc;
};


/// The BaseSystem provides Gameware Navigation components with a set of objects used for memory allocation, logging,
/// performance profiling, etc. 
/// Only one BaseSystem can exist at any time. You can customize the default behavior of the BaseSystem by
/// setting up an instance of the BaseSystemConfig class, and passing it in a call to Init(). See the BaseSystemConfig
/// class for details.
/// To release any resources allocated internally by the BaseSystem, call Destroy().
/// There are two ways to use BaseSystem, either using static function Init() and Destroy() or by creating a BaseSystem instance.
/// When creating an instance, Init() is called in the constructor and Destroy() is called in the destructor.
class BaseSystem
{
public:
	typedef BaseSystemConfig Config; ///< Defines a type for the configuration object used to initialize the BaseSystem.

public:
	/// Creates and sets up the new BaseSystem with a default configuration.
	BaseSystem() { Init(Config()); }

	/// Creates and sets up the BaseSystem according to the specified configuration.
	BaseSystem(const Config& config) { Init(config); }

	/// Calls static function BaseSystem::Destroy()
	~BaseSystem() { Destroy(); }

	/// Sets up the BaseSystem according to the specified configuration.
	static void Init(const Config& config)
	{
		CheckHeaderLibDesync::Check(); // Creates "unresolved external symbol" link error in case of failure.
		DoInit(config); // do the "real" init
	}

	/// Indicates whether or not the BaseSystem has been successfully initialized.
	static bool IsInitialized() { return s_isInitialized; }

	/// Indicates whether or not any memory allocated by the BaseSystem remains to be freed.
	/// Call this method only after you destroy the BaseSystem. 
	static bool HasMemoryLeaks() { return System::hasMemoryLeaks; }

	/// Destroys the BaseSystem.
	static void Destroy();

private:
	static void DoInit(const Config& config);
	static void DestroyMemorySystemAndLog();
	static void RegisterBlobs();

private:
	static bool s_isInitialized;
};

// Wii U stuff
#define KY_INTERNAL_PLACEHOLDER_BASESYSTEM_H

} // namespace Kaim


#endif
