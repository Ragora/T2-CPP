#pragma once

#include <DXAPI/SceneObject.h>
   enum GameBaseMasks {
      InitialUpdateMask =     1 << 0,
      DataBlockMask =         1 << 1,
      ExtendedInfoMask =      1 << 2,
      NextFreeMask =          ExtendedInfoMask << 1
   };
namespace DX
{
	class GameBase : public SceneObject
	{

	public:
		GameBase(unsigned int obj);
		void setProcessTicks(bool proc);
	};
} // End NameSpace DX
