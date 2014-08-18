#pragma once

namespace DX
{
	class SimObject
	{
	public:
		SimObject(unsigned int obj);

		void deleteObject(void);

	protected:
		const unsigned int base_pointer_value;
	};
} // End NameSpace DX