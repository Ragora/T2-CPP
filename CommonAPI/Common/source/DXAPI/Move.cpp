#include <algorithm>
#include <DXAPI/Move.h>
#define M_PI 3.14159265358979323846
#define M_2PI 6.28318530717958647692528676655900576
namespace DX
{
	 void generateNullMove(Move * ret) {
	DX::Move nullmove = {
	16,
	16,
	16,

	0,
	0,
	0,

	0,
	0,
	0,

	0,
	0,
	0,

	0,
	0,
	false,false,false,false,false,false


};
		ret = &nullmove;
	}
	float clamp(float in, float min, float max) {
		if (in < min) {
			return min;
		} else if (in > max) {
			return max;
		}
		return in;
	}
	float clip(float n, float lower, float upper) {
		return std::max(lower, std::min(n, upper));
	}	
	float clampFloat(float in){
		return clip(in,-1.0f,1.0f);
	}

	float clampMove(float in) {
		float tmpfloat = clamp(in,-M_PI,M_PI);
		if (tmpfloat < 0.0) {
			return tmpfloat + M_2PI;
		} else {
			return tmpfloat;
		}
	}
}