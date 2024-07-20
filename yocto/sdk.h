#pragma once
//================================================================================================
#include <Windows.h>
#include <string_view>
#include <string>
#include <time.h>
#include <math.h>
#include <thread>
#include <intrin.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <cstdint>
#include <stdint.h>
#include <map>
#include "dt_common2.h"
#include "dt_recv2.h"
#include "vmt.h"
#include "vector.h"
//================================================================================================
#define WIN32_LEAN_AND_MEAN
#pragma optimize("gsy",on)

#define vmt_manager toolkit::vmt_manager
#define vmt_base_manager toolkit::vmt_base_manager

typedef void* (__cdecl* create_interface)(const char *, int *);
typedef void* (*create_interface_fn)(const char * name, int * returncode);
typedef unsigned long hfont;

#define get_proc_address(mod) GetProcAddress(api.get_module(mod), "CreateInterface")
#define get_base_entity ints.ent_list->get_client_entity
#define baseentity ssdk::c_base_entity

typedef float matrix3x4[3][4];

constexpr DWORD MASK_SHOT{0x46004003};
constexpr int ESP_HEIGHT{12};


#define deg2rad( x )		( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define rad2deg( x )		( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define square ( x )		( x*x )

typedef struct cscreensize_t
{
	int screen_height, screen_width;
} cscreensize;

extern cscreensize screen_size;
inline void**& get_vtable(void * inst, size_t offset = 0) { return *reinterpret_cast<void***>((size_t)inst + offset); }

inline const void** get_vtable(const void * inst, size_t offset = 0)
{
	return *reinterpret_cast<const void***>((size_t)inst + offset);
}

template <typename Fn>
Fn get_vfunc(const void * inst, size_t index, size_t offset = 0)
{
	return reinterpret_cast<Fn>(get_vtable(inst, offset)[index]);
}

//================================================================================================
namespace handle_structs
{
	class mat
	{
	public:
		typedef enum mat_var_flags
		{
			MATERIAL_VAR_DEBUG = (1 << 0),
			MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
			MATERIAL_VAR_NO_DRAW = (1 << 2),
			MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
			MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
			MATERIAL_VAR_VERTEXALPHA = (1 << 5),
			MATERIAL_VAR_SELFILLUM = (1 << 6),
			MATERIAL_VAR_ADDITIVE = (1 << 7),
			MATERIAL_VAR_ALPHATEST = (1 << 8),
			MATERIAL_VAR_ZNEARER = (1 << 10),
			MATERIAL_VAR_MODEL = (1 << 11),
			MATERIAL_VAR_FLAT = (1 << 12),
			MATERIAL_VAR_NOCULL = (1 << 13),
			MATERIAL_VAR_NOFOG = (1 << 14),
			MATERIAL_VAR_IGNOREZ = (1 << 15),
			MATERIAL_VAR_DECAL = (1 << 16),
			MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
			MATERIAL_VAR_UNUSED = (1 << 18),
			MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
			MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
			MATERIAL_VAR_TRANSLUCENT = (1 << 21),
			MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
			MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
			MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
			MATERIAL_VAR_ENVMAPMODE = (1 << 25),
			MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
			MATERIAL_VAR_HALFLAMBERT = (1 << 27),
			MATERIAL_VAR_WIREFRAME = (1 << 28),
			MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
			MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
			MATERIAL_VAR_VERTEXFOG = (1 << 31),
		} mat_var_flags_t;
	};

	struct client_class
	{
		BYTE _chPadding[8];
		char * name;
		RecvTable * table;
		client_class * nextclass;
		int classid;
	};

	struct manage_global
	{
		int speed_value;
	};

	struct view_setup
	{
		//ricoy
		char pad_0000[4]; //0x0000
		uint32_t N00000050; //0x0004
		uint32_t N00000051; //0x0008
		uint32_t N00000052; //0x000C
		uint32_t width; //0x0010
		uint32_t unscale_width; //0x0014
		uint32_t height; //0x0018
		uint32_t unscale_height; //0x001C
		bool N00000057; //0x0020
		char pad_0021[3]; //0x0021
		float N00000058; //0x0024
		float N00000059; //0x0028
		float N0000005A; //0x002C
		float N0000005B; //0x0030
		float m_fov; //0x0034
		float m_viewmodelfov; //0x0038
		char pad_003C[2052]; //0x003C
	};

	struct class_groups
	{
		enum groups
		{
			NONE = 0,
			STRONK,
			SPECIAL,
			INFECTED,
			PLAYER
		};
	};

	typedef struct player_info_s
	{
		char header[0x8];
	public:
		char name[32];
		int userid;
	private:
		char unused[0x150];
	} player_info_t;

	struct mstudiobbox_t
	{
		int bone;
		int group;
		vector bbmin;
		vector bbmax;
		int szhitboxnameindex;
		int m_iPad01[3];
		float m_flRadius;
		int m_iPad02[4];
	};

	struct mstudiohitboxset_t
	{
		int sznameindex;
		char* const get_name(void) const { return ((char*)this) + sznameindex; }
		int numhitboxes;
		int hitboxindex;
		mstudiobbox_t* get_hitbox(int i) const { return (mstudiobbox_t*)(((BYTE*)this) + hitboxindex) + i; };
	};

	struct mstudiobone_t
	{
		int sznameindex;
		char* const get_name(void) const { return ((char*)this) + sznameindex; }
		int parent;
		int bonecontroller[6];
		vector pos;
		float quat[4];
		vector rot;
		vector posscale;
		vector rotscale;
		matrix3x4 poseToBone;
		float qAlignment[4];
		int flags;
		int proctype;
		int procindex; // procedural rule
		mutable int physicsbone; // index into physically simulated bone
		void* get_procedure() const
		{
			if (procindex == 0) return NULL;
			return static_cast<void*>(((BYTE*)this) + procindex);
		};
		int surfacepropidx; // index into string tablefor property name
		char* const get_surface_props(void) const { return ((char*)this) + surfacepropidx; }
		int contents; // See BSPFlags.h for the contents flags
		int unused[8]; // remove as appropriate
	};

	struct studiohdr_t
	{
		int id;
		int version;
		int checksum;
		char name[64];
		int length;
		vector eyeposition;
		vector illumposition;
		vector hull_min;
		vector hull_max;
		vector view_bbmin;
		vector view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		mstudiobone_t* get_bone(int i) const { return (mstudiobone_t*)(((BYTE*)this) + boneindex) + i; };
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;

		mstudiohitboxset_t* get_hitbox_set(int i) const
		{
			return (mstudiohitboxset_t*)(((BYTE*)this) + hitboxsetindex) + i;
		}

		mstudiobbox_t* get_hitbox(int i, int set) const
		{
			mstudiohitboxset_t const * s = get_hitbox_set(set);

			if (!s)
				return NULL;

			return s->get_hitbox(i);
		}

		int GetHitboxCount(int set) const
		{
			mstudiohitboxset_t const * s = get_hitbox_set(set);
			if (!s)
				return 0;
			return s->numhitboxes;
		}

		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int localseqindex;
		mutable int activitylistversion;
		mutable int eventsindexed;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int numflexdesc;
		int flexdescindex;
		int numflexcontrollers;
		int flexcontrollerindex;
		int numflexrules;
		int flexruleindex;
		int numikchains;
		int ikchainindex;
		int nummouths;
		int mouthindex;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;
		int includemodelindex;
		mutable void * virtualModel;
		int szanimblocknameindex;
		int numanimblocks;
		int animblockindex;
		mutable void * animblockModel;
		int bonetablebynameindex;
		void * pVertexBase;
		void * pIndexBase;
		BYTE constdirectionallightdot;
		BYTE rootLOD;
		BYTE numAllowedRootLODs;
		BYTE unused[1];
		int unused4;
		int numflexcontrollerui;
		int flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		int unused3[1];
		int studiohdr2index;
		int unused2[1];
	};

	struct player_related
	{
		struct user_cmd
		{
			virtual ~user_cmd()
			{
			};

			std::int32_t command_number;
			std::int32_t tick_count;

			vector viewangles;
			float forwardmove;
			float sidemove;
			float upmove;
			std::int32_t buttons;

			BYTE impulse;

			std::int32_t weaponselect;
			std::int32_t weaponsubtype;
			std::int32_t random_seed;
			std::int16_t mousedx;
			std::int16_t mousedy;

			bool hasbeenpredicted;
		};

		enum player_controls
		{
			IN_ATTACK = (1 << 0),
			IN_JUMP = (1 << 1),
			IN_DUCK = (1 << 2),
			IN_FORWARD = (1 << 3),
			IN_BACK = (1 << 4),
			IN_USE = (1 << 5),
			IN_CANCEL = (1 << 6),
			IN_LEFT = (1 << 7),
			IN_RIGHT = (1 << 8),
			IN_MOVELEFT = (1 << 9),
			IN_MOVERIGHT = (1 << 10),
			IN_ATTACK2 = (1 << 11),
			IN_RUN = (1 << 12),
			IN_RELOAD = (1 << 13),
			IN_ALT1 = (1 << 14),
			IN_ALT2 = (1 << 15),
			IN_SCORE = (1 << 16),
			// Used by client.dll for when scoreboard is held down
			IN_SPEED = (1 << 17),
			// Player is holding the speed key
			IN_WALK = (1 << 18),
			// Player holding walk key
			IN_ZOOM = (1 << 19),
			// Zoom key for HUD zoom
			IN_WEAPON1 = (1 << 20),
			// weapon defines these bits
			IN_WEAPON2 = (1 << 21),
			// weapon defines these bits
			IN_BULLRUSH = (1 << 22),
		};

		enum player_flags
		{
			FL_ONGROUND = (1 << 0),
			FL_DUCKING = (1 << 1),
			FL_WATERJUMP = (1 << 2),
			FL_ONTRAIN = (1 << 3),
			FL_INWATER = (1 << 9),
		};
	};

	class global_vars
	{
	public:
		float real_time;
		int frame_count;
		float absolute_frame_time;
		float curtime;
		float frame_time;
		int max_clients;
		int tick_count;
		float interval_per_tick;
		float interpolation_amount;
		int simTicksThisFrame;
		int network_protocol;
		void * pSaveData;
		bool m_bClient;
		bool m_bRemoteClient;
	};

	struct class_ids
	{
		enum
		{
			CWorld = 260,
			CTerrorPlayer = 231,
			SurvivorBot = 274,
			CCSTeam = 50,
			CTerrorPlayerResource = 232,
			CVoteController = 252,
			CTerrorGameRulesProxy = 228,
			CBeamSpotlight = 32,
			CSprite = 177,
			CWeaponAmmoSpawn = 255,
			CWeaponSpawn = 259,
			CFuncPlayerInfectedClip = 86,
			CFuncSimpleLadder = 90,
			CRopeKeyframe = 156,
			C_BaseEntity = 15,
			CFogController = 75,
			CShadowControl = 160,
			CEnvTonemapController = 65,
			CEnvDetailController = 57,
			CWaterLODControl = 254,
			CTerrorViewModel = 233,
			CPhysicsProp = 127,
			CSteamJet = 180,
			CPistol = 131,
			CBasePropDoor = 21,
			CFuncAreaPortalWindow = 78,
			Infected = 263,
			CPostProcessController = 138,
			CParticleSystem = 124,
			CShotgun_Chrome = 161,
			CColorCorrection = 45,
			CFuncOccluder = 84,
			CFirstAidKit = 73,
			CDynamicProp = 52,
			CSurvivorPosition = 184,
			CBaseUpgradeItem = 29,
			CSpotlightEnd = 176,
			CPropDoorRotatingCheckpoint = 143,
			CBeam = 31,
			CFireSmoke = 70,
			CBreakable = 35,
			CSceneEntity = 158,
			Smoker = 269,
			CSmokerClaw = 166,
			CTongue = 245,
			Hunter = 262,
			CHunterClaw = 100,
			CCSRagdoll = 49,
			Jockey = 264,
			CJockeyClaw = 112,
			CBoomerClaw = 34,
			Charger = 99,
			CChargerClaw = 41,
			Spitter = 271,
			CSpitterClaw = 174,
			CSpitterProjectile = 175,
			CInsectSwarm = 104,
			Witch = 276,
			Tank = 275,
			CTankClaw = 186,
			Boomer = 0,
			CBaseCSGrenadeProjectile = 13,
			CMolotov = 118,
			CMolotovProjectile = 119,
			CInferno = 101,
			CEntityFlame = 55,
			CPipeBomb = 129,
			CPipeBombProjectile = 130,
			CItem_Adrenaline = 105,
			CAutoShotgun = 2,
			CTerrorMeleeWeapon = 230,
			CChainsaw = 39,
			CColaBottles = 44,
			CItemDefibrillator = 109,
			CFireworkCrate = 72,
			CGasCan = 94,
			CGnome = 95,
			CGrenadeLauncher = 96,
			CSniperRifle = 171,
			COxygenTank = 120,
			CPainPills = 121,
			CMagnumPistol = 116,
			CPropaneTank = 142,
			CPumpShotgun = 148,
			CFireCrackerBlast = 69,
			CAssaultRifle = 1,
			CRifle_AK47 = 152,
			CRifle_Desert = 153,
			CRifle_M60 = 154,
			CRifle_SG552 = 155,
			CShotgun_SPAS = 162,
			CSubMachinegun = 181,
			CSMG_MP5 = 164,
			CSMG_Silenced = 165,
			CSniper_AWP = 168,
			CSniper_Military = 169,
			CSniper_Scout = 170,
			CItemUpgradePackExplosive = 110,
			CItemUpgradePackIncendiary = 111,
			CItem_VomitJar = 106,
		};
	};

	struct file_weapon_info
	{
	private:
		void * vtable;
	public:
		bool bParsedScript;
		bool bLoadedHudElements;
		char szClassName[80];
		char szPrintName[80];
		char szViewModel[80];
		char szWorldModel[80];
		char szAnimationPrefix[16];
		std::int16_t pad1;
		std::int32_t iSlot;
		std::int32_t iPosition;
		std::int32_t iMaxClip1;
		std::int32_t iMaxClip2;
		std::int32_t iDefaultClip1;
		std::int32_t iDefaultClip12;
		std::int32_t iWeight;
		std::int32_t iRumble;
		bool bAutoSwitchTo;
		bool bAutoSwitchFrom;
		std::int32_t iFlags;
	};

	struct life_states
	{
		enum
		{
			LIFE_ALIVE,
			LIFE_DYING,
			LIFE_DEAD,
			LIFE_RESPAWNABLE,
			LIFE_DISCARDBODY,
		};
	};

	class move_data //no gamemovement
	{
	public:
		bool m_bFirstRunOfFunctions : 1;
		bool m_bGameCodeMovedPlayer : 1;
		bool m_bNoAirControl : 1;

		unsigned long m_nPlayerHandle;
		int m_nImpulseCommand;
		q_angle m_vecViewAngles;
		q_angle m_vecAbsViewAngles;
		int m_nButtons;
		int m_nOldButtons;
		float m_flForwardMove;
		float m_flSideMove;
		float m_flUpMove;

		float m_flMaxSpeed;
		float m_flClientMaxSpeed;

		vector m_vecVelocity;
		vector m_vecOldVelocity;
		float somefloat;
		q_angle m_vecAngles;
		q_angle m_vecOldAngles;

		float m_outStepHeight;
		vector m_outWishVel;
		vector m_outJumpVel;

		vector m_vecConstraintCenter;
		float m_flConstraintRadius;
		float m_flConstraintWidth;
		float m_flConstraintSpeedFactor;
		bool m_bConstraintPastRadius;

		void SetAbsOrigin(const vector & vec);
		const vector& GetAbsOrigin() const;

	private:
		vector m_vecAbsOrigin; // edict::origin
	};
};

//================================================================================================
namespace ssdk
{
	class i_collideable
	{
	public:
		virtual ~i_collideable()
		{
		};

		unsigned short get_solid_flags()
		{
			return get_vfunc<unsigned short(__thiscall*)(i_collideable *)>(this, 12)(this);
		}
	};

	//=======================================================================
	class chl_client
	{
	public:
		handle_structs::client_class* get_all_classes(void)
		{
			typedef handle_structs::client_class* (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 7)(this);
		}
	};

	//=======================================================================
	class c_base_entity
	{
		// unknown 0x0
		// renderable 0x4
		// networkable 0x8
	private:
		template <class T>
		T get_value(const int offset)
		{
			return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
		}

	public:
		i_collideable* get_collideable()
		{
			PVOID unknown = static_cast<PVOID>(this + 0x0);
			typedef i_collideable* (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(unknown, 3)(unknown);
		}

		void set_abs_origin(const vector & origin)
		{
			typedef void (__thiscall* ofunc)(PVOID, const vector &);
			return get_vfunc<ofunc>(this, 53)(this, origin);
		}

		void set_abs_angles(const q_angle & angles)
		{
			typedef void (__thiscall* ofunc)(PVOID, const q_angle &);
			return get_vfunc<ofunc>(this, 54)(this, angles);
		}

		handle_structs::studiohdr_t* get_mdl()
		{
			PVOID renderable = static_cast<PVOID>(this + 0x4); //i_clientrenderable
			typedef handle_structs::studiohdr_t* (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(renderable, 9)(renderable);
		}

		bool is_dormant()
		{
			PVOID networkable = static_cast<PVOID>(this + 0x8); //i_clientnetworkable
			typedef bool (__thiscall* OriginalFn)(PVOID);
			return get_vfunc<OriginalFn>(networkable, 7)(networkable);
		}

		int get_index()
		{
			PVOID networkable = static_cast<PVOID>(this + 0x8);
			typedef int (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(networkable, 8)(networkable);
		}

		bool setup_bones(matrix3x4 * pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
		{
			__asm
				{
				MOV EDI, this
				LEA ECX, DWORD PTR DS : [EDI + 0x4]
				MOV EDX, DWORD PTR DS : [ECX]
				PUSH currentTime
				PUSH boneMask
				PUSH nMaxBones
				PUSH pBoneToWorldOut
				CALL DWORD PTR DS : [EDX + 0xD]
				}
		}

		void get_render_bounds(vector & mins, vector & maxs)
		{
			PVOID render = static_cast<PVOID>(this + 0x4);
			typedef void (__thiscall* ofunc)(PVOID, vector &, vector &);
			get_vfunc<ofunc>(render, 14)(render, mins, maxs);
		}

		handle_structs::client_class* get_client_class()
		{
			PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8); // fucking cast.
			typedef handle_structs::client_class* (__thiscall* OriginalFn)(PVOID);
			return get_vfunc<OriginalFn>(pNetworkable, 1)(pNetworkable);
		}

		//========================= ================= =================== =============
		const handle_structs::class_groups::groups get_entity_group(c_base_entity * ent);
		const bool is_entity_valid();
		//========================= ================= =================== =============
	};

	//=======================================================================
	class render_view
	{
	public:
		void set_color_modulation(float r, float g, float b)
		{
			float color[3] = {r, g, b};
			return get_vfunc<void(__thiscall*)(render_view *, float [])>(this, 7)(this, color);
		}
	};

	//=======================================================================
	class material
	{
	public:
		void set_mat_var_flag(handle_structs::mat::mat_var_flags flag, bool on)
		{
			return get_vfunc<void(__thiscall*)(material *, handle_structs::mat::mat_var_flags, bool)>(this, 29)(
				this, flag, on);
		}

		void color_modulate(float r, float g, float b)
		{
			return get_vfunc<void(__thiscall*)(material *, float, float, float)>(this, 28)(this, r, g, b);
		}
	};

	//=======================================================================
	class material_system
	{
	public:
		material* find_mat(char const * pMaterialName, const char * pTextureGroupName, bool complain = true,
		                   const char * pComplainPrefix = 0)
		{
			return get_vfunc<material* (__thiscall*)(material_system *, const char *, const char *, bool, const char *)
			>(this, 71)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
		}
	};

	//=======================================================================
	class model_renderer
	{
	public:
		void forced_mat_override(material * newMaterial, int nOverrideType = 0)
		{
			return get_vfunc<void(__thiscall*)(model_renderer *, material *, int)>(this, 1)(
				this, newMaterial, nOverrideType);
		}
	};

	//=======================================================================
	class c_base_combat_weapon : public c_base_entity
	{
	public:
		bool has_ammo()
		{
			typedef bool (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 13)(this);
		}

		handle_structs::file_weapon_info* get_cswpn_data()
		{
			typedef handle_structs::file_weapon_info* (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 98)(this);
		}

		char* get_print_name()
		{
			typedef char* (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 113)(this);
		}

		// 98
	};

	//=======================================================================
	class i_move_helper
	{
	public:
		virtual void nigger_man() = 0;
		virtual void set_host(c_base_entity * host) = 0;
	};

	//=======================================================================
	class prediction
	{
	public:
		bool in_pred(int edx)
		{
			typedef bool (__thiscall* o)(PVOID, int);
			return get_vfunc<o>(this, 14)(this, edx);
		}

		void setup_move(c_base_entity * player, handle_structs::player_related::user_cmd * cmd, i_move_helper * helper,
		                handle_structs::move_data * move)
		{
			typedef void (__thiscall* o)(PVOID, c_base_entity *, handle_structs::player_related::user_cmd *,
			                             i_move_helper *, handle_structs::move_data *);
			get_vfunc<o>(this, 19)(this, player, cmd, helper, move);
		}

		void finish_move(c_base_entity * player, handle_structs::player_related::user_cmd * cmd,
		                 handle_structs::move_data * move)
		{
			typedef void (__thiscall* o)(PVOID, c_base_entity *, handle_structs::player_related::user_cmd *,
			                             handle_structs::move_data *);
			get_vfunc<o>(this, 20)(this, player, cmd, move);
		}
	};

	//=======================================================================
	class c_model_info
	{
	public:
		const char* get_mdl_name(DWORD * model)
		{
			typedef const char* (__thiscall* ofunc)(PVOID, DWORD *);
			return get_vfunc<ofunc>(this, 3)(this, model);
		}

		handle_structs::studiohdr_t* get_studio_model(handle_structs::studiohdr_t * model)
		{
			typedef handle_structs::studiohdr_t* (__thiscall* o_func)(PVOID, handle_structs::studiohdr_t *);
			return get_vfunc<o_func>(this, 30)(this, model);
		}
	};

	//=======================================================================
	class entity_list
	{
	public:
		virtual DWORD* get_client_networkable(int entindex) = 0;
		virtual DWORD* get_client_networkable_from_handle(HANDLE hEntity) = 0;
		virtual DWORD* get_client_unknown_from_handle(HANDLE hEntity) = 0;
		virtual c_base_entity* get_client_entity(int entindex) = 0;
		virtual c_base_entity* get_client_entity_from_handle(HANDLE hEntity) = 0;
		virtual int number_of_entities(bool include_non_networkable) = 0;
		virtual int get_highest_entity_index(void) = 0;
		virtual void set_max_entities(int maxents) = 0;
		virtual int get_max_entities() = 0;
	};

	//=======================================================================
	class engine_vgui
	{
	public:
	private:
	};

	//=======================================================================
	class player_info_manager
	{
	public:
		handle_structs::global_vars* get_global_vars()
		{
			typedef handle_structs::global_vars* (__thiscall* OriginalFn)(PVOID);
			return get_vfunc<OriginalFn>(this, 1)(this);
		}
	};

	//=======================================================================
	class panel
	{
	public:
		const char* get_name(unsigned int vguiPanel)
		{
			typedef const char* (__thiscall* ofunc)(PVOID, unsigned int);
			return get_vfunc<ofunc>(this, 36)(this, vguiPanel);
		}
	};

	//=======================================================================
	class surface
	{
	public:
		void draw_set_text_font(unsigned long font)
		{
			typedef void (__thiscall* OriginalFn)(PVOID, unsigned long);
			get_vfunc<OriginalFn>(this, 17)(this, font);
		}

		void draw_set_text_color(int r, int g, int b, int a)
		{
			typedef void (__thiscall* OriginalFn)(PVOID, int, int, int, int);
			get_vfunc<OriginalFn>(this, 19)(this, r, g, b, a);
		}

		void draw_set_text_pos(int x, int y)
		{
			typedef void (__thiscall* OriginalFn)(PVOID, int, int);
			get_vfunc<OriginalFn>(this, 20)(this, x, y);
		}

		void draw_print_text(const wchar_t * text, int textLen)
		{
			typedef void (__thiscall* OriginalFn)(PVOID, const wchar_t *, int, int);
			return get_vfunc<OriginalFn>(this, 22)(this, text, textLen, 0);
		}

		hfont create_font()
		{
			typedef hfont (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 63)(this);
		}

		void set_font(unsigned long & font, const char * windowsFontName, int tall, int weight, int blur, int scanlines,
		              int flags)
		{
			typedef void (__thiscall* OriginalFn)(PVOID, unsigned long, const char *, int, int, int, int, int, int,
			                                      int);
			get_vfunc<OriginalFn>(this, 64)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
		}

		void get_screen_size(int & width, int & height)
		{
			typedef void (__thiscall* OriginalFn)(PVOID, int &, int &);
			return get_vfunc<OriginalFn>(this, 35)(this, width, height);
		}
	};

	//=======================================================================
	class engine_client
	{
	public:
		void get_screen_size(int & width, int & height)
		{
			typedef void (__thiscall* ofunc)(PVOID, int &, int &);
			return get_vfunc<ofunc>(this, 5)(this, width, height);
		}

		int get_local_player(void)
		{
			typedef int (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 12)(this);
		}

		bool get_player_info(int ent_num, handle_structs::player_info_s * h)
		{
			return get_vfunc<bool(__thiscall*)(PVOID, int, handle_structs::player_info_s *)>(this, 8)(this, ent_num, h);
		}

		void get_viewangles(vector & va)
		{
			typedef void (__thiscall* ofunc)(PVOID, vector & va);
			return get_vfunc<ofunc>(this, 19)(this, va);
		}

		void set_viewangles(vector & va)
		{
			typedef void (__thiscall* ofunc)(PVOID, vector & va);
			return get_vfunc<ofunc>(this, 20)(this, va);
		}

		const matrix3x4& w2s_matrix(void)
		{
			typedef const matrix3x4& (__thiscall* ofunc)(PVOID);
			return get_vfunc<ofunc>(this, 37)(this);
		}

		void clientcmd_unrestricted(const char * chCommandString)
		{
			typedef void (__thiscall* ofunc)(PVOID, const char *);
			return get_vfunc<ofunc>(this, 108)(this, chCommandString);
		}
	};
};

//================================================================================================
class interfaces
{
public:
	ssdk::chl_client * chlclient;
	ssdk::c_model_info * model_info;
	ssdk::engine_client * engine;
	ssdk::panel * panels;
	ssdk::surface * isurface;
	ssdk::entity_list * ent_list;
	ssdk::engine_vgui * eng_vgui;
	ssdk::material * i_material;
	ssdk::render_view * renderview;
	ssdk::material_system * i_material_system;
	ssdk::model_renderer * i_model_renderer;
	ssdk::prediction * i_prediction;
	ssdk::i_move_helper * move_helper;
	ssdk::player_info_manager * pinfo_manager;
	handle_structs::global_vars * globals;
};

extern interfaces ints;

#include "c_netvars.h"
#include "draw.h"
#include "utils.h"
//================================================================================================
