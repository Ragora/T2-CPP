#pragma once

namespace DX
{
	class SimObject
	{
	public:
		SimObject(unsigned int obj);

		void deleteObject(void);
		const char *CallMethod(const char *name, unsigned int argc, ...);
		const char *getFieldValue(const char *slotname);
		void setDataField(const char *slotname, const char *array, const char *value);
		const unsigned int &fieldDictionary;
		const unsigned int &identifier;
		const unsigned int base_pointer_value;
		const unsigned int &dataBlock;
	};
} // End NameSpace DX