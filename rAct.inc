/*
Anti car troll include by RogueDrifter Jan 15th 2018.
-=-=-=-=-=-

Last updated on June 28th.
-=-=-=-=-=-=-=-=-=-=-=-=

Callback:
-=-=-=-=-

OnPlayerCarTroll(playerid, vehicleid, trolledid, trolltype);
-=-=-=-=-=-=-=-=--=-=-=-=

Type 1: Teleporting between vehicles (can give cheaters chance to control other's players) (also lag cheat detected here).
Type 2: Control other player's cars or spin/rotate (Done by checking the player's actual vehicle).
Type 3: Fast kick/rotate or teleporting inside drivers. (detectd by position change)
Type 4: Teleporting inside a vehicle illegally as a driver (without PutPlayerInVehicle or OnPlayerEnterVehicle).
Type 5: Teleporting inside a vehicle illegally as a passenger (without PutPlayerInVehicle or OnPlayerEnterVehicle).
Type 6: Remote kick a player by tping a vehicle inside of him. (Too fast the server reads it as an unoccupied vehicle).
*/

#if defined _AntiCarTroll_included
	#endinput
#endif
 
#define _AntiCarTroll_included
 
#include <a_samp>
 
#if !defined FILTERSCRIPT

enum ActPData
{
	bool:ActAntiSpam,
	bool:AntiJackBug,
	bool:ActProtectFalse,
	bool:ActProtectPos,
	bool:ActFixCars,
	bool:ActCheater,
	bool:ActSPrtct,
	bool:ActPassgBug,
	bool:ActJustDied,
	bool:ActDoubleProtec,
	bool:ActFFProtec,
	bool:ActDetectSpam,
	
	Float:ActPX,
	Float:ActPY,
	Float:ActPZ,

	ActRealCar,
	pVehicleID,
	pPauseTick,
	ActOVeh,
	ActEnV,

	ActBFastTimer,
	ActJustDTim,
	ActSPrtimer,
	ActStateTimer,
	ActTimer,
	ResetTAct
};
 
enum ActVData
{
	VehicleTempOwner,
	ActVehEnterer,

	bool:ActETPBugFix
};
 
new
	ActPlayer[MAX_PLAYERS][ActPData],
	ActVehicle[MAX_VEHICLES][ActVData],
	SafeTimer;
 
#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid); // Unlock the hidden native
#endif

#define RACT_BETWEEN_VEHICLES   1
#define RACT_CONTROL_CARS_SPIN  2
#define RACT_SKICK_CAR		  3
#define RACT_TELEPORT_DRIVER	4
#define RACT_TELEPORT_PASSENGER 5
#define RACT_KICKPLAYER_CAR	 6

forward ActTogglePlayerSpectating(playerid, bool:toggle);
forward ActPutPlayerInVehicle(playerid, vehicleid, seatid);
forward ActSetPlayerPos(playerid, Float:x, Float:y, Float:z);
forward ActRemovePlayerFromVehicle(playerid);

forward ActDelayState(playerid, delaytype);
forward ActResetHandler(handleid, handletype);
forward ActFixPPIV(playerid, vehicleid, seatid);
forward ActSafeReset();
 
#if defined OnPlayerEnterVehicleACT
	forward OnPlayerEnterVehicleACT(playerid, vehicleid, ispassenger);
#endif
 
#if defined OnPlayerUpdateACT
	forward OnPlayerUpdateACT(playerid);
#endif

#if defined OnPlayerConnectACT
	forward OnPlayerConnectACT(playerid);
#endif
 
#if defined OnPlayerDisconnectACT
	forward OnPlayerDisconnectACT(playerid, reason);
#endif
 
#if defined OnPlayerExitVehicleACT
	forward OnPlayerExitVehicleACT(playerid, vehicleid);
#endif

#if defined OnPlayerDeathACT
	forward OnPlayerDeathACT(playerid, killerid, reason);
#endif
 
#if defined OnPlayerStateChangeACT
	forward OnPlayerStateChangeACT(playerid, newstate, oldstate);
#endif
 
#if defined OnGameModeInitACT
	forward OnGameModeInitACT();
#endif
 
#if defined OnGameModeExitACT
	forward OnGameModeExitACT();
#endif

#if defined OnPlayerSpawnAct
	forward OnPlayerSpawnAct(playerid);
#endif

#if defined OnUnoccupiedVehicleUpdateAct
	forward OnUnoccupiedVehicleUpdateAct(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z);
#endif

static stock rAct_Detected(playerid, vehicleid, trolledid, trolltype)
{
	#pragma unused trolltype, vehicleid

	if(ActPlayer[playerid][ActDetectSpam]) return 0;
	if(!ActPlayer[playerid][ActCheater]) 
	{
		if(trolledid != INVALID_PLAYER_ID)
		{
			if(!ActPlayer[trolledid][ActCheater]) ActPlayer[playerid][ActCheater]  = true;
		}
		else
		{
			ActPlayer[playerid][ActCheater]  = true;
		}
	}

	if(trolledid != INVALID_PLAYER_ID)
	{
		if(!ActPlayer[trolledid][ActCheater]) 
		{
			#if defined OnPlayerCarTroll
				OnPlayerCarTroll(playerid, vehicleid, trolledid, trolltype);
			#endif
		}
	}
	else 
	{
		#if defined OnPlayerCarTroll
			OnPlayerCarTroll(playerid, vehicleid, trolledid, trolltype);
		#endif
	}
	ActPlayer[playerid][ActDetectSpam] = true;
	SetTimerEx("ActResetHandler", 3000, false, "ii", playerid, 1);
	return 1;
}

static ActIsPEnExCar(playerid)
{
	switch(GetPlayerAnimationIndex(playerid))
	{
		case 1010..1012: return 1;
		case 1024..1025: return 1;
		case 1043..1045: return 1;
		case 1009, 1041: return 1;
	}
	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_ENTER_VEHICLE) return 1;
	return 0;
}

static ActPlayerDown(playerid)
{
	switch(GetPlayerAnimationIndex(playerid))
	{
		case 1208..1209: return 1;
		case 1155..1156: return 1;
	}
	return 0;
}

static IsPlayerNearVehicle(playerid, vehicleid, Float:range)
{
	new Float:x, Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);
	return IsPlayerInRangeOfPoint(playerid, range, x, y, z);
}

static ActPausedP(playerid) return (GetTickCount() > (ActPlayer[playerid][pPauseTick]+2000)) ? true : false;

public ActDelayState(playerid, delaytype)
{
	KillTimer(ActPlayer[playerid][ActStateTimer]);
	switch(delaytype)
	{
		case 1:
		{
			if(!ActPlayer[playerid][ActProtectFalse] && GetPlayerState(playerid) == 2 && !ActPlayer[playerid][ActAntiSpam] && !ActPlayer[playerid][ActSPrtct] && !ActPlayer[playerid][ActDoubleProtec] && !ActPlayer[playerid][ActFFProtec])
			{				
				ActPlayer[playerid][ActAntiSpam]=true;
				SetTimerEx("ActResetHandler",2500, false,"ii", playerid, 2);
				rAct_Detected(playerid, GetPlayerVehicleID(playerid), INVALID_PLAYER_ID, RACT_TELEPORT_DRIVER);
			}
			else if(ActPlayer[playerid][ActProtectFalse]) ActPlayer[playerid][ActProtectFalse] = false;
		}
		case 2:
		{
			if(!ActPlayer[playerid][ActProtectFalse] && GetPlayerState(playerid) == 3 && !ActPlayer[playerid][ActAntiSpam] && !ActPlayer[playerid][ActSPrtct] && !ActPlayer[playerid][ActDoubleProtec] && !ActPlayer[playerid][ActFFProtec])
			{			
				ActPlayer[playerid][ActAntiSpam]=true;
				SetTimerEx("ActResetHandler",2500, false,"ii", playerid, 2);
				rAct_Detected(playerid, GetPlayerVehicleID(playerid), INVALID_PLAYER_ID, RACT_TELEPORT_PASSENGER);
			}
			else if(ActPlayer[playerid][ActProtectFalse]) ActPlayer[playerid][ActProtectFalse] = false;
		}
	}
	return 1;
}

public ActSafeReset()
{
	ACT_memset(ActVehicle[MAX_VEHICLES-1][VehicleTempOwner], INVALID_PLAYER_ID);
	for(new i, j = GetPlayerPoolSize(); i <= j; i++) 
	{ 
		if(!IsPlayerConnected(i)) continue; 
		if(GetPlayerState(i)== PLAYER_STATE_DRIVER) 
		ActVehicle[GetPlayerVehicleID(i)][VehicleTempOwner]= i; 
	}
	return 1;
}

#if defined OnPlayerCarTroll
	forward OnPlayerCarTroll(playerid, vehicleid, trolledid, trolltype);

#endif

public OnPlayerUpdate(playerid)
{
	new ActVeh = GetPlayerVehicleID(playerid);

	if(ActIsPEnExCar(playerid) && !ActPlayer[playerid][ActFFProtec])
	{
		ActPlayer[playerid][ActFFProtec] = true;
		SetTimerEx("ActResetHandler", 5000, false, "ii", playerid, 13);	
	}

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && ActPlayer[playerid][ActRealCar] != ActVeh && !ActPlayer[playerid][ActProtectFalse])
	{
		ActPlayer[playerid][ActRealCar] = ActVeh;

		if(ActVehicle[ActVeh][VehicleTempOwner] != playerid &&  ActVehicle[ActVeh][VehicleTempOwner] != INVALID_PLAYER_ID) 
		{
			rAct_Detected(playerid, ActVeh, ActVehicle[ActVeh][VehicleTempOwner], RACT_CONTROL_CARS_SPIN);
		}
		else rAct_Detected(playerid, ActVeh, INVALID_PLAYER_ID, RACT_CONTROL_CARS_SPIN);

		SetVehicleToRespawn(ActVeh);
		SetPlayerHealth(playerid, 0);
	}

	if(ActIsPEnExCar(playerid))
	{
		new Float:PPOSX, Float:PPOSY, Float:PPOSZ;
		GetPlayerPos(playerid, PPOSX, PPOSY, PPOSZ);

		if(GetVehicleDistanceFromPoint(ActPlayer[playerid][ActEnV], PPOSX, PPOSY, PPOSZ) >= 5 && !ActPlayer[playerid][ActProtectFalse])
		{
			if(!ActPlayer[playerid][ActProtectFalse]) ClearAnimations(playerid);
		}
	}

	if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER && ActVeh != ActPlayer[playerid][ActOVeh])
	{
		if(!ActPlayer[playerid][ActProtectFalse])
		{			
			ActPlayer[playerid][ActOVeh] = ActVeh;

			if(ActVehicle[ActVeh][VehicleTempOwner] != playerid &&  ActVehicle[ActVeh][VehicleTempOwner] != INVALID_PLAYER_ID) 
			{
				rAct_Detected(playerid, ActVeh, ActVehicle[ActVeh][VehicleTempOwner], RACT_BETWEEN_VEHICLES);
			}
			else rAct_Detected(playerid, ActVeh, INVALID_PLAYER_ID, RACT_BETWEEN_VEHICLES);

			SetVehicleToRespawn(ActVeh);
			SetPlayerHealth(playerid, 0);
		}
	}

	if(!IsPlayerInRangeOfPoint(playerid, 25.0, ActPlayer[playerid][ActPX], ActPlayer[playerid][ActPY], ActPlayer[playerid][ActPZ]) 
	&& ActPlayer[playerid][ActPX] != 0 && ActPlayer[playerid][ActPY] != 0 && !ActIsPEnExCar(playerid) && !ActPlayer[playerid][ActPassgBug] && !ActPlayer[playerid][ActJustDied] 
	&& ActPlayer[playerid][ActPZ] != 0 && !ActPlayer[playerid][ActProtectPos]  && !ActPlayerDown(playerid)
	&& GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && GetPlayerInterior(playerid) == 0)
	{
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			new Float:ActTempX, Float:ActTempY, Float:ActTempZ;
			GetPlayerPos(i, ActTempX, ActTempY, ActTempZ);
			if(IsPlayerInRangeOfPoint(playerid, 1.2, ActTempX, ActTempY, ActTempZ)
			&& !ActPlayer[i][ActAntiSpam] && GetPlayerState(i) == PLAYER_STATE_DRIVER 
			&& i != playerid && IsPlayerNearVehicle(playerid, GetPlayerVehicleID(i), 5.0) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
			{
				ActPlayer[i][ActAntiSpam]=true; 
				SetVehicleToRespawn(GetPlayerVehicleID(i));
				TogglePlayerControllable(i, 0);
				SetPlayerPos(i, ActTempX, ActTempY, ActTempZ);
				SetPlayerHealth(playerid, 0.0);

				SetTimerEx("ActResetHandler", 1500, false, "ii", i, 7);
				SetTimerEx("ActResetHandler",1500, false,"ii", i, 2);
				rAct_Detected(playerid,  GetPlayerVehicleID(i), i, RACT_SKICK_CAR);
			}
		}
	}
	ActPlayer[playerid][pPauseTick] = GetTickCount();

		#if defined OnPlayerUpdateACT
			return OnPlayerUpdateACT(playerid);
		#else
			return 1;
		#endif
}

public OnGameModeInit()
{
	SafeTimer = SetTimer("ActSafeReset",2500,true);

	#if defined OnGameModeInitACT
		return OnGameModeInitACT();
	#else
		return 1;
	#endif
}
 
public OnGameModeExit()
{
	KillTimer(SafeTimer);

	#if defined OnGameModeExitACT
		return OnGameModeExitACT();
	#else
		return 1;
	#endif
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	ActPlayer[playerid][ActEnV] = vehicleid;

	if(ispassenger)  ActPlayer[playerid][ActOVeh]	= vehicleid;
	if(!ispassenger) ActPlayer[playerid][ActRealCar] = vehicleid;

	ActPlayer[playerid][ActProtectFalse] = true;

	if(!ispassenger && !ActPlayer[playerid][AntiJackBug] && ActVehicle[vehicleid][VehicleTempOwner] != INVALID_PLAYER_ID && ActVehicle[vehicleid][VehicleTempOwner] != playerid)
	{
		ActPlayer[playerid][AntiJackBug]=true;
		SetTimerEx("ActResetHandler",6000, false,"ii", playerid, 6);

		ActPlayer[ActVehicle[vehicleid][VehicleTempOwner]][AntiJackBug]=true;
		SetTimerEx("ActResetHandler",6000, false,"ii", ActVehicle[vehicleid][VehicleTempOwner], 6);
	}

	else if(!ActVehicle[vehicleid][ActETPBugFix] && !ispassenger && ActVehicle[vehicleid][VehicleTempOwner] == INVALID_PLAYER_ID)
	{
		ActVehicle[vehicleid][ActETPBugFix] = true;
		ActVehicle[vehicleid][ActVehEnterer] = playerid;

		SetTimerEx("ActResetHandler",3000, false,"ii", vehicleid, 4);
	}

	#if defined OnPlayerEnterVehicleACT
		return OnPlayerEnterVehicleACT(playerid, vehicleid, ispassenger);
	#else
		return 1;
	#endif
}
 
public OnPlayerConnect(playerid)
{
	ActPlayer[playerid][ActAntiSpam]=false;
	ActPlayer[playerid][AntiJackBug]=false;
	ActPlayer[playerid][ActTimer] = SetTimerEx("ActResetHandler", 1000, true, "ii", playerid, 0);

	#if defined OnPlayerConnectACT
		return OnPlayerConnectACT(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	if(!ActPlayer[playerid][ActProtectPos]) SetTimerEx("ActResetHandler",2100, false,"ii", playerid, 3);
	ActPlayer[playerid][ActProtectPos] = true;
	GetPlayerPos(playerid, ActPlayer[playerid][ActPX], ActPlayer[playerid][ActPY], ActPlayer[playerid][ActPZ]);

	KillTimer(ActPlayer[playerid][ActJustDTim]);
	ActPlayer[playerid][ActJustDied]	= true;
	ActPlayer[playerid][ActJustDTim] = SetTimerEx("ActResetHandler", 5000, false, "ii", playerid, 11);

	KillTimer(ActPlayer[playerid][ActSPrtimer]);
	ActPlayer[playerid][ActSPrtimer] = SetTimerEx("ActResetHandler", 5500, false, "ii", playerid, 9);
	ActPlayer[playerid][ActSPrtct]   = true;
	#if defined OnPlayerSpawnAct
		return OnPlayerSpawnAct(playerid);
	#else
		return 1;
	#endif
}

public OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z)
{
	new Float:TempFloatX, Float:TempFloatY, Float:TempFloatZ;
	new ActTempState, ActTempCar;
	if(vel_z >= 300.000000)
	{
		SetVehicleToRespawn(vehicleid);
		GetVehiclePos(vehicleid, TempFloatX, TempFloatY, TempFloatZ);
		for(new x; x < MAX_VEHICLES; x++)
		{
			if(!IsValidVehicle(x) || ActVehicle[x][VehicleTempOwner] != INVALID_PLAYER_ID) continue;
			if(GetVehicleDistanceFromPoint(vehicleid, TempFloatX, TempFloatY, TempFloatZ) <= 20) SetVehicleToRespawn(x);
		}
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			ActTempState = GetPlayerState(i);
			ActTempCar = GetPlayerVehicleID(i);

			if(!IsPlayerConnected(i)) continue;
			if(IsPlayerInRangeOfPoint(i, 4.0, new_x, new_y, new_z))
			{
				if (i != playerid)
				{
					if(ActTempState != PLAYER_STATE_ONFOOT) GetVehiclePos(ActTempCar, TempFloatX, TempFloatY, TempFloatZ);
					SetPlayerHealth(playerid, 0.0);

					if(ActTempState == PLAYER_STATE_ONFOOT && !ActPlayer[i][ActFixCars])
					{
						ActPlayer[i][ActFixCars] = true,
						TogglePlayerControllable(i, 0),
						SetTimerEx("ActResetHandler", 2500, false, "ii", i, 7);
					}
					else if(ActTempState != PLAYER_STATE_ONFOOT && !ActPlayer[i][ActFixCars])
					{
						SetVehicleToRespawn(ActTempCar);

						ActPlayer[i][ActFixCars] = true,
						TogglePlayerControllable(i, 0),
						SetTimerEx("ActResetHandler", 2500, false, "ii", i, 7);
					}

					rAct_Detected(playerid, vehicleid, i, RACT_KICKPLAYER_CAR);
					break;
				}
			}
		}
	}

	#if defined OnUnoccupiedVehicleUpdateAct
		return OnUnoccupiedVehicleUpdateAct(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z);
	#else
		return 1;
	#endif
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	switch(newstate)
	{
		case 1:
		{
			KillTimer(ActPlayer[playerid][ActStateTimer]);
			switch(oldstate)
			{
				case 2: 
				{
					ActPlayer[playerid][ResetTAct] = SetTimerEx("ActResetHandler",500, false,"ii", playerid, 5);
					ActPlayer[playerid][ActProtectFalse]= false;
				}
				case 3:
				{   
					ActPlayer[playerid][ActPassgBug] = true;
					SetTimerEx("ActResetHandler", 2000, false, "ii", playerid, 10);
					ActPlayer[playerid][ActProtectFalse]= false;
				} 
			}
		}
 
		case 2:
		{
			ActPlayer[playerid][pVehicleID] = GetPlayerVehicleID(playerid);
			if(ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner] != INVALID_PLAYER_ID && ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner] != playerid && ActPausedP(ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner]))
			{
				new Float:ActPosX, Float:ActPosY, Float:ActPosZ;
				GetPlayerPos(ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner], ActPosX, ActPosY, ActPosZ);
				SetPlayerPos(ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner], ActPosX, ActPosY, ActPosZ+2);
			}
			
			KillTimer(ActPlayer[playerid][ActStateTimer]);
			ActPlayer[playerid][ActStateTimer] = SetTimerEx("ActDelayState", 350, false, "ii", playerid, 1);

			if(ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner] == INVALID_PLAYER_ID)
			{
				KillTimer(ActPlayer[playerid][ResetTAct]);
				ActVehicle[ActPlayer[playerid][pVehicleID]][VehicleTempOwner] = playerid;
			}
		}

		case 3:
		{
			KillTimer(ActPlayer[playerid][ActStateTimer]);
			ActPlayer[playerid][ActStateTimer] = SetTimerEx("ActDelayState", 350, false, "ii", playerid, 2);
		}
	}

	#if defined OnPlayerStateChangeACT
		return OnPlayerStateChangeACT(playerid, newstate, oldstate);
	#else
		return 1;
	#endif
}

public OnPlayerDeath(playerid, killerid, reason)
{
	ActPlayer[playerid][ActAntiSpam]=false;

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) ActPlayer[playerid][ResetTAct] = SetTimerEx("ActResetHandler",500, false,"ii", playerid, 5);

	KillTimer(ActPlayer[playerid][ActJustDTim]);
	ActPlayer[playerid][ActJustDied]	= true;
	ActPlayer[playerid][ActJustDTim] = SetTimerEx("ActResetHandler", 10000, false, "ii", playerid, 11);
		#if defined OnPlayerDeathACT
		return OnPlayerDeathACT(playerid, killerid, reason);
		#else
		return 1;
			#endif
}
 
public OnPlayerExitVehicle(playerid, vehicleid)
{
	ActPlayer[playerid][ResetTAct] = SetTimerEx("ActResetHandler",500, false,"ii", playerid, 5);

	#if defined OnPlayerExitVehicleACT
		return OnPlayerExitVehicleACT(playerid, vehicleid);
	#else
		return 1;
	#endif
}
 
public OnPlayerDisconnect(playerid, reason)
{
	KillTimer(ActPlayer[playerid][ActTimer]);
	new gpVehicleID;
	gpVehicleID = GetPlayerVehicleID(playerid);

	ActPlayer[playerid][ActDetectSpam]  = false;
	ActPlayer[playerid][ActFFProtec]	= false;
	ActPlayer[playerid][ActJustDied]	= false;
	ActPlayer[playerid][ActSPrtct]	  = false;
	ActPlayer[playerid][ActCheater]	 = false;
	ActPlayer[playerid][ActProtectPos]  = false;
	ActPlayer[playerid][ActProtectFalse]= false;
	ActPlayer[playerid][ActFixCars]	 = false;
	ActPlayer[playerid][ActPassgBug]	= false;
	ActPlayer[playerid][ActDoubleProtec]= false;

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		ActVehicle[gpVehicleID][VehicleTempOwner] = INVALID_PLAYER_ID;

	#if defined OnPlayerDisconnectACT
		return OnPlayerDisconnectACT(playerid, reason);
	#else
		return 1;
	#endif
}

public ActResetHandler(handleid, handletype)
{
	switch(handletype)
	{
		case 0:
		{
			GetPlayerPos(handleid, ActPlayer[handleid][ActPX], ActPlayer[handleid][ActPY], ActPlayer[handleid][ActPZ]);
		}
		case 1:
		{
			ActPlayer[handleid][ActDetectSpam]   = false;
		}
		case 2:
		{
			ActPlayer[handleid][ActAntiSpam]	  = false;
		}
		case 3:
		{
			ActPlayer[handleid][ActProtectPos]	= false;
		}
		case 4:
		{
			ActVehicle[handleid][ActETPBugFix]   = false, 
			ActVehicle[handleid][ActVehEnterer] = INVALID_PLAYER_ID;
		}
		case 5:
		{
			ActVehicle[ActPlayer[handleid][pVehicleID]][VehicleTempOwner] = INVALID_PLAYER_ID;
		}
		case 6:
		{
			ActPlayer[handleid][AntiJackBug]	  = false;
		}
		case 7:
		{
			TogglePlayerControllable(handleid, 1);
			ActPlayer[handleid][ActFixCars]	   = false;
		}
		case 8:
		{
			ActPlayer[handleid][ActProtectFalse]  = false;
		}
		case 9:
		{
			ActPlayer[handleid][ActSPrtct]		= false;
		}
		case 10:
		{
			ActPlayer[handleid][ActPassgBug]	  = false;
		}
		case 11:
		{
			ActPlayer[handleid][ActJustDied]	  = false;
		}
		case 12:
		{
			ActPlayer[handleid][ActDoubleProtec] = false;
		}
		case 13:
		{
			ActPlayer[handleid][ActFFProtec]	 = false;
		}
	}
	return 1;
}

public ActFixPPIV(playerid, vehicleid, seatid)
{
	ActPlayer[playerid][ActProtectFalse] = true; 
	return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

public ActTogglePlayerSpectating(playerid, bool:toggle)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(!ActPlayer[playerid][ActProtectFalse]) SetTimerEx("ActResetHandler",2100, false,"ii", playerid, 8);
	ActPlayer[playerid][ActProtectFalse] = true;
	return TogglePlayerSpectating(playerid, toggle);
}

public ActRemovePlayerFromVehicle(playerid)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(!ActPlayer[playerid][ActDoubleProtec]) SetTimerEx("ActResetHandler",2100, false,"ii", playerid, 12);
	ActPlayer[playerid][ActDoubleProtec] = true;
	return RemovePlayerFromVehicle(playerid);
}

public ActPutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if(playerid > MAX_PLAYERS || playerid < 0 || vehicleid < 0 || vehicleid > MAX_VEHICLES) return 0;
	if(ActVehicle[vehicleid][ActETPBugFix]) ClearAnimations(ActVehicle[vehicleid][ActVehEnterer]);
	ActPlayer[playerid][ActProtectFalse] = true;

	if(seatid != 0 ) ActPlayer[playerid][ActOVeh] = vehicleid;
	if(seatid == 0 )  ActPlayer[playerid][ActRealCar] = vehicleid;

	if(GetPlayerState(playerid) == 2 || GetPlayerState(playerid) == 3)
	{
		KillTimer(ActPlayer[playerid][ActBFastTimer]);
		new Float:PX,Float:PY,Float:PZ;
		GetPlayerPos(playerid, PX,PY,PZ);
		SetPlayerPos(playerid, PX,PY,PZ+2); 
		ActPlayer[playerid][ActBFastTimer] = SetTimerEx("ActFixPPIV", 1000 , false, "iii", playerid, vehicleid, seatid);
	}

	else PutPlayerInVehicle(playerid, vehicleid, seatid);
	return 1;
}

public ActSetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(!ActPlayer[playerid][ActProtectPos]) SetTimerEx("ActResetHandler",4000, false,"ii", playerid, 3);
	ActPlayer[playerid][ActProtectPos] = true;
	return SetPlayerPos(playerid, x, y, z);
}

#if defined _ALS_OnUnoccupiedVehicleUpdate
  #undef OnUnoccupiedVehicleUpdate
#else
	#define _ALS_OnUnoccupiedVehicleUpdate
#endif

#if defined _ALS_OnPlayerStateChange
  #undef OnPlayerStateChange
#else
	#define _ALS_OnPlayerStateChange
#endif
 
#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
 
#if defined _ALS_OnPlayerExitVehicle
  #undef OnPlayerExitVehicle
#else
	#define _ALS_OnPlayerExitVehicle
#endif
 
#if defined _ALS_OnPlayerDeath
  #undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif
 
#if defined _ALS_OnGameModeInit
  #undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
 
#if defined _ALS_OnGameModeExit
  #undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif
 
#if defined _ALS_OnPlayerEnterVehicle
  #undef OnPlayerEnterVehicle
#else
	#define _ALS_OnPlayerEnterVehicle
#endif
 
#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif
 
#if defined _ALS_PutPlayerInVehicle
  #undef PutPlayerInVehicle
#else
	#define _ALS_PutPlayerInVehicle
#endif
 
#if defined _ALS_SetPlayerPos
  #undef SetPlayerPos
#else
	#define _ALS_SetPlayerPos
#endif

#if defined _ALS_TogglePlayerSpectating
  #undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif

#if defined _ALS_RemovePlayerFromVehicle
  #undef RemovePlayerFromVehicle
#else
	#define _ALS_RemovePlayerFromVehicle
#endif

#define OnUnoccupiedVehicleUpdate OnUnoccupiedVehicleUpdateAct
#define OnPlayerExitVehicle OnPlayerExitVehicleACT
#define OnGameModeInit OnGameModeInitACT
#define OnPlayerDeath OnPlayerDeathACT
#define OnPlayerDisconnect OnPlayerDisconnectACT
#define OnPlayerSpawn OnPlayerSpawnAct
#define OnPlayerConnect OnPlayerConnectACT
#define OnPlayerStateChange OnPlayerStateChangeACT
#define OnGameModeExit OnGameModeExitACT
#define OnPlayerEnterVehicle OnPlayerEnterVehicleACT
#define OnPlayerUpdate OnPlayerUpdateACT

#define TogglePlayerSpectating ActTogglePlayerSpectating
#define RemovePlayerFromVehicle ActRemovePlayerFromVehicle
#define PutPlayerInVehicle ActPutPlayerInVehicle
#define SetPlayerPos ActSetPlayerPos

#else //If it's a filterscript

#if defined _ALS_RemovePlayerFromVehicle
  #undef RemovePlayerFromVehicle
#else
	#define _ALS_RemovePlayerFromVehicle
#endif

#if defined _ALS_PutPlayerInVehicle
  #undef PutPlayerInVehicle
#else
	#define _ALS_PutPlayerInVehicle
#endif

#if defined _ALS_SetPlayerPos
  #undef SetPlayerPos
#else
	#define _ALS_SetPlayerPos
#endif

#if defined _ALS_TogglePlayerSpectating
  #undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif

#define RemovePlayerFromVehicle ActFRemovePlayerFromVehicle
#define TogglePlayerSpectating ActFTogglePlayerSpectating
#define PutPlayerInVehicle ActFPutPlayerInVehicle
#define SetPlayerPos ActFSetPlayerPos

stock ActFRemovePlayerFromVehicle(playerid)
	return CallRemoteFunction("ActRemovePlayerFromVehicle","i", playerid);

stock ActFTogglePlayerSpectating(playerid, bool:toggle)
	return CallRemoteFunction("ActTogglePlayerSpectating","ii", playerid, toggle);

stock ActFPutPlayerInVehicle(playerid, vehicleid, seatid)
	return CallRemoteFunction("ActPutPlayerInVehicle","iii", playerid, vehicleid, seatid);

stock ActFSetPlayerPos(playerid, Float:x, Float:y, Float:z)
	return CallRemoteFunction("ActSetPlayerPos","ifff", playerid, x, y, z);

#endif

static stock ACT_memset(aArray[], iValue, iSize = sizeof(aArray)) {
	new
		iAddress
	;
	
	// Store the address of the array
	#emit LOAD.S.pri 12
	#emit STOR.S.pri iAddress
	
	// Convert the size from cells to bytes
	iSize *= 4;
	
	// Loop until there is nothing more to fill
	while (iSize > 0) {
		// I have to do this because the FILL instruction doesn't accept a dynamic number.
		if (iSize >= 4096) {
			#emit LOAD.S.alt iAddress
			#emit LOAD.S.pri iValue
			#emit FILL 4096
		
			iSize	-= 4096;
			iAddress += 4096;
		} else if (iSize >= 1024) {
			#emit LOAD.S.alt iAddress
			#emit LOAD.S.pri iValue
			#emit FILL 1024

			iSize	-= 1024;
			iAddress += 1024;
		} else if (iSize >= 256) {
			#emit LOAD.S.alt iAddress
			#emit LOAD.S.pri iValue
			#emit FILL 256

			iSize	-= 256;
			iAddress += 256;
		} else if (iSize >= 64) {
			#emit LOAD.S.alt iAddress
			#emit LOAD.S.pri iValue
			#emit FILL 64

			iSize	-= 64;
			iAddress += 64;
		} else if (iSize >= 16) {
			#emit LOAD.S.alt iAddress
			#emit LOAD.S.pri iValue
			#emit FILL 16

			iSize	-= 16;
			iAddress += 16;
		} else {
			#emit LOAD.S.alt iAddress
			#emit LOAD.S.pri iValue
			#emit FILL 4

			iSize	-= 4;
			iAddress += 4;
		}
	}
	
	// aArray is used, just not by its symbol name
	#pragma unused aArray
}
