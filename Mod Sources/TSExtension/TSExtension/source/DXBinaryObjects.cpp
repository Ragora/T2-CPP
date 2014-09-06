/**
 */

#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>

#include <map>

typedef struct
{
	char *buffer;
	int buffer_length;
	int buffer_pointer;
	bool disk_streaming;
	FILE *handle;
} DXBinaryObject;

typedef std::map<unsigned int, DXBinaryObject*> DXBinaryObjectMapping;
static DXBinaryObjectMapping DXBinaryObjectMap;

bool conBinaryObjectOpenForRead(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// By default we don't do disk streaming but this allows for the support
	bool should_stream = false;
	if (argc == 4)
	{
		int stream_value = atoi(argv[3]);
		if (stream_value != 0)
			should_stream = true;
	}

	DX::ScriptObject scriptobject((unsigned int)obj);

	char filepath[256];
	if (!DX::GetRelativePath(argv[2], filepath, 256))
	{
		scriptobject.CallMethod("onOpenFailed", 0);
		return false;
	}
	
	// Already open
	if (DXBinaryObjectMap.count(scriptobject.identifier) != 0)
	{
		// TODO: Make these report the actual errors
		scriptobject.CallMethod("onOpenFailed", 0);
		return false;
	}

	FILE *handle = fopen(filepath, "rb");
	if (!handle)
	{
		Con::errorf(0, "Failed to open: %s", filepath);
		scriptobject.CallMethod("onOpenFailed", 0);
		return false;
	}
	fseek(handle, 0, SEEK_END);

	// Init the Binobj
	DXBinaryObject *binobj = new DXBinaryObject;
	binobj->disk_streaming = should_stream;
	binobj->buffer_length = ftell(handle);
	binobj->buffer_pointer = binobj->buffer_length;
	binobj->handle = handle;

	if (!should_stream)
		binobj->buffer = new char[binobj->buffer_length];

	// We don't need the file object anymore after we've read the buffer
	// so we ditch it.
	fseek(handle, 0, SEEK_SET);

	if (!should_stream)
	{
		fread(binobj->buffer, binobj->buffer_length, 1, handle);
		fclose(handle);
	}

	DXBinaryObjectMap[scriptobject.identifier] = binobj;
	return true;
}

const char *conBinaryObjectReadU32(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::ScriptObject scriptobject((unsigned int)obj);

	// Not open
	if (DXBinaryObjectMap.count(scriptobject.identifier) == 0)
	{
		scriptobject.CallMethod("onReadFailed", 0);
		return "-1";
	}

	DXBinaryObject *binobj = DXBinaryObjectMap[scriptobject.identifier];

	// Okay, so we're open, now we need to make sure we have enough bytes left in the buffer
	int read_length = sizeof(unsigned int);

	if (binobj->buffer_length - (binobj->buffer_length - binobj->buffer_pointer) < read_length)
	{
		scriptobject.CallMethod("onReadFailed", 0);
		return "-1";
	}

	unsigned int return_value = 0;
	binobj->buffer_pointer -= read_length;
	if (binobj->disk_streaming)
	{
		char out_memory[4];
		fseek(binobj->handle, binobj->buffer_pointer, SEEK_SET);
		fread(out_memory, 4, 1, binobj->handle);
		return_value = *(unsigned int*)out_memory;
	}
	else
		// Get a pointer to the object in the buffer and deref it
		return_value = *(unsigned int*)&binobj->buffer[binobj->buffer_pointer];

	char result[256];
	sprintf_s<256>(result, "%u", return_value);
	return result;
}

const char *conBinaryObjectReadF32(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::ScriptObject scriptobject((unsigned int)obj);

	// Not open
	if (DXBinaryObjectMap.count(scriptobject.identifier) == 0)
	{
		scriptobject.CallMethod("onReadFailed", 0);
		return "-1";
	}

	DXBinaryObject *binobj = DXBinaryObjectMap[scriptobject.identifier];

	// Okay, so we're open, now we need to make sure we have enough bytes left in the buffer
	int read_length = sizeof(float);

	if (binobj->buffer_length - (binobj->buffer_length - binobj->buffer_pointer) < read_length)
	{
		scriptobject.CallMethod("onReadFailed", 0);
		return "-1";
	}

	float return_value = 0;
	binobj->buffer_pointer -= read_length;
	if (binobj->disk_streaming)
	{
		char out_memory[4];
		fseek(binobj->handle, binobj->buffer_pointer, SEEK_SET);
		fread(out_memory, 4, 1, binobj->handle);
		return_value = *(float*)out_memory;
	}
	else
		// Get a pointer to the object in the buffer and deref it
		return_value = *(float*)&binobj->buffer[binobj->buffer_pointer];

	char result[256];
	sprintf_s<256>(result, "%f", return_value);
	return result;
}

const char *conBinaryObjectReadU8(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::ScriptObject scriptobject((unsigned int)obj);

	// Not open
	if (DXBinaryObjectMap.count(scriptobject.identifier) == 0)
	{
		scriptobject.CallMethod("onReadFailed", 0);
		return "-1";
	}

	DXBinaryObject *binobj = DXBinaryObjectMap[scriptobject.identifier];

	// Okay, so we're open, now we need to make sure we have enough bytes left in the buffer
	int read_length = sizeof(unsigned char);

	if (binobj->buffer_length - (binobj->buffer_length - binobj->buffer_pointer) < read_length)
	{
		scriptobject.CallMethod("onReadFailed", 0);
		return "-1";
	}

	unsigned char return_value = 0;
	binobj->buffer_pointer -= read_length;
	if (binobj->disk_streaming)
	{
		char out_memory[1];
		fseek(binobj->handle, binobj->buffer_pointer, SEEK_SET);
		fread(out_memory, 4, 1, binobj->handle);
		return_value = *(unsigned char*)out_memory;
	}
	else
		// Get a pointer to the object in the buffer and deref it
		return_value = *(unsigned char*)&binobj->buffer[binobj->buffer_pointer];

	char result[256];
	sprintf_s<256>(result, "%u", return_value);
	return result;
}

bool conBinaryObjectSetBufferPointer(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// We don't need a full object instance for some of these
	unsigned int identifier = atoi(argv[1]);
	if (DXBinaryObjectMap.count(identifier) == 0)
		return false;

	DXBinaryObject *binobj = DXBinaryObjectMap[identifier];

	int desired_value = atoi(argv[2]);
	if (desired_value < 0 || desired_value > binobj->buffer_length)
		return false;

	binobj->buffer_pointer = desired_value;
	return true;
}

const char *conBinaryObjectGetBufferPointer(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	unsigned int identifier = atoi(argv[1]);
	if (DXBinaryObjectMap.count(identifier) == 0)
		return "-1";

	DXBinaryObject *binobj = DXBinaryObjectMap[identifier];

	char result[256];
	sprintf_s<256>(result, "%u", binobj->buffer_pointer);
	return result;
}

const char *conBinaryObjectGetBufferLength(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	unsigned int identifier = atoi(argv[1]);
	if (DXBinaryObjectMap.count(identifier) == 0)
		return "-1";

	DXBinaryObject *binobj = DXBinaryObjectMap[identifier];

	char result[256];
	sprintf_s<256>(result, "%u", binobj->buffer_length);
	return result;
}

bool conBinaryObjectClose(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	unsigned int identifier = atoi(argv[1]);
	if (DXBinaryObjectMap.count(identifier) == 0)
		return false;

	DXBinaryObject *binobj = DXBinaryObjectMap[identifier];
	if (!binobj->disk_streaming)
		delete binobj->buffer;
	else
		fclose(binobj->handle);

	delete binobj;

	DXBinaryObjectMap.erase(identifier);
	return true;
}

bool conBinaryObjectSave(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	unsigned int identifier = atoi(argv[1]);
	if (DXBinaryObjectMap.count(identifier) == 0)
		return false;

	char runningmod[256];
	DX::GetRunningMod(runningmod, 256);
	int runningmod_length = strlen(runningmod);

	Con::errorf(0, "Got Mod: %s", runningmod);

	return true;

	int filename_length = strlen(argv[2]);
	char *desired_filename = (char*)malloc(filename_length + runningmod_length + 2);
	memcpy(desired_filename, argv[2], filename_length + 1);

	DX::SanitizeFileName(desired_filename, filename_length);
	sprintf(desired_filename, "%s/%s", runningmod, desired_filename);
	//Con::errorf(0, "Wrote to: %s", desired_filename);

	FILE *handle = fopen(desired_filename, "wb");
	DXBinaryObject *binobj = DXBinaryObjectMap[identifier];
	fwrite(binobj->buffer, binobj->buffer_length, 1, handle);
	fclose(handle);

	return true;
}