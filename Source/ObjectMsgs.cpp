
#include "StdAfx.h"
#include "WeenieObject.h"
#include "PhysicsObj.h"
#include "Monster.h"
#include "Player.h"
#include "Appraisal.h"
#include "PartArray.h"

//Network access.
#include "Client.h"
#include "BinaryWriter.h"
#include "ObjectMsgs.h"
#include "ClientCommands.h"

#define MESSAGE_BEGIN(x)	BinaryWriter *x = new BinaryWriter
#define MESSAGE_END(x)		return x

BinaryWriter *GetWeenieObjData(CWeenieObject *pEntity)
{
	BinaryWriter *pWriter = new BinaryWriter;

	PublicWeenieDesc *pDesc = PublicWeenieDesc::CreateFromWeenie(pEntity);
	pDesc->Pack(pWriter);
	delete pDesc;

	return pWriter;

#if 0
	BinaryWriter OptionalWeenieObjData;
	DWORD dwSections = 0;

	if (pEntity->Container_HasContents())
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_ItemsCapacity;
		OptionalWeenieObjData.Write<BYTE>(pEntity->Container_MaxInventorySlots()); // item capacity

		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_ContainersCapacity;
		OptionalWeenieObjData.Write<BYTE>(pEntity->Container_MaxContainerSlots()); // container capacity
	}

	int value = 0;
	if (pEntity->m_Qualities.InqInt(VALUE_INT, value, FALSE, FALSE))
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_Value;
		OptionalWeenieObjData.Write<DWORD>(value);
	}

	int useability = 0;
	if (pEntity->m_Qualities.InqInt(ITEM_USEABLE_INT, useability))
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_Useability;
		OptionalWeenieObjData.Write<int>(useability);
	}

	double useRadius;
	if (pEntity->m_Qualities.InqFloat(USE_RADIUS_FLOAT, useRadius))
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_UseRadius;
		OptionalWeenieObjData.Write<float>(useRadius);
	}

	if (pEntity->m_TargetType != 0)
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_TargetType;
		OptionalWeenieObjData.Write<DWORD>(pEntity->m_TargetType);
	}

	if (pEntity->m_UIEffects)
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_UIEffects;
		OptionalWeenieObjData.Write<DWORD>(pEntity->m_UIEffects);
	}

	if (pEntity->HasEquipType())
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_CombatUse;
		OptionalWeenieObjData.Write<BYTE>((BYTE)pEntity->GetEquipType());
	}

	if (pEntity->IsContained())
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_ContainerID;
		OptionalWeenieObjData.Write<DWORD>(pEntity->GetContainerID());
	}

	if (pEntity->HasCoverage())
	{
		CBaseItem *pItem = (CBaseItem*)pEntity;
		DWORD dwCoverage1 = pItem->GetCoverage1();
		DWORD dwCoverage2 = pItem->GetCoverage2();
		DWORD dwCoverage3 = pItem->GetCoverage3();

		if (dwCoverage1)
		{
			dwSections |= PublicWeenieDescPackHeader::PWD_Packed_ValidLocations;
			OptionalWeenieObjData.Write<DWORD>(dwCoverage1);
		}
		if (dwCoverage2)
		{
			dwSections |= PublicWeenieDescPackHeader::PWD_Packed_Location;
			OptionalWeenieObjData.Write<DWORD>(dwCoverage2);
		}
		if (dwCoverage3)
		{
			dwSections |= PublicWeenieDescPackHeader::PWD_Packed_Priority;
			OptionalWeenieObjData.Write<DWORD>(dwCoverage3);
		}
	}

	int blipColor = 0;
	if (pEntity->m_Qualities.InqInt(RADARBLIP_COLOR_INT, blipColor))
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_BlipColor;
		OptionalWeenieObjData.Write<BYTE>(blipColor);
	}

	int showRadar;
	if (pEntity->m_Qualities.InqInt(SHOWABLE_ON_RADAR_INT, showRadar))
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_RadarEnum;
		OptionalWeenieObjData.Write<BYTE>((BYTE)showRadar);
	}

	int burden = 0;
	if (pEntity->m_Qualities.InqInt(ENCUMB_VAL_INT, value, FALSE, FALSE))
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_Burden;
		OptionalWeenieObjData.Write<WORD>(burden);
	}

	WORD spell_id = (WORD) pEntity->GetSpellID();
	if (spell_id)
	{
		dwSections |= PublicWeenieDescPackHeader::PWD_Packed_SpellID;
		OptionalWeenieObjData.Write<WORD>(spell_id);
	}

	BinaryWriter *WeenieObjData = new BinaryWriter;
	WeenieObjData->Write<DWORD>(dwSections);
	WeenieObjData->WriteString(pEntity->GetName().c_str());
	WeenieObjData->WritePackedDWORD(pEntity->m_Qualities.id);
	WeenieObjData->Pack_AsDataIDOfKnownType(0x06000000, 0x06000000|pEntity->GetIcon());
	WeenieObjData->Write<DWORD>(pEntity->GetItemType());
	WeenieObjData->Write<DWORD>(pEntity->m_WeenieBitfield);
	WeenieObjData->Align();

	WeenieObjData->Write(&OptionalWeenieObjData);

	return WeenieObjData;
#endif
}

BinaryWriter *GetPhysicsObjData(CWeenieObject *pEntity)
{
	BinaryWriter OptionalPhysicsObjData;
	DWORD dwSections = 0; //0x019803

	if (pEntity->movement_manager)
	{
		dwSections |= PhysicsDescInfo::MOVEMENT;

		BinaryWriter *AnimInfo = pEntity->Animation_GetAnimationInfo();

		OptionalPhysicsObjData.Write<DWORD>(AnimInfo->GetSize());
		OptionalPhysicsObjData.Write(AnimInfo);
		OptionalPhysicsObjData.Write<DWORD>(pEntity->last_move_was_autonomous); // autonomous movement?

		delete AnimInfo;
	}
	else
	{
		dwSections |= PhysicsDescInfo::ANIMFRAME_ID;
		OptionalPhysicsObjData.Write<DWORD>(pEntity->GetPlacementFrameID());
	}

	if (!pEntity->HasOwner())
	{
		dwSections |= PhysicsDescInfo::POSITION;
		OptionalPhysicsObjData.Write(&pEntity->m_Position);
	}

	DWORD motion_table = 0;
	if (pEntity->m_Qualities.InqDataID(MOTION_TABLE_DID, motion_table) && motion_table)
	{
		dwSections |= PhysicsDescInfo::MTABLE;
		OptionalPhysicsObjData.Write<DWORD>(motion_table);
	}

	DWORD sound_table = 0;
	if (pEntity->m_Qualities.InqDataID(SOUND_TABLE_DID, sound_table) && sound_table)
	{
		dwSections |= PhysicsDescInfo::STABLE;
		OptionalPhysicsObjData.Write<DWORD>(sound_table);
	}

	DWORD pe_table = 0;
	if (pEntity->m_Qualities.InqDataID(PHYSICS_EFFECT_TABLE_DID, pe_table) && pe_table)
	{
		dwSections |= PhysicsDescInfo::PETABLE;
		OptionalPhysicsObjData.Write<DWORD>(pe_table);
	}

	DWORD setup_id = 0;
	if (pEntity->m_Qualities.InqDataID(SETUP_DID, setup_id) && setup_id)
	{
		dwSections |= PhysicsDescInfo::CSETUP;
		OptionalPhysicsObjData.Write<DWORD>(setup_id);
	}

	if (pEntity->parent)
	{
		dwSections |= PhysicsDescInfo::PARENT;
		OptionalPhysicsObjData.Write<DWORD>(pEntity->parent->id);
		OptionalPhysicsObjData.Write<DWORD>(pEntity->InqIntQuality(PARENT_LOCATION_INT, 0));
	}

	if (pEntity->children && pEntity->children->num_objects)
	{
		dwSections |= PhysicsDescInfo::CHILDREN;
		OptionalPhysicsObjData.Write<DWORD>(pEntity->children->num_objects);

		for (DWORD i = 0; i < pEntity->children->num_objects; i++)
		{
			OptionalPhysicsObjData.Write<DWORD>(pEntity->children->objects.array_data[i]->id);
			OptionalPhysicsObjData.Write<DWORD>(pEntity->children->location_ids.array_data[i]);
		}
	}

	if (pEntity->m_scale != 0.0f && pEntity->m_scale != 1.0f)
	{
		dwSections |= PhysicsDescInfo::OBJSCALE;
		OptionalPhysicsObjData.Write<float>(pEntity->m_scale);
	}

	if (pEntity->m_fFriction != CWeenieObject::DEFAULT_FRICTION)
	{
		dwSections |= PhysicsDescInfo::FRICTION;
		OptionalPhysicsObjData.Write<float>(pEntity->m_fFriction);
	}

	if (pEntity->m_fElasticity != CWeenieObject::DEFAULT_ELASTICITY)
	{
		dwSections |= PhysicsDescInfo::ELASTICITY;
		OptionalPhysicsObjData.Write<float>(pEntity->m_fElasticity);
	}

	if (pEntity->m_fTranslucency != CWeenieObject::DEFAULT_TRANSLUCENCY)
	{
		dwSections |= PhysicsDescInfo::TRANSLUCENCY;
		OptionalPhysicsObjData.Write<float>(pEntity->m_fTranslucency);
	}

	if (pEntity->m_velocityVector != Vector(0, 0, 0))
	{
		dwSections |= PhysicsDescInfo::VELOCITY;
		pEntity->m_velocityVector.Pack(&OptionalPhysicsObjData);
	}

	if (pEntity->m_Acceleration != Vector(0, 0, 0))
	{
		dwSections |= PhysicsDescInfo::ACCELERATION;
		pEntity->m_Acceleration.Pack(&OptionalPhysicsObjData);
	}

	if (pEntity->m_Omega != Vector(0, 0, 0))
	{
		dwSections |= PhysicsDescInfo::OMEGA;
		pEntity->m_Omega.Pack(&OptionalPhysicsObjData);
	}

	if (pEntity->m_DefaultScript != 0)
	{
		dwSections |= PhysicsDescInfo::DEFAULT_SCRIPT;
		OptionalPhysicsObjData.Write<DWORD>(pEntity->m_DefaultScript);
	}

	if (pEntity->m_DefaultScriptIntensity != 0)
	{
		dwSections |= PhysicsDescInfo::DEFAULT_SCRIPT_INTENSITY;
		OptionalPhysicsObjData.Write<float>(pEntity->m_DefaultScriptIntensity);
	}

	BinaryWriter *PhysicsObjData = new BinaryWriter;
	PhysicsObjData->Write<DWORD>(dwSections);
	PhysicsObjData->Write<DWORD>(pEntity->m_PhysicsState); //VIS flags
	PhysicsObjData->Write(&OptionalPhysicsObjData);

	//Moved from CO
	PhysicsObjData->Write<WORD>(pEntity->_position_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_movement_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_state_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_vector_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_teleport_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_server_control_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_force_position_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_objdesc_timestamp);
	PhysicsObjData->Write<WORD>(pEntity->_instance_timestamp);

	PhysicsObjData->Align();

	return PhysicsObjData;
}

BinaryWriter *CreateObject(CWeenieObject *pEntity)
{
	BinaryWriter *CO = new BinaryWriter;

	CO->Write<DWORD>(0xF745);
	CO->Write<DWORD>(pEntity->GetID());

	ObjDesc objDesc;
	pEntity->GetObjDesc(objDesc);
	objDesc.Pack(CO);

	BinaryWriter *PhysicsObjData = GetPhysicsObjData(pEntity);
	CO->Write(PhysicsObjData);
	delete PhysicsObjData;

	BinaryWriter *WeenieObjData = GetWeenieObjData(pEntity);
	CO->Write(WeenieObjData);
	delete WeenieObjData;

	CO->Align();
	return CO;
}

BinaryWriter *UpdateObject(CWeenieObject *pEntity)
{
	BinaryWriter *UO = new BinaryWriter;

	UO->Write<DWORD>(0xF7DB);
	UO->Write<DWORD>(pEntity->GetID());

	ObjDesc objDesc;
	pEntity->GetObjDesc(objDesc);
	objDesc.Pack(UO);

	pEntity->_objdesc_timestamp++;
	pEntity->_teleport_timestamp++;
	pEntity->_movement_timestamp++;
	pEntity->_state_timestamp++;
	pEntity->_vector_timestamp++;
	pEntity->_position_timestamp++;
	pEntity->_server_control_timestamp++;
	pEntity->_force_position_timestamp++;
	pEntity->_objdesc_timestamp++;

	BinaryWriter *PhysicsObjData = GetPhysicsObjData(pEntity);
	UO->Write(PhysicsObjData);
	delete PhysicsObjData;

	BinaryWriter *WeenieObjData = GetWeenieObjData(pEntity);
	UO->Write(WeenieObjData);
	delete WeenieObjData;

	UO->Align();
	return UO;
}

BinaryWriter *IdentifyObjectFail(CWeenieObject *pEntity, bool bShowLevel)
{
	BinaryWriter *pWriter = new BinaryWriter;

	pWriter->Write<DWORD>(0xC9); // message ID
	pWriter->Write<DWORD>(pEntity->GetID());

	AppraisalProfile profile;
	profile.success_flag = FALSE;

	if (pEntity->m_Qualities.m_IntStats)
	{
		profile._intStatsTable = new PackableHashTableWithJson<STypeInt, int>();
		*profile._intStatsTable = *pEntity->m_Qualities.m_IntStats;

		for (PackableHashTableWithJson<STypeInt, int>::iterator i = profile._intStatsTable->begin(); i != profile._intStatsTable->end(); )
		{
			switch (i->first)
			{
			case CREATURE_TYPE_INT:
			case ENCUMB_VAL_INT:
			case PLAYER_KILLER_STATUS_INT:
				i++;
				continue;
			}

			if (i->first == LEVEL_INT && bShowLevel)
			{
				i++;
				continue;
			}

			i = profile._intStatsTable->erase(i);
		}
		
		if (pEntity->m_Qualities._enchantment_reg)
		{
			for (auto &entry : *profile._intStatsTable)
			{
				pEntity->m_Qualities._enchantment_reg->EnchantInt(entry.first, &entry.second, FALSE);
			}
		}

		if (profile._intStatsTable->empty())
		{
			delete profile._intStatsTable;
			profile._intStatsTable = NULL;
		}
	}
	
	if (pEntity->m_Qualities.m_BoolStats)
	{
		profile._boolStatsTable = new PackableHashTableWithJson<STypeBool, BOOL>();
		*profile._boolStatsTable = *pEntity->m_Qualities.m_BoolStats;

		for (PackableHashTableWithJson<STypeBool, BOOL>::iterator i = profile._boolStatsTable->begin(); i != profile._boolStatsTable->end(); )
		{
			switch (i->first)
			{
			case OPEN_BOOL:
			case LOCKED_BOOL:
				i++;
				continue;
			}

			i = profile._boolStatsTable->erase(i);
		}

		if (profile._boolStatsTable->empty())
		{
			delete profile._boolStatsTable;
			profile._boolStatsTable = NULL;
		}
	}

	if (pEntity->m_Qualities.m_StringStats)
	{
		profile._strStatsTable = new PackableHashTableWithJson<STypeString, std::string>();
		*profile._strStatsTable = *pEntity->m_Qualities.m_StringStats;

		for (PackableHashTableWithJson<STypeString, std::string>::iterator i = profile._strStatsTable->begin(); i != profile._strStatsTable->end(); )
		{
			switch (i->first)
			{
			case USE_STRING:
			case TEMPLATE_STRING:
				i++;
				continue;
			}

			i = profile._strStatsTable->erase(i);
		}

		if (profile._strStatsTable->empty())
		{
			delete profile._strStatsTable;
			profile._strStatsTable = NULL;
		}
	}

	if (pEntity->m_Qualities.m_DIDStats)
	{
		profile._didStatsTable = new PackableHashTableWithJson<STypeDID, DWORD>();
		*profile._didStatsTable = *pEntity->m_Qualities.m_DIDStats;
		
		for (PackableHashTableWithJson<STypeDID, DWORD>::iterator i = profile._didStatsTable->begin(); i != profile._didStatsTable->end(); )
		{
			switch (i->first)
			{
			case EYES_PALETTE_DID:
			case EYES_TEXTURE_DID:
			case HAIR_PALETTE_DID:
			case NOSE_TEXTURE_DID:
			case SKIN_PALETTE_DID:
			case MOUTH_TEXTURE_DID:
				i++;
				continue;
			}

			i = profile._didStatsTable->erase(i);
		}

		if (profile._didStatsTable->empty())
		{
			delete profile._didStatsTable;
			profile._didStatsTable = NULL;
		}
	}

	if (pEntity->IsCreature())
	{
		profile.creature_profile = new CreatureAppraisalProfile();
		
		auto setCreatureProfileAttribute2nd = [&](STypeAttribute2nd attrib2nd, DWORD &value, DWORD &bitfield)
		{
			DWORD raw = 0;
			pEntity->m_Qualities.InqAttribute2nd(attrib2nd, raw, TRUE);
			pEntity->m_Qualities.InqAttribute2nd(attrib2nd, value, FALSE);

			if (raw != value)
			{
				switch (attrib2nd)
				{
				case MAX_HEALTH_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_HEALTH; break;
				case MAX_STAMINA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_STAMINA; break;
				case MAX_MANA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_MANA; break;
				}

				if (value > raw)
				{
					switch (attrib2nd)
					{
					case MAX_HEALTH_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_HEALTH_HI; break;
					case MAX_STAMINA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_STAMINA_HI; break;
					case MAX_MANA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_MANA_HI; break;
					}
				}
			}
		};

		setCreatureProfileAttribute2nd(HEALTH_ATTRIBUTE_2ND, profile.creature_profile->health, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(MAX_HEALTH_ATTRIBUTE_2ND, profile.creature_profile->max_health, profile.creature_profile->enchantment_bitfield);
	}

	pWriter->Write(&profile);

	return pWriter;
}

BinaryWriter *IdentifyObject(CWeenieObject *pSource, CWeenieObject *pEntity, DWORD overrideId)
{	
	BinaryWriter *pWriter = new BinaryWriter;

	pWriter->Write<DWORD>(0xC9); // message ID
	if(overrideId)
		pWriter->Write<DWORD>(overrideId);
	else
		pWriter->Write<DWORD>(pEntity->GetID());

	AppraisalProfile profile;

	// always true for now
	profile.success_flag = TRUE;

	BOOL bIsSourceAdmin = FALSE;
	BOOL bIsPlayer = FALSE;
	DWORD characterOptions;
	DWORD characterOptions2;

	if (CPlayerWeenie *pPlayerSource = pSource->AsPlayer())
	{
		bIsSourceAdmin = pPlayerSource->GetAccessLevel() >= ADMIN_ACCESS;
	}

	if (CPlayerWeenie *pPlayer = pEntity->AsPlayer())
	{
		bIsPlayer = true;
		characterOptions = pPlayer->GetCharacterOptions();
		characterOptions2 = pPlayer->GetCharacterOptions2();
	}

	if (pEntity->m_Qualities.m_IntStats)
	{
		profile._intStatsTable = new PackableHashTableWithJson<STypeInt, int>();
		*profile._intStatsTable = *pEntity->m_Qualities.m_IntStats;
		
		profile._intStatsTable->remove(PHYSICS_STATE_INT);
		profile._intStatsTable->remove(XP_OVERRIDE_INT);
		profile._intStatsTable->remove(MAX_GENERATED_OBJECTS_INT);
		profile._intStatsTable->remove(INIT_GENERATED_OBJECTS_INT);

		if (bIsPlayer)
		{
			if (!(characterOptions2 & DisplayAge_CharacterOptions2))
				profile._intStatsTable->remove(AGE_INT);
			if (!(characterOptions2 & DisplayNumberDeaths_CharacterOptions2))
				profile._intStatsTable->remove(NUM_DEATHS_INT);
		}

		if (pEntity->AsCaster())
		{
			profile._intStatsTable->remove(DAMAGE_INT);
		}

		if (pEntity->m_Qualities._enchantment_reg)
		{
			for (auto &entry : *profile._intStatsTable)
			{
				pEntity->m_Qualities._enchantment_reg->EnchantInt(entry.first, &entry.second, FALSE);
			}
		}

		if (int skillActivationType = (int) pEntity->m_Qualities.GetDID(ITEM_SKILL_LIMIT_DID, 0))
		{
			profile._intStatsTable->add(APPRAISAL_ITEM_SKILL_INT, &skillActivationType);
		}
	}

	if (pEntity->m_Qualities.m_Int64Stats)
	{
		profile._int64StatsTable = new PackableHashTableWithJson<STypeInt64, __int64>();
		*profile._int64StatsTable = *pEntity->m_Qualities.m_Int64Stats;
	}

	if (pEntity->m_Qualities.m_BoolStats)
	{
		profile._boolStatsTable = new PackableHashTableWithJson<STypeBool, int>();
		*profile._boolStatsTable = *pEntity->m_Qualities.m_BoolStats;
		
		profile._boolStatsTable->remove(REPORT_COLLISIONS_AS_ENVIRONMENT_BOOL);
		profile._boolStatsTable->remove(ATTACKABLE_BOOL);
		profile._boolStatsTable->remove(IS_HOT_BOOL);
		profile._boolStatsTable->remove(AFFECTS_AIS_BOOL);
		profile._boolStatsTable->remove(UI_HIDDEN_BOOL);
		profile._boolStatsTable->remove(GRAVITY_STATUS_BOOL);
		profile._boolStatsTable->remove(ETHEREAL_BOOL);
		profile._boolStatsTable->remove(IGNORE_COLLISIONS_BOOL);
		profile._boolStatsTable->remove(REPORT_COLLISIONS_BOOL);
		profile._boolStatsTable->remove(STUCK_BOOL);
		profile._boolStatsTable->remove(VISIBILITY_BOOL);
		profile._boolStatsTable->remove(LIGHTS_STATUS_BOOL);
	}

	if (pEntity->m_Qualities.m_FloatStats)
	{
		profile._floatStatsTable = new PackableHashTableWithJson<STypeFloat, double>();
		*profile._floatStatsTable = *pEntity->m_Qualities.m_FloatStats;

		profile._floatStatsTable->remove(HEARTBEAT_INTERVAL_FLOAT);
		profile._floatStatsTable->remove(HEARTBEAT_TIMESTAMP_FLOAT);
		profile._floatStatsTable->remove(OBVIOUS_RADAR_RANGE_FLOAT);
		profile._floatStatsTable->remove(HOTSPOT_CYCLE_TIME_FLOAT);
		profile._floatStatsTable->remove(HOTSPOT_CYCLE_TIME_VARIANCE_FLOAT);
		profile._floatStatsTable->remove(DEFAULT_SCALE_FLOAT);
		profile._floatStatsTable->remove(RESET_INTERVAL_FLOAT);
		profile._floatStatsTable->remove(REGENERATION_INTERVAL_FLOAT);
		profile._floatStatsTable->remove(USE_RADIUS_FLOAT);

		double *weapon_defense = profile._floatStatsTable->lookup(WEAPON_DEFENSE_FLOAT);
		double old_weapon_defense = weapon_defense ? *weapon_defense : 1.0;

		if (pEntity->m_Qualities._enchantment_reg)
		{
			for (auto &entry : *profile._floatStatsTable)
			{
				pEntity->m_Qualities._enchantment_reg->EnchantFloat(entry.first, &entry.second);
			}
		}

		// special case scenarios for item buffs...
		if (weapon_defense)
		{
			*weapon_defense = pEntity->GetMeleeDefenseMod();

			if (fabs(*weapon_defense - old_weapon_defense) >= F_EPSILON)
			{
				profile.weapon_ench_bitfield |= BF_WEAPON_DEFENSE;
				if (*weapon_defense > old_weapon_defense)
					profile.weapon_ench_bitfield |= BF_WEAPON_DEFENSE_HI;
			}
		}
	}

	if (pEntity->m_Qualities.m_StringStats)
	{
		profile._strStatsTable = new PackableHashTableWithJson<STypeString, std::string>();
		*profile._strStatsTable = *pEntity->m_Qualities.m_StringStats;

		if (bIsPlayer)
		{
			if (!(characterOptions2 & DisplayDateOfBirth_CharacterOptions2))
				profile._strStatsTable->remove(DATE_OF_BIRTH_STRING);
		}

		profile._strStatsTable->remove(KEY_CODE_STRING);
		profile._strStatsTable->remove(LOCK_CODE_STRING);
		profile._strStatsTable->remove(QUEST_STRING);
		profile._strStatsTable->remove(QUEST_RESTRICTION_STRING);
		profile._strStatsTable->remove(ACTIVATION_TALK_STRING);
		profile._strStatsTable->remove(ACTIVATION_FAILURE_STRING);
		profile._strStatsTable->remove(USE_MESSAGE_STRING);
		profile._strStatsTable->remove(GENERATOR_EVENT_STRING);
		profile._strStatsTable->remove(KILL_QUEST_STRING);

		if (false) // bIsSourceAdmin)
		{
			/*
			profile._strStatsTable->add(LONG_DESC_STRING, csprintf(
				"For debug purposes: 0x%08X %u\nWT: %d\n%d",
				pEntity->GetID(),
				pEntity->m_Qualities.GetID(),
				pEntity->m_Qualities.m_WeenieType,
				pEntity->m_Qualities.GetInt(ITEM_TYPE_INT, 0)));
				*/
		}
	}

	if (pEntity->m_Qualities.m_DIDStats)
	{
		profile._didStatsTable = new PackableHashTableWithJson<STypeDID, DWORD>();
		*profile._didStatsTable = *pEntity->m_Qualities.m_DIDStats;
		
		/*
		profile._didStatsTable->remove(SETUP_DID);
		profile._didStatsTable->remove(MOTION_TABLE_DID);
		profile._didStatsTable->remove(COMBAT_TABLE_DID);
		profile._didStatsTable->remove(SOUND_TABLE_DID);
		profile._didStatsTable->remove(PALETTE_BASE_DID);
		profile._didStatsTable->remove(CLOTHINGBASE_DID);
		profile._didStatsTable->remove(ICON_DID);
		profile._didStatsTable->remove(PHYSICS_EFFECT_TABLE_DID);
		profile._didStatsTable->remove(DEATH_TREASURE_TYPE_DID);
		profile._didStatsTable->remove(TSYS_MUTATION_FILTER_DID);
		profile._didStatsTable->remove(MUTATE_FILTER_DID);
		*/

		for (PackableHashTable<STypeDID, DWORD>::iterator i = profile._didStatsTable->begin(); i != profile._didStatsTable->end(); )
		{
			switch (i->first)
			{
			case EYES_PALETTE_DID:
			case EYES_TEXTURE_DID:
			case HAIR_PALETTE_DID:
			case NOSE_TEXTURE_DID:
			case SKIN_PALETTE_DID:
			case MOUTH_TEXTURE_DID:
			case ITEM_SKILL_LIMIT_DID:
			case SPELL_DID:
			case SPELL_COMPONENT_DID:
			case DEATH_SPELL_DID:
				i++;
				continue;
			}

			i = profile._didStatsTable->erase(i);
		}
	}

	if (!pEntity->IsCreature() && pEntity->m_Qualities._spell_book)
	{
		profile._spellBook = new SmartArray<DWORD>();
		profile._spellBook->grow((DWORD)pEntity->m_Qualities._spell_book->_spellbook.size());

		for (auto &spell : pEntity->m_Qualities._spell_book->_spellbook)
			profile._spellBook->add((DWORD *)&spell.first);
	}

	if (pEntity->IsCreature())
	{
		profile.creature_profile = new CreatureAppraisalProfile();

		auto setCreatureProfileAttribute = [&](STypeAttribute attrib, DWORD &value, DWORD &bitfield)
		{
			DWORD raw = 0;
			pEntity->m_Qualities.InqAttribute(attrib, raw, TRUE);
			pEntity->m_Qualities.InqAttribute(attrib, value, FALSE);

			if (raw != value)
			{
				switch (attrib)
				{
					case STRENGTH_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_STRENGTH; break;
					case ENDURANCE_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_ENDURANCE; break;
					case QUICKNESS_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_QUICKNESS; break;
					case COORDINATION_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_COORDINATION; break;
					case FOCUS_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_FOCUS; break;
					case SELF_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_SELF; break;
				}

				if (value > raw)
				{
					switch (attrib)
					{
					case STRENGTH_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_STRENGTH_HI; break;
					case ENDURANCE_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_ENDURANCE_HI; break;
					case QUICKNESS_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_QUICKNESS_HI; break;
					case COORDINATION_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_COORDINATION_HI; break;
					case FOCUS_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_FOCUS_HI; break;
					case SELF_ATTRIBUTE: bitfield |= CreatureAppraisalProfile::BF_SELF_HI; break;
					}
				}
			}
		};

		auto setCreatureProfileAttribute2nd = [&](STypeAttribute2nd attrib2nd, DWORD &value, DWORD &bitfield)
		{
			DWORD raw = 0;
			pEntity->m_Qualities.InqAttribute2nd(attrib2nd, raw, TRUE);
			pEntity->m_Qualities.InqAttribute2nd(attrib2nd, value, FALSE);

			if (raw != value)
			{
				switch (attrib2nd)
				{
				case MAX_HEALTH_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_HEALTH; break;
				case MAX_STAMINA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_STAMINA; break;
				case MAX_MANA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_MANA; break;
				}

				if (value > raw)
				{
					switch (attrib2nd)
					{
					case MAX_HEALTH_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_HEALTH_HI; break;
					case MAX_STAMINA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_STAMINA_HI; break;
					case MAX_MANA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_MANA_HI; break;
					}
				}
			}
		};

		setCreatureProfileAttribute(STRENGTH_ATTRIBUTE, profile.creature_profile->strength, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute(ENDURANCE_ATTRIBUTE, profile.creature_profile->endurance, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute(QUICKNESS_ATTRIBUTE, profile.creature_profile->quickness, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute(COORDINATION_ATTRIBUTE, profile.creature_profile->coordination, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute(FOCUS_ATTRIBUTE, profile.creature_profile->focus, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute(SELF_ATTRIBUTE, profile.creature_profile->self, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(HEALTH_ATTRIBUTE_2ND, profile.creature_profile->health, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(MAX_HEALTH_ATTRIBUTE_2ND, profile.creature_profile->max_health, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(STAMINA_ATTRIBUTE_2ND, profile.creature_profile->stamina, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(MAX_STAMINA_ATTRIBUTE_2ND, profile.creature_profile->max_stamina, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(MANA_ATTRIBUTE_2ND, profile.creature_profile->mana, profile.creature_profile->enchantment_bitfield);
		setCreatureProfileAttribute2nd(MAX_MANA_ATTRIBUTE_2ND, profile.creature_profile->max_mana, profile.creature_profile->enchantment_bitfield);
	}

	if (pEntity->InqType() & TYPE_WEAPON)
	{
		profile.weapon_profile = new WeaponProfile();

		auto setCreatureProfileAttribute2nd = [&](STypeAttribute2nd attrib2nd, DWORD &value, DWORD &bitfield)
		{
			DWORD raw = 0;
			pEntity->m_Qualities.InqAttribute2nd(attrib2nd, raw, TRUE);
			pEntity->m_Qualities.InqAttribute2nd(attrib2nd, value, FALSE);

			if (raw != value)
			{
				switch (attrib2nd)
				{
				case MAX_HEALTH_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_HEALTH; break;
				case MAX_STAMINA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_STAMINA; break;
				case MAX_MANA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_MANA; break;
				}

				if (value > raw)
				{
					switch (attrib2nd)
					{
					case MAX_HEALTH_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_HEALTH_HI; break;
					case MAX_STAMINA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_STAMINA_HI; break;
					case MAX_MANA_ATTRIBUTE_2ND: bitfield |= CreatureAppraisalProfile::BF_MAX_MANA_HI; break;
					}
				}
			}
		};

		profile.weapon_profile->damage_type = (DAMAGE_TYPE) pEntity->InqIntQuality(DAMAGE_TYPE_INT, 0);

		int baseWeaponTime = max(0, min(200, pEntity->InqIntQuality(WEAPON_TIME_INT, 0, TRUE)));
		profile.weapon_profile->weapon_time = max(0, min(200, pEntity->GetAttackTime()));

		if (baseWeaponTime != profile.weapon_profile->weapon_time)
		{
			profile.weapon_ench_bitfield |= WeaponEnchantment_BFIndex::BF_WEAPON_TIME;
			if (profile.weapon_profile->weapon_time < baseWeaponTime)
				profile.weapon_ench_bitfield |= WeaponEnchantment_BFIndex::BF_WEAPON_TIME_HI;
		}

		profile.weapon_profile->weapon_skill = pEntity->InqIntQuality(WEAPON_SKILL_INT, 0);

		if (!pEntity->AsCaster())
		{
			int baseWeaponDamage = pEntity->InqIntQuality(DAMAGE_INT, 0, TRUE);
			profile.weapon_profile->weapon_damage = pEntity->GetAttackDamage();

			if (baseWeaponDamage != profile.weapon_profile->weapon_damage)
			{
				profile.weapon_ench_bitfield |= WeaponEnchantment_BFIndex::BF_DAMAGE;
				if (profile.weapon_profile->weapon_damage > baseWeaponDamage)
					profile.weapon_ench_bitfield |= WeaponEnchantment_BFIndex::BF_DAMAGE_HI;
			}

			profile.weapon_profile->damage_variance = pEntity->InqFloatQuality(DAMAGE_VARIANCE_FLOAT, 0.0);
			profile.weapon_profile->damage_mod = pEntity->InqFloatQuality(DAMAGE_MOD_FLOAT, 1.0);
			profile.weapon_profile->weapon_length = pEntity->InqFloatQuality(WEAPON_LENGTH_FLOAT, 0);

			double baseWeaponOffense = pEntity->InqFloatQuality(WEAPON_OFFENSE_FLOAT, 0, TRUE);
			profile.weapon_profile->weapon_offense = pEntity->GetOffenseMod();

			if (fabs(baseWeaponOffense - profile.weapon_profile->weapon_offense) >= F_EPSILON)
			{
				profile.weapon_ench_bitfield |= WeaponEnchantment_BFIndex::BF_WEAPON_OFFENSE;
				if (profile.weapon_profile->weapon_offense > baseWeaponOffense)
					profile.weapon_ench_bitfield |= WeaponEnchantment_BFIndex::BF_WEAPON_OFFENSE_HI;
			}

			profile.weapon_profile->max_velocity = pEntity->InqFloatQuality(MAXIMUM_VELOCITY_FLOAT, 0, TRUE);
			profile.weapon_profile->max_velocity_estimated = (int)0;
		}
		else
		{
			profile.weapon_profile->weapon_damage = 0;
			profile.weapon_profile->damage_variance = 0;
		}
	}

	if (pEntity->InqType() & TYPE_VESTEMENTS)
	{
		auto setArmorProfileMod = [&](STypeFloat floatStat, float &value, DWORD &bitfield)
		{
			double raw = 0.0;
			double dbl_value = 0.0;
			pEntity->m_Qualities.InqFloat(floatStat, raw, TRUE);
			pEntity->m_Qualities.InqFloat(floatStat, dbl_value, FALSE);
			value = (float) dbl_value;
			
			if (abs(raw - dbl_value) > F_EPSILON)
			{
				switch (floatStat)
				{
				case ARMOR_MOD_VS_SLASH_FLOAT: bitfield |= BF_ARMOR_MOD_VS_SLASH; break;
				case ARMOR_MOD_VS_PIERCE_FLOAT: bitfield |= BF_ARMOR_MOD_VS_PIERCE; break;
				case ARMOR_MOD_VS_BLUDGEON_FLOAT: bitfield |= BF_ARMOR_MOD_VS_BLUDGEON; break;				
				case ARMOR_MOD_VS_COLD_FLOAT: bitfield |= BF_ARMOR_MOD_VS_COLD; break;
				case ARMOR_MOD_VS_FIRE_FLOAT: bitfield |= BF_ARMOR_MOD_VS_FIRE; break;
				case ARMOR_MOD_VS_ACID_FLOAT: bitfield |= BF_ARMOR_MOD_VS_ACID; break;
				case ARMOR_MOD_VS_ELECTRIC_FLOAT: bitfield |= BF_ARMOR_MOD_VS_ELECTRIC; break;
				case ARMOR_MOD_VS_NETHER_FLOAT: bitfield |= BF_ARMOR_MOD_VS_NETHER; break;
				}

				if (dbl_value > raw)
				{
					switch (floatStat)
					{
					case ARMOR_MOD_VS_SLASH_FLOAT: bitfield |= BF_ARMOR_MOD_VS_SLASH_HI; break;
					case ARMOR_MOD_VS_PIERCE_FLOAT: bitfield |= BF_ARMOR_MOD_VS_PIERCE_HI; break;
					case ARMOR_MOD_VS_BLUDGEON_FLOAT: bitfield |= BF_ARMOR_MOD_VS_BLUDGEON_HI; break;
					case ARMOR_MOD_VS_COLD_FLOAT: bitfield |= BF_ARMOR_MOD_VS_COLD_HI; break;
					case ARMOR_MOD_VS_FIRE_FLOAT: bitfield |= BF_ARMOR_MOD_VS_FIRE_HI; break;
					case ARMOR_MOD_VS_ACID_FLOAT: bitfield |= BF_ARMOR_MOD_VS_ACID_HI; break;
					case ARMOR_MOD_VS_ELECTRIC_FLOAT: bitfield |= BF_ARMOR_MOD_VS_ELECTRIC_HI; break;
					case ARMOR_MOD_VS_NETHER_FLOAT: bitfield |= BF_ARMOR_MOD_VS_NETHER_HI; break;
					}
				}
			}
		};
		
		profile.armor_profile = new ArmorProfile();		
		setArmorProfileMod(ARMOR_MOD_VS_SLASH_FLOAT, profile.armor_profile->mod_vs_slash, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_PIERCE_FLOAT, profile.armor_profile->mod_vs_pierce, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_BLUDGEON_FLOAT, profile.armor_profile->mod_vs_bludgeon, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_COLD_FLOAT, profile.armor_profile->mod_vs_cold, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_FIRE_FLOAT, profile.armor_profile->mod_vs_fire, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_ACID_FLOAT, profile.armor_profile->mod_vs_acid, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_ELECTRIC_FLOAT, profile.armor_profile->mod_vs_electric, profile.armor_ench_bitfield);
		setArmorProfileMod(ARMOR_MOD_VS_NETHER_FLOAT, profile.armor_profile->mod_vs_nether, profile.armor_ench_bitfield);

		int *enchanted_armor_level = NULL;
		if (profile._intStatsTable && (enchanted_armor_level = profile._intStatsTable->lookup(ARMOR_LEVEL_INT)))
		{
			int raw_armor_level = pEntity->InqIntQuality(ARMOR_LEVEL_INT, 0, TRUE);
			if (raw_armor_level != *enchanted_armor_level)
			{
				profile.armor_ench_bitfield |= BF_ARMOR_LEVEL;
				if (*enchanted_armor_level > raw_armor_level)
					profile.armor_ench_bitfield |= BF_ARMOR_LEVEL_HI;
			}
		}
	}

	pWriter->Write(&profile);

	return pWriter;
}

BinaryWriter *LoginCharacter(CPlayerWeenie *pPlayer)
{
	BinaryWriter *LC = new BinaryWriter;

	LC->Write<DWORD>(0x13);

	CACQualities loginQualities;
	loginQualities.CopyFrom(&pPlayer->m_Qualities);
	SafeDelete(loginQualities._body);
	SafeDelete(loginQualities._generator_table);
	SafeDelete(loginQualities._generator_registry);
	SafeDelete(loginQualities._generator_queue);
	SafeDelete(loginQualities._emote_table);
	SafeDelete(loginQualities._create_list);
	SafeDelete(loginQualities._event_filter);

	LC->Write(&loginQualities);

	PlayerModule module = pPlayer->_playerModule;
	module.Pack(LC);

	/*
	// Player Module
	LC->Write<DWORD>(0x0460); // flags for what is after this
	LC->Write<DWORD>(pPlayer->m_CharacterOptions); // options
	LC->Write<DWORD>(0x00000000);//spellbar 1 count
	LC->Write<DWORD>(0x00000000);//spellbar 2 count
	LC->Write<DWORD>(0x00000000);//spellbar 3 count
	LC->Write<DWORD>(0x00000000);//spellbar 4 count
	LC->Write<DWORD>(0x00000000);//spellbar 5 count
	LC->Write<DWORD>(0x00000000);//spellbar 6 count
	LC->Write<DWORD>(0x00000000);//spellbar 7 count
	LC->Write<DWORD>(0x00000000);//spellbar 8 count
	LC->Write<DWORD>(0x00003FFF); // more stuff
	LC->Write<DWORD>(pPlayer->m_CharacterOptions2); // more stuff
	LC->Align();
	*/

	PackableList<ContentProfile> inventoryList;
	for (auto item : pPlayer->m_Items)
	{
		ContentProfile prof;
		prof.m_iid = item->GetID();
		prof.m_uContainerProperties = 0;
		inventoryList.push_back(prof);
	}
	for (auto item : pPlayer->m_Packs)
	{
		ContentProfile prof;
		prof.m_iid = item->GetID();
		prof.m_uContainerProperties = item->AsContainer() ? 1 : 2;
		inventoryList.push_back(prof);
	}
	inventoryList.Pack(LC);

	PackableList<InventoryPlacement> wieldedList;
	for (auto wielded : pPlayer->m_Wielded)
	{
		InventoryPlacement place;
		place.iid_ = wielded->GetID();
		place.loc_ = wielded->InqIntQuality(CURRENT_WIELDED_LOCATION_INT, 0);
		place.priority_ = wielded->InqIntQuality(CLOTHING_PRIORITY_INT, 0);
		wieldedList.push_back(place);
	}
	wieldedList.Pack(LC);

	return LC;
}

BinaryWriter *HealthUpdate(CWeenieObject *pWeenie)
{
	MESSAGE_BEGIN(HealthUpdate);

	HealthUpdate->Write<DWORD>(0x1C0);
	HealthUpdate->Write<DWORD>(pWeenie->GetID());

	DWORD health = 0, maxHealth = 1;
	pWeenie->m_Qualities.InqAttribute2nd(HEALTH_ATTRIBUTE_2ND, health, FALSE);
	pWeenie->m_Qualities.InqAttribute2nd(MAX_HEALTH_ATTRIBUTE_2ND, maxHealth, FALSE);

	double fraction = min(1.0, max(0.0, maxHealth != 0 ? health / (double)maxHealth : 0.0));
	HealthUpdate->Write<float>((float)fraction);

	MESSAGE_END(HealthUpdate);
}

BinaryWriter *ItemManaUpdate(CWeenieObject *item)
{
	MESSAGE_BEGIN(ItemManaUpdate);

	int currentMana = item->InqIntQuality(ITEM_CUR_MANA_INT, -1, TRUE);
	int maxMana = item->InqIntQuality(ITEM_MAX_MANA_INT, -1, TRUE);

	DWORD hasMana;
	float manaFraction;
	if (maxMana < 0 || currentMana < 0)
	{
		hasMana = 0;
		manaFraction = 0;
	}
	else
	{
		hasMana = 1;
		manaFraction = (float)currentMana / (float)maxMana;
	}

	manaFraction = min(max(manaFraction, 0), 1);
	ItemManaUpdate->Write<DWORD>(0x0264);
	ItemManaUpdate->Write<DWORD>(item->GetID());
	ItemManaUpdate->Write<float>(manaFraction);
	ItemManaUpdate->Write<DWORD>(hasMana);

	MESSAGE_END(ItemManaUpdate);
}

BinaryWriter *InventoryEquip(DWORD dwItemID, DWORD dwCoverage)
{
	MESSAGE_BEGIN(InventoryEquip);

	InventoryEquip->Write<DWORD>(0x23);
	InventoryEquip->Write<DWORD>(dwItemID);
	InventoryEquip->Write<DWORD>(dwCoverage);

	MESSAGE_END(InventoryEquip);
}

BinaryWriter *InventoryMove(DWORD dwItemID, DWORD dwContainerID, DWORD dwSlot, DWORD dwType)
{
	MESSAGE_BEGIN(InventoryMove);

	InventoryMove->Write<DWORD>(0x22);
	InventoryMove->Write<DWORD>(dwItemID);
	InventoryMove->Write<DWORD>(dwContainerID);
	InventoryMove->Write<DWORD>(dwSlot);
	InventoryMove->Write<DWORD>(dwType);

	MESSAGE_END(InventoryMove);
}

BinaryWriter *InventoryDrop(DWORD dwItemID)
{
	MESSAGE_BEGIN(InventoryDrop);

	InventoryDrop->Write<DWORD>(0x19A);
	InventoryDrop->Write<DWORD>(dwItemID);

	MESSAGE_END(InventoryDrop);
}

BinaryWriter *MoveUpdate(CWeenieObject *pEntity)
{
	BinaryWriter *pWriter = new BinaryWriter();

	pWriter->Write<DWORD>(0xF748);
	pWriter->Write<DWORD>(pEntity->GetID());

	PositionPack position;
	position.position = pEntity->m_Position;
	position.has_contact = (pEntity->transient_state & CONTACT_TS && pEntity->transient_state & ON_WALKABLE_TS) ? TRUE : FALSE;
	position.placement_id = pEntity->GetActivePlacementFrameID();
	position.instance_timestamp = pEntity->_instance_timestamp;
	position.position_timestamp = pEntity->_position_timestamp;
	position.teleport_timestamp = pEntity->_teleport_timestamp;
	// position.velocity = pEntity->m_velocityVector;
	position.force_position_timestamp = pEntity->_force_position_timestamp;

	position.Pack(pWriter);

	return pWriter;
}
