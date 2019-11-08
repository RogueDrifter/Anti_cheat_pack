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

#if defined ACS_OnPlayerConnect
	forward ACS_OnPlayerConnect(playerid);
#endif

#if defined ACS_OnPlayerDisconnect
	forward ACS_OnPlayerDisconnect(playerid, reason);
#endif

#if defined ACS_OnVehicleStreamIn
	forward ACS_OnVehicleStreamIn(vehicleid, forplayerid);
#endif

#if defined ACS_OnVehicleStreamOut
	forward ACS_OnVehicleStreamOut(vehicleid, forplayerid);
#endif

new 
	g_checkTimer[MAX_PLAYERS] ;
new
	g_streamingPlayer[MAX_VEHICLES];
new 
	g_streamedCars[MAX_PLAYERS] ;

forward CheckCarSpam(playerid);

public CheckCarSpam(playerid)
{
 	if(g_streamedCars[playerid] >= ACS_MAX_STREAMIN)
	{
		for(new v; v < MAX_VEHICLES; v++)
		{
			if(!IsValidVehicle(v) || g_streamingPlayer[v] != playerid) continue;
			SetVehicleToRespawn(v);
		}

		#if defined OnPlayerSpamCars
			OnPlayerSpamCars(playerid, g_streamedCars[playerid]);
		#endif
	}

	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	KillTimer(g_checkTimer[playerid]);

	#if defined ACS_OnPlayerDisconnect
		return ACS_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	g_streamedCars[playerid]	= 0;
	g_checkTimer[playerid] = SetTimerEx("CheckCarSpam", 2000, true, "i", playerid);
	#if defined ACS_OnPlayerConnect
		return ACS_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	g_streamingPlayer[vehicleid] = forplayerid;
	g_streamedCars[forplayerid]			 ++;
	#if defined ACS_OnVehicleStreamIn
		return  ACS_OnVehicleStreamIn(vehicleid, forplayerid);
	#else
		return 1;
	#endif
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	g_streamingPlayer[vehicleid] = -1;
	g_streamedCars[forplayerid]	--;

	#if defined ACS_OnVehicleStreamOut
		return  ACS_OnVehicleStreamOut(vehicleid, forplayerid);
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

#define OnPlayerConnect ACS_OnPlayerConnect

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect ACS_OnPlayerDisconnect

#if defined _ALS_OnVehicleStreamIn
  #undef OnVehicleStreamIn
#else
	#define _ALS_OnVehicleStreamIn
#endif

#define OnVehicleStreamIn ACS_OnVehicleStreamIn

#if defined _ALS_OnVehicleStreamOut
  #undef OnVehicleStreamOut
#else
	#define _ALS_OnVehicleStreamOut
#endif

#define OnVehicleStreamOut ACS_OnVehicleStreamOut

#endif