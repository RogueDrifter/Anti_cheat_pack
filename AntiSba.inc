/*Anti-Slide bug include by RogueDrifter 2017-12-13.
-=-=-=-=
Callbacks:
-=-=-=-=-=
	OnPlayerSlide(playerid, weaponid, Float:speed);
-=-=-=-==-=*/

#if !defined FILTERSCRIPT 

#if defined _rSBAIncluded_
	#endinput
#endif

#define _rSBAIncluded_

#include <a_samp>
 
enum SBA_DATA_INFO
{
	slidecheck,

	bool:possibleslide
};

static
	SBA_Player[MAX_PLAYERS][SBA_DATA_INFO];
 
forward RemoveSlideCheck(playerid);
forward CheckPlayerSlide(playerid);

#if defined OnPlayerConnectAS
	forward OnPlayerConnectAS(playerid);
#endif

#if defined OnPlayerDisconnectAS
	forward OnPlayerDisconnectAS(playerid,reason);
#endif

#if defined OnPlayerKeyStateChangeAS
	forward OnPlayerKeyStateChangeAS(playerid,newkeys,oldkeys);
#endif

public CheckPlayerSlide(playerid)
{
	new SBA_Wep = GetPlayerWeapon(playerid);
	new Float:SBA_Speed = GetPlayerOnFootSpeed(playerid);
	if(SBA_Speed > 15 && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && IsPAiming(playerid) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && SBA_Player[playerid][possibleslide] && 22<=SBA_Wep<=38)
	{
		CallLocalFunction("OnPlayerSlide", "ddf", playerid, SBA_Wep, SBA_Speed);
	}

	return 1;
}

public RemoveSlideCheck(playerid)
{
	SBA_Player[playerid][possibleslide] =false;

	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if( (newkeys & 128) && ( newkeys & 8)  && (newkeys & 2))
	{
		if(!SBA_Player[playerid][possibleslide]) SetTimerEx("RemoveSlideCheck",2000,false,"i",playerid);
		SBA_Player[playerid][possibleslide] =true;
	}

	#if defined OnPlayerKeyStateChangeAS
		return OnPlayerKeyStateChangeAS(playerid, newkeys,oldkeys);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	SBA_Player[playerid][slidecheck] = SetTimerEx("CheckPlayerSlide",1000,true,"d",playerid);

	#if defined OnPlayerConnectAS
		return OnPlayerConnectAS(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid,reason)
{
	SBA_Player[playerid][possibleslide]=false;
	KillTimer(SBA_Player[playerid][slidecheck]);

	#if defined OnPlayerDisconnectAS
		return  OnPlayerDisconnectAS(playerid,reason);
	#else
		return 1;
	#endif
}
 
stock GetPlayerOnFootSpeed(playerid)
{
	new Float:ST[4];
	GetPlayerVelocity(playerid,ST[0],ST[1],ST[2]);
	ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
	
	return floatround(ST[3]);
}

stock IsPAiming(playerid)
{
	new playeranim = GetPlayerAnimationIndex(playerid);
	if (((playeranim >= 1160) && (playeranim <= 1163)) || (playeranim == 1167) || (playeranim == 1365) ||
	(playeranim == 1643) || (playeranim == 1453) || (playeranim == 220)) return 1;

	return 0;
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

#define OnPlayerKeyStateChange OnPlayerKeyStateChangeAS
#define OnPlayerConnect OnPlayerConnectAS
#define OnPlayerDisconnect OnPlayerDisconnectAS

#if defined OnPlayerSlide
	forward OnPlayerSlide(playerid, weaponid, Float:speed);
#endif

#endif