#pragma once
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#define M_PI 3.14159265359
inline float mDegToRad(float d)
{
   return float((d * M_PI) / float(180));
}

inline float mRadToDeg(float r)
{
   return float((r * 180.0) / M_PI);
}

inline double mDegToRad(double d)
{
   return (d * M_PI) / double(180);
}

inline double mRadToDeg(double r)
{
   return (r * 180.0) / M_PI;
}

namespace DX
{

	struct Point {
	float x;
	float y;
	float z;
	};
	/**
	 *	@brief A class representing a referenced 3D vector of floats in Tribes 2.
	 */
	#define idx(r,c) (r*4 + c)
	class MatrixF;
	class Point3F
	{
	public:
		Point3F();
		Point3F(float &X, float &Y, float &Z);
		float x;
		float y;
		float z;
	};
	class Angle
	{
	public:
		Angle(float *mat){
		   float const *m = (float const *)mat;

		   float trace = m[idx(0, 0)] + m[idx(1, 1)] + m[idx(2, 2)];
		   if (trace > 0.0) {
			  float s = sqrt(trace + float(1));
			  w = s * 0.5;
			  s = 0.5 / s;
			  x = (m[idx(1,2)] - m[idx(2,1)]) * s;
			  y = (m[idx(2,0)] - m[idx(0,2)]) * s;
			  z = (m[idx(0,1)] - m[idx(1,0)]) * s;
		   } else {
			  float* q = &x;
			  unsigned int i = 0;
			  if (m[idx(1, 1)] > m[idx(0, 0)]) i = 1;
			  if (m[idx(2, 2)] > m[idx(i, i)]) i = 2;
			  unsigned int j = (i + 1) % 3;
			  unsigned int k = (j + 1) % 3;

			  float s = sqrt((m[idx(i, i)] - (m[idx(j, j)] + m[idx(k, k)])) + 1.0);
			  q[i] = s * 0.5;
			  s = 0.5 / s;
			  q[j] = (m[idx(i,j)] + m[idx(j,i)]) * s;
			  q[k] = (m[idx(i,k)] + m[idx(k, i)]) * s;
			  w = (m[idx(j,k)] - m[idx(k, j)]) * s;



			     angle = acos( w ) * 2;
   float sinHalfAngle = sqrt(1 - w * w);
   if (sinHalfAngle != 0)
   {
	    axis.x=(x / sinHalfAngle);
		axis.y=(y / sinHalfAngle);
		axis.z=(z / sinHalfAngle);
   }
   else
   {
      axis.x=1.0;
	  axis.y=0;
	  axis.z=0;
   }

	   }
	}
	void Angle::identity()
	{ 
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}
	void Angle::normalize() {
	   float l = sqrt( x*x + y*y + z*z + w*w );
		if( l == float(0.0) )
			identity();
		else
		{
			x /= l;
			y /= l;
			z /= l;
			w /= l;
		}
	}
	float angle;
	Point axis;
	float x,y,z,w,s;

	};
	class MatrixF
	{
	public:
		MatrixF(float *inm) {
			m=inm;
		}
		float *m;
		void MatrixF::m_quatF_set_matF_C( float x, float y, float z, float w)
		{
			#define qidx(r,c) (r*4 + c)
				  float xs = x * 2.0f;
				  float ys = y * 2.0f;
				  float zs = z * 2.0f;
				  float wx = w * xs;
				  float wy = w * ys;
				  float wz = w * zs;
				  float xx = x * xs;
				  float xy = x * ys;
				  float xz = x * zs;
				  float yy = y * ys;
				  float yz = y * zs;
				  float zz = z * zs;
				  m[qidx(0,0)] = 1.0f - (yy + zz);
				  m[qidx(1,0)] = xy - wz;
				  m[qidx(2,0)] = xz + wy;
				  m[qidx(3,0)] = 0.0f;
				  m[qidx(0,1)] = xy + wz;
				  m[qidx(1,1)] = 1.0f - (xx + zz);
				  m[qidx(2,1)] = yz - wx;
				  m[qidx(3,1)] = 0.0f;
				  m[qidx(0,2)] = xz - wy;
				  m[qidx(1,2)] = yz + wx;
				  m[qidx(2,2)] = 1.0f - (xx + yy);
				  m[qidx(3,2)] = 0.0f;

				  m[qidx(0,3)] = 0.0f;
				  m[qidx(1,3)] = 0.0f;
				  m[qidx(2,3)] = 0.0f;
				  m[qidx(3,3)] = 1.0f;
			#undef qidx
		}
		inline void getColumn(int col, Point3F *cptr)
			{
			   cptr->x = m[col];
			   cptr->y = m[col+4];
			   cptr->z = m[col+8];
			}


		inline void setColumn(int col, Point3F *cptr)
		{
		   m[col]   = cptr->x;
		   m[col+4] = cptr->y; 
		   m[col+8] = cptr->z; 
		}
		MatrixF(Angle *ang) {
			m_quatF_set_matF_C(ang->x,ang->y,ang->z,ang->w);
		}
	};

} // End NameSpace DX
