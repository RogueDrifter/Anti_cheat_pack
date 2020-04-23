/*Anti car swing, particle spam and fixes breached boundaries.
Updated: June 27th.
-=-==-=-=-=-==
Originally made by Lorenc_ (Extracted/Reposted & edited by Rogue)
-=-=-=-=
Callbacks:
----------
OnPlayerCarSwing(playerid, vehicleid);
OnPlayerParticleSpam(playerid, vehicleid);
-=-=-=-=-=-=-=-=-=-
*/
 
#if defined r_EAC_Included
	#endinput
#endif
 
#define r_EAC_Included
 
#include <a_samp>
 
#if !defined FILTERSCRIPT
 
enum E_VEHICLESPAM_DATA
{
	bool:vehicleSpeed, 
 
	timeTicks, 
	spamTicks, 
 
	Float:playerX, 
	Float:playerY, 
	Float:playerZ, 

	checkTimer
}
 
static
	playerData[MAX_PLAYERS][E_VEHICLESPAM_DATA];

static GetTickDiff_AC(newtick, oldtick)
{
	if (oldtick > newtick) {
		return (cellmax - oldtick + 1) - (cellmin - newtick);
	}
	return newtick - oldtick;
}

#if defined EAC_OnVehicleDamageStatusUpd
	forward EAC_OnVehicleDamageStatusUpd(vehicleid, playerid);
#endif
 
#if defined EAC_OnPlayerDisconnect
	forward EAC_OnPlayerDisconnect(playerid, reason);
#endif
 
#if defined EAC_OnPlayerUpdate
	forward EAC_OnPlayerUpdate(playerid);
#endif
 
#if defined OnPlayerParticleSpam
	forward OnPlayerParticleSpam(playerid, vehicleid);
#endif
 
#if defined OnPlayerCarSwing
	forward OnPlayerCarSwing(playerid, vehicleid);
#endif
 
forward EAC_RepairVehicle(vehicleid);
forward EAC_SetVehicleVelocity(vehicleid, Float:X, Float:Y, Float:Z);
 
forward ResetCarSpeed(playerid);
public  ResetCarSpeed(playerid) return playerData[playerid][vehicleSpeed] = false;
 
public EAC_RepairVehicle(vehicleid)
{
	if(vehicleid <0 || vehicleid > MAX_VEHICLES) return 0;
	#if !defined foreach
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			if(GetPlayerVehicleID(i) == vehicleid)
			{
				playerData[i][timeTicks] = 0;
				playerData[i][spamTicks] = 0;
			}
		}
	#else
		foreach(new i: Player)
		{
			if(GetPlayerVehicleID(i) == vehicleid)
			{
				playerData[i][timeTicks] = 0;
				playerData[i][spamTicks] = 0;
			}
		}
	#endif
	return RepairVehicle(vehicleid);
}
 
public EAC_SetVehicleVelocity(vehicleid, Float:X, Float:Y, Float:Z)
{
	if(vehicleid <0 || vehicleid > MAX_VEHICLES) return 0;
	#if !defined foreach
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			if(GetPlayerVehicleID(i) == vehicleid)
			{
				KillTimer(playerData[i][checkTimer]);
				playerData[i][checkTimer] = SetTimerEx("ResetCarSpeed", 2000, false, "i", i);
				playerData[i][vehicleSpeed] = true;
			}
		}
	#else
		foreach(new i: Player)
		{
			if(GetPlayerVehicleID(i) == vehicleid)
			{
				KillTimer(playerData[i][checkTimer]);
				playerData[i][checkTimer] = SetTimerEx("ResetCarSpeed", 2000, false, "i", i);
				playerData[i][vehicleSpeed] = true;
			}
		}
	#endif
	return SetVehicleVelocity(vehicleid, X, Y, Z);
}
 
public OnPlayerDisconnect(playerid, reason)
{
	playerData[playerid][spamTicks]	= 0;
	playerData[playerid][timeTicks]	= 0;
	playerData[playerid][vehicleSpeed] = false;
	#if defined EAC_OnPlayerDisconnect
		return EAC_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
 
public OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
	new carTires, carLights;
	GetVehicleDamageStatus(vehicleid, carLights, carTires, carLights, carTires );
	if(GetPlayerState(playerid) == 2)
	{
		if(carLights || carTires) return 1;
		new tickTimes;
		tickTimes = GetTickCount();

		if(GetTickDiff_AC(tickTimes, playerData[playerid][timeTicks]) >= 0 && GetTickDiff_AC(tickTimes, playerData[playerid][timeTicks]) <= 500 )
		{
			playerData[playerid][spamTicks] ++;
			if(playerData[playerid][spamTicks] >= 10)
			{
				playerData[playerid][spamTicks] = 0;
				#if defined OnPlayerParticleSpam
				OnPlayerParticleSpam(playerid, vehicleid);
				#endif
				SetPlayerHealth(playerid, 0.0);
				EAC_RepairVehicle(vehicleid);
				SetVehicleHealth(vehicleid, 999.000);
				return 1;
			}
		}
		else playerData[playerid][spamTicks] = 0;
		playerData[playerid][timeTicks] = tickTimes;
	}
	#if defined EAC_OnVehicleDamageStatusUpd
		return EAC_OnVehicleDamageStatusUpd(vehicleid, playerid);
	#else
		return 1;
	#endif
}
 
public OnPlayerUpdate(playerid)
{
	new Float:fposX,  Float:fposY,  Float:fposZ, 
		Float:fposVX, Float:fposVY, Float:fposVZ, playerCar;

	GetPlayerPos(playerid, fposX, fposY, fposZ);
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && playerCar != GetPlayerVehicleID(playerid))
		playerCar = GetPlayerVehicleID(playerid);

	if(fposX >= 99999.0 || fposY >= 99999.0 || fposZ >= 99999.0 || fposX <= -99999.0 || fposY <= -99999.0 || fposZ <= -99999.0)
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) SetVehicleToRespawn(playerCar);
		SendClientMessage(playerid, 0xa9c4e4ff, "Warning: Excessive X, Y, Z has been breached thus old location set.");
		SetPlayerPos(playerid, playerData[playerid][playerX], playerData[playerid][playerY], playerData[playerid][playerZ]);
	}
	else GetPlayerPos(playerid, playerData[playerid][playerX], playerData[playerid][playerY], playerData[playerid][playerZ]);

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		GetVehicleVelocity(playerCar, fposVX, fposVY, fposVZ);
		if(( fposVX > 3.0 || fposVY > 3.0 || fposVZ > 3.0 || fposVX < -3.0 || fposVY < -3.0 || fposVZ < -3.0 )
			&& ( fposVX != fposX && fposVY != fposY && fposVZ != fposZ ) && !playerData[playerid][vehicleSpeed])
		{
			#if defined OnPlayerCarSwing
				OnPlayerCarSwing(playerid, playerCar);
			#endif
			SetPlayerHealth(playerid, 0.0);
			SetVehicleToRespawn(playerCar);
		}
	}
	#if defined EAC_OnPlayerUpdate
		return EAC_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}
 
#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
 
#define OnPlayerDisconnect EAC_OnPlayerDisconnect
 
#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
 
#define OnPlayerUpdate EAC_OnPlayerUpdate

#if defined _ALS_OnVehicleDamageStatusUpd
  #undef OnVehicleDamageStatusUpdate
#else
	#define _ALS_OnVehicleDamageStatusUpd
#endif
 
#define OnVehicleDamageStatusUpdate EAC_OnVehicleDamageStatusUpd
 
#if defined _ALS_SetVehicleVelocity
  #undef SetVehicleVelocity
#else
	#define _ALS_SetVehicleVelocity
#endif
 
#define SetVehicleVelocity EAC_SetVehicleVelocity
 
#if defined _ALS_RepairVehicle
  #undef RepairVehicle
#else
	#define _ALS_RepairVehicle
#endif
 
#define RepairVehicle EAC_RepairVehicle
#else //If it's a FILTERSCRIPT
 
stock EAC_FRepairVehicle(vehicleid)
	return CallRemoteFunction("EAC_RepairVehicle", "i", vehicleid);
 
stock EAC_FSetVehicleVelocity(vehicleid, Float:X, Float:Y, Float:Z)
	return CallRemoteFunction("EAC_SetVehicleVelocity", "ifff", vehicleid, X, Y, Z);
 
#if defined _ALS_SetVehicleVelocity
  #undef SetVehicleVelocity
#else
	#define _ALS_SetVehicleVelocity
#endif
 
#define SetVehicleVelocity EAC_FSetVehicleVelocity
 
#if defined _ALS_RepairVehicle
  #undef RepairVehicle
#else
	#define _ALS_RepairVehicle
#endif
 
#define RepairVehicle EAC_FRepairVehicle
#endif
