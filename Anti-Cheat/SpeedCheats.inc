/* 
Anti speed cheats by Rogue 2018/3/26
-=-=-=
Last updated on Apr 6th
-=-=-=-=
OnPlayerSpeedCheat(playerid, speedtype);
-=-=-=-=-

playerid =  the cheater
speedtype 0 = on foot
speedtype 1 = in car
speedtype 2 = in plane
-=-=-=-=-
*/

#if !defined FILTERSCRIPT 

#include <a_samp>

#if defined _rAntiSpeed_Included
	#endinput
#endif

#define _rAntiSpeed_Included
#define ASC_MAX_FOOT_SPEED 110
#define ASC_MAX_CAR_SPEED 290
#define ASC_MAX_PLANE_SPEED 340
#define ASC_MAX_SPEED_WARNS 2

static bool:IsModelAPlane(vehicleid)
{
	switch(GetVehicleModel(vehicleid))
	{
		case 592, 577, 511, 512, 593, 520, 553, 476, 519, 460, 513, 464: return true;
		case 548, 425, 417, 487, 488, 497, 563, 447, 469, 465, 501: return true; 
	}
	return false;
}

static GetPlayerSpeed(playerid)
{
	new Float:ST[4];
	if(IsPlayerInAnyVehicle(playerid))
	GetVehicleVelocity(GetPlayerVehicleID(playerid), ST[0], ST[1], ST[2]);
	else GetPlayerVelocity(playerid, ST[0], ST[1], ST[2]);
	ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
	return floatround(ST[3]);
}

forward CheckSpeedCheats(playerid);
forward ResetSpamCall(playerid);
forward ResetSpeedWarnings(playerid);
forward ResetFallTeleport(playerid);

#if defined ASC_OnPlayerDisconnect	
	forward ASC_OnPlayerDisconnect(playerid, reason);
#endif

#if defined ASC_OnPlayerConnect
	forward ASC_OnPlayerConnect(playerid);
#endif

#if defined OnPlayerSpeedCheat	
	forward OnPlayerSpeedCheat(playerid, speedtype);
#endif

static bool:s_callSpam[MAX_PLAYERS char], 
s_playerFalling[MAX_PLAYERS char], 
s_speedWarnings[MAX_PLAYERS char], 

Float:s_playerZ[MAX_PLAYERS], 	
s_speedChecker[MAX_PLAYERS], 
s_resetTimer[MAX_PLAYERS];


public OnPlayerDisconnect(playerid, reason)
{
	s_callSpam{playerid}   = false;
	s_playerFalling{playerid} = 0;
	s_speedWarnings{playerid} = 0;
	s_playerZ[playerid]	= 0.0;

	KillTimer(s_speedChecker[playerid]);
	KillTimer(s_resetTimer[playerid]);

	#if defined ASC_OnPlayerDisconnect
		return ASC_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	s_speedChecker[playerid] = SetTimerEx("CheckSpeedCheats", 1000, true, "i", playerid);

	#if defined ASC_OnPlayerConnect
		return ASC_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public ResetSpeedWarnings(playerid) return s_speedWarnings{playerid} = 0;
public ResetSpamCall(playerid) 		 return s_callSpam{playerid} = false;
public ResetFallTeleport(playerid) 		 return s_playerFalling{playerid} = 0;
public CheckSpeedCheats(playerid)
{
	new Float:AscX, Float:AscY, Float:AscZ;
	if(s_callSpam{playerid}) return 0;
	GetPlayerPos(playerid, AscX, AscY, AscZ);

	if(AscZ < 0.0 || s_playerZ[playerid] < 0.0 ) return 0; 
	if(s_playerZ[playerid] - AscZ > 3 && s_playerFalling{playerid} == 0)
	{
		s_playerFalling{playerid} = 1;
	}
	else if(s_playerFalling{playerid} == 1 && s_playerZ[playerid] - AscZ < 3)
	{
		s_playerFalling{playerid} = 2;
		SetTimerEx("ResetFallTeleport", 3000, false, "i", playerid);
	} 

	if(s_playerFalling{playerid} == 0)
	{
		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				if(GetPlayerSpeed(playerid) >= ASC_MAX_FOOT_SPEED && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID)
				{
					s_callSpam{playerid} = true;
					SetTimerEx("ResetSpamCall", 1000, false, "i", playerid);
					if(s_speedWarnings{playerid} == 0) s_resetTimer[playerid] = SetTimerEx("ResetSpeedWarnings", 50*1000, false, "i", playerid);
					if(s_speedWarnings{playerid} < ASC_MAX_SPEED_WARNS) s_speedWarnings{playerid}++;
					if(s_speedWarnings{playerid} == ASC_MAX_SPEED_WARNS) CallLocalFunction("OnPlayerSpeedCheat", "ii", playerid, 0);
				}
			}
			case 2:
			{
				switch(IsModelAPlane(GetPlayerVehicleID(playerid)))
				{
					case 0:
					{
						if(GetPlayerSpeed(playerid) >= ASC_MAX_CAR_SPEED)
						{
							s_callSpam{playerid} = true;
							SetTimerEx("ResetSpamCall", 2000, false, "i", playerid);
							if(s_speedWarnings{playerid} == 0) s_resetTimer[playerid] = SetTimerEx("ResetSpeedWarnings", 50*1000, false, "i", playerid);
							if(s_speedWarnings{playerid} < ASC_MAX_SPEED_WARNS) s_speedWarnings{playerid}++;
							if(s_speedWarnings{playerid} == ASC_MAX_SPEED_WARNS) s_speedWarnings{playerid} = 0, CallLocalFunction("OnPlayerSpeedCheat", "ii", playerid, 1);
						}
					}
					case 1:
					{
						if(GetPlayerSpeed(playerid) >= ASC_MAX_PLANE_SPEED)
						{
							s_callSpam{playerid} = true;
							SetTimerEx("ResetSpamCall", 2000, false, "i", playerid);
							if(s_speedWarnings{playerid} == 0) s_resetTimer[playerid] = SetTimerEx("ResetSpeedWarnings", 50*1000, false, "i", playerid);
							if(s_speedWarnings{playerid} < ASC_MAX_SPEED_WARNS) s_speedWarnings{playerid}++;
							if(s_speedWarnings{playerid} == ASC_MAX_SPEED_WARNS) s_speedWarnings{playerid} = 0, CallLocalFunction("OnPlayerSpeedCheat", "ii", playerid, 2);
						}
					}
				}
			}
		}
	}

	s_playerZ[playerid] = AscZ;
	return 1;
}

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect ASC_OnPlayerConnect

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect ASC_OnPlayerDisconnect

#endif
