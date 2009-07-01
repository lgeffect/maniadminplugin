//
// Mani Admin Plugin
//
// Copyright (c) 2009 Giles Millward (Mani). All rights reserved.
//
// This file is part of ManiAdminPlugin.
//
// Mani Admin Plugin is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Mani Admin Plugin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Mani Admin Plugin.  If not, see <http://www.gnu.org/licenses/>.
//

//




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#ifndef __linux__
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#endif
#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "dlls/iplayerinfo.h"
#include "convar.h"
#include "eiface.h"
#include "inetchannelinfo.h"
#include "mani_main.h"
#include "mani_gametype.h"
#include "mani_player.h"
#include "mani_output.h"
#include "mani_admin_flags.h"
#include "mani_admin.h"
#include "mani_immunity_flags.h"
#include "mani_immunity.h"
#include "mani_convar.h"
#include "cbaseentity.h"
#include "mani_vfuncs.h"

extern IFileSystem	*filesystem;
extern	IVEngineServer	*engine; // helper functions (messaging clients, loading content, making entities, running commands, etc)
extern	IPlayerInfoManager *playerinfomanager;
extern	CGlobalVars *gpGlobals;
extern	IVoiceServer *voiceserver;
extern	ITempEntsSystem *temp_ents;
extern	int	max_players;

#ifdef __linux__
static	void	CheckVFunc(DWORD *class_ptr, char *class_name, char *class_function, char *gametype_ptr, int vfunc_type);
static	int		FindVFunc(DWORD *class_ptr, char *class_name, char *class_function, char *mangled_ptr);
#endif

inline bool FStruEq(const char *sz1, const char *sz2)
{
	return(Q_strcmp(sz1, sz2) == 0);
}

CBaseEntity *EdictToCBE(edict_t *pEdict)
{
	return (CBaseEntity *) pEdict->GetUnknown()->GetBaseEntity();
}

class ManiEmptyClass {};

#ifdef __linux__
#define VFUNC_OS_DEP void *addr;	} u; 	u.addr = func;
#else
#define VFUNC_OS_DEP struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

#define VFUNC_SETUP_PTR(_vfunc_index)  \
{ \
	void **this_ptr = *(void ***)&pThisPtr; \
	void **vtable = *(void ***)pThisPtr; \
	void *func = vtable[gpManiGameType->GetVFuncIndex(_vfunc_index)]

//********************************************************************
// CBaseEntity

#define VFUNC_CALL0(_vfunc_index, _return_type, _class_type, _func_name ) \
	_return_type _func_name(_class_type *pThisPtr) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)();} 

#define VFUNC_CALL1(_vfunc_index, _return_type, _class_type, _func_name, _param1) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1);} 

#define VFUNC_CALL2(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2);} 

#define VFUNC_CALL3(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3);} 

#define VFUNC_CALL4(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3, _param4) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4);} 

#define VFUNC_CALL5(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5);} 

#define VFUNC_CALL6(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6);} 

#define VFUNC_CALL7(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6, _param7) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6, _param7 p7) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6, _param7); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6, p7);} 

#define VFUNC_CALL8(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6, _param7 p7, _param8 p8) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6, p7, p8);} 

#define VFUNC_CALL9(_vfunc_index, _return_type, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8, _param9) \
	_return_type _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6, _param7 p7, _param8 p8, _param9) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { _return_type (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8, _param9); \
	VFUNC_OS_DEP \
 	return (_return_type) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6, p7, p8, p9);} 

#define VFUNC_CALL0_void(_vfunc_index, _class_type, _func_name ) \
	void _func_name(_class_type *pThisPtr) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)();} 

#define VFUNC_CALL1_void(_vfunc_index, _class_type, _func_name, _param1) \
	void _func_name(_class_type *pThisPtr, _param1 p1) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1);} 

#define VFUNC_CALL2_void(_vfunc_index, _class_type, _func_name, _param1, _param2) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2);} 

#define VFUNC_CALL3_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3);} 

#define VFUNC_CALL4_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3, _param4) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4);} 

#define VFUNC_CALL5_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5);} 

#define VFUNC_CALL6_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6);} 

#define VFUNC_CALL7_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6, _param7) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6, _param7 p7) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6, _param7); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6, p7);} 

#define VFUNC_CALL8_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6, _param7 p7, _param8 p8) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6, p7, p8);} 

#define VFUNC_CALL9_void(_vfunc_index, _class_type, _func_name, _param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8, _param9) \
	void _func_name(_class_type *pThisPtr, _param1 p1, _param2 p2, _param3 p3, _param4 p4, _param5 p5, _param6 p6, _param7 p7, _param8 p8, _param9) \
	VFUNC_SETUP_PTR(_vfunc_index); \
	union { void (ManiEmptyClass::*mfpnew)(_param1, _param2, _param3, _param4, _param5, _param6, _param7, _param8, _param9); \
	VFUNC_OS_DEP \
 	(void) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)(p1, p2, p3, p4, p5, p6, p7, p8, p9);} 


// virtual const QAngle &EyeAngles( void );
VFUNC_CALL0(MANI_VFUNC_EYE_ANGLES, const QAngle &, CBaseEntity, CBaseEntity_EyeAngles)

// virtual void	Teleport( const Vector *newPosition, const QAngle *newAngles, const Vector *newVelocity );
VFUNC_CALL3_void(MANI_VFUNC_TELEPORT, CBaseEntity, CBaseEntity_Teleport, const Vector *, const QAngle *, const Vector *)

// virtual Vector	EyePosition( void );
VFUNC_CALL0(MANI_VFUNC_EYE_POSITION, Vector, CBaseEntity, CBaseEntity_EyePosition)

//virtual void	GetVelocity(Vector *vVelocity, AngularImpulse *vAngVelocity = NULL);
VFUNC_CALL2_void(MANI_VFUNC_GET_VELOCITY, CBaseEntity, CBaseEntity_GetVelocity, Vector *, AngularImpulse *)

// virtual CBaseCombatCharacter *MyCombatCharacterPointer( void );
VFUNC_CALL0(MANI_VFUNC_MY_COMBAT_CHARACTER, CBaseCombatCharacter *, CBaseEntity, CBaseEntity_MyCombatCharacterPointer)

// virtual void SetModelIndex( int index );
VFUNC_CALL1_void(MANI_VFUNC_SET_MODEL_INDEX, CBaseEntity, CBaseEntity_SetModelIndex, short)

// virtual void Ignite( float flFlameLifetime, bool bNPCOnly = true, float flSize = 0.0f, bool bCalledByLevelDesigner = false );
VFUNC_CALL4_void(MANI_VFUNC_IGNITE, CBasePlayer, CBasePlayer_Ignite, float, bool, float, bool)

// virtual bool	RemovePlayerItem( CBaseCombatWeapon *pItem )
VFUNC_CALL1(MANI_VFUNC_REMOVE_PLAYER_ITEM, bool, CBasePlayer, CBasePlayer_RemovePlayerItem, CBaseCombatWeapon *)

// virtual void	Weapon_Drop( CBaseCombatWeapon *pWeapon, const Vector *pvecTarget = NULL, const Vector *pVelocity = NULL );
VFUNC_CALL3_void(MANI_VFUNC_WEAPON_DROP, CBasePlayer, CBasePlayer_WeaponDrop, CBaseCombatWeapon *, const Vector *, const Vector *)

// virtual CBaseEntity	*GiveNamedItem( const char *szName, int iSubType = 0 );
VFUNC_CALL2(MANI_VFUNC_GIVE_ITEM, CBaseEntity *, CBasePlayer, CBasePlayer_GiveNamedItem, const char *, int)

// virtual CBaseCombatWeapon *Weapon_GetSlot( int slot ) const;
VFUNC_CALL1(MANI_VFUNC_GET_WEAPON_SLOT, CBaseCombatWeapon *, CBaseCombatCharacter, CBaseCombatCharacter_Weapon_GetSlot, int)

// virtual bool Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0 );
VFUNC_CALL2_void(MANI_VFUNC_WEAPON_SWITCH, CBaseCombatCharacter, CBaseCombatCharacter_Weapon_Switch, CBaseCombatWeapon *, int)

// virtual int GiveAmmo( int iCount, int iAmmoIndex, bool bSuppressSound = false );
VFUNC_CALL3_void(MANI_VFUNC_GIVE_AMMO, CBaseCombatCharacter, CBaseCombatCharacter_GiveAmmo, int, int, bool)

// virtual int	GetPrimaryAmmoType( void );
VFUNC_CALL0(MANI_VFUNC_GET_PRIMARY_AMMO_TYPE, int, CBaseCombatWeapon, CBaseCombatWeapon_GetPrimaryAmmoType)

// virtual int	GetSecondaryAmmoType( void );
VFUNC_CALL0(MANI_VFUNC_GET_SECONDARY_AMMO_TYPE, int, CBaseCombatWeapon, CBaseCombatWeapon_GetSecondaryAmmoType)

// virtual char const *GetName( void );
VFUNC_CALL0(MANI_VFUNC_WEAPON_GET_NAME, const char *, CBaseCombatWeapon, CBaseCombatWeapon_GetName)

// virtual int CommitSuicide( void );
VFUNC_CALL0_void(MANI_VFUNC_COMMIT_SUICIDE, CBasePlayer, CBasePlayer_CommitSuicide)

datamap_t *CBaseEntity_GetDataDescMap(CBaseEntity *pThisPtr)
{
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[gpManiGameType->GetVFuncIndex(MANI_VFUNC_MAP)]; 

	union {datamap_t *(ManiEmptyClass::*mfpnew)();
#ifndef __linux__
        void *addr;	} u; 	u.addr = func;
#else /* GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 */
			struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

	return (datamap_t *) (reinterpret_cast<ManiEmptyClass*>(this_ptr)->*u.mfpnew)();
}



//********************************************************************
// Find offset functions

void VFunc_CallCBaseEntity(player_t *player_ptr)
{

	CBaseEntity *pCBE;

	pCBE = EdictToCBE(player_ptr->entity);

	QAngle eye_angles = pCBE->EyeAngles();
	pCBE->Teleport(NULL, NULL, NULL);
//	int health = pCBE->GetHealth();
	pCBE->SetHealth(100);
	Vector velocity;

	pCBE->GetVelocity(&velocity);
	pCBE->SetRenderColor(255, 255, 255, 255);
	pCBE->SetRenderColor(255, 255, 255);

//	CBaseCombatCharacter *pCombat = pCBE->MyCombatCharacterPointer();
	pCBE->SetGravity(1.0);
	pCBE->SetModelIndex(-1);
	return;
}

#ifdef __linux__
CON_COMMAND(ma_vfuncs, "Debug Tool")
{

        player_t player;

        player.entity = NULL;

        if (!IsCommandIssuedByServerAdmin()) return;
        if (ProcessPluginPaused()) return;

        if (engine->Cmd_Argc() < 4)
        {
                Msg("Need more args :)\n");
                return;
        }

        // Whoever issued the commmand is authorised to do it.
        if (!FindTargetPlayers(&player, engine->Cmd_Argv(1), IMMUNITY_DONT_CARE))
        {
                return;
        }

        player_t *target_ptr = &(target_player_list[0]);


        CBaseEntity *pPlayer = target_ptr->entity->GetUnknown()->GetBaseEntity();

        Dl_info d;

        void    *handle;
        void    *var_address;

        handle = dlopen(gpManiGameType->GetLinuxBin(), RTLD_NOW);

        if (handle == NULL)
        {
                Msg("Failed to open server image, error [%s]\n", dlerror());
        }
        else
        {
                FileHandle_t file_handle;
                char    base_filename[512];
                DWORD   *type_ptr;

                //Write to disk
                if (FStrEq("CBE", engine->Cmd_Argv(2)))
                {
                        Q_snprintf(base_filename, sizeof (base_filename), "./cfg/%s/cbe.out", mani_path.GetString());
                        type_ptr = (DWORD *) pPlayer;
                }
                else if (FStrEq("VOICE", engine->Cmd_Argv(2)))
                {
                        Q_snprintf(base_filename, sizeof (base_filename), "./cfg/%s/voice.out", mani_path.GetString());
                        type_ptr = (DWORD *) voiceserver;
                }
				else if (FStrEq("TE", engine->Cmd_Argv(2)))
                {
                        Q_snprintf(base_filename, sizeof (base_filename), "./cfg/%s/te.out", mani_path.GetString());
                        type_ptr = (DWORD *) temp_ents;
                }
				else if (FStrEq("CBCC", engine->Cmd_Argv(2)))
                {
                        Q_snprintf(base_filename, sizeof (base_filename), "./cfg/%s/cbcc.out", mani_path.GetString());
//                        CBaseCombatCharacter *pCombat = pPlayer->MyCombatCharacterPointer();
						CBaseCombatCharacter *pCombat = CBaseEntity_MyCombatCharacterPointer(pPlayer);
                        if (pCombat)
                        {
                                type_ptr = (DWORD *)pCombat;
                        }
                        else
                        {
                                Msg("Failed to get Combat Character\n");
								dlclose(handle);
                                return;
                        }
                }
                else if (FStrEq("CBCW", engine->Cmd_Argv(2)))
                {
                        Q_snprintf(base_filename, sizeof (base_filename), "./cfg/%s/cbcw.out", mani_path.GetString());
						CBaseCombatCharacter *pCombat = CBaseEntity_MyCombatCharacterPointer(pPlayer);
//                        CBaseCombatCharacter *pCombat = pPlayer->MyCombatCharacterPointer();
                        if (!pCombat)
                        {
                                Msg("Failed to get combat character\n");
								dlclose(handle);
                                return;
                        }

                        // Get pistol type
//                        CBaseCombatWeapon *pWeapon = pCombat->Weapon_GetSlot(1);
						CBaseCombatWeapon *pWeapon = CBaseCombatCharacter_Weapon_GetSlot(pCombat, 1);
                        if (!pWeapon)
                        {
                                Msg("Failed to get weapon info\n");
								dlclose(handle);
                                return;
                        }

                        type_ptr = (DWORD *) pWeapon;
                }
                else
                {
                        Msg("Invalid 3rd arg\n");
						dlclose(handle);
                        return;
                }

                file_handle = filesystem->Open (base_filename,"wt", NULL);
                if (file_handle == NULL)
                {
                        Msg("Failed to open file [%s] for writing\n", base_filename);
						dlclose(handle);
                        return;
                }

                for (int i = 0; i < Q_atoi(engine->Cmd_Argv(3)); i++)
                {
                        DWORD *FuncPtr = (DWORD *)(*(DWORD *)((*((DWORD *) type_ptr)) + (i * 4)));

                        int status = dladdr(FuncPtr, &d);

                        if (status)
                        {
                                char temp_string[2048];
                                int temp_length = Q_snprintf(temp_string, sizeof(temp_string), "%s\n", d.dli_sname);

                                if (filesystem->Write((void *) temp_string, temp_length, file_handle) == 0)
                                {
                                        Msg("Failed to write data !!\n");
                                        filesystem->Close(file_handle);
                                        return;
                                }

                                Msg("%s\n", d.dli_sname);
                        }
                        else
                        {
                                Msg("Failed offset [%i]\n", i);
                        }
                }

                filesystem->Close(file_handle);
                dlclose(handle);
        }
}

CON_COMMAND(ma_getvfunc, "Debug Tool")
{

	player_t player;

	player.entity = NULL;

	if (!IsCommandIssuedByServerAdmin()) return;
	if (ProcessPluginPaused()) return;

	if (engine->Cmd_Argc() < 3)
	{
		Msg("Need more args :)\n");
		return;
	}

	bool	found_player = false;

	for (int i = 1; i <= max_players; i++)
	{
		player.index = i;
		if (!FindPlayerByIndex(&player)) continue;
		if (player.is_bot) continue;
		found_player = true;
	}

	if (!found_player)
	{
		for (int i = 1; i <= max_players; i++)
		{
			player.index = i;
			if (!FindPlayerByIndex(&player)) continue;
			found_player = true;
		}
	}

	// Whoever issued the commmand is authorised to do it.
	if (!found_player)
	{
		Msg("Need a target player to work the magic\n");
		return;
	}

	player_t *target_ptr = &player;

	CBaseEntity *pPlayer = target_ptr->entity->GetUnknown()->GetBaseEntity();
	DWORD *type_ptr;

	//Write to disk
	if (FStrEq("CBE", engine->Cmd_Argv(1)))
	{
		type_ptr = (DWORD *) pPlayer;
	}
	else if (FStrEq("VOICE", engine->Cmd_Argv(1)))
	{
		type_ptr = (DWORD *) voiceserver;
	}
	else if (FStrEq("TE", engine->Cmd_Argv(1)))
	{
		type_ptr = (DWORD *) temp_ents;
	}
	else if (FStrEq("CBCC", engine->Cmd_Argv(1)))
	{
		CBaseCombatCharacter *pCombat = CBaseEntity_MyCombatCharacterPointer(pPlayer);
		//		CBaseCombatCharacter *pCombat = pPlayer->MyCombatCharacterPointer();
		if (pCombat)
		{
			type_ptr = (DWORD *)pCombat;
		}
		else
		{
			Msg("Failed to get Combat Character\n");
			return;
		}
	}
	else if (FStrEq("CBCW", engine->Cmd_Argv(1)))
	{
		CBaseCombatCharacter *pCombat = CBaseEntity_MyCombatCharacterPointer(pPlayer);
		//		CBaseCombatCharacter *pCombat = pPlayer->MyCombatCharacterPointer();
		if (!pCombat)
		{
			Msg("Failed to get combat character\n");
			return;
		}

		// Get pistol type
		CBaseCombatWeapon *pWeapon = CBaseCombatCharacter_Weapon_GetSlot(pCombat, 1);
		if (!pWeapon)
		{
			Msg("Failed to get weapon info\n");
			return;
		}

		type_ptr = (DWORD *) pWeapon;
	}
	else
	{
		Msg("Invalid 2nd arg\n");
		return;
	}

	char mangled_name[256];

	int index;

	if (engine->Cmd_Argc() < 4)
	{
		index = FindVFunc(type_ptr, engine->Cmd_Argv(2), NULL, mangled_name);
	}
	else
	{
		index = FindVFunc(type_ptr, engine->Cmd_Argv(2), engine->Cmd_Argv(3), mangled_name);
	}

	if (index == -1)
	{
		Msg("Did not find index :(\n");
		return;
	}

	Msg("Found Index [%i] [0x%x] [%s]\n", index, index, mangled_name);
}

CON_COMMAND(ma_autovfunc, "Debug Tool <player> <level>")
{

	player_t player;

	player.entity = NULL;

	if (!IsCommandIssuedByServerAdmin()) return;
	if (ProcessPluginPaused()) return;

	if (engine->Cmd_Argc() < 2)
	{
		Msg("Need more args :)\n");
		return;
	}

	bool	found_player = false;

	for (int i = 1; i <= max_players; i++)
	{
		player.index = i;
		if (!FindPlayerByIndex(&player)) continue;
		if (player.is_bot) continue;
		found_player = true;
	}

	if (!found_player)
	{
		for (int i = 1; i <= max_players; i++)
		{
			player.index = i;
			if (!FindPlayerByIndex(&player)) continue;
			found_player = true;
		}
	}

	// Whoever issued the commmand is authorised to do it.
	if (!found_player)
	{
		Msg("Need a target player to work the magic\n");
		return;
	}

	int level = Q_atoi(engine->Cmd_Argv(1));

	player_t *target_ptr = &player;

	CBaseEntity *pPlayer = target_ptr->entity->GetUnknown()->GetBaseEntity();
	CBasePlayer *pBase = (CBasePlayer *) pPlayer;

	DWORD *type_ptr;

	int index = -1;

	Msg("Errors starting with 'Missing ...' are usually not a problem\n");
	Msg("Any strings highlighted need correcting in gametypes.txt for this mod type\n");
	Msg("This only shows vfunc indexes that are currently wrong in the gametypes.txt file\n");

	if (level > 0)
	{
		type_ptr = (DWORD *) pPlayer;

		CheckVFunc(type_ptr, "CBasePlayer", "EyeAngles", "eye_angles", MANI_VFUNC_EYE_ANGLES);
		CheckVFunc(type_ptr, "CBaseEntity", "SetModelIndex", "set_model_index", MANI_VFUNC_SET_MODEL_INDEX);
		CheckVFunc(type_ptr, "CBaseAnimating", "Teleport", "teleport", MANI_VFUNC_TELEPORT);
		CheckVFunc(type_ptr, "CBasePlayer", "EyePosition", "eye_position", MANI_VFUNC_EYE_POSITION);
		CheckVFunc(type_ptr, "CBasePlayer", "GiveNamedItem", "give_item", MANI_VFUNC_GIVE_ITEM);
		// Dods and CS inherit from CBasePlayer
		if (gpManiGameType->IsGameType(MANI_GAME_CSS)) CheckVFunc(type_ptr, "CCSPlayer", "GiveNamedItem", "give_item", MANI_VFUNC_GIVE_ITEM);
		if (gpManiGameType->IsGameType(MANI_GAME_DOD)) CheckVFunc(type_ptr, "CDODPlayer", "GiveNamedItem", "give_item", MANI_VFUNC_GIVE_ITEM);

		CheckVFunc(type_ptr, "CBaseCombatCharacter", "MyCombatCharacterPointer", "my_combat_character", MANI_VFUNC_MY_COMBAT_CHARACTER);
		CheckVFunc(type_ptr, "CBaseAnimating", "GetVelocity", "get_velocity", MANI_VFUNC_GET_VELOCITY);

		CheckVFunc(type_ptr, "CBaseEntity", "GetDataDescMap", "map_desc", MANI_VFUNC_MAP);
		CheckVFunc(type_ptr, "CBasePlayer", "GetDataDescMap", "map_desc", MANI_VFUNC_MAP);
		if (gpManiGameType->IsGameType(MANI_GAME_CSS)) CheckVFunc(type_ptr, "CCSPlayer", "GetDataDescMap", "map_desc", MANI_VFUNC_MAP);
		if (gpManiGameType->IsGameType(MANI_GAME_DOD)) CheckVFunc(type_ptr, "CDODPlayer", "GetDataDescMap", "map_desc", MANI_VFUNC_MAP);

		type_ptr = (DWORD *) pBase;

		CheckVFunc(type_ptr, "CBaseAnimating", "Ignite", "ignite", MANI_VFUNC_IGNITE);
		CheckVFunc(type_ptr, "CBasePlayer", "Weapon_Drop", "weapon_drop", MANI_VFUNC_WEAPON_DROP);
		CheckVFunc(type_ptr, "CBasePlayer", "ProcessUsercmds", "user_cmds", MANI_VFUNC_USER_CMDS);
		CheckVFunc(type_ptr, "CBasePlayer", "CommitSuicide", "commit_suicide", MANI_VFUNC_COMMIT_SUICIDE);
		if (gpManiGameType->IsGameType(MANI_GAME_DOD)) CheckVFunc(type_ptr, "CDODPlayer", "CommitSuicide", "commit_suicide", MANI_VFUNC_COMMIT_SUICIDE);
	}

	if (level > 1)
	{
		CBaseCombatCharacter *pCombat = CBaseEntity_MyCombatCharacterPointer(pPlayer);
		type_ptr = (DWORD *) pCombat;

		CheckVFunc(type_ptr, "CBasePlayer", "RemovePlayerItem", "remove_player_item", MANI_VFUNC_REMOVE_PLAYER_ITEM);
		CheckVFunc(type_ptr, "CBaseCombatCharacter", "Weapon_GetSlot", "get_weapon_slot", MANI_VFUNC_GET_WEAPON_SLOT);
		if (gpManiGameType->IsGameType(MANI_GAME_CSS)) CheckVFunc(type_ptr, "CCSPlayer", "Weapon_Switch", "weapon_switch", MANI_VFUNC_WEAPON_SWITCH);
		CheckVFunc(type_ptr, "CBaseCombatCharacter", "GiveAmmo", "give_ammo", MANI_VFUNC_GIVE_AMMO);
	}

	if (level > 2)
	{
		CBaseCombatCharacter *pCombat = CBaseEntity_MyCombatCharacterPointer(pPlayer);
		CBaseCombatWeapon *pWeapon = CBaseCombatCharacter_Weapon_GetSlot(pCombat, 1);
		if (pWeapon)
		{

			type_ptr = (DWORD *) pWeapon;
			CheckVFunc(type_ptr, "CBaseCombatWeapon", "GetPrimaryAmmoType", "get_primary_ammo_type", MANI_VFUNC_GET_PRIMARY_AMMO_TYPE);
			CheckVFunc(type_ptr, "CBaseCombatWeapon", "GetSecondaryAmmoType", "get_secondary_ammo_type", MANI_VFUNC_GET_SECONDARY_AMMO_TYPE);
			CheckVFunc(type_ptr, "CBaseCombatWeapon", "GetName", "weapon_get_name", MANI_VFUNC_WEAPON_GET_NAME);
		}
	}
}

static	void CheckVFunc(DWORD *class_ptr, char *class_name, char *class_function, char *gametype_ptr, int vfunc_type)
{
	int index;
	char mangled_ptr[256]="";

	index = FindVFunc(class_ptr, class_name, class_function, mangled_ptr);

	if (index != -1)
	{
		if (gpManiGameType->vfunc_index[vfunc_type] != index)
		{
			Msg("\t\t\t\"%s\"\t\"%i\"\n", gametype_ptr, index);
		}
	}
	else
	{
		Msg("Missing %s::%s (Probably not a problem)\n", class_name, class_function);
	}
}

static	int		FindVFunc(DWORD *class_ptr, char *class_name, char *class_function, char *mangled_ptr)
{
        void    *handle;
        void    *var_address;
        Dl_info d;

        handle = dlopen(gpManiGameType->GetLinuxBin(), RTLD_NOW);

        if (handle == NULL)
        {
                Msg("Failed to open server image, error [%s]\n", dlerror());
                gpManiGameType->SetAdvancedEffectsAllowed(false);
        }
        else
        {
                for (int i = 0; i < 1000; i++)
                {
                        DWORD *FuncPtr = (DWORD *)(*(DWORD *)((*((DWORD *) class_ptr)) + (i * 4)));

                        int status = dladdr(FuncPtr, &d);

                        if (status)
                        {
							if (class_function)
							{
								if (Q_stristr(d.dli_sname, class_name) != NULL &&
									Q_stristr(d.dli_sname, class_function) != NULL)
								{
									// Found Match
									Q_strcpy(mangled_ptr, d.dli_sname);
									dlclose(handle);
									return i;
								}
							}
							else
							{
								// Just match one parameter
 								if (Q_stristr(d.dli_sname, class_name) != NULL)
								{
									// Found Match
									Q_strcpy(mangled_ptr, d.dli_sname);
									dlclose(handle);
									return i;
								}
							}
                       }
                }

                dlclose(handle);
        }

		return -1;
}


#endif




