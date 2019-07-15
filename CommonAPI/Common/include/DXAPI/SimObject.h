#pragma once

#define MEMBER_POINTER(obj, type, offset) (type*)(obj + offset)
#define MEMBER_FIELD(obj, type, offset) *(type*)(obj + offset)
#define MEMBER_POINT3F(obj, offset, spacing) MEMBER_FIELD(obj, float, offset), MEMBER_FIELD(obj, float, offset + spacing), MEMBER_FIELD(obj, float, offset + (spacing * 2))

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

		char& mName;
	};
} // End NameSpace DX