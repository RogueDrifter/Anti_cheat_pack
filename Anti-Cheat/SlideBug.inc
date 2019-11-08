/*Anti-Slide bug include by RogueDrifter 2017-12-13.
-=-=-=-=
Callbacks:
-=-=-=-=-=
	OnPlayerSlide(playerid, weaponid, speed);
-=-=-=-==-=*/

#if !defined FILTERSCRIPT 

#if defined _rSBAIncluded_
	#endinput
#endif

#define _rSBAIncluded_

#include <a_samp>
 
enum E_PLAYER_SLIDE_INFO
{
	slideCheck, 

	bool:isSliding
};

static
	playerData[MAX_PLAYERS][E_PLAYER_SLIDE_INFO];
 
forward RemoveSlideCheck(playerid);
forward CheckPlayerSlide(playerid);

#if defined SBA_OnPlayerConnect
	forward SBA_OnPlayerConnect(playerid);
#endif

#if defined SBA_OnPlayerDisconnect
	forward SBA_OnPlayerDisconnect(playerid, reason);
#endif

#if defined SBA_OnPlayerKeyStateChange
	forward SBA_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif

static GetPlayerOnFootSpeed(playerid)//I didn't create this
{
	new Float:ST[4];
	GetPlayerVelocity(playerid, ST[0], ST[1], ST[2]);
	ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
	
	return floatround(ST[3]);
}

static bool:IsPlayerAiming(playerid)
{
	new playerAnim = GetPlayerAnimationIndex(playerid);
	if (((playerAnim >= 1160) && (playerAnim <= 1163)) || (playerAnim == 1167) || (playerAnim == 1365) ||
	(playerAnim == 1643) || (playerAnim == 1453) || (playerAnim == 220)) return true;

	return false;
}

public CheckPlayerSlide(playerid)
{
	new playerWeapon = GetPlayerWeapon(playerid);
	new playerSpeed = GetPlayerOnFootSpeed(playerid);
	if(playerSpeed > 15 && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && IsPlayerAiming(playerid) 
	&& GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && playerData[playerid][isSliding] 
	&& (playerWeapon >= 22 && playerWeapon<=38) )
	{
		CallLocalFunction("OnPlayerSlide", "ddd", playerid, playerWeapon, playerSpeed);
	}

	return 1;
}

public RemoveSlideCheck(playerid)
{
	playerData[playerid][isSliding] =false;

	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if( (newkeys & 128) && ( newkeys & 8)  && (newkeys & 2))
	{
		if(!playerData[playerid][isSliding]) SetTimerEx("RemoveSlideCheck", 2000, false, "i", playerid);
		playerData[playerid][isSliding] =true;
	}

	#if defined SBA_OnPlayerKeyStateChange
		return SBA_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	playerData[playerid][slideCheck] = SetTimerEx("CheckPlayerSlide", 1000, true, "d", playerid);

	#if defined SBA_OnPlayerConnect
		return SBA_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid, reason)
{
	playerData[playerid][isSliding]=false;
	KillTimer(playerData[playerid][slideCheck]);

	#if defined SBA_OnPlayerDisconnect
		return  SBA_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
 
#if defined _ALS_OnPlayerKeyStateChange
  #undef OnPlayerKeyStateChange
#else
#define _ALS_OnPlayerKeyStateChange
#endif
 
#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
#define _ALS_OnPlayerConnect
#endif

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerKeyStateChange SBA_OnPlayerKeyStateChange
#define OnPlayerConnect SBA_OnPlayerConnect
#define OnPlayerDisconnect SBA_OnPlayerDisconnect

#if defined OnPlayerSlide
	forward OnPlayerSlide(playerid, weaponid, speed);
#endif

#endif