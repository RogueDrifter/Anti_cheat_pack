/*
Anti fake kill by Rogue 2018/3/25
----
Last Updated April 5th.
-=-=-=-=-=-=-

Callbacks:
----------

OnPlayerFakeKill(playerid, spoofedid, spoofedreason, faketype);
-------------
playerid = the cheater who did a fake kill action
spoofedid = the player alleged to have killed the cheater
spoofedreason = the weapon used in the process (spoofed by the cheater)
faketype 1 = normal fake kill (one time)
faketype 2 = spammed fake kill (must kick or ban instantly)
-=-=-=-=-=--=-=-=
*/

#if !defined FILTERSCRIPT 

#if defined _rAntiFakeKill_Included
	#endinput
#endif

#define _rAntiFakeKill_Included
#define FKI_MAX_ERROR_WARNS 3

#include <a_samp>

#if defined OnPlayerFakeKill
	forward OnPlayerFakeKill(playerid, spoofedid, spoofedreason, faketype);
#endif

#if defined FKI_OnPlayerDisconnect
	forward FKI_OnPlayerDisconnect(playerid, reason);
#endif

#if defined FKI_OnPlayerTakeDamage
	forward FKI_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
#endif

#if defined FKI_OnPlayerGiveDamage
	forward FKI_OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart);
#endif

#if defined FKI_OnPlayerWeaponShot
	forward FKI_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

#if defined FKI_OnPlayerDeath
	forward FKI_OnPlayerDeath(playerid, killerid, reason);
#endif

#if defined FKI_OnPlayerSpawn
	forward FKI_OnPlayerSpawn(playerid);
#endif

enum FKI_PLAYER_DATA
{
	bool:FkiDmgChk,
	bool:FkiSpwnChk,
	bool:FkiJustDied,

	FkiWTicks,

	FkiDeTimer
}

static 
	FKI_Player[MAX_PLAYERS][FKI_PLAYER_DATA];

forward FKI_RJDead(playerid);
public FKI_RJDead(playerid) return FKI_Player[playerid][FkiJustDied] = false;

public OnPlayerDisconnect(playerid, reason)
{
	FKI_Player[playerid][FkiSpwnChk]= false;
	FKI_Player[playerid][FkiDmgChk] = false;
	FKI_Player[playerid][FkiWTicks] = 0;
	#if defined FKI_OnPlayerDisconnect
		return FKI_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if(!FKI_Player[playerid][FkiSpwnChk] || FKI_Player[playerid][FkiJustDied])
	{
		CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 2);
		return 0;
	}
	if(!FKI_Player[playerid][FkiDmgChk] && killerid != INVALID_PLAYER_ID  && IsPlayerConnected(killerid))
	{
		if(!IsKillFalse(reason)) 
		{
			#if defined FKI_OnPlayerDeath
				return FKI_OnPlayerDeath(playerid, killerid, reason);
			#else
				return 1;
			#endif
		}
		if(GetPlayerState(killerid) == 2 && !FKI_IsAPlane(GetPlayerVehicleID(playerid)))
		{
			static Float:TPX, Float:TPY, Float:TPZ;
			GetPlayerPos(killerid, TPX, TPY, TPZ);
			if(!IsPlayerInRangeOfPoint(playerid, 5.0, TPX, TPY, TPZ))
			{
				CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 1);
				return 0;
			}
		}
		else if(!FKI_IsWeaponValid(reason) || reason == 0)
		{
			FKI_Player[playerid][FkiWTicks]++;
			if(FKI_Player[playerid][FkiWTicks] == FKI_MAX_ERROR_WARNS) 
			{
				FKI_Player[playerid][FkiWTicks] = 0;
				CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 1);
			}
		}
		else CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 1);
	}
	KillTimer(FKI_Player[playerid][FkiDeTimer]);
	FKI_Player[playerid][FkiDeTimer] = SetTimerEx("FKI_RJDead", 5000, false, "i", playerid);

	FKI_Player[playerid][FkiJustDied] = true;
	FKI_Player[playerid][FkiSpwnChk] = false;
	FKI_Player[playerid][FkiDmgChk] = false;
	#if defined FKI_OnPlayerDeath
		return FKI_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart)
{
	if(damagedid != INVALID_PLAYER_ID && IsPlayerConnected(damagedid))
	{
		FKI_Player[damagedid][FkiDmgChk] = true;
	}
	#if defined FKI_OnPlayerGiveDamage
		return FKI_OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart);
	#else
		return 1;
	#endif
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if(issuerid != INVALID_PLAYER_ID && IsPlayerConnected(issuerid))
	{
		FKI_Player[playerid][FkiDmgChk] = true;
	}
	#if defined FKI_OnPlayerTakeDamage
		return FKI_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
	#else
		return 1;
	#endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	switch(hittype)
	{
		case BULLET_HIT_TYPE_PLAYER:
		{
			FKI_Player[hitid][FkiDmgChk] = true;
		}
		case BULLET_HIT_TYPE_VEHICLE:
		{
			#if defined foreach
				foreach(new i: Player)
				{
					if(GetPlayerVehicleID(i) == hitid) FKI_Player[i][FkiDmgChk] = true;
				}
			#else
				for(new i, j = GetPlayerPoolSize(); i <= j; i++)
				{
					if(GetPlayerVehicleID(i) == hitid) FKI_Player[i][FkiDmgChk] = true;
				}
			#endif
		}
	}
	#if defined FKI_OnPlayerWeaponShot
		return FKI_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	FKI_Player[playerid][FkiSpwnChk] = true;
	#if defined FKI_OnPlayerSpawn
		return FKI_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

static FKI_IsWeaponValid(Weapon) 
	return ( 0 <= Weapon <= 18 || 22 <= Weapon <= 46   );

static FKI_IsAPlane(vehicleid)
{
	switch(GetVehicleModel(vehicleid))
	{
		case 592, 577, 511, 512, 593, 520, 553, 476, 519, 460, 513, 464: return 1;
		case 548, 425, 417, 487, 488, 497, 563, 447, 469, 465, 501: return 1; 
	}
	return 0;
}

static IsKillFalse(killid)
{
	switch(killid)
	{
		case 54, 255, 53, 51, 47: return 1;
	}
	return 0;
}
#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn FKI_OnPlayerSpawn

#if defined _ALS_OnPlayerDeath
	#undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath FKI_OnPlayerDeath

#if defined _ALS_OnPlayerTakeDamage
	#undef OnPlayerTakeDamage
#else
	#define _ALS_OnPlayerTakeDamage
#endif

#define OnPlayerTakeDamage FKI_OnPlayerTakeDamage

#if defined _ALS_OnPlayerGiveDamage
	#undef OnPlayerGiveDamage
#else
	#define _ALS_OnPlayerGiveDamage
#endif

#define OnPlayerGiveDamage FKI_OnPlayerGiveDamage

#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot FKI_OnPlayerWeaponShot

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect FKI_OnPlayerDisconnect

#endif
