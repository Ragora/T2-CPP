#pragma once

namespace DX
{
	/**
	 *	@brief A class representing a referenced 3D vector of floats in Tribes 2.
	 */
	class Point3F
	{
	public:
		Point3F(float &X, float &Y, float &Z);

		float &x;
		float &y;
		float &z;
	};
} // End NameSpace DX