/* 
Made by RogueDrifter 2018/2/13 .
-=-=-=-=-=-=-=-=-=-=-=-=-=-
-=-=- The reconnecting feature was originally made by Lordzy.
-=-=-=-=-=-=-=-=-=-=-=-=-=
Last updated on Apr 18th.
-=-=-=-=-=-=-
Callbacks:
-=-=-=-=-
	OnPlayerBreakAir(playerid, breaktype);
-=-=-=-=Usable Defenitions:-=-=-=-=
USE THIS BEFORE THE INCLUDE IF YOU CANCEL THE ORIGINAL SAMP INTERIORS (INCLUDING MOD GARAGES!) TO AVOID EXTRA CHECKS!
#define OPBA_CUSTOM_INTERIORS 
if you don't, use this to define maximum interiors you use:
#define OPBA_MAX_INTERIORS [number]
If you don't want to use the callback it will auto kick,
If you want it to ban you can use:
#define OPBA_BAN_VERSION
breaktype 1: airbreak on foot.
breaktype 2: airbreak as driver.
breaktype 3: airbreak as passenger.
breaktype 4: teleport on foot.
breaktype 5: teleport as driver.
breaktype 6: teleport as passenger.
*/

#if defined r_BreakAir_ 
	#endinput
#endif

#define r_BreakAir_

#include <a_samp>

#if !defined FILTERSCRIPT

forward OPBA_SetPlayerPos(playerid, Float:x, Float:y, Float:z);
forward OPBA_SetPlayerPosFindZ(playerid, Float:x, Float:y, Float:z);
forward OPBA_TogglePlayerSpectating(playerid, bool:toggle);
forward OPBA_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
forward OPBA_PutPlayerInVehicle(playerid, vehicleid, seatid);
forward OPBA_SetVehicleToRespawn(vehicleid);

#if !defined OPBA_CUSTOM_INTERIORS
	forward FixInteriorChange(playerid, newinteriorid);
	forward FixDeathInterior(playerid);
#endif

forward ResetVehicleToRespawn(playerid);
forward VoidAreaAbuse(playerid);
forward ResetCarFalling(playerid);
forward FixPlayerFallBug(playerid);
forward ProcessCheatDetection(playerid);
forward ResetPlayerImmunity(playerid);
forward UpdatePlayerPositions(playerid);
forward FixPlayerPassengerBug(playerid);
forward ResetVehicleDeath(playerid);
forward ResetPlayerTeleport(playerid);
forward ResetPlayerPause(playerid);
forward ResetPlayerSurf(playerid);
forward PlayerPassengerTeleport(playerid);
forward VehicleBroadReset();

forward CheckActualTeleport(playerid, tpcode);

#define OPBA_TP_DELAY_TIME	  1900
#define OPBA_AB_TICKS 		  2
#define OPBA_MAX_ABUSES 	  2
#define OPBA_TELEPORT_RANGE   92.0
#define OPBA_ONFOOT_DETECTION 22.0
#define OPBA_INVEHI_DETECTION 27.0
#define OPBA_MAX_DETECT_SPEED 30.0
#define OPBA_MAX_PASS_RANGE   40.0

#define OPBA_RESET_TIME 3000

#if defined _FLaggersIncluded_
	#define OPBA_MAX_LAG_WARNS 3
#endif

#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid); 
#endif

#if !defined OPBA_CUSTOM_INTERIORS

#if !defined OPBA_MAX_INTERIORS
	#define OPBA_MAX_INTERIORS 255
#endif

#endif

#if !defined OnPlayerBreakAir
	forward DelayDetectionCall(playerid);
#endif

#if defined OPBA_OnPlayerDisconnect
	forward OPBA_OnPlayerDisconnect(playerid, reason);
#endif

#if defined OPBA_OnPlayerInteriorChange
	forward OPBA_OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
#endif

#if defined OPBA_OnPlayerConnect
	forward OPBA_OnPlayerConnect(playerid);
#endif

#if defined OPBA_OnPlayerUpdate
	forward OPBA_OnPlayerUpdate(playerid);
#endif

#if defined OPBA_OnPlayerDeath
	forward OPBA_OnPlayerDeath(playerid, killerid, reason);
#endif

#if defined OPBA_OnPlayerSpawn
	forward OPBA_OnPlayerSpawn(playerid);
#endif

#if defined OnPlayerBreakAir
	forward OnPlayerBreakAir(playerid, breaktype);
#endif

#if defined OPBA_OnPlayerStateChange
	forward OPBA_OnPlayerStateChange(playerid, newstate, oldstate);
#endif

#if defined OPBA_OnPlayerExitVehicle
	forward OPBA_OnPlayerExitVehicle(playerid, vehicleid);
#endif

#if defined OPBA_OnPlayerEnterVehicle
	forward OPBA_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
#endif

#if defined OPBA_OnGameModeInit
	forward OPBA_OnGameModeInit();
#endif

#if defined OPBA_OnGameModeExit
	forward OPBA_OnGameModeExit();
#endif

#if defined OPBA_OnVehicleDeath
	forward OPBA_OnVehicleDeath(vehicleid, killerid);
#endif

#if !defined OPBA_CUSTOM_INTERIORS
	enum E_OPBA_PLAYER_DATA
	{
		bool:playerBreakImmunity,
		bool:playerSuperImmunity,
		bool:playerAbuseVoid,
		bool:playerInteriorProtection,
		bool:playerPassenger,
		bool:playerBreakFix,
		bool:playerVehicleDeath,
		bool:playerVehicleSpawn,
		bool:playerHoldCheck,
		bool:playerPassFall,
		bool:playerPassengerTeleport,

		playerSurf,
		playerPause,
		playerBreakTicks,
		playerPauseTicks,
		playerFallBug,
		playerCarFall,
		playerInterior,
		playerFallAbuse,

		Float:posPlayerX,
		Float:posPlayerY,
		Float:posPlayerZ,
		
		resetTeleportTimer,
		playerInteriorTimer,
		playerVoidTimer,	
		playerFixTimer,	
		playerProtectionTimer,
		playerDetectionTimer
	};

#else
	enum E_OPBA_PLAYER_DATA
	{
		bool:playerBreakImmunity,
		bool:playerSuperImmunity,
		bool:playerAbuseVoid,
		bool:playerPassenger,
		bool:playerBreakFix,
		bool:playerVehicleDeath,
		bool:playerPassFall,
		bool:playerPassengerTeleport,
		bool:playerVehicleSpawn,
		bool:playerHoldCheck,

		playerSurf,
		playerPause,
		playerBreakTicks,
		playerPauseTicks,
		playerInterior,
		playerFallBug,
		playerCarFall,
		playerFallAbuse,

		Float:posPlayerX,
		Float:posPlayerY,
		Float:posPlayerZ,
		
		resetTeleportTimer,
		playerVoidTimer,	
		playerFixTimer,	
		playerProtectionTimer,
		playerDetectionTimer
	};
#endif

#if defined _FLaggersIncluded_
	static s_warningsIncrease[MAX_PLAYERS char],	
	bool:s_playerDelays[MAX_PLAYERS char],
	bool:s_lagInit,
	s_lagTimer[MAX_PLAYERS] = {-1, ...},
	s_playerIP[MAX_PLAYERS][16];
#endif

#if !defined OPBA_CUSTOM_INTERIORS

	static 
		playerData[MAX_PLAYERS][E_OPBA_PLAYER_DATA],
		
		bool:playerInteriors[MAX_PLAYERS][OPBA_MAX_INTERIORS],

		s_vehicleOwner[MAX_VEHICLES],

		s_vehiclePassenger[MAX_VEHICLES],

		s_broadTimer;

#else

	static 
		playerData[MAX_PLAYERS][E_OPBA_PLAYER_DATA],

		s_vehicleOwner[MAX_VEHICLES],

		s_vehiclePassenger[MAX_VEHICLES],

		s_broadTimer;

#endif

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

static GetPlayerSpeed(playerid)
{
	new Float:ST[4];

	if(IsPlayerInAnyVehicle(playerid))
	GetVehicleVelocity(GetPlayerVehicleID(playerid),ST[0],ST[1],ST[2]);

	else GetPlayerVelocity(playerid,ST[0],ST[1],ST[2]);
	ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
	return floatround(ST[3]);
}

static bool:IsPlayerFalling(playerid) 
{
	switch(GetPlayerAnimationIndex(playerid))
	{
		case 1130 , 1133: return true;
	}
	return false;
}

public ProcessCheatDetection(playerid)
{
	#if !defined OPBA_CUSTOM_INTERIORS
		new OpbaTempInt = GetPlayerInterior(playerid);
	#endif
	
	new OPBA_Vehicle; 
	OPBA_Vehicle = GetPlayerVehicleID(playerid);

	new Float:OPBA_X, Float:OPBA_Y, Float:OPBA_Z;
	GetVehiclePos(OPBA_Vehicle, OPBA_X, OPBA_Y, OPBA_Z);

	if(playerData[playerid][playerFallAbuse] < OPBA_MAX_ABUSES)
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER)
		{
			new Float:OPBA_PX, Float:OPBA_PY, Float:OPBA_PZ;
			GetPlayerPos(playerid, OPBA_PX, OPBA_PY, OPBA_PZ);
			if(OPBA_PZ > 0 && playerData[playerid][posPlayerZ] > 0 && playerData[playerid][posPlayerZ] > OPBA_PZ + 3 && playerData[playerid][posPlayerZ] - OPBA_PZ >= 0 && !playerData[playerid][playerPassFall])
			{
				playerData[playerid][playerPassFall] = true;
			}
			else if(playerData[playerid][playerPassFall]) playerData[playerid][playerPassFall] = false, playerData[playerid][playerFallAbuse]++;
		}
	}

	if(playerData[playerid][posPlayerZ] > OPBA_Z && playerData[playerid][posPlayerZ] < 0 && OPBA_Z < 0 && playerData[playerid][playerCarFall] == 0 && IsPlayerInAnyVehicle(playerid))
	{
		if(!playerData[playerid][playerAbuseVoid])
		{
			if(IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
			{
				playerData[playerid][playerCarFall] = 1;
				playerData[playerid][playerVoidTimer] = SetTimerEx("VoidAreaAbuse", 10000, false, "i", playerid);
			}
			else
			{
				playerData[playerid][playerAbuseVoid] = false;
			}
		}
	}

	else if(playerData[playerid][playerCarFall] == 1)
	{
		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				new Float:pOPBA_X, Float:pOPBA_Y, Float:pOPBA_Z;
				GetPlayerPos(playerid, pOPBA_X, pOPBA_Y, pOPBA_Z);
				if(pOPBA_Z >= 0.0)
				{
					playerData[playerid][playerCarFall] = 2;
					SetTimerEx("ResetCarFalling", 2000, false, "i", playerid);
				}
			}
			case 2..3:
			{
				if(OPBA_Z >= 0.0)
				{
					playerData[playerid][playerCarFall] = 2;
					SetTimerEx("ResetCarFalling", 2000, false, "i", playerid);
				}
			}
		}
	}

	else if(playerData[playerid][playerAbuseVoid] && playerData[playerid][playerCarFall] == 0)
	{
		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				new Float:pOPBA_X, Float:pOPBA_Y, Float:pOPBA_Z;
				GetPlayerPos(playerid, pOPBA_X, pOPBA_Y, pOPBA_Z);
				if(pOPBA_Z >= 0.0)
				{
					playerData[playerid][playerAbuseVoid] = false;
				}
			}
			case 2..3:
			{
				if(OPBA_Z >= 0.0)
				{
					playerData[playerid][playerAbuseVoid] = false;
				}
			}
		}
	}

	if(gettime() < (playerData[playerid][playerPauseTicks]+2) && playerData[playerid][playerPause] == 1)
	{
		GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);
		playerData[playerid][playerPause] = 2;
		SetTimerEx("ResetPlayerPause", 2500, false, "i", playerid);
	}
	else if(gettime() > (playerData[playerid][playerPauseTicks]+2) &&  playerData[playerid][playerPause] == 0)
	{
		playerData[playerid][playerPause] = 1;
	}
	if(playerData[playerid][playerHoldCheck]) return 0;
	if(playerData[playerid][playerBreakImmunity] ) return GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);
	#if !defined OPBA_CUSTOM_INTERIORS
		if(OpbaTempInt != playerData[playerid][playerInterior] || playerData[playerid][playerInteriorProtection]) return GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);
	#endif

	if(GetPlayerSpeed(playerid) <= OPBA_MAX_DETECT_SPEED && playerData[playerid][playerPause] == 0 && playerData[playerid][playerFallBug] == 0
	&& playerData[playerid][playerSuperImmunity] && playerData[playerid][playerCarFall] == 0)
	{
		#if !defined OnPlayerBreakAir
			new OPBA_CheaterName[MAX_PLAYER_NAME], OPBA_CheatString[60];
			GetPlayerName(playerid, OPBA_CheaterName, sizeof(OPBA_CheaterName));

			#if !defined OPBA_BAN_VERSION
				format(OPBA_CheatString, sizeof(OPBA_CheatString), "Server has kicked %s, reason: Airbreak", OPBA_CheaterName);
			#else
				format(OPBA_CheatString, sizeof(OPBA_CheatString), "Server has banned %s, reason: Airbreak", OPBA_CheaterName);
			#endif

		#endif

		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				if(playerData[playerid][playerBreakFix]) return GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);

				if(GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && GetPlayerSpecialAction(playerid) != 2)
				{
					#if !defined OPBA_CUSTOM_INTERIORS
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]) && playerInteriors[playerid][OpbaTempInt])
						{

							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
								{
									s_playerDelays{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									s_warningsIncrease{playerid}++;
									if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
								}
								else if(!s_playerDelays{playerid}) playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
							#else
								playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
							#endif


						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_ONFOOT_DETECTION, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]) && playerInteriors[playerid][OpbaTempInt])
						{
							if(playerData[playerid][playerBreakTicks] == 0) 
							{
								KillTimer(playerData[playerid][resetTeleportTimer]);
								playerData[playerid][resetTeleportTimer] = SetTimerEx("ResetPlayerTeleport", 50000, false, "i", playerid);
							}
							if(playerData[playerid][playerBreakTicks] < OPBA_AB_TICKS) playerData[playerid][playerBreakTicks]++;
							if(playerData[playerid][playerBreakTicks] == OPBA_AB_TICKS)
							{
								playerData[playerid][playerBreakTicks] = 0;

								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
									{
										s_playerDelays{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										s_warningsIncrease{playerid}++;
										if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
									}
									else if(!s_playerDelays{playerid}) 
									{
										#if defined OnPlayerBreakAir
											OnPlayerBreakAir(playerid, 1);
										#else
											SendClientMessageToAll(-1, OPBA_CheatString);
											SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
										#endif
									}
								#else
									#if defined OnPlayerBreakAir
										OnPlayerBreakAir(playerid, 1);
									#else
										SendClientMessageToAll(-1, OPBA_CheatString);
										SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
									#endif
								#endif


							}
						}
					#else
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
						{

							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
								{
									s_playerDelays{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									s_warningsIncrease{playerid}++;
									if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
								}
								else if(!s_playerDelays{playerid}) playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
							#else
								playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
							#endif

						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_ONFOOT_DETECTION, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
						{
							if(playerData[playerid][playerBreakTicks] == 0) 
							{
								KillTimer(playerData[playerid][resetTeleportTimer]);
								playerData[playerid][resetTeleportTimer] = SetTimerEx("ResetPlayerTeleport", 50000, false, "i", playerid);
							}
							if(playerData[playerid][playerBreakTicks] < OPBA_AB_TICKS) playerData[playerid][playerBreakTicks]++;
							if(playerData[playerid][playerBreakTicks] == OPBA_AB_TICKS)
							{
								playerData[playerid][playerBreakTicks] = 0;
								
								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
									{
										s_playerDelays{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										s_warningsIncrease{playerid}++;
										if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
									}
									else if(!s_playerDelays{playerid})
									{
										#if defined OnPlayerBreakAir
											OnPlayerBreakAir(playerid, 1);
										#else 
											SendClientMessageToAll(-1, OPBA_CheatString);
											SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
										#endif
									}
								#else
									#if defined OnPlayerBreakAir
										OnPlayerBreakAir(playerid, 1);
									#else
										SendClientMessageToAll(-1, OPBA_CheatString);
										SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
									#endif
								#endif

							}
						}
					#endif
				}
			}
			case 2:
			{
				if(playerData[playerid][playerVehicleSpawn]) return GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);

				#if !defined OPBA_CUSTOM_INTERIORS
					if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]) && playerInteriors[playerid][OpbaTempInt])
					{
							
						#if defined _FLaggersIncluded_
							if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
							{
								s_playerDelays{playerid} = true;
								SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
								s_warningsIncrease{playerid}++;
								if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
							}
							else if(!s_playerDelays{playerid}) playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
						#else
							playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
						#endif

					}
					else if(!IsPlayerInRangeOfPoint(playerid, OPBA_INVEHI_DETECTION, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]) && playerInteriors[playerid][OpbaTempInt])
					{
						if(playerData[playerid][playerBreakTicks] == 0) 
						{
							KillTimer(playerData[playerid][resetTeleportTimer]);
							playerData[playerid][resetTeleportTimer] = SetTimerEx("ResetPlayerTeleport", 50000, false, "i", playerid);
						}
						if(playerData[playerid][playerBreakTicks] < OPBA_AB_TICKS) playerData[playerid][playerBreakTicks]++;
						if(playerData[playerid][playerBreakTicks] == OPBA_AB_TICKS)
						{
							playerData[playerid][playerBreakTicks] = 0;
								
							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
								{
									s_playerDelays{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									s_warningsIncrease{playerid}++;
									if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
								}
								else if(!s_playerDelays{playerid}) 
								{
									#if defined OnPlayerBreakAir
										OnPlayerBreakAir(playerid, 2);
									#else
										SendClientMessageToAll(-1, OPBA_CheatString);
										SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
									#endif
								}
							#else
								#if defined OnPlayerBreakAir
									OnPlayerBreakAir(playerid, 2);
								#else
									SendClientMessageToAll(-1, OPBA_CheatString);
									SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
								#endif
							#endif
						}
					}

				#else

					if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
					{
							
						#if defined _FLaggersIncluded_
							if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
							{
								s_playerDelays{playerid} = true;
								SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
								s_warningsIncrease{playerid}++;
								if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
							}
							else if(!s_playerDelays{playerid}) playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
						#else
							playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
						#endif

					}
					else if(!IsPlayerInRangeOfPoint(playerid, OPBA_INVEHI_DETECTION, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
					{
						if(playerData[playerid][playerBreakTicks] == 0) 
						{
							KillTimer(playerData[playerid][resetTeleportTimer]);
							playerData[playerid][resetTeleportTimer] = SetTimerEx("ResetPlayerTeleport", 50000, false, "i", playerid);
						}
						if(playerData[playerid][playerBreakTicks] < OPBA_AB_TICKS) playerData[playerid][playerBreakTicks]++;
						if(playerData[playerid][playerBreakTicks] == OPBA_AB_TICKS)
						{
							playerData[playerid][playerBreakTicks] = 0;
								
							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
								{
									s_playerDelays{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									s_warningsIncrease{playerid}++;
									if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
								}
								else if(!s_playerDelays{playerid}) 
								{
									#if defined OnPlayerBreakAir
										OnPlayerBreakAir(playerid, 2);
									#else
										SendClientMessageToAll(-1, OPBA_CheatString);
										SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
									#endif
								}
							#else
								#if defined OnPlayerBreakAir
									OnPlayerBreakAir(playerid, 2);
								#else
									SendClientMessageToAll(-1, OPBA_CheatString);
									SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
								#endif
							#endif
								

						}
					}
				#endif
			}
			case 3:
			{
	 			if(s_vehicleOwner[OPBA_Vehicle] == INVALID_PLAYER_ID && !playerData[playerid][playerPassFall] && !playerData[playerid][playerVehicleDeath])
				{
					#if !defined OPBA_CUSTOM_INTERIORS
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]) && playerInteriors[playerid][OpbaTempInt])
						{							
							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
								{
									s_playerDelays{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									s_warningsIncrease{playerid}++;
									if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
								}
								else if(!s_playerDelays{playerid}) playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
							#else
								playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
							#endif
	
						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_MAX_PASS_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]) && playerInteriors[playerid][OpbaTempInt])
						{
							if(playerData[playerid][playerBreakTicks] == 0) 
							{
								KillTimer(playerData[playerid][resetTeleportTimer]);
								playerData[playerid][resetTeleportTimer] = SetTimerEx("ResetPlayerTeleport", 50000, false, "i", playerid);
							}
							if(playerData[playerid][playerBreakTicks] < OPBA_AB_TICKS) playerData[playerid][playerBreakTicks]++;
							if(playerData[playerid][playerBreakTicks] == OPBA_AB_TICKS)
							{
								playerData[playerid][playerBreakTicks] = 0;
								
								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
									{
										s_playerDelays{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										s_warningsIncrease{playerid}++;
										if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
									}
									else if(!s_playerDelays{playerid}) 
									{
										#if defined OnPlayerBreakAir
											OnPlayerBreakAir(playerid, 3);
										#else
											SendClientMessageToAll(-1, OPBA_CheatString);
											SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
										#endif
									}
								#else
									#if defined OnPlayerBreakAir
										OnPlayerBreakAir(playerid, 3);
									#else
										SendClientMessageToAll(-1, OPBA_CheatString);
										SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
									#endif
								#endif

							}
						}
					#else
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
						{		
							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
								{
									s_playerDelays{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									s_warningsIncrease{playerid}++;
									if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
								}
								else if(!s_playerDelays{playerid}) playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
							#else
								playerData[playerid][playerHoldCheck] = true, SetTimerEx("CheckActualTeleport", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
							#endif

						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_MAX_PASS_RANGE, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]))
						{
							if(playerData[playerid][playerBreakTicks] == 0) 
							{
								KillTimer(playerData[playerid][resetTeleportTimer]);
								playerData[playerid][resetTeleportTimer] = SetTimerEx("ResetPlayerTeleport", 50000, false, "i", playerid);
							}
							if(playerData[playerid][playerBreakTicks] < OPBA_AB_TICKS) playerData[playerid][playerBreakTicks]++;
							if(playerData[playerid][playerBreakTicks] == OPBA_AB_TICKS)
							{
								playerData[playerid][playerBreakTicks] = 0;

								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || s_warningsIncrease{playerid} > 0) && !s_playerDelays{playerid})
									{
										s_playerDelays{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										s_warningsIncrease{playerid}++;
										if(s_warningsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) s_warningsIncrease{playerid} = 0, TogglePlayerLag(playerid, true, 20000);
									}
									else if(!s_playerDelays{playerid}) 
									{
										#if defined OnPlayerBreakAir
											OnPlayerBreakAir(playerid, 3);
										#else
											SendClientMessageToAll(-1, OPBA_CheatString);
											SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
										#endif
									}
								#else
									#if defined OnPlayerBreakAir
										OnPlayerBreakAir(playerid, 3);
									#else
										SendClientMessageToAll(-1, OPBA_CheatString);
										SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
									#endif
								#endif
									
							}
						}
					#endif
				}
			}
		}
	}
	
	if(!playerData[playerid][playerHoldCheck]) GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);
	return 1;
}

public CheckActualTeleport(playerid, tpcode)
{
	#if !defined OnPlayerBreakAir
		new OPBA_CheaterName[MAX_PLAYER_NAME], OPBA_CheatString[60];
		GetPlayerName(playerid, OPBA_CheaterName, sizeof(OPBA_CheaterName));

		#if !defined OPBA_BAN_VERSION
			format(OPBA_CheatString, sizeof(OPBA_CheatString), "Server has kicked %s, reason: Airbreak", OPBA_CheaterName);
		#else
			format(OPBA_CheatString, sizeof(OPBA_CheatString), "Server has banned %s, reason: Airbreak", OPBA_CheaterName);
		#endif

	#endif
	if(tpcode == 4 && IsPlayerEnterExitCar(playerid)) return playerData[playerid][playerHoldCheck] = false;
	if(tpcode == 6 && playerData[playerid][playerPassengerTeleport]) return playerData[playerid][playerHoldCheck] = false;
	#if !defined OPBA_CUSTOM_INTERIORS
		if(!playerData[playerid][playerInteriorProtection])
		{
			#if defined OnPlayerBreakAir
				OnPlayerBreakAir(playerid, tpcode);
			#else
				SendClientMessageToAll(-1, OPBA_CheatString);
				SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
			#endif
		}
	#else
		#if defined OnPlayerBreakAir
			OnPlayerBreakAir(playerid, tpcode);
		#else
			SendClientMessageToAll(-1, OPBA_CheatString);
			SetTimerEx("DelayDetectionCall", 250, false, "i", playerid);
		#endif
	#endif
	playerData[playerid][playerHoldCheck] = false;
	return 1;
}

#if !defined OnPlayerBreakAir
	public DelayDetectionCall(playerid)
	{
		#if defined OPBA_BAN_VERSION
			BanEx(playerid, "Airbreak");
		#else
			Kick(playerid);
		#endif
		return 1;
	}
#endif

public VehicleBroadReset() 
{ 
	memoryset(s_vehiclePassenger, INVALID_PLAYER_ID);
	memoryset(s_vehicleOwner, INVALID_PLAYER_ID);
	new RacVeh;

	for(new i, j = GetPlayerPoolSize(); i <= j; i++) 
	{ 
		RacVeh = GetPlayerVehicleID(i);
		if(!IsPlayerConnected(i)) continue; 
		if(GetPlayerState(i)== PLAYER_STATE_DRIVER) 
		s_vehicleOwner[RacVeh]	 = i ; 
		if(GetPlayerState(i) == PLAYER_STATE_PASSENGER)
		s_vehiclePassenger[RacVeh] = i  ;
	} 

	return 1; 
} 

public VoidAreaAbuse(playerid)	return playerData[playerid][playerCarFall]		= 0,	playerData[playerid][playerAbuseVoid]	= true;
public ResetCarFalling(playerid)		return playerData[playerid][playerCarFall]		= 0,	KillTimer(playerData[playerid][playerVoidTimer]);
public FixPlayerFallBug(playerid)	return playerData[playerid][playerFallBug]		= 0;
public ResetPlayerImmunity(playerid)return playerData[playerid][playerBreakImmunity]	= false;
public UpdatePlayerPositions(playerid)		return GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]), playerData[playerid][playerSuperImmunity]	= true;
public FixPlayerPassengerBug(playerid)	return playerData[playerid][playerBreakFix]			= false;
public PlayerPassengerTeleport(playerid)	return playerData[playerid][playerPassengerTeleport]	= false;

public ResetVehicleDeath(playerid)	return playerData[playerid][playerVehicleDeath]			= false;
public ResetVehicleToRespawn(playerid)		return playerData[playerid][playerVehicleSpawn]			= false;
public ResetPlayerTeleport(playerid)		return playerData[playerid][playerBreakTicks]		= 0;
public ResetPlayerPause(playerid)		return playerData[playerid][playerPause]			= 0;
public ResetPlayerSurf(playerid)		return playerData[playerid][playerSurf]			= 0;

public OPBA_SetVehicleToRespawn(vehicleid)
{
	if(vehicleid < 0 || vehicleid > MAX_VEHICLES) return 0;
	if(s_vehicleOwner[vehicleid] != INVALID_PLAYER_ID)
	{
		playerData[ s_vehicleOwner[vehicleid] ][playerVehicleSpawn] = true;
		SetTimerEx("ResetVehicleToRespawn", 1000, false, "i", s_vehicleOwner[vehicleid]);
	}
	else if(s_vehiclePassenger[vehicleid] != INVALID_PLAYER_ID)
	{
		playerData[ s_vehiclePassenger[vehicleid] ][playerPassengerTeleport] = true;
		SetTimerEx("PlayerPassengerTeleport", OPBA_RESET_TIME, false, "i", s_vehiclePassenger[vehicleid]);
	}

	return SetVehicleToRespawn(vehicleid);
}

public OPBA_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	KillTimer(playerData[playerid][playerProtectionTimer]);
	playerData[playerid][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", playerid);
	playerData[playerid][playerBreakImmunity] = true;

	playerData[playerid][posPlayerX] = x, playerData[playerid][posPlayerY] = y, playerData[playerid][posPlayerZ] = z;
	return SetPlayerPos(playerid, x, y, z);
}

public OPBA_TogglePlayerSpectating(playerid, bool:toggle)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	KillTimer(playerData[playerid][playerProtectionTimer]);
	playerData[playerid][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", playerid);

	playerData[playerid][playerBreakImmunity] = true;
	return TogglePlayerSpectating(playerid, toggle);
}

public OPBA_SetPlayerPosFindZ(playerid, Float:x, Float:y, Float:z)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	KillTimer(playerData[playerid][playerProtectionTimer]);
	playerData[playerid][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", playerid);
	playerData[playerid][playerBreakImmunity] = true;

	playerData[playerid][posPlayerX] = x, playerData[playerid][posPlayerY] = y, playerData[playerid][posPlayerZ] = z;
	return SetPlayerPosFindZ(playerid, x, y, z);
}

public OPBA_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z)
{
	if(vehicleid > MAX_VEHICLES || vehicleid < 0) return 0;
	if(s_vehicleOwner[vehicleid] != INVALID_PLAYER_ID)
	{
		KillTimer(playerData[s_vehicleOwner[vehicleid]][playerProtectionTimer]);
		playerData[s_vehicleOwner[vehicleid]][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", s_vehicleOwner[vehicleid]),
		playerData[s_vehicleOwner[vehicleid]][playerBreakImmunity] = true;

		playerData[s_vehicleOwner[vehicleid]][posPlayerX] = x, playerData[s_vehicleOwner[vehicleid]][posPlayerY] = y,
		playerData[s_vehicleOwner[vehicleid]][posPlayerZ] = z;
		SetVehiclePos(vehicleid, x, y, z);
	}

	else if(s_vehiclePassenger[vehicleid] != INVALID_PLAYER_ID)
	{
		playerData[ s_vehiclePassenger[vehicleid] ][playerPassengerTeleport] = true;
		SetTimerEx("PlayerPassengerTeleport", 1000, false, "i", s_vehiclePassenger[vehicleid]);

		playerData[s_vehiclePassenger[vehicleid]][posPlayerX] = x, playerData[s_vehiclePassenger[vehicleid]][posPlayerY] = y,
		playerData[s_vehiclePassenger[vehicleid]][posPlayerZ] = z;
		SetVehiclePos(vehicleid, x, y, z);
	}

	else SetVehiclePos(vehicleid, x, y, z);
	return 1;
}

public OPBA_PutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if(playerid > MAX_PLAYERS || playerid < 0 || vehicleid < 0 || vehicleid > MAX_VEHICLES) return 0;
	KillTimer(playerData[playerid][playerProtectionTimer]);
	playerData[playerid][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", playerid);

	playerData[playerid][playerBreakImmunity] = true;
	new Float:x , Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);

	playerData[playerid][posPlayerX] = x, playerData[playerid][posPlayerY] = y, playerData[playerid][posPlayerZ] = z;
	return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(s_vehicleOwner[vehicleid] != INVALID_PLAYER_ID && !ispassenger && !playerData[playerid][playerBreakImmunity])
	{
		KillTimer(playerData[playerid][playerProtectionTimer]);
		playerData[playerid][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", playerid);
		playerData[playerid][playerBreakImmunity] = true;
	}

	#if defined OPBA_OnPlayerEnterVehicle
		return OPBA_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	#else
		return 1;
	#endif
}

public OnVehicleDeath(vehicleid, killerid)
{
	if(s_vehiclePassenger[vehicleid] != INVALID_PLAYER_ID)
	{
		if(!playerData[ s_vehiclePassenger[vehicleid] ][playerVehicleDeath])
		{
			playerData[ s_vehiclePassenger[vehicleid] ][playerVehicleDeath] = true;
			SetTimerEx("ResetVehicleDeath", 1200, false, "i", s_vehiclePassenger[vehicleid]);
		}
	}

	#if defined OPBA_OnVehicleDeath
		return OPBA_OnVehicleDeath(vehicleid, killerid);
	#else
		return 1;
	#endif
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	KillTimer(playerData[playerid][playerProtectionTimer]);
	playerData[playerid][playerProtectionTimer] = SetTimerEx("ResetPlayerImmunity", OPBA_RESET_TIME, false, "i", playerid);
	playerData[playerid][playerBreakImmunity] = true;

	#if defined OPBA_OnPlayerExitVehicle
		return OPBA_OnPlayerExitVehicle(playerid, vehicleid);
	#else
		return 1;
	#endif
}

public OnGameModeInit()
{
	s_broadTimer = SetTimer("VehicleBroadReset", 5000, true);

	#if defined OPBA_OnGameModeInit
		return OPBA_OnGameModeInit();
	#else
		return 1;
	#endif
}

public OnGameModeExit()
{
	#if defined _FLaggersIncluded_
		if(!s_lagInit)
			InitiateLag();
	#endif

	KillTimer(s_broadTimer);

	#if defined OPBA_OnGameModeExit
		return OPBA_OnGameModeExit();
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	#if defined _FLaggersIncluded_
		if(s_lagTimer[playerid] != -1) 
		{
			KillTimer(s_lagTimer[playerid]);
			s_lagTimer[playerid] = -1;
		}
		GetPlayerIp(playerid, s_playerIP[playerid], 16);
	#endif
	playerData[playerid][playerInterior] = -1;
	playerData[playerid][playerDetectionTimer] = SetTimerEx("ProcessCheatDetection", 300, true, "i", playerid);

	#if defined OPBA_OnPlayerConnect
		return OPBA_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	new OPBA_TempCar = GetPlayerVehicleID(playerid);

	switch(newstate)
	{
		case 1:
		{
			if(oldstate ==2) s_vehicleOwner[OPBA_TempCar] = INVALID_PLAYER_ID;
		}
		case 2:
		{
			s_vehicleOwner[OPBA_TempCar] = playerid;
		}
	}

	#if defined OPBA_OnPlayerStateChange
		return OPBA_OnPlayerStateChange(playerid, newstate, oldstate);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid, reason)
{ 

	KillTimer(playerData[playerid][resetTeleportTimer]);
	#if defined _FLaggersIncluded_
		s_playerDelays{playerid} = false;
		if(s_lagTimer[playerid] != -1) 
		{
			KillTimer(s_lagTimer[playerid]);
			s_lagTimer[playerid] = -1;
			if(reason != 2) 
			{
				#if defined BlockIpAddress
					UnBlockIpAddress(s_playerIP[playerid]);
				#else
					new OPBA_String[32];
					format(OPBA_String, sizeof(OPBA_String), "unbanip %s", s_playerIP[playerid]);
					SendRconCommand(OPBA_String);
				#endif
			}
		}
		s_warningsIncrease{playerid} = 0;
	#endif

	KillTimer(playerData[playerid][playerDetectionTimer]);
	KillTimer(playerData[playerid][playerProtectionTimer]);

	if(GetPlayerState(playerid) == 2) s_vehicleOwner[GetPlayerVehicleID(playerid)] = INVALID_PLAYER_ID;

	playerData[playerid][playerPassengerTeleport]	  = false;
	playerData[playerid][playerHoldCheck]		  = false;
	playerData[playerid][playerPassFall] 		  = false;
	playerData[playerid][playerVehicleSpawn]		  = false;
	playerData[playerid][playerVehicleDeath]		  = false;
	playerData[playerid][playerPassenger] 	  = false;
	playerData[playerid][playerBreakFix] 		  = false;
	playerData[playerid][playerAbuseVoid]		  = false;
	playerData[playerid][playerBreakImmunity]   = false;
	playerData[playerid][playerSuperImmunity] 			  = false;	
	playerData[playerid][playerFallBug] 		  = 0;
	playerData[playerid][playerPause] 		  = 0;
	playerData[playerid][playerCarFall]		 = 0;
	playerData[playerid][playerBreakTicks] 		  = 0;
	playerData[playerid][playerFallAbuse]   = 0;
	playerData[playerid][playerSurf] 			  = 0;	

	#if !defined OPBA_CUSTOM_INTERIORS
		playerData[playerid][playerInteriorProtection] = false;
		for(new i; i < OPBA_MAX_INTERIORS; i++)
		{
			playerInteriors[playerid][i] = false;
		}

	#endif
	#if defined OPBA_OnPlayerDisconnect
		return OPBA_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	playerData[playerid][playerSuperImmunity]	= false;
	GetPlayerPos(playerid, playerData[playerid][posPlayerX], playerData[playerid][posPlayerY], playerData[playerid][posPlayerZ]);
	SetTimerEx("UpdatePlayerPositions", 4000, false, "i", playerid);

	#if !defined OPBA_CUSTOM_INTERIORS
		playerData[playerid][playerInterior] = GetPlayerInterior(playerid);
		playerInteriors[playerid][GetPlayerInterior(playerid)] = true;
	#endif

	#if defined OPBA_OnPlayerSpawn
		return OPBA_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

#if !defined OPBA_CUSTOM_INTERIORS

	public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
	{
		KillTimer(playerData[playerid][playerFixTimer]);
		playerInteriors[playerid][oldinteriorid] = false;
		playerData[playerid][playerFixTimer] = SetTimerEx("FixInteriorChange", 1500, false, "ii", playerid, newinteriorid);

		#if defined OPBA_OnPlayerInteriorChange
			return OPBA_OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
		#else
			return 1;
		#endif
	}

	public FixInteriorChange(playerid, newinteriorid) return playerInteriors[playerid][newinteriorid] = true;
	public FixDeathInterior(playerid) return playerData[playerid][playerInteriorProtection] = false;
#endif

public OnPlayerDeath(playerid, killerid, reason)
{
	playerData[playerid][playerSuperImmunity] = false;

	#if defined OPBA_OnPlayerDeath	
		return OPBA_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerUpdate(playerid)
{
	if(GetPlayerSurfingVehicleID(playerid) != INVALID_VEHICLE_ID && playerData[playerid][playerSurf] == 0)
	{
		playerData[playerid][playerSurf] =1;
	}
	else if( playerData[playerid][playerSurf] == 1 && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID)
	{
		playerData[playerid][playerSurf] =2;
		SetTimerEx("ResetPlayerSurf", 3000, false, "i", playerid);
	}
	if(GetPlayerState(playerid) == 3 && !playerData[playerid][playerPassenger])
		playerData[playerid][playerPassenger] = true;
	else if(GetPlayerState(playerid) != 3 && playerData[playerid][playerPassenger] )
	{
		playerData[playerid][playerPassenger] = false;
		playerData[playerid][playerBreakFix] = true;
		SetTimerEx("FixPlayerPassengerBug", 1200, false, "i", playerid);
	}
	#if !defined OPBA_CUSTOM_INTERIORS
		new OPBA_PInt;
		OPBA_PInt = GetPlayerInterior(playerid);
		if(OPBA_PInt != playerData[playerid][playerInterior] && playerData[playerid][playerInterior] != -1)
		{
			playerData[playerid][playerInteriorProtection] = true;
			KillTimer(playerData[playerid][playerInteriorTimer]);
			playerData[playerid][playerInteriorTimer] = SetTimerEx("FixDeathInterior", 4500, false, "i", playerid);
			playerData[playerid][playerInterior] = OPBA_PInt;
		}
	#endif
	playerData[playerid][playerPauseTicks] = gettime();

	if(IsPlayerFalling(playerid) && playerData[playerid][playerFallBug] == 0) playerData[playerid][playerFallBug] = 1;
	else if(!IsPlayerFalling(playerid) && playerData[playerid][playerFallBug] == 1) playerData[playerid][playerFallBug] = 2, SetTimerEx("FixPlayerFallBug", 2500, false, "i", playerid);
		
	#if defined OPBA_OnPlayerUpdate	
		return OPBA_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

#if defined _FLaggersIncluded_
	forward OPBA_PlayerRDelayer(playerid);
	public OPBA_PlayerRDelayer(playerid) return s_playerDelays{playerid} = false;

	static InitiateLag() 
	{
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			OnPlayerConnect(i);
		}
		s_lagInit = true;
		return 1;
	}

	static TogglePlayerLag(playerid, bool:toggle, lag_delay = 3000) 
	{
		#if !defined BlockIpAddress
			new Opba_Temp_string[32];
		#endif
		
		if(toggle) 
		{
			if(s_lagTimer[playerid] != -1)
				return 0;

		#if defined BlockIpAddress
			BlockIpAddress(s_playerIP[playerid], lag_delay);
		#else
			format(Opba_Temp_string, sizeof(Opba_Temp_string), "banip %s", s_playerIP[playerid]);
			SendRconCommand(Opba_Temp_string);
		#endif
			s_lagTimer[playerid] = SetTimerEx("L_LagPlayer", lag_delay, true, "dd", playerid, lag_delay);
		}
		else 
		{
			if(s_lagTimer[playerid] == -1)
				return 0;
		
			KillTimer(s_lagTimer[playerid]);
			s_lagTimer[playerid] = -1;

		#if defined BlockIpAddress
			UnBlockIpAddress(s_playerIP[playerid]);
		#else
			format(Opba_Temp_string, sizeof(Opba_Temp_string), "unbanip %s", s_playerIP[playerid]);
			SendRconCommand(Opba_Temp_string);
		#endif
		}
		return 1;
	}
#endif


#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect OPBA_OnPlayerConnect

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect OPBA_OnPlayerDisconnect

#if defined _ALS_OnPlayerStateChange
  #undef OnPlayerStateChange
#else
	#define _ALS_OnPlayerStateChange
#endif

#define OnPlayerStateChange OPBA_OnPlayerStateChange

#if defined _ALS_OnPlayerInteriorChange
  #undef OnPlayerInteriorChange
#else
	#define _ALS_OnPlayerInteriorChange
#endif

#define OnPlayerInteriorChange OPBA_OnPlayerInteriorChange

#if defined _ALS_OnVehicleDeath
  #undef OnVehicleDeath
#else
	#define _ALS_OnVehicleDeath
#endif

#define OnVehicleDeath OPBA_OnVehicleDeath

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn OPBA_OnPlayerSpawn

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate OPBA_OnPlayerUpdate

#if defined _ALS_OnPlayerExitVehicle
  #undef OnPlayerExitVehicle
#else
	#define _ALS_OnPlayerExitVehicle
#endif

#define OnPlayerExitVehicle OPBA_OnPlayerExitVehicle

#if defined _ALS_OnPlayerEnterVehicle
  #undef OnPlayerEnterVehicle
#else
	#define _ALS_OnPlayerEnterVehicle
#endif

#define OnPlayerEnterVehicle OPBA_OnPlayerEnterVehicle

#if defined _ALS_OnGameModeInit
  #undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit OPBA_OnGameModeInit

#if defined _ALS_OnGameModeExit
  #undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit OPBA_OnGameModeExit

#if defined _ALS_OnPlayerDeath
  #undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath OPBA_OnPlayerDeath

#if defined _ALS_SetPlayerPos
  #undef SetPlayerPos
#else
	#define _ALS_SetPlayerPos
#endif

#define SetPlayerPos OPBA_SetPlayerPos

#if defined _ALS_SetVehiclePos
  #undef SetVehiclePos
#else
	#define _ALS_SetVehiclePos
#endif

#define SetVehiclePos OPBA_SetVehiclePos

#if defined _ALS_TogglePlayerSpectating
  #undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif

#define TogglePlayerSpectating OPBA_TogglePlayerSpectating

#if defined _ALS_SetPlayerPosFindZ
  #undef SetPlayerPosFindZ
#else
	#define _ALS_SetPlayerPosFindZ
#endif

#define SetPlayerPosFindZ OPBA_SetPlayerPosFindZ

#if defined _ALS_SetVehicleToRespawn
  #undef SetVehicleToRespawn
#else
	#define _ALS_SetVehicleToRespawn
#endif

#define SetVehicleToRespawn OPBA_SetVehicleToRespawn

#if defined _ALS_PutPlayerInVehicle
  #undef PutPlayerInVehicle
#else
	#define _ALS_PutPlayerInVehicle
#endif

#define PutPlayerInVehicle OPBA_PutPlayerInVehicle

#else //If its a filterscript.

stock OPBA_FSetVehicleToRespawn(vehicleid)
	return CallRemoteFunction("OPBA_SetVehicleToRespawn", "i", vehicleid);
	
stock OPBA_FSetPlayerPos(playerid, Float:x, Float:y, Float:z)
	return CallRemoteFunction("OPBA_SetPlayerPos", "ifff", playerid, x, y, z);

stock OPBA_FSetVehiclePos(vehicleid, Float:x, Float:y, Float:z)
	return CallRemoteFunction("OPBA_SetVehiclePos", "ifff", vehicleid, x, y, z);

stock OPBA_FSetPlayerPosFindZ(playerid, Float:x, Float:y, Float:z)
	return CallRemoteFunction("OPBA_SetPlayerPosFindZ", "ifff", playerid, x, y, z);

stock OPBA_FPutPlayerInVehicle(playerid, vehicleid, seatid)
	return CallRemoteFunction("OPBA_PutPlayerInVehicle", "iii", playerid, vehicleid, seatid);

stock OPBA_FTogglePlayerSpectating(playerid, bool:toggle)
	return CallRemoteFunction("OPBA_TogglePlayerSpectating", "ii", playerid, toggle);

#if defined _ALS_SetPlayerPos
  #undef SetPlayerPos
#else
	#define _ALS_SetPlayerPos
#endif

#define SetPlayerPos OPBA_FSetPlayerPos

#if defined _ALS_SetVehicleToRespawn
  #undef SetVehicleToRespawn
#else
	#define _ALS_SetVehicleToRespawn
#endif

#define SetVehicleToRespawn OPBA_FSetVehicleToRespawn

#if defined _ALS_SetVehiclePos
  #undef SetVehiclePos
#else
	#define _ALS_SetVehiclePos
#endif

#define SetVehiclePos OPBA_FSetVehiclePos

#if defined _ALS_SetPlayerPosFindZ
  #undef SetPlayerPosFindZ
#else
	#define _ALS_SetPlayerPosFindZ
#endif

#define SetPlayerPosFindZ OPBA_FSetPlayerPosFindZ

#if defined _ALS_PutPlayerInVehicle
  #undef PutPlayerInVehicle
#else
	#define _ALS_PutPlayerInVehicle
#endif

#define PutPlayerInVehicle OPBA_FPutPlayerInVehicle

#if defined _ALS_TogglePlayerSpectating
  #undef TogglePlayerSpectating
#else
	#define _ALS_TogglePlayerSpectating
#endif

#define TogglePlayerSpectating OPBA_FTogglePlayerSpectating

#endif

static memoryset(aArray[], iValue, iSize = sizeof(aArray)) {//by Slice
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
