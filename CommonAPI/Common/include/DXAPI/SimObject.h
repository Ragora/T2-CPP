#pragma once

#define MEMBER_POINTER(obj, type, offset) (type*)(obj + offset)
#define MEMBER_FIELD(obj, type, offset) *(type*)(obj + offset)
#define MEMBER_POINT3F(obj, offset, spacing) MEMBER_FIELD(obj, float, offset), MEMBER_FIELD(obj, float, offset + spacing), MEMBER_FIELD(obj, float, offset + (spacing * 2))

namespace DX
{
	//! All type masks used in the engine.
	enum class TypeMasks : unsigned int
	{
		StaticObjectType = 1,
		EnvironmentObjectType = 2,
		TerrainObjectType = 4,
		InteriorObjectType = 8,
		WaterObjectType = 0x10,
		TriggerObjectType = 0x40,
		MarkerObjectType = 0x40,
		ForceFieldObjectType = 0x100,
		GameBaseObjectType = 0x400,
		ShapeBaseObjectType = 0x800,
		CameraObjectType = 0x1000,
		StaticShapeObjectType = 0x2000,
		PlayerObjectType = 0x4000,
		ItemObjectType = 0x8000,
		VehicleObjectType = 0x10000,
		VehicleBlockerObject = 0x20000,
		ProjectileObjectType = 0x40000,
		ExplosionObjectType = 0x80000,
		CorpseObjectType = 0x100000,
		TurretObjectType = 0x200000,
		DebrisObjectType = 0x400000,
		PhysicalZoneObjectType = 0x800000,
		StatocTSObjectType = 0x1000000,
		GuiControlObjectType = 0x2000000,
		StaticRenderedObjectType = 0x4000000,
		DamagableItemObjectType = 0x8000000,
		SensorObjectType = 0x10000000,
		StationObjectType = 0x20000000,
		GeneratorObjectType = 0x40000000
	};

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
		const TypeMasks &type;

		char& mName;
	};
} // End NameSpace DX