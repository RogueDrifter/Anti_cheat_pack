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

This only detects godmode cheats that disable the damage callbacks completely. Yes, it is very limited.
I do plan to expand this one day.
*/

#include <a_samp>

#if defined _rAGM_Included
	#endinput
#endif

#define _rAGM_Included

#if !defined FILTERSCRIPT 

#define RAGM_MAX_SHOT_TICKS 0

forward rAGM_TogglePlayerControllable(playerid, bool:toggle);

forward CheckFinalResult(hitid);
forward ResetWarningTicks(playerid);

enum E_GODMODE_PLAYER_INFO
{
	bool:isPlayerFrozen, 

	delayTimerOne, 
	delayTimerTwo, 

	finalWarnings, 
	damageWarnings, 
	pausedTicks
};

static
	playerData[MAX_PLAYERS][E_GODMODE_PLAYER_INFO];

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

static bool:IsWeaponAmmoable(weaponid)
{
	return (22 <= weaponid <= 33);
}

static bool:IsPlayerPaused(playerid)
{
	return (gettime() > (playerData[playerid][pausedTicks]+2));
}

public OnPlayerUpdate(playerid)
{
	playerData[playerid][pausedTicks] = gettime();

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
		if(IsWeaponAmmoable(weaponid) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && GetPlayerState(hitid) == PLAYER_STATE_ONFOOT)
		{
			if(!IsPlayerPaused(hitid) && !IsPlayerPaused(playerid) && !IsPlayerNPC(hitid) && !playerData[hitid][isPlayerFrozen])
			{
				#if defined _FLaggersIncluded_
					if(!IsPlayerDesynced(hitid))
					{
						KillTimer(playerData[hitid][delayTimerOne]);
						if(IsPlayerLagging(hitid))
							playerData[hitid][delayTimerOne] = SetTimerEx("CheckFinalResult", 500, false, "i", hitid);
						else
							playerData[hitid][delayTimerOne] = SetTimerEx("CheckFinalResult", 950, false, "i", hitid);
					}
				#else
					KillTimer(playerData[hitid][delayTimerOne]);
					playerData[hitid][delayTimerOne] = SetTimerEx("CheckFinalResult", 950, false, "i", hitid);
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
	if(issuerid != INVALID_PLAYER_ID && IsPlayerConnected(issuerid) && IsWeaponAmmoable(weaponid))
	{
		KillTimer(playerData[playerid][delayTimerTwo]);
		playerData[playerid][finalWarnings]		= 0;
		playerData[playerid][damageWarnings] = gettime();
	}
	#if defined rAGM_OnPlayerTakeDamage
		return rAGM_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	playerData[playerid][isPlayerFrozen] = false;

	#if defined rAGM_OnPlayerSpawn	
		return rAGM_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	playerData[playerid][isPlayerFrozen]	= false;
	playerData[playerid][finalWarnings]		= 0;

	#if defined rAGM_OnPlayerConnect
		return rAGM_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	KillTimer(playerData[playerid][delayTimerTwo]);
	playerData[playerid][finalWarnings]		= 0;
	playerData[playerid][isPlayerFrozen]		= true;

	#if defined rAGM_OnPlayerDeath
		return rAGM_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public rAGM_TogglePlayerControllable(playerid, bool:toggle)
{
	playerData[playerid][isPlayerFrozen] = toggle;
	return TogglePlayerControllable(playerid, toggle);
}

public CheckFinalResult(hitid)
{
	if(!IsPlayerConnected(hitid)) return 0;
	if(gettime() > (playerData[hitid][damageWarnings]+ 2))
	{
		if(playerData[hitid][finalWarnings] <= RAGM_MAX_SHOT_TICKS)
		{
			KillTimer(playerData[hitid][delayTimerTwo]);
			playerData[hitid][delayTimerTwo] = SetTimerEx("ResetWarningTicks", 60*1000, false, "i", hitid);
			playerData[hitid][finalWarnings]++;
		}
		else
		{
			KillTimer(playerData[hitid][delayTimerTwo]);
			playerData[hitid][finalWarnings] = 0;
			#if defined OnPlayerGodmode
				OnPlayerGodmode(hitid, 0);
			#endif
		}
	}
	return 1;
}

public ResetWarningTicks(playerid) 
	return playerData[playerid][finalWarnings] = 0;

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
	
