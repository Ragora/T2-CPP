/**
 *	@brief ModLoader main include file.
 */

namespace ModLoader 
{
	//! A pointer type for a server process function.
	typedef void (*ServerProcessPointer)(unsigned int);

	typedef void (*InitializeModPointer)();

	typedef void (*DeinitializeModPointer)();

	typedef const char* (*GetManagementNamePointer)();

	//! A pointer type for reading the supported mod loader version for this mod.
	typedef unsigned int (*GetModLoaderVersionPointer)();

	class ModLoaderCallables
	{
		public:
			//! Hook function that is called when the mod is loaded.
			InitializeModPointer mInitializeModPointer;

			//! Hook function that is called when the mod is unloaded.
			DeinitializeModPointer mDeinitializeModPointer;

			//! Hook function for the server update loop.
			ServerProcessPointer mServerProcessPointer;
			
			//! Function called to get the name of the script object to use for registering mod management functions.
			GetManagementNamePointer mGetManagementName;
	};

	/**
	 *	@brief Returns the hook information from the loaded mod.
	 */
	typedef ModLoaderCallables* (*GetModCallablesPointer)();
}