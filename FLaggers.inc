/*
Laggers filter by Rogue 2018/2/21.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

LAST UPDATED: April 18th.
-=-=-=-=-=-=-=-=-=-=-=-=

Callbacks:
-=-=-=-=-=
OnPlayerLagout(playerid, lagtype, ping);
-=-=-=-=-
lagtype 1 = ping lag.
lagtype 2 = packet-loss sync.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Inner functions:
-=-=-=-=-=-=-=-=
IsPlayerLagging(playerid);
IsPlayerDesynced(playerid);
-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Defeinitions to use:
-=-=-=-=-=-=-=-=-=-=
#define FL_MAX_WAIT_TIME		[number]			( max wait time for the lags check  )
#define FL_MAX_LAG_PING		 [number]			( max ping to set a warning		 )
#define FL_MAX_WARNINGS		 [number]			( max warnings to call the callback )
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#if defined _FLaggersIncluded_
	#endinput
#endif

#define _FLaggersIncluded_

#include <a_samp>

#if !defined FILTERSCRIPT

#define FL_INVALID_PLAYER_STATS -1
#define FL_ADDED_VALUE	  80
#define FL_MAX_DESYNC_WARNS 3

#define FL_KICK_TYPE_PING_LAG 1
#define FL_KICK_TYPE_DESYNC_LAG 2

#if !defined FL_MAX_LAG_PING
	#define FL_MAX_LAG_PING 450
#endif

#if !defined FL_MAX_WAIT_TIME
	#define FL_MAX_WAIT_TIME 3
#endif

#if !defined FL_MAX_WARNINGS
	#define FL_MAX_WARNINGS 3
#endif

forward FL_GivePlayerWeapon(playerid, weaponid, ammo);
forward FL_SetPlayerAmmo(playerid, weaponid, ammo);
forward FL_ResetPlayerWeapons(playerid);

forward FL_Cooloff(playerid);
forward FL_FixSBug(playerid);
forward FL_CheckStats(playerid);

#if defined OnPlayerLagout
	forward OnPlayerLagout(playerid, lagtype, ping);
#endif

#if defined FL_OnPlayerConnect
	forward FL_OnPlayerConnect(playerid);
#endif

#if defined FL_OnPlayerDisconnect
	forward FL_OnPlayerDisconnect(playerid, reason);
#endif

#if defined FL_OnPlayerSpawn
	forward FL_OnPlayerSpawn(playerid);
#endif

#if defined FL_OnPlayerUpdate
	forward FL_OnPlayerUpdate(playerid);
#endif

#if defined FL_OnPlayerDeath
	forward FL_OnPlayerDeath(playerid, killerid, reason);
#endif

enum FLAGGERS_PLAYER_INFO
{
	bool:FLdesynced,
	bool:FLlagger,
	bool:FLspawn,

	FLUpdated,
	FLwarns,
	FLAmmo,
	FLPing,
	FLDwarns,

	FLSTimer,
	FLTimer
};

new 
	FLaggers_Player[MAX_PLAYERS][FLAGGERS_PLAYER_INFO];

//Inner functions:
stock FL_GetWeaponSlot(weaponid) 
{ 
	switch(weaponid) 
	{ 
		case 0..1:   return 0;
		case 2..9:   return 1;
		case 22..24: return 2;
		case 25..27: return 3;
		case 28..29: return 4;
		case 32:	 return 4;
		case 30..31: return 5;
		case 33..34: return 6;
		case 35..38: return 7;
		case 16..18: return 8;
		case 39:	 return 8;
		case 41..43: return 9;
		case 10..15: return 10;
		case 44..46: return 11;
		case 40:	 return 12;
	} 
	return 0; 
}

stock IsPlayerLagging(playerid)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	return (FLaggers_Player[playerid][FLwarns] > 1);
}
stock IsPlayerDesynced(playerid) 
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	return FLaggers_Player[playerid][FLdesynced];
}
//Hooked functions:
public FL_GivePlayerWeapon(playerid, weaponid, ammo)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(FL_GetWeaponSlot(weaponid) == 0) 
	{
		switch(weaponid)
		{
			case 0:
			{
				GivePlayerWeapon(playerid, weaponid, ammo);
				SetPlayerAmmo(playerid, 0, FLaggers_Player[playerid][FLAmmo]);
			}
			case 1:
			{
				GivePlayerWeapon(playerid, weaponid, ammo);
				SetPlayerAmmo(playerid, 0, FLaggers_Player[playerid][FLAmmo]);
			}
		}
	}

	else GivePlayerWeapon(playerid, weaponid, ammo);
	return 1;
}

public FL_SetPlayerAmmo(playerid, weaponid, ammo)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	return (FL_GetWeaponSlot(weaponid) == 0) ? SetPlayerAmmo(playerid, 0, FLaggers_Player[playerid][FLAmmo]) : SetPlayerAmmo(playerid, weaponid, ammo);
}

public FL_ResetPlayerWeapons(playerid)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	ResetPlayerWeapons(playerid);
	
	return SetPlayerAmmo(playerid, 0, FLaggers_Player[playerid][FLAmmo]);
}

//Hooked callbacks:
public OnPlayerUpdate(playerid)
{
	FLaggers_Player[playerid][FLUpdated] = GetTickCount();

	#if defined FL_OnPlayerUpdate
		return FL_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	FLaggers_Player[playerid][FLPing]	 = FL_INVALID_PLAYER_STATS;

	FLaggers_Player[playerid][FLTimer]	= SetTimerEx("FL_CheckStats", FL_MAX_WAIT_TIME*1000, true, "i", playerid);

	#if defined FL_OnPlayerConnect
		return FL_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid, reason)
{
	FLaggers_Player[playerid][FLdesynced] = false;
	FLaggers_Player[playerid][FLlagger]   = false;
	FLaggers_Player[playerid][FLspawn]	= false;

	FLaggers_Player[playerid][FLAmmo]	 = 0;
	FLaggers_Player[playerid][FLwarns]	= 0;
	FLaggers_Player[playerid][FLDwarns]   = 0;

	KillTimer(FLaggers_Player[playerid][FLTimer]);

	#if defined FL_OnPlayerDisconnect
		return FL_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	KillTimer(FLaggers_Player[playerid][FLSTimer]);
	FLaggers_Player[playerid][FLSTimer] = SetTimerEx("FL_FixSBug", 3000, false, "i", playerid);

	#if defined FL_OnPlayerSpawn
		return FL_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	FLaggers_Player[playerid][FLspawn] = false;

	#if defined FL_OnPlayerDeath
		return FL_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

//Inner callbacks:
public FL_FixSBug(playerid)
{
	SetPlayerAmmo(playerid, 0, FLaggers_Player[playerid][FLAmmo]);
	FLaggers_Player[playerid][FLspawn] = true;

	return 1;
}

public FL_Cooloff(playerid) 
	return  FLaggers_Player[playerid][FLDwarns]   = 0;

public FL_CheckStats(playerid)
{
	new FL_PING = GetPlayerPing(playerid);

	if(FLaggers_Player[playerid][FLspawn] &&  GetTickCount() < (FLaggers_Player[playerid][FLUpdated]+1500))
	{
		if(FLaggers_Player[playerid][FLwarns] < FL_MAX_WARNINGS)
		{
			//Ping filter:
			if(FL_PING >= FL_MAX_LAG_PING && FLaggers_Player[playerid][FLPing] == FL_INVALID_PLAYER_STATS)
			{
				FLaggers_Player[playerid][FLPing] = FL_PING;
			}

			else if(FL_PING >= FLaggers_Player[playerid][FLPing] && FLaggers_Player[playerid][FLPing] != FL_INVALID_PLAYER_STATS)
			{
				FLaggers_Player[playerid][FLwarns]++, 
				FLaggers_Player[playerid][FLPing] = FL_PING;
			}

			else if(FLaggers_Player[playerid][FLPing] != FL_INVALID_PLAYER_STATS && FL_PING + FL_ADDED_VALUE < FLaggers_Player[playerid][FLPing]) 
			{
				if(FLaggers_Player[playerid][FLwarns] > 0 ) FLaggers_Player[playerid][FLwarns]--;
				FLaggers_Player[playerid][FLPing] = FL_INVALID_PLAYER_STATS;
			}

			else if(FLaggers_Player[playerid][FLPing] != FL_INVALID_PLAYER_STATS) 
			{
				FLaggers_Player[playerid][FLPing] = FL_INVALID_PLAYER_STATS;
			}
		}

		//Final-Stage filter:
		else if(FLaggers_Player[playerid][FLwarns] >= FL_MAX_WARNINGS)
		{
			FLaggers_Player[playerid][FLwarns] = 0;
			#if defined OnPlayerLagout
				OnPlayerLagout(playerid, 1, FL_PING);
			#endif
		}
	
		//Desync filter:
		if(FLaggers_Player[playerid][FLDwarns] < FL_MAX_DESYNC_WARNS)
		{
			new
				FLaggersWep,
				FlaggersAmmo;

			GetPlayerWeaponData(playerid, 0, FLaggersWep, FlaggersAmmo);

			switch(FLaggers_Player[playerid][FLAmmo])
			{
				case 0:
				{
					FLaggers_Player[playerid][FLAmmo] = 1;
					SetPlayerAmmo(playerid, FLaggersWep, 1);
				}
				case 1:
				{
					if(FlaggersAmmo != FLaggers_Player[playerid][FLAmmo])
					{
						if(FLaggers_Player[playerid][FLDwarns] == 0) SetTimerEx("FL_Cooloff", 50000, false, "i", playerid);
						FLaggers_Player[playerid][FLDwarns]++;
						SetPlayerAmmo(playerid, FLaggersWep, FLaggers_Player[playerid][FLAmmo]);
					}
					FLaggers_Player[playerid][FLAmmo] = 0;
					SetPlayerAmmo(playerid, FLaggersWep, 0);
				}
			}
		}

		else if(FLaggers_Player[playerid][FLDwarns] >= FL_MAX_DESYNC_WARNS && !FLaggers_Player[playerid][FLdesynced])
		{
			FLaggers_Player[playerid][FLdesynced] = true;
			#if defined OnPlayerLagout
				OnPlayerLagout(playerid, 2, GetPlayerPing(playerid));
			#endif
		}
	}

	return 1;
}

//Hookings:
#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect FL_OnPlayerDisconnect

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect FL_OnPlayerConnect

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate FL_OnPlayerUpdate

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn FL_OnPlayerSpawn

#if defined _ALS_OnPlayerDeath
  #undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath FL_OnPlayerDeath

#if defined _ALS_GivePlayerWeapon
  #undef GivePlayerWeapon
#else
	#define _ALS_GivePlayerWeapon
#endif

#define GivePlayerWeapon FL_GivePlayerWeapon

#if defined _ALS_SetPlayerAmmo
  #undef SetPlayerAmmo
#else
	#define _ALS_SetPlayerAmmo
#endif

#define SetPlayerAmmo FL_SetPlayerAmmo

#if defined _ALS_ResetPlayerWeapons
  #undef ResetPlayerWeapons
#else
	#define _ALS_ResetPlayerWeapons
#endif

#define ResetPlayerWeapons FL_ResetPlayerWeapons

#else //if it's a filterscript

//Linking to the gamemode:
stock FLA_GivePlayerWeapon(playerid, weaponid, ammo)
	return CallRemoteFunction("FL_GivePlayerWeapon","iii",playerid, weaponid, ammo);

stock FLA_SetPlayerAmmo(playerid, weaponid, ammo)
	return CallRemoteFunction("FL_SetPlayerAmmo","iii",playerid, weaponid, ammo);
 
stock FLA_ResetPlayerWeapons(playerid)
	return CallRemoteFunction("FL_ResetPlayerWeapons","i",playerid);

//Hookings:
#if defined _ALS_GivePlayerWeapon
  #undef GivePlayerWeapon
#else
	#define _ALS_GivePlayerWeapon
#endif

#define GivePlayerWeapon FLA_GivePlayerWeapon

#if defined _ALS_SetPlayerAmmo
  #undef SetPlayerAmmo
#else
	#define _ALS_SetPlayerAmmo
#endif

#define SetPlayerAmmo FLA_SetPlayerAmmo

#if defined _ALS_ResetPlayerWeapons
  #undef ResetPlayerWeapons
#else
	#define _ALS_ResetPlayerWeapons
#endif

#define ResetPlayerWeapons FLA_ResetPlayerWeapons

#endif