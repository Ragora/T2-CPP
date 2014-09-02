#pragma once

namespace DX
{
	class SimObject
	{
	public:
		SimObject(unsigned int obj);

		void deleteObject(void);
		const char *TSCall(const char *name, unsigned int argc, ...);

		const unsigned int &identifier;
		const unsigned int base_pointer_value;
	};
} // End NameSpace DX