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
faketype 1 = normal fake kill (one time) - Detected when a player dies without getting hit or when killed by a vehicle from too far that isnt a plane
faketype 2 = spammed fake kill (must kick or ban instantly) - Detected when a player dies before spawning or dies too many times in a row or uses a spoofed invalid parameter
-=-=-=-=-=--=-=-=
*/

#if !defined FILTERSCRIPT 

#if defined _rAntiFakeKill_Included
	#endinput
#endif

#define _rAntiFakeKill_Included
#define FKI_MAX_ERROR_WARNS 2

#include <a_samp>

forward FKI_SetPlayerHealth(playerid, Float:health);

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

enum E_ANTI_FAKE_KILL
{
	bool:damageCheck, 
	bool:spawnCheck, 
	bool:justDied, 
	bool:healthImmune, 

	warningTicks, 

	deathRefresher
}

static 
	playerData[MAX_PLAYERS][E_ANTI_FAKE_KILL];

static bool:IsReasonValid(reason) 
	return ( 0 <= reason <= 18 || 22 <= reason <= 46   );

static bool:IsReasonDifferentlyValid(reason) //Helicopter blades and Explosion
{
	switch(reason)
	{
		case 50, 51: return true;
	}
	return false;
}

static bool:IsModelAPlane(vehicleid)
{
	switch(GetVehicleModel(vehicleid))
	{
		case 592, 577, 511, 512, 593, 520, 553, 476, 519, 460, 513, 464: return true;
		case 548, 425, 417, 487, 488, 497, 563, 447, 469, 465, 501: return true; 
	}
	return false;
}

static bool:IsReasonIgnoreable(reason)
{
	switch(reason)
	{
		case 54, 255, 53: return true;
	}
	return false;
}

forward FakeKillJustDied(playerid);
public FakeKillJustDied(playerid) 
	return playerData[playerid][justDied] = false;


forward FakeKillImmunity(playerid);
public FakeKillImmunity(playerid) 
	return 	playerData[playerid][healthImmune] = false;

public FKI_SetPlayerHealth(playerid, Float:health)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(health == 0.0) 
	{
		playerData[playerid][healthImmune] = true;
		SetTimerEx("FakeKillImmunity", 3000, false, "i", playerid);
	}
	return SetPlayerHealth(playerid, health);
}

public OnPlayerDisconnect(playerid, reason)
{
	playerData[playerid][spawnCheck]= false;
	playerData[playerid][damageCheck] = false;
	playerData[playerid][healthImmune] = false;
	playerData[playerid][justDied] = false;

	playerData[playerid][warningTicks] = 0;
	#if defined FKI_OnPlayerDisconnect
		return FKI_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if(!playerData[playerid][spawnCheck] || playerData[playerid][justDied])
	{
		if(!playerData[playerid][healthImmune]) 
			return CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 2);
	}
	if(!playerData[playerid][damageCheck] && killerid != INVALID_PLAYER_ID  && IsPlayerConnected(killerid))
	{
		if(IsReasonIgnoreable(reason)) 
		{
			if(IsPlayerConnected(killerid))
			{
				if(!playerData[playerid][healthImmune]) 
					return CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 2);
			}
			#if defined FKI_OnPlayerDeath
				return FKI_OnPlayerDeath(playerid, killerid, reason);
			#else
				return 1;
			#endif
		}
		if(GetPlayerState(killerid) == PLAYER_STATE_DRIVER && !IsModelAPlane(GetPlayerVehicleID(playerid)) && reason == WEAPON_VEHICLE)
		{
			static Float:TPX, Float:TPY, Float:TPZ;
			GetPlayerPos(killerid, TPX, TPY, TPZ);
			if(!IsPlayerInRangeOfPoint(playerid, 5.0, TPX, TPY, TPZ))
			{
				if(!playerData[playerid][healthImmune]) 
					return CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 1);
			}
		}
		if(IsReasonValid(reason))
		{
			playerData[playerid][warningTicks]++;
			if(playerData[playerid][warningTicks] == FKI_MAX_ERROR_WARNS) 
			{
				playerData[playerid][warningTicks] = 0;
				if(!playerData[playerid][healthImmune]) 
					return CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 1);
			}
		}
		else 
		{
			if(!playerData[playerid][healthImmune])	
				return CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 1);
		}
	}
	if(!IsReasonValid(reason) && !IsReasonIgnoreable(reason) && !IsReasonDifferentlyValid(reason) && reason != WEAPON_VEHICLE) 
		return CallLocalFunction("OnPlayerFakeKill", "iiii", playerid, killerid, reason, 2);

	KillTimer(playerData[playerid][deathRefresher]);
	playerData[playerid][deathRefresher] = SetTimerEx("FakeKillJustDied", 5000, false, "i", playerid);

	playerData[playerid][justDied] = true;
	playerData[playerid][spawnCheck] = false;
	playerData[playerid][damageCheck] = false;
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
		playerData[damagedid][damageCheck] = true;
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
		playerData[playerid][damageCheck] = true;
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
			playerData[hitid][damageCheck] = true;
		}
		case BULLET_HIT_TYPE_VEHICLE:
		{
			#if defined foreach
				foreach(new i: Player)
				{
					if(GetPlayerVehicleID(i) == hitid) playerData[i][damageCheck] = true;
				}
			#else
				for(new i, j = GetPlayerPoolSize(); i <= j; i++)
				{
					if(GetPlayerVehicleID(i) == hitid) playerData[i][damageCheck] = true;
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
	playerData[playerid][spawnCheck] = true;
	#if defined FKI_OnPlayerSpawn
		return FKI_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

#if defined _ALS_SetPlayerHealth
	#undef SetPlayerHealth
#else
	#define _ALS_SetPlayerHealth
#endif

#define SetPlayerHealth FKI_SetPlayerHealth

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

#else //If its a filterscript

stock FKIS_SetPlayerHealth(playerid, Float:health)
	return CallRemoteFunction("FKI_SetPlayerHealth", "if", playerid, health);

#if defined _ALS_SetPlayerHealth
	#undef SetPlayerHealth
#else
	#define _ALS_SetPlayerHealth
#endif

#define SetPlayerHealth FKIS_SetPlayerHealth

#endif
