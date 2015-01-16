#pragma once

#include <DXAPI/Point3F.h>
#include <DXAPI/SimObject.h>
#include <DXAPI/NetObject.h>
#include <LinkerAPI.h>
struct GhostInfo;
namespace DX
{
	class NetConnection : public SimObject
	{
	public:
		NetConnection(unsigned int obj);
		S32 getGhostIndex(NetObject obj);
		unsigned char getGhostFrom();
		unsigned char getGhostTo();
		NetObject resolveGhostParent(S32 id);
		NetObject resolveGhost(S32 id);
		unsigned int actualbaseptr;
		GhostInfo * mGhostRefs;
		NetObject **mLocalGhosts;
	};
} // End NameSpace DX
struct GhostRef;
struct GhostInfo
{
 //  public:  // required for MSVC

   // NOTE:
   // if the size of this structure changes, the
   // NetConnection::getGhostIndex function MUST be changed 
   // to reflect.
   
   U32 *obj;            // the real object
   U32 updateMask;         // 32 bits of object info
   GhostRef *updateChain;     // chain of updates for this object in packets
   GhostInfo *nextObjectRef;  // next ghost ref for this object (another connection)

   GhostInfo *prevObjectRef;  // prev ghost ref for this object
   U32 *connection;
   GhostInfo *nextLookupInfo;
   U32 updateSkipCount;
   
   U32 flags;
   F32 priority;
   U32 index;
   U32 arrayIndex;

	enum Flags
	{
		Valid = BIT(0),
		InScope = BIT(1),
      ScopeAlways = BIT(2),
      NotYetGhosted = BIT(3),
      Ghosting = BIT(4),
      KillGhost = BIT(5),
      KillingGhost = BIT(6),
      ScopedEvent = BIT(7),
      ScopeLocalAlways = BIT(8),
	};
};
	struct GhostRef
   {
      U32 mask;
      U32 ghostInfoFlags;
      GhostInfo *ghost;
      GhostRef *nextRef;
      GhostRef *nextUpdateChain;
   };
