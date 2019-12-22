/*rAntiBuggers by RogueDrifter 
-Feb the 10th 2018
-Last updated: June 27th.
-Callbacks: OnPlayerBugAttempt(playerid, bugcode).
_____________________________________________

bugcode 0 = AFK ghost	  bugcode 1 = NPC spoof
bugcode 2 = Fake spawn	 bugcode 3 = Fake connect
				bugcode 4 = cj run
_____________________________________________
*/
#if defined _RABUGGERS_INCLUDED_
	#endinput
#endif

#define _RABUGGERS_INCLUDED_

#include <a_samp>

#if !defined FILTERSCRIPT 

forward rAB_SpawnPlayer(playerid);
forward rAB_UsePlayerPedAnims();
forward rAB_TogglePlayerSpectating(playerid, bool:toggle);

forward ResetPlayerFall(playerid);

enum E_BUGS_PLAYER_DATA
{
	bool:isPlayerConnected, 
	bool:isPlayerSpawned, 

	fallingTechnique, 

	Float:playerOldZ, 

	falingTicks, 
	damageTicks, 
	pausedTicks
}

static 
	playerData[MAX_PLAYERS][E_BUGS_PLAYER_DATA], 
	bool:g_playerAnims;

#define RAB_MAX_DMG_TICKS 3

#if defined OnPlayerBugAttempt
	forward OnPlayerBugAttempt(playerid, bugcode);
#endif

#if defined RAB_OnPlayerConnect
	forward RAB_OnPlayerConnect(playerid);
#endif

#if defined RAB_OnPlayerDisconnect
	forward RAB_OnPlayerDisconnect(playerid, reason);
#endif

#if defined RAB_OnPlayerGiveDamage
	forward RAB_OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart);
#endif

#if defined RAB_OnPlayerSpawn
	forward RAB_OnPlayerSpawn(playerid);
#endif

#if defined RAB_OnPlayerDeath
	forward RAB_OnPlayerDeath(playerid, killerid, reason);
#endif

#if defined RAB_OnPlayerRequestClass
	forward RAB_OnPlayerRequestClass(playerid, classid);
#endif

#if defined RAB_OnPlayerUpdate
	forward RAB_OnPlayerUpdate(playerid);
#endif

static bool:IsPlayerPaused(playerid)
	return (gettime() > (playerData[playerid][pausedTicks]+3));

static bool:IsValidSkin(skinid)
	return(1 <= skinid <= 73 || 75 <= skinid <= 311);

public OnPlayerConnect(playerid)
{
	playerData[playerid][falingTicks] = -1;
	playerData[playerid][playerOldZ] = -999.0;
	if(playerData[playerid][isPlayerConnected])
	{
		#if defined OnPlayerBugAttempt
			OnPlayerBugAttempt(playerid, 3);
		#endif
	}

	else playerData[playerid][isPlayerConnected] = true;
	if(IsPlayerNPC(playerid))
	{
		new rAB_PIP[17];
		GetPlayerIp(playerid, rAB_PIP, sizeof(rAB_PIP));
		new rAB_ServerIP[17];
		GetSVarString("bind", rAB_ServerIP, sizeof(rAB_ServerIP));
		if (!!strcmp(rAB_PIP, rAB_ServerIP))
		{
			#if defined OnPlayerBugAttempt
				OnPlayerBugAttempt(playerid, 1);
			#endif
		}
	}
	#if defined RAB_OnPlayerConnect
		return RAB_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	if(playerData[playerid][isPlayerSpawned])
	{
		#if defined OnPlayerBugAttempt
			OnPlayerBugAttempt(playerid, 2);
		#endif
	}
	#if defined RAB_OnPlayerSpawn
		return RAB_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	playerData[playerid][isPlayerSpawned] = false;
	#if defined RAB_OnPlayerDeath
		return RAB_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid, reason)
{
	playerData[playerid][isPlayerSpawned]   = false;
	playerData[playerid][isPlayerConnected] = false;
	playerData[playerid][fallingTechnique] = 0;
	playerData[playerid][damageTicks]  =	 0;

	#if defined RAB_OnPlayerDisconnect
		return RAB_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerRequestClass(playerid, classid)
{
	playerData[playerid][isPlayerSpawned] = false;
	#if defined RAB_OnPlayerRequestClass
		return RAB_OnPlayerRequestClass(playerid, classid);
	#else
		return 1;
	#endif
}

public OnPlayerUpdate(playerid)
{
	if(playerData[playerid][falingTicks] == -1) playerData[playerid][falingTicks] = gettime();
	new Float:rAB_TempX, Float:rAB_TempY, Float:rAB_TempZ;
	if(gettime() - playerData[playerid][falingTicks] >= 1)
	{
		playerData[playerid][falingTicks] = gettime();
		if(playerData[playerid][playerOldZ] == -999.0) GetPlayerPos(playerid, rAB_TempX, rAB_TempY, rAB_TempZ);
		GetPlayerPos(playerid, rAB_TempX, rAB_TempY, rAB_TempZ);

		if(playerData[playerid][fallingTechnique] == 0)
		{
			if(rAB_TempZ - playerData[playerid][playerOldZ] > 2.0 && rAB_TempZ > 0.0 && playerData[playerid][playerOldZ] > 0.0)
			{//Player fell but above the void area.
				playerData[playerid][fallingTechnique] = 1;
			}

			if(rAB_TempZ + playerData[playerid][playerOldZ] < -2.0 && rAB_TempZ < 0.0 && playerData[playerid][playerOldZ] > 0.0)
			{//Player going below surface
				playerData[playerid][fallingTechnique] = 1;
			}

			if(playerData[playerid][playerOldZ] - rAB_TempZ > 2.0 && playerData[playerid][playerOldZ] < 0.0 && rAB_TempZ < 0.0)
			{//Player been falling in the void part
				playerData[playerid][fallingTechnique] = 1;
			}
			if(rAB_TempZ == 0.0 && playerData[playerid][playerOldZ] > 2.0)
			{//Player just fell on the ground
				playerData[playerid][fallingTechnique] = 1;
			}
			if(playerData[playerid][playerOldZ] == 0.0 && rAB_TempZ < -2.0)
			{//Player just went below surface
				playerData[playerid][fallingTechnique] = 1;
			}
		}

		if(playerData[playerid][playerOldZ] == rAB_TempZ && playerData[playerid][fallingTechnique] == 1) 
		{
			playerData[playerid][fallingTechnique] = 2;
			SetTimerEx("ResetPlayerFall", 2000, false, "i", playerid);
		}
	}

	if(!g_playerAnims)
	{
		if(GetPlayerAnimationIndex(playerid) == 1231 && IsValidSkin(GetPlayerSkin(playerid)) && GetPlayerWeapon(playerid) != 46 && playerData[playerid][fallingTechnique] == 0)
		{
			#if defined OnPlayerBugAttempt
				OnPlayerBugAttempt(playerid, 4);
			#endif
		}
	}
	playerData[playerid][pausedTicks] = gettime();
	GetPlayerPos(playerid, rAB_TempX, rAB_TempY, playerData[playerid][playerOldZ]);
	#if defined RAB_OnPlayerUpdate
		return RAB_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart)
{
	if(IsPlayerPaused(playerid))
	{
		playerData[playerid][damageTicks]++;
		if(playerData[playerid][damageTicks] == RAB_MAX_DMG_TICKS)
		{	  
			playerData[playerid][damageTicks] = 0;
			#if defined OnPlayerBugAttempt
				OnPlayerBugAttempt(playerid, 0);
			#endif
		}
	}
	#if defined RAB_OnPlayerGiveDamage
		return RAB_OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart);
	#else
		return 1;
	#endif
}

public rAB_SpawnPlayer(playerid)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	playerData[playerid][isPlayerSpawned] = false;
	return SpawnPlayer(playerid);
}

public rAB_UsePlayerPedAnims() 
{
	g_playerAnims = true;
	return UsePlayerPedAnims();
}

public rAB_TogglePlayerSpectating(playerid, bool:toggle)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!toggle) playerData[playerid][isPlayerSpawned] = false;
	return TogglePlayerSpectating(playerid, toggle);
}

public ResetPlayerFall(playerid) 
	return playerData[playerid][fallingTechnique] = 0;

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect RAB_OnPlayerConnect

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect RAB_OnPlayerDisconnect

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn RAB_OnPlayerSpawn

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate RAB_OnPlayerUpdate

#if defined _ALS_OnPlayerDeath
  #undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath RAB_OnPlayerDeath

#if defined _ALS_OnPlayerGiveDamage
	#undef OnPlayerGiveDamage
#else
	#define _ALS_OnPlayerGiveDamage
#endif

#define OnPlayerGiveDamage RAB_OnPlayerGiveDamage

#if defined _ALS_OnPlayerRequestClass
	#undef OnPlayerRequestClass
#else
	#define _ALS_OnPlayerRequestClass
#endif

#define OnPlayerRequestClass RAB_OnPlayerRequestClass

#if defined _ALS_TogglePlayerSpectating
  #undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif

#define TogglePlayerSpectating rAB_TogglePlayerSpectating

#if defined _ALS_SpawnPlayer
  #undef SpawnPlayer
#else
	#define _ALS_SpawnPlayer
#endif

#define SpawnPlayer rAB_SpawnPlayer

#if defined _ALS_UsePlayerPedAnims
  #undef UsePlayerPedAnims
#else
	#define _ALS_UsePlayerPedAnims
#endif

#define UsePlayerPedAnims rAB_UsePlayerPedAnims

#else //If it's a FS

stock rAB_FTogglePlayerSpectating(playerid, bool:toggle)
	return CallRemoteFunction("rAB_TogglePlayerSpectating", "ii", playerid, toggle);

stock rAB_FSpawnPlayer(playerid)
	return CallRemoteFunction("rAB_SpawnPlayer", "i", playerid);

stock rAB_FUsePlayerPedAnims()
	return CallRemoteFunction("rAB_UsePlayerPedAnims");

#if defined _ALS_TogglePlayerSpectating
  #undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif

#define TogglePlayerSpectating rAB_FTogglePlayerSpectating

#if defined _ALS_SpawnPlayer
  #undef SpawnPlayer
#else
	#define _ALS_SpawnPlayer
#endif

#define SpawnPlayer rAB_FSpawnPlayer

#if defined _ALS_UsePlayerPedAnims
  #undef UsePlayerPedAnims
#else
	#define _ALS_UsePlayerPedAnims
#endif

#define UsePlayerPedAnims rAB_FUsePlayerPedAnims

#endif
