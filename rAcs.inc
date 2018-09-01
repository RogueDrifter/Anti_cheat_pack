/*
Anti car spammer by RogueDrifter 2018/1/30 .
(Will auto respawn the spammed cars after the callback).
===Usables====
-Number of cars a player can stream in- 
#define ACS_MAX_STREAMIN [number]

-Public function called when a player exceeds maximum vehicles streamed in- 
OnPlayerSpamCars(playerid, number);
*/

#if !defined FILTERSCRIPT 

#if defined ACS_Included
	#endinput
#endif

#define ACS_Included

#include <a_samp> 

#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid);
#endif

#if !defined ACS_MAX_STREAMIN
	#define  ACS_MAX_STREAMIN 100
#endif

#if defined acs_OnPlayerConnect
	forward acs_OnPlayerConnect(playerid);
#endif

#if defined acs_OnPlayerDisconnect
	forward acs_OnPlayerDisconnect(playerid, reason);
#endif

#if defined acs_OnVehicleStreamIn
	forward acs_OnVehicleStreamIn(vehicleid, forplayerid);
#endif

#if defined acs_OnVehicleStreamOut
	forward acs_OnVehicleStreamOut(vehicleid, forplayerid);
#endif

new 
	AcsCheckTimer[MAX_PLAYERS] ;
new
	AcsStreamForP[MAX_VEHICLES];
new 
	AcsStreamedIn[MAX_PLAYERS] ;

forward acs_SpamCheck(playerid);

public acs_SpamCheck(playerid)
{
 	if(AcsStreamedIn[playerid] >= ACS_MAX_STREAMIN)
	{
		for(new v; v < MAX_VEHICLES; v++)
		{
			if(!IsValidVehicle(v) || AcsStreamForP[v] != playerid) continue;
			SetVehicleToRespawn(v);
		}

		#if defined OnPlayerSpamCars
			OnPlayerSpamCars(playerid, AcsStreamedIn[playerid]);
		#endif
	}

	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	KillTimer(AcsCheckTimer[playerid]);

	#if defined acs_OnPlayerDisconnect
		return acs_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	AcsStreamedIn[playerid]	= 0;
	AcsCheckTimer[playerid] = SetTimerEx("acs_SpamCheck", 2000, true, "i", playerid);
	#if defined acs_OnPlayerConnect
		return acs_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	AcsStreamForP[vehicleid] = forplayerid;
	AcsStreamedIn[forplayerid]			 ++;
	#if defined acs_OnVehicleStreamIn
		return  acs_OnVehicleStreamIn(vehicleid, forplayerid);
	#else
		return 1;
	#endif
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	AcsStreamForP[vehicleid] = -1;
	AcsStreamedIn[forplayerid]	--;

	#if defined acs_OnVehicleStreamOut
		return  acs_OnVehicleStreamOut(vehicleid, forplayerid);
	#else
		return 1;
	#endif
}

#if defined OnPlayerSpamCars
	forward OnPlayerSpamCars(playerid, number);
#endif

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect acs_OnPlayerConnect

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect acs_OnPlayerDisconnect

#if defined _ALS_OnVehicleStreamIn
  #undef OnVehicleStreamIn
#else
	#define _ALS_OnVehicleStreamIn
#endif

#define OnVehicleStreamIn acs_OnVehicleStreamIn

#if defined _ALS_OnVehicleStreamOut
  #undef OnVehicleStreamOut
#else
	#define _ALS_OnVehicleStreamOut
#endif

#define OnVehicleStreamOut acs_OnVehicleStreamOut

#endif