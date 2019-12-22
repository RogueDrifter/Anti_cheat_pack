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

enum E_TROLL_PLAYER_DATA
{
	bool:playerAntiSpam, 
	bool:playerAntiJack, 
	bool:playerFalseDetection, 
	bool:playerPosProtection, 
	bool:playerCarsFix, 
	bool:isPlayerCheater, 
	bool:playerSpawnProtection, 
	bool:playerPassenger, 
	bool:playerJustDied, 
	bool:playerDoubleProtection, 
	bool:playerAnimationProtection, 
	bool:playerCallSpam, 
	
	Float:playerPosX, 
	Float:playerPosY, 
	Float:playerPosZ, 

	playerActualCar, 
	playerStateVehicle, 
	pauseTicks, 
	playerUpdateVehicle, 
	vehicleRangeID, 

	playerInVehicleFix, 
	resetPlayerDeath, 
	resetPlayerSpawn, 
	playerStateTimer, 
	connectionTimer, 
	resetVehicleOwner
};
 
enum E_TROLL_VEHICLE_DATA
{
	broadVehicleOwner, 
	specificVehicleOwner, 

	bool:enterVehicleFix
};
 
static
	playerData[MAX_PLAYERS][E_TROLL_PLAYER_DATA], 
	vehicleData[MAX_VEHICLES][E_TROLL_VEHICLE_DATA], 
	s_safeTimer;
 
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

forward DelayPlayerState(playerid, delaytype);
forward ResetVariableValues(handleid, handletype);
forward FixPlayerPutInVehicle(playerid, vehicleid, seatid);
forward BroadVehicleReset();
 
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

static DetectTrollCheat(playerid, vehicleid, trolledid, trolltype)
{
	#pragma unused trolltype, vehicleid

	if(playerData[playerid][playerCallSpam]) return 0;
	if(!playerData[playerid][isPlayerCheater]) 
	{
		if(trolledid != INVALID_PLAYER_ID)
		{
			if(!playerData[trolledid][isPlayerCheater]) playerData[playerid][isPlayerCheater]  = true;
		}
		else
		{
			playerData[playerid][isPlayerCheater]  = true;
		}
	}

	if(trolledid != INVALID_PLAYER_ID)
	{
		if(!playerData[trolledid][isPlayerCheater]) 
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
	playerData[playerid][playerCallSpam] = true;
	SetTimerEx("ResetVariableValues", 3000, false, "ii", playerid, 1);
	return 1;
}

static bool:IsPlayerEnterExitCar(playerid)
{
	switch(GetPlayerAnimationIndex(playerid))
	{
		case 1010..1012: return true;
		case 1024..1025: return true;
		case 1043..1045: return true;
		case 1009, 1041: return true;
	}
	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_ENTER_VEHICLE) return true;
	return false;
}

static bool:IsPlayerDown(playerid)
{
	switch(GetPlayerAnimationIndex(playerid))
	{
		case 1208..1209: return true;
		case 1155..1156: return true;
	}
	return false;
}

static bool:IsPlayerNearVehicle(playerid, vehicleid, Float:range)
{
	new Float:x, Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);
	return (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) ? true : false;
}

static bool:IsPlayerPaused(playerid) 
	return (gettime() > (playerData[playerid][pauseTicks]+2));

public DelayPlayerState(playerid, delaytype)
{
	KillTimer(playerData[playerid][playerStateTimer]);
	switch(delaytype)
	{
		case 1:
		{
			if(!playerData[playerid][playerFalseDetection] && GetPlayerState(playerid) == 2 && !playerData[playerid][playerAntiSpam] && !playerData[playerid][playerSpawnProtection] && !playerData[playerid][playerDoubleProtection] && !playerData[playerid][playerAnimationProtection])
			{				
				playerData[playerid][playerAntiSpam]=true;
				SetTimerEx("ResetVariableValues", 2500, false, "ii", playerid, 2);
				DetectTrollCheat(playerid, GetPlayerVehicleID(playerid), INVALID_PLAYER_ID, RACT_TELEPORT_DRIVER);
			}
			else if(playerData[playerid][playerFalseDetection]) playerData[playerid][playerFalseDetection] = false;
		}
		case 2:
		{
			if(!playerData[playerid][playerFalseDetection] && GetPlayerState(playerid) == 3 && !playerData[playerid][playerAntiSpam] && !playerData[playerid][playerSpawnProtection] && !playerData[playerid][playerDoubleProtection] && !playerData[playerid][playerAnimationProtection])
			{			
				playerData[playerid][playerAntiSpam]=true;
				SetTimerEx("ResetVariableValues", 2500, false, "ii", playerid, 2);
				DetectTrollCheat(playerid, GetPlayerVehicleID(playerid), INVALID_PLAYER_ID, RACT_TELEPORT_PASSENGER);
			}
			else if(playerData[playerid][playerFalseDetection]) playerData[playerid][playerFalseDetection] = false;
		}
	}
	return 1;
}

public BroadVehicleReset()
{
	memoryset(vehicleData[MAX_VEHICLES-1][broadVehicleOwner], INVALID_PLAYER_ID);
	for(new i, j = GetPlayerPoolSize(); i <= j; i++) 
	{ 
		if(!IsPlayerConnected(i)) continue; 
		if(GetPlayerState(i)== PLAYER_STATE_DRIVER) 
		vehicleData[GetPlayerVehicleID(i)][broadVehicleOwner]= i; 
	}
	return 1;
}

#if defined OnPlayerCarTroll
	forward OnPlayerCarTroll(playerid, vehicleid, trolledid, trolltype);

#endif

public OnPlayerUpdate(playerid)
{
	new ActVeh = GetPlayerVehicleID(playerid);

	if(IsPlayerEnterExitCar(playerid) && !playerData[playerid][playerAnimationProtection])
	{
		playerData[playerid][playerAnimationProtection] = true;
		SetTimerEx("ResetVariableValues", 5000, false, "ii", playerid, 13);	
	}

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && playerData[playerid][playerActualCar] != ActVeh && !playerData[playerid][playerFalseDetection])
	{
		playerData[playerid][playerActualCar] = ActVeh;

		if(vehicleData[ActVeh][broadVehicleOwner] != playerid &&  vehicleData[ActVeh][broadVehicleOwner] != INVALID_PLAYER_ID) 
		{
			DetectTrollCheat(playerid, ActVeh, vehicleData[ActVeh][broadVehicleOwner], RACT_CONTROL_CARS_SPIN);
		}
		else DetectTrollCheat(playerid, ActVeh, INVALID_PLAYER_ID, RACT_CONTROL_CARS_SPIN);

		SetVehicleToRespawn(ActVeh);
		SetPlayerHealth(playerid, 0);
	}

	if(IsPlayerEnterExitCar(playerid))
	{
		new Float:PPOSX, Float:PPOSY, Float:PPOSZ;
		GetPlayerPos(playerid, PPOSX, PPOSY, PPOSZ);

		if(GetVehicleDistanceFromPoint(playerData[playerid][vehicleRangeID], PPOSX, PPOSY, PPOSZ) >= 5 && !playerData[playerid][playerFalseDetection])
		{
			if(!playerData[playerid][playerFalseDetection]) ClearAnimations(playerid);
		}
	}

	if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER && ActVeh != playerData[playerid][playerUpdateVehicle])
	{
		if(!playerData[playerid][playerFalseDetection])
		{			
			playerData[playerid][playerUpdateVehicle] = ActVeh;

			if(vehicleData[ActVeh][broadVehicleOwner] != playerid &&  vehicleData[ActVeh][broadVehicleOwner] != INVALID_PLAYER_ID) 
			{
				DetectTrollCheat(playerid, ActVeh, vehicleData[ActVeh][broadVehicleOwner], RACT_BETWEEN_VEHICLES);
			}
			else DetectTrollCheat(playerid, ActVeh, INVALID_PLAYER_ID, RACT_BETWEEN_VEHICLES);

			SetVehicleToRespawn(ActVeh);
			SetPlayerHealth(playerid, 0);
		}
	}

	if(!IsPlayerInRangeOfPoint(playerid, 25.0, playerData[playerid][playerPosX], playerData[playerid][playerPosY], playerData[playerid][playerPosZ]) 
	&& playerData[playerid][playerPosX] != 0 && playerData[playerid][playerPosY] != 0 && !IsPlayerEnterExitCar(playerid) && !playerData[playerid][playerPassenger] && !playerData[playerid][playerJustDied] 
	&& playerData[playerid][playerPosZ] != 0 && !playerData[playerid][playerPosProtection]  && !IsPlayerDown(playerid)
	&& GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && GetPlayerInterior(playerid) == 0)
	{
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			new Float:ActTempX, Float:ActTempY, Float:ActTempZ;
			GetPlayerPos(i, ActTempX, ActTempY, ActTempZ);
			if(IsPlayerInRangeOfPoint(playerid, 1.2, ActTempX, ActTempY, ActTempZ)
			&& !playerData[i][playerAntiSpam] && GetPlayerState(i) == PLAYER_STATE_DRIVER 
			&& i != playerid && IsPlayerNearVehicle(playerid, GetPlayerVehicleID(i), 5.0) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
			{
				playerData[i][playerAntiSpam]=true; 
				SetVehicleToRespawn(GetPlayerVehicleID(i));
				TogglePlayerControllable(i, 0);
				SetPlayerPos(i, ActTempX, ActTempY, ActTempZ);
				SetPlayerHealth(playerid, 0.0);

				SetTimerEx("ResetVariableValues", 1500, false, "ii", i, 7);
				SetTimerEx("ResetVariableValues", 1500, false, "ii", i, 2);
				DetectTrollCheat(playerid,  GetPlayerVehicleID(i), i, RACT_SKICK_CAR);
			}
		}
	}
	playerData[playerid][pauseTicks] = gettime();

		#if defined OnPlayerUpdateACT
			return OnPlayerUpdateACT(playerid);
		#else
			return 1;
		#endif
}

public OnGameModeInit()
{
	s_safeTimer = SetTimer("BroadVehicleReset", 2500, true);

	#if defined OnGameModeInitACT
		return OnGameModeInitACT();
	#else
		return 1;
	#endif
}
 
public OnGameModeExit()
{
	KillTimer(s_safeTimer);

	#if defined OnGameModeExitACT
		return OnGameModeExitACT();
	#else
		return 1;
	#endif
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	playerData[playerid][vehicleRangeID] = vehicleid;

	if(ispassenger)  playerData[playerid][playerUpdateVehicle]	= vehicleid;
	if(!ispassenger) playerData[playerid][playerActualCar] = vehicleid;

	playerData[playerid][playerFalseDetection] = true;

	if(!ispassenger && !playerData[playerid][playerAntiJack] && vehicleData[vehicleid][broadVehicleOwner] != INVALID_PLAYER_ID && vehicleData[vehicleid][broadVehicleOwner] != playerid)
	{
		playerData[playerid][playerAntiJack]=true;
		SetTimerEx("ResetVariableValues", 6000, false, "ii", playerid, 6);

		playerData[vehicleData[vehicleid][broadVehicleOwner]][playerAntiJack]=true;
		SetTimerEx("ResetVariableValues", 6000, false, "ii", vehicleData[vehicleid][broadVehicleOwner], 6);
	}

	else if(!vehicleData[vehicleid][enterVehicleFix] && !ispassenger && vehicleData[vehicleid][broadVehicleOwner] == INVALID_PLAYER_ID)
	{
		vehicleData[vehicleid][enterVehicleFix] = true;
		vehicleData[vehicleid][specificVehicleOwner] = playerid;

		SetTimerEx("ResetVariableValues", 3000, false, "ii", vehicleid, 4);
	}

	#if defined OnPlayerEnterVehicleACT
		return OnPlayerEnterVehicleACT(playerid, vehicleid, ispassenger);
	#else
		return 1;
	#endif
}
 
public OnPlayerConnect(playerid)
{
	playerData[playerid][playerAntiSpam]=false;
	playerData[playerid][playerAntiJack]=false;
	playerData[playerid][connectionTimer] = SetTimerEx("ResetVariableValues", 1000, true, "ii", playerid, 0);

	#if defined OnPlayerConnectACT
		return OnPlayerConnectACT(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	if(!playerData[playerid][playerPosProtection]) SetTimerEx("ResetVariableValues", 2100, false, "ii", playerid, 3);
	playerData[playerid][playerPosProtection] = true;
	GetPlayerPos(playerid, playerData[playerid][playerPosX], playerData[playerid][playerPosY], playerData[playerid][playerPosZ]);

	KillTimer(playerData[playerid][resetPlayerDeath]);
	playerData[playerid][playerJustDied]	= true;
	playerData[playerid][resetPlayerDeath] = SetTimerEx("ResetVariableValues", 5000, false, "ii", playerid, 11);

	KillTimer(playerData[playerid][resetPlayerSpawn]);
	playerData[playerid][resetPlayerSpawn] = SetTimerEx("ResetVariableValues", 5500, false, "ii", playerid, 9);
	playerData[playerid][playerSpawnProtection]   = true;
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
			if(!IsValidVehicle(x) || vehicleData[x][broadVehicleOwner] != INVALID_PLAYER_ID) continue;
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

					if(ActTempState == PLAYER_STATE_ONFOOT && !playerData[i][playerCarsFix])
					{
						playerData[i][playerCarsFix] = true, 
						TogglePlayerControllable(i, 0), 
						SetTimerEx("ResetVariableValues", 2500, false, "ii", i, 7);
					}
					else if(ActTempState != PLAYER_STATE_ONFOOT && !playerData[i][playerCarsFix])
					{
						SetVehicleToRespawn(ActTempCar);

						playerData[i][playerCarsFix] = true, 
						TogglePlayerControllable(i, 0), 
						SetTimerEx("ResetVariableValues", 2500, false, "ii", i, 7);
					}

					DetectTrollCheat(playerid, vehicleid, i, RACT_KICKPLAYER_CAR);
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
			KillTimer(playerData[playerid][playerStateTimer]);
			switch(oldstate)
			{
				case 2: 
				{
					playerData[playerid][resetVehicleOwner] = SetTimerEx("ResetVariableValues", 500, false, "ii", playerid, 5);
					playerData[playerid][playerFalseDetection]= false;
				}
				case 3:
				{   
					playerData[playerid][playerPassenger] = true;
					SetTimerEx("ResetVariableValues", 2000, false, "ii", playerid, 10);
					playerData[playerid][playerFalseDetection]= false;
				} 
			}
		}
 
		case 2:
		{
			playerData[playerid][playerStateVehicle] = GetPlayerVehicleID(playerid);
			if(vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner] != INVALID_PLAYER_ID && vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner] != playerid && IsPlayerPaused(vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner]))
			{
				new Float:ActPosX, Float:ActPosY, Float:ActPosZ;
				GetPlayerPos(vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner], ActPosX, ActPosY, ActPosZ);
				SetPlayerPos(vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner], ActPosX, ActPosY, ActPosZ+2);
			}
			
			KillTimer(playerData[playerid][playerStateTimer]);
			playerData[playerid][playerStateTimer] = SetTimerEx("DelayPlayerState", 350, false, "ii", playerid, 1);

			if(vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner] == INVALID_PLAYER_ID)
			{
				KillTimer(playerData[playerid][resetVehicleOwner]);
				vehicleData[playerData[playerid][playerStateVehicle]][broadVehicleOwner] = playerid;
			}
		}

		case 3:
		{
			KillTimer(playerData[playerid][playerStateTimer]);
			playerData[playerid][playerStateTimer] = SetTimerEx("DelayPlayerState", 350, false, "ii", playerid, 2);
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
	playerData[playerid][playerAntiSpam]=false;

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) playerData[playerid][resetVehicleOwner] = SetTimerEx("ResetVariableValues", 500, false, "ii", playerid, 5);

	KillTimer(playerData[playerid][resetPlayerDeath]);
	playerData[playerid][playerJustDied]	= true;
	playerData[playerid][resetPlayerDeath] = SetTimerEx("ResetVariableValues", 10000, false, "ii", playerid, 11);
		#if defined OnPlayerDeathACT
		return OnPlayerDeathACT(playerid, killerid, reason);
		#else
		return 1;
			#endif
}
 
public OnPlayerExitVehicle(playerid, vehicleid)
{
	playerData[playerid][resetVehicleOwner] = SetTimerEx("ResetVariableValues", 500, false, "ii", playerid, 5);

	#if defined OnPlayerExitVehicleACT
		return OnPlayerExitVehicleACT(playerid, vehicleid);
	#else
		return 1;
	#endif
}
 
public OnPlayerDisconnect(playerid, reason)
{
	KillTimer(playerData[playerid][connectionTimer]);
	new gplayerStateVehicle;
	gplayerStateVehicle = GetPlayerVehicleID(playerid);

	playerData[playerid][playerCallSpam]  = false;
	playerData[playerid][playerAnimationProtection]	= false;
	playerData[playerid][playerJustDied]	= false;
	playerData[playerid][playerSpawnProtection]	  = false;
	playerData[playerid][isPlayerCheater]	 = false;
	playerData[playerid][playerPosProtection]  = false;
	playerData[playerid][playerFalseDetection]= false;
	playerData[playerid][playerCarsFix]	 = false;
	playerData[playerid][playerPassenger]	= false;
	playerData[playerid][playerDoubleProtection]= false;

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		vehicleData[gplayerStateVehicle][broadVehicleOwner] = INVALID_PLAYER_ID;

	#if defined OnPlayerDisconnectACT
		return OnPlayerDisconnectACT(playerid, reason);
	#else
		return 1;
	#endif
}

public ResetVariableValues(handleid, handletype)
{
	switch(handletype)
	{
		case 0:
		{
			GetPlayerPos(handleid, playerData[handleid][playerPosX], playerData[handleid][playerPosY], playerData[handleid][playerPosZ]);
		}
		case 1:
		{
			playerData[handleid][playerCallSpam]   = false;
		}
		case 2:
		{
			playerData[handleid][playerAntiSpam]	  = false;
		}
		case 3:
		{
			playerData[handleid][playerPosProtection]	= false;
		}
		case 4:
		{
			vehicleData[handleid][enterVehicleFix]   = false, 
			vehicleData[handleid][specificVehicleOwner] = INVALID_PLAYER_ID;
		}
		case 5:
		{
			vehicleData[ playerData[handleid][playerStateVehicle] ][broadVehicleOwner] = INVALID_PLAYER_ID;
		}
		case 6:
		{
			playerData[handleid][playerAntiJack]	  = false;
		}
		case 7:
		{
			TogglePlayerControllable(handleid, 1);
			playerData[handleid][playerCarsFix]	   = false;
		}
		case 8:
		{
			playerData[handleid][playerFalseDetection]  = false;
		}
		case 9:
		{
			playerData[handleid][playerSpawnProtection]		= false;
		}
		case 10:
		{
			playerData[handleid][playerPassenger]	  = false;
		}
		case 11:
		{
			playerData[handleid][playerJustDied]	  = false;
		}
		case 12:
		{
			playerData[handleid][playerDoubleProtection] = false;
		}
		case 13:
		{
			playerData[handleid][playerAnimationProtection]	 = false;
		}
	}
	return 1;
}

public FixPlayerPutInVehicle(playerid, vehicleid, seatid)
{
	playerData[playerid][playerFalseDetection] = true; 
	return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

public ActTogglePlayerSpectating(playerid, bool:toggle)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(!playerData[playerid][playerFalseDetection]) SetTimerEx("ResetVariableValues", 2100, false, "ii", playerid, 8);
	playerData[playerid][playerFalseDetection] = true;
	return TogglePlayerSpectating(playerid, toggle);
}

public ActRemovePlayerFromVehicle(playerid)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(!playerData[playerid][playerDoubleProtection]) SetTimerEx("ResetVariableValues", 2100, false, "ii", playerid, 12);
	playerData[playerid][playerDoubleProtection] = true;
	return RemovePlayerFromVehicle(playerid);
}

public ActPutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if(playerid > MAX_PLAYERS || playerid < 0 || vehicleid < 0 || vehicleid > MAX_VEHICLES) return 0;
	if(vehicleData[vehicleid][enterVehicleFix]) ClearAnimations(vehicleData[vehicleid][specificVehicleOwner]);
	playerData[playerid][playerFalseDetection] = true;

	if(seatid != 0 ) playerData[playerid][playerUpdateVehicle] = vehicleid;
	if(seatid == 0 )  playerData[playerid][playerActualCar] = vehicleid;

	if(GetPlayerState(playerid) == 2 || GetPlayerState(playerid) == 3)
	{
		KillTimer(playerData[playerid][playerInVehicleFix]);
		new Float:PX, Float:PY, Float:PZ;
		GetPlayerPos(playerid, PX, PY, PZ);
		SetPlayerPos(playerid, PX, PY, PZ+2); 
		playerData[playerid][playerInVehicleFix] = SetTimerEx("FixPlayerPutInVehicle", 1000 , false, "iii", playerid, vehicleid, seatid);
	}

	else PutPlayerInVehicle(playerid, vehicleid, seatid);
	return 1;
}

public ActSetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(!playerData[playerid][playerPosProtection]) SetTimerEx("ResetVariableValues", 4000, false, "ii", playerid, 3);
	playerData[playerid][playerPosProtection] = true;
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
	return CallRemoteFunction("ActRemovePlayerFromVehicle", "i", playerid);

stock ActFTogglePlayerSpectating(playerid, bool:toggle)
	return CallRemoteFunction("ActTogglePlayerSpectating", "ii", playerid, toggle);

stock ActFPutPlayerInVehicle(playerid, vehicleid, seatid)
	return CallRemoteFunction("ActPutPlayerInVehicle", "iii", playerid, vehicleid, seatid);

stock ActFSetPlayerPos(playerid, Float:x, Float:y, Float:z)
	return CallRemoteFunction("ActSetPlayerPos", "ifff", playerid, x, y, z);

#endif

static memoryset(aArray[], iValue, iSize = sizeof(aArray)) { //I didn't create this
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
