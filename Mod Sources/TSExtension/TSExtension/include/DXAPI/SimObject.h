#pragma once

namespace DX
{
	class SimObject
	{
	public:
		SimObject(unsigned int obj);

		void deleteObject(void);
		const char *SimObject::getClassName();
		const char *CallMethod(const char *name, unsigned int argc, ...);
		const unsigned int &type;
		const unsigned int &identifier;
		const unsigned int base_pointer_value;
	};
} // End NameSpace DX