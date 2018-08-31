/* Anti godmode by RogueDrifter created August 30th 

Callback:
OnPlayerGodmode(playerid, gmtype);

gmtype 0 = on foot
gmtype 1 = in car (not added yet).

-Cases excluded:
1- Frozen
2- Paused
3- Kill shot
4- Npcs
5- Player state
*/

#include <a_samp>

#if defined _rAGM_Included
	#endinput
#endif

#define _rAGM_Included

#if !defined FILTERSCRIPT 

#define RAGM_MAX_SHOT_TICKS 0

forward rAGM_TogglePlayerControllable(playerid, bool:toggle);
forward rAGM_FinalCheck(hitid);
forward rAGM_ResetTicks(playerid);

enum RAGM_PLAYER_INFO
{
	bool:RagPFrozen,

	RagPTimer,
	RagPRTimer,

	RagPTicks,
	RagDmgTicks,
	RagPauseTicks
};

static
	rAGM_Player[MAX_PLAYERS][RAGM_PLAYER_INFO];

#if defined rAGM_OnPlayerUpdate
	forward rAGM_OnPlayerUpdate(playerid);
#endif

#if defined rAGM_OnPlayerWeaponShot
	forward rAGM_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

#if defined rAGM_OnPlayerTakeDamage
	forward rAGM_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
#endif

#if defined rAGM_OnPlayerConnect
	forward rAGM_OnPlayerConnect(playerid);
#endif

#if defined rAGM_OnPlayerSpawn
	forward rAGM_OnPlayerSpawn(playerid);
#endif

#if defined rAGM_OnPlayerDeath
	forward rAGM_OnPlayerDeath(playerid, killerid, reason);
#endif

#if defined OnPlayerGodmode
	forward OnPlayerGodmode(playerid, gmtype);
#endif

public OnPlayerUpdate(playerid)
{
	rAGM_Player[playerid][RagPauseTicks] = GetTickCount();

	#if defined rAGM_OnPlayerUpdate
		return rAGM_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(hittype == BULLET_HIT_TYPE_PLAYER)
	{
		if(rAGM_IsWeaponAmmoable(weaponid) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && GetPlayerState(hitid) == PLAYER_STATE_ONFOOT)
		{
			if(!rAGM_IsPlayerPaused(hitid) && !rAGM_IsPlayerPaused(playerid) && !IsPlayerNPC(hitid) && !rAGM_Player[hitid][RagPFrozen])
			{
				#if defined _FLaggersIncluded_
					if(!IsPlayerDesynced(hitid))
					{
						KillTimer(rAGM_Player[hitid][RagPTimer]);
						if(IsPlayerLagging(hitid))
							rAGM_Player[hitid][RagPTimer] = SetTimerEx("rAGM_FinalCheck", 500, false, "i", hitid);
						else
							rAGM_Player[hitid][RagPTimer] = SetTimerEx("rAGM_FinalCheck", 950, false, "i", hitid);
					}
				#else
					KillTimer(rAGM_Player[hitid][RagPTimer]);
					rAGM_Player[hitid][RagPTimer] = SetTimerEx("rAGM_FinalCheck", 950, false, "i", hitid);
				#endif
			}
		}
	}
	#if defined rAGM_OnPlayerWeaponShot
		return rAGM_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#else
		return 1;
	#endif
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if(issuerid != INVALID_PLAYER_ID && IsPlayerConnected(issuerid) && rAGM_IsWeaponAmmoable(weaponid))
	{
		KillTimer(rAGM_Player[playerid][RagPRTimer]);
		rAGM_Player[playerid][RagPTicks]		= 0;
		rAGM_Player[playerid][RagDmgTicks] = GetTickCount();
	}
	#if defined rAGM_OnPlayerTakeDamage
		return rAGM_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	rAGM_Player[playerid][RagPFrozen] = false;

	#if defined rAGM_OnPlayerSpawn	
		return rAGM_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	rAGM_Player[playerid][RagPFrozen]	= false;
	rAGM_Player[playerid][RagPTicks]		= 0;

	#if defined rAGM_OnPlayerConnect
		return rAGM_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	KillTimer(rAGM_Player[playerid][RagPRTimer]);
	rAGM_Player[playerid][RagPTicks]		= 0;
	rAGM_Player[playerid][RagPFrozen]		= true;

	#if defined rAGM_OnPlayerDeath
		return rAGM_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public rAGM_TogglePlayerControllable(playerid, bool:toggle)
{
	rAGM_Player[playerid][RagPFrozen] = toggle;
	return TogglePlayerControllable(playerid, toggle);
}

public rAGM_FinalCheck(hitid)
{
	if(!IsPlayerConnected(hitid)) return 0;
	if(GetTickCount() > (rAGM_Player[hitid][RagDmgTicks]+ 2000))
	{
		if(rAGM_Player[hitid][RagPTicks] <= RAGM_MAX_SHOT_TICKS)
		{
			KillTimer(rAGM_Player[hitid][RagPRTimer]);
			rAGM_Player[hitid][RagPRTimer] = SetTimerEx("rAGM_ResetTicks", 60*1000, false, "i", hitid);
			rAGM_Player[hitid][RagPTicks]++;
		}
		else
		{
			KillTimer(rAGM_Player[hitid][RagPRTimer]);
			rAGM_Player[hitid][RagPTicks] = 0;
			#if defined OnPlayerGodmode
				OnPlayerGodmode(hitid, 0);
			#endif
		}
	}
	return 1;
}

public rAGM_ResetTicks(playerid) return rAGM_Player[playerid][RagPTicks] = 0;

static rAGM_IsWeaponAmmoable(weaponid)
{
	return (22 <= weaponid <= 33);
}

static rAGM_IsPlayerPaused(playerid)
{
	return (GetTickCount() > (rAGM_Player[playerid][RagPauseTicks]+1500));
}

#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot rAGM_OnPlayerWeaponShot

#if defined _ALS_OnPlayerDeath
	#undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath rAGM_OnPlayerDeath

#if defined _ALS_OnPlayerTakeDamage
	#undef OnPlayerTakeDamage
#else
	#define _ALS_OnPlayerTakeDamage
#endif

#define OnPlayerTakeDamage rAGM_OnPlayerTakeDamage

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate rAGM_OnPlayerUpdate

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect rAGM_OnPlayerConnect

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn rAGM_OnPlayerSpawn

#if defined _ALS_TogglePlayerControllable
  #undef TogglePlayerControllable
#else
	#define _ALS_TogglePlayerControllable
#endif

#define TogglePlayerControllable rAGM_TogglePlayerControllable

#else

stock rAGMF_TogglePlayerControllable(playerid, bool:toggle)
	return CallRemoteFunction("rAGM_TogglePlayerControllable", "ii", playerid, toggle);

#if defined _ALS_TogglePlayerControllable
  #undef TogglePlayerControllable
#else
	#define _ALS_TogglePlayerControllable
#endif

#define TogglePlayerControllable rAGMF_TogglePlayerControllable

#endif
	
