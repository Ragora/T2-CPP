#include <DXAPI/MatMath.h>

namespace DX
{
	Point3F::Point3F(float &X, float &Y, float &Z) : x(X), y(Y), z(Z)
	{
	}
	Point3F::Point3F()
	{
		x=0;
		y=0;
		z=0;
	}
}
