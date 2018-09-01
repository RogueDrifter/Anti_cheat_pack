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

forward ASC_AntiSpeedCheats(playerid);
forward ASC_ResetCall(playerid);
forward ASC_ResetSpeedWarns(playerid);
forward ASC_RFallTP(playerid);

#if defined ASC_OnPlayerDisconnect	
	forward ASC_OnPlayerDisconnect(playerid, reason);
#endif

#if defined ASC_OnPlayerConnect
	forward ASC_OnPlayerConnect(playerid);
#endif

#if defined OnPlayerSpeedCheat	
	forward OnPlayerSpeedCheat(playerid, speedtype);
#endif

static bool:ASC_PlayerASpam[MAX_PLAYERS char],
ASC_PlayerFalling[MAX_PLAYERS char],
ASC_SpeedWarns[MAX_PLAYERS char],

Float:ASC_PlayerZFall[MAX_PLAYERS],	
ASC_TimerSpeedChecker[MAX_PLAYERS],
ASC_RFWTimer[MAX_PLAYERS];


public OnPlayerDisconnect(playerid, reason)
{
	ASC_PlayerASpam{playerid}   = false;
	ASC_PlayerFalling{playerid} = 0;
	ASC_SpeedWarns{playerid} = 0;
	ASC_PlayerZFall[playerid]	= 0.0;

	KillTimer(ASC_TimerSpeedChecker[playerid]);
	KillTimer(ASC_RFWTimer[playerid]);

	#if defined ASC_OnPlayerDisconnect
		return ASC_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	ASC_TimerSpeedChecker[playerid] = SetTimerEx("ASC_AntiSpeedCheats", 1000, true, "i", playerid);

	#if defined ASC_OnPlayerConnect
		return ASC_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public ASC_ResetSpeedWarns(playerid) return ASC_SpeedWarns{playerid} = 0;
public ASC_ResetCall(playerid) 		 return ASC_PlayerASpam{playerid} = false;
public ASC_RFallTP(playerid) 		 return ASC_PlayerFalling{playerid} = 0;
public ASC_AntiSpeedCheats(playerid)
{
	new Float:AscX, Float:AscY, Float:AscZ;
	if(ASC_PlayerASpam{playerid}) return 0;
	GetPlayerPos(playerid, AscX, AscY, AscZ);

	if(AscZ < 0.0 || ASC_PlayerZFall[playerid] < 0.0 ) return 0; 
	if(ASC_PlayerZFall[playerid] - AscZ > 3 && ASC_PlayerFalling{playerid} == 0)
	{
		ASC_PlayerFalling{playerid} = 1;
	}
	else if(ASC_PlayerFalling{playerid} == 1 && ASC_PlayerZFall[playerid] - AscZ < 3)
	{
		ASC_PlayerFalling{playerid} = 2;
		SetTimerEx("ASC_RFallTP", 3000, false, "i", playerid);
	} 

	if(ASC_PlayerFalling{playerid} == 0)
	{
		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				if(ASC_GetPlayerSpeed(playerid) >= ASC_MAX_FOOT_SPEED && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID)
				{
					ASC_PlayerASpam{playerid} = true;
					SetTimerEx("ASC_ResetCall", 1000, false, "i", playerid);
					if(ASC_SpeedWarns{playerid} == 0) ASC_RFWTimer[playerid] = SetTimerEx("ASC_ResetSpeedWarns", 50*1000, false, "i", playerid);
					if(ASC_SpeedWarns{playerid} < ASC_MAX_SPEED_WARNS) ASC_SpeedWarns{playerid}++;
					if(ASC_SpeedWarns{playerid} == ASC_MAX_SPEED_WARNS) CallLocalFunction("OnPlayerSpeedCheat", "ii", playerid, 0);
				}
			}
			case 2:
			{
				switch(ASC_IsAPlane(GetPlayerVehicleID(playerid)))
				{
					case 0:
					{
						if(ASC_GetPlayerSpeed(playerid) >= ASC_MAX_CAR_SPEED)
						{
							ASC_PlayerASpam{playerid} = true;
							SetTimerEx("ASC_ResetCall", 2000, false, "i", playerid);
							if(ASC_SpeedWarns{playerid} == 0) ASC_RFWTimer[playerid] = SetTimerEx("ASC_ResetSpeedWarns", 50*1000, false, "i", playerid);
							if(ASC_SpeedWarns{playerid} < ASC_MAX_SPEED_WARNS) ASC_SpeedWarns{playerid}++;
							if(ASC_SpeedWarns{playerid} == ASC_MAX_SPEED_WARNS) ASC_SpeedWarns{playerid} = 0, CallLocalFunction("OnPlayerSpeedCheat", "ii", playerid, 1);
						}
					}
					case 1:
					{
						if(ASC_GetPlayerSpeed(playerid) >= ASC_MAX_PLANE_SPEED)
						{
							ASC_PlayerASpam{playerid} = true;
							SetTimerEx("ASC_ResetCall", 2000, false, "i", playerid);
							if(ASC_SpeedWarns{playerid} == 0) ASC_RFWTimer[playerid] = SetTimerEx("ASC_ResetSpeedWarns", 50*1000, false, "i", playerid);
							if(ASC_SpeedWarns{playerid} < ASC_MAX_SPEED_WARNS) ASC_SpeedWarns{playerid}++;
							if(ASC_SpeedWarns{playerid} == ASC_MAX_SPEED_WARNS) ASC_SpeedWarns{playerid} = 0, CallLocalFunction("OnPlayerSpeedCheat", "ii", playerid, 2);
						}
					}
				}
			}
		}
	}

	ASC_PlayerZFall[playerid] = AscZ;
	return 1;
}

static ASC_IsAPlane(vehicleid)
{
	switch(GetVehicleModel(vehicleid))
	{
		case 592, 577, 511, 512, 593, 520, 553, 476, 519, 460, 513, 464: return 1;
		case 548, 425, 417, 487, 488, 497, 563, 447, 469, 465, 501: return 1; 
	}
	return 0;
}

static ASC_GetPlayerSpeed(playerid)
{
	new Float:ST[4];
	if(IsPlayerInAnyVehicle(playerid))
	GetVehicleVelocity(GetPlayerVehicleID(playerid),ST[0],ST[1],ST[2]);
	else GetPlayerVelocity(playerid,ST[0],ST[1],ST[2]);
	ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
	return floatround(ST[3]);
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
