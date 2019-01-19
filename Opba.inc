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
	forward OPBA_FixIBug(playerid, newinteriorid);
	forward OPBA_FixInteriors(playerid);
#endif

forward OPBA_RVRSPWN(playerid);
forward OPBA_PreventAbuse(playerid);
forward OPBA_RCarFall(playerid);
forward OPBA_FixFallBug(playerid);
forward OPBA_AirbrakeTeleport(playerid);
forward OPBA_ResetPImuunity(playerid);
forward OPBA_SetPPos(playerid);
forward OPBA_FixPassenger(playerid);
forward OPBA_ResetVDeath(playerid);
forward OPBA_ResetTP(playerid);
forward OPBA_RPFALL(playerid);
forward OPBA_RPSURF(playerid);
forward OPBA_PassengerRTP(playerid);
forward OPBA_SafeReset();

#if defined OnPlayerBreakAir
	forward OPBA_CheckRealTP(playerid, tpcode);
#endif

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
	forward OPBA_DelayCall(playerid);
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
		bool:RacBreakImmunity,
		bool:RacSImu,
		bool:RacAbuseP,
		bool:RacProtecInt,
		bool:RacPassenger,
		bool:RacPBFix,
		bool:RacVDeath,
		bool:RacVRspwn,
		bool:RacHoldCheck,
		bool:RacPassFall,
		bool:RacPassengerTP,

		RacSurf,
		RacPause,
		RacABTicks,
		RacPauseTicks,
		RacFallBug,
		RacCarFall,
		RacPlayerInt,
		RacPassFallAbuse,

		Float:RacX,
		Float:RacY,
		Float:RacZ,
		
		RacRTPTimer,
		RacIntTimer,
		RacPATimer,	
		RacFixTimer,	
		RacProTimer,
		RacDetectTimer
	};

#else
	enum E_OPBA_PLAYER_DATA
	{
		bool:RacBreakImmunity,
		bool:RacSImu,
		bool:RacAbuseP,
		bool:RacPassenger,
		bool:RacPBFix,
		bool:RacVDeath,
		bool:RacPassFall,
		bool:RacPassengerTP,
		bool:RacVRspwn,
		bool:RacHoldCheck,

		RacSurf,
		RacPause,
		RacABTicks,
		RacPauseTicks,
		RacPlayerInt,
		RacFallBug,
		RacCarFall,
		RacPassFallAbuse,

		Float:RacX,
		Float:RacY,
		Float:RacZ,
		
		RacRTPTimer,
		RacPATimer,	
		RacFixTimer,	
		RacProTimer,
		RacDetectTimer
	};
#endif

#if defined _FLaggersIncluded_
	static OPBA_WarnsIncrease[MAX_PLAYERS char],	
	bool:OPBA_PlayerDelayer[MAX_PLAYERS char],
	bool:OPBAG_LagInit,
	OPBA_PLagTimer[MAX_PLAYERS] = {-1, ...},
	OPBA_PLagIP[MAX_PLAYERS][16];
#endif

#if !defined OPBA_CUSTOM_INTERIORS

	static 
		OPBA_Player[MAX_PLAYERS][E_OPBA_PLAYER_DATA],
		
		bool:OPBA_PInteriors[MAX_PLAYERS][OPBA_MAX_INTERIORS],

		OPBA_VOwner[MAX_VEHICLES],

		OPBA_VPassenger[MAX_VEHICLES],

		OP_SafeTimer;

#else

	static 
		OPBA_Player[MAX_PLAYERS][E_OPBA_PLAYER_DATA],

		OPBA_VOwner[MAX_VEHICLES],

		OPBA_VPassenger[MAX_VEHICLES],

		OP_SafeTimer;

#endif

public OPBA_AirbrakeTeleport(playerid)
{
	#if !defined OPBA_CUSTOM_INTERIORS
		new OpbaTempInt = GetPlayerInterior(playerid);
	#endif
	
	new OPBA_Vehicle; 
	OPBA_Vehicle = GetPlayerVehicleID(playerid);

	new Float:OPBA_X, Float:OPBA_Y, Float:OPBA_Z;
	GetVehiclePos(OPBA_Vehicle, OPBA_X, OPBA_Y, OPBA_Z);

	if(OPBA_Player[playerid][RacPassFallAbuse] < OPBA_MAX_ABUSES)
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER)
		{
			new Float:OPBA_PX, Float:OPBA_PY, Float:OPBA_PZ;
			GetPlayerPos(playerid, OPBA_PX, OPBA_PY, OPBA_PZ);
			if(OPBA_PZ > 0 && OPBA_Player[playerid][RacZ] > 0 && OPBA_Player[playerid][RacZ] > OPBA_PZ + 3 && OPBA_Player[playerid][RacZ] - OPBA_PZ >= 0 && !OPBA_Player[playerid][RacPassFall])
			{
				OPBA_Player[playerid][RacPassFall] = true;
			}
			else if(OPBA_Player[playerid][RacPassFall]) OPBA_Player[playerid][RacPassFall] = false, OPBA_Player[playerid][RacPassFallAbuse]++;
		}
	}

	if(OPBA_Player[playerid][RacZ] > OPBA_Z && OPBA_Player[playerid][RacZ] < 0 && OPBA_Z < 0 && OPBA_Player[playerid][RacCarFall] == 0 && IsPlayerInAnyVehicle(playerid))
	{
		if(!OPBA_Player[playerid][RacAbuseP])
		{
			if(IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
			{
				OPBA_Player[playerid][RacCarFall] = 1;
				OPBA_Player[playerid][RacPATimer] = SetTimerEx("OPBA_PreventAbuse", 10000, false, "i", playerid);
			}
			else
			{
				OPBA_Player[playerid][RacAbuseP] = false;
			}
		}
	}

	else if(OPBA_Player[playerid][RacCarFall] == 1)
	{
		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				new Float:pOPBA_X, Float:pOPBA_Y, Float:pOPBA_Z;
				GetPlayerPos(playerid, pOPBA_X, pOPBA_Y, pOPBA_Z);
				if(pOPBA_Z >= 0.0)
				{
					OPBA_Player[playerid][RacCarFall] = 2;
					SetTimerEx("OPBA_RCarFall", 2000, false, "i", playerid);
				}
			}
			case 2..3:
			{
				if(OPBA_Z >= 0.0)
				{
					OPBA_Player[playerid][RacCarFall] = 2;
					SetTimerEx("OPBA_RCarFall", 2000, false, "i", playerid);
				}
			}
		}
	}

	else if(OPBA_Player[playerid][RacAbuseP] && OPBA_Player[playerid][RacCarFall] == 0)
	{
		switch(GetPlayerState(playerid))
		{
			case 1:
			{
				new Float:pOPBA_X, Float:pOPBA_Y, Float:pOPBA_Z;
				GetPlayerPos(playerid, pOPBA_X, pOPBA_Y, pOPBA_Z);
				if(pOPBA_Z >= 0.0)
				{
					OPBA_Player[playerid][RacAbuseP] = false;
				}
			}
			case 2..3:
			{
				if(OPBA_Z >= 0.0)
				{
					OPBA_Player[playerid][RacAbuseP] = false;
				}
			}
		}
	}

	if(GetTickCount() < (OPBA_Player[playerid][RacPauseTicks]+1500) && OPBA_Player[playerid][RacPause] == 1)
	{
		GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);
		OPBA_Player[playerid][RacPause] = 2;
		SetTimerEx("OPBA_RPFALL", 2500, false, "i", playerid);
	}
	else if(GetTickCount() > (OPBA_Player[playerid][RacPauseTicks]+1500) &&  OPBA_Player[playerid][RacPause] == 0)
	{
		OPBA_Player[playerid][RacPause] = 1;
	}
	if(OPBA_Player[playerid][RacHoldCheck]) return 0;
	if(OPBA_Player[playerid][RacBreakImmunity] ) return GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);
	#if !defined OPBA_CUSTOM_INTERIORS
		if(OpbaTempInt != OPBA_Player[playerid][RacPlayerInt] || OPBA_Player[playerid][RacProtecInt]) return GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);
	#endif

	if(OPBA_GetPlayerSpeed(playerid) <= OPBA_MAX_DETECT_SPEED && OPBA_Player[playerid][RacPause] == 0 && OPBA_Player[playerid][RacFallBug] == 0
	&& OPBA_Player[playerid][RacSImu] && OPBA_Player[playerid][RacCarFall] == 0)
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
				if(OPBA_Player[playerid][RacPBFix]) return GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);

				if(GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID && GetPlayerSpecialAction(playerid) != 2)
				{
					#if !defined OPBA_CUSTOM_INTERIORS
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]) && OPBA_PInteriors[playerid][OpbaTempInt])
						{
							#if defined OnPlayerBreakAir

								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
									{
										OPBA_PlayerDelayer{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										OPBA_WarnsIncrease{playerid}++;
										if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
									}
									else if(!OPBA_PlayerDelayer{playerid}) OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
								#else
									OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
								#endif

							#else
								SendClientMessageToAll(-1, OPBA_CheatString);
								SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
							#endif
						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_ONFOOT_DETECTION, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]) && OPBA_PInteriors[playerid][OpbaTempInt])
						{
							if(OPBA_Player[playerid][RacABTicks] == 0) 
							{
								KillTimer(OPBA_Player[playerid][RacRTPTimer]);
								OPBA_Player[playerid][RacRTPTimer] = SetTimerEx("OPBA_ResetTP", 50000, false, "i", playerid);
							}
							if(OPBA_Player[playerid][RacABTicks] < OPBA_AB_TICKS) OPBA_Player[playerid][RacABTicks]++;
							if(OPBA_Player[playerid][RacABTicks] == OPBA_AB_TICKS)
							{
								OPBA_Player[playerid][RacABTicks] = 0;
								#if defined OnPlayerBreakAir

									#if defined _FLaggersIncluded_
										if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
										{
											OPBA_PlayerDelayer{playerid} = true;
											SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
											OPBA_WarnsIncrease{playerid}++;
											if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
										}
										else if(!OPBA_PlayerDelayer{playerid}) OnPlayerBreakAir(playerid, 1);
									#else
										OnPlayerBreakAir(playerid, 1);
									#endif

								#else
									SendClientMessageToAll(-1, OPBA_CheatString);
									SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
								#endif
							}
						}
					#else
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
						{
							#if defined OnPlayerBreakAir

								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
									{
										OPBA_PlayerDelayer{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										OPBA_WarnsIncrease{playerid}++;
										if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
									}
									else if(!OPBA_PlayerDelayer{playerid}) OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
								#else
									OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 4);
								#endif

							#else
								SendClientMessageToAll(-1, OPBA_CheatString);
								SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
							#endif
						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_ONFOOT_DETECTION, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
						{
							if(OPBA_Player[playerid][RacABTicks] == 0) 
							{
								KillTimer(OPBA_Player[playerid][RacRTPTimer]);
								OPBA_Player[playerid][RacRTPTimer] = SetTimerEx("OPBA_ResetTP", 50000, false, "i", playerid);
							}
							if(OPBA_Player[playerid][RacABTicks] < OPBA_AB_TICKS) OPBA_Player[playerid][RacABTicks]++;
							if(OPBA_Player[playerid][RacABTicks] == OPBA_AB_TICKS)
							{
								OPBA_Player[playerid][RacABTicks] = 0;
								#if defined OnPlayerBreakAir
								
									#if defined _FLaggersIncluded_
										if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
										{
											OPBA_PlayerDelayer{playerid} = true;
											SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
											OPBA_WarnsIncrease{playerid}++;
											if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
										}
										else if(!OPBA_PlayerDelayer{playerid}) OnPlayerBreakAir(playerid, 1);
									#else
										OnPlayerBreakAir(playerid, 1);
									#endif
									
								#else
									SendClientMessageToAll(-1, OPBA_CheatString);
									SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
								#endif
							}
						}
					#endif
				}
			}
			case 2:
			{
				if(OPBA_Player[playerid][RacVRspwn]) return GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);

				#if !defined OPBA_CUSTOM_INTERIORS
					if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]) && OPBA_PInteriors[playerid][OpbaTempInt])
					{
						#if defined OnPlayerBreakAir
							
							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
								{
									OPBA_PlayerDelayer{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									OPBA_WarnsIncrease{playerid}++;
									if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
								}
								else if(!OPBA_PlayerDelayer{playerid}) OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
							#else
								OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
							#endif

						#else
							SendClientMessageToAll(-1, OPBA_CheatString);
							SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
						#endif
					}
					else if(!IsPlayerInRangeOfPoint(playerid, OPBA_INVEHI_DETECTION, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]) && OPBA_PInteriors[playerid][OpbaTempInt])
					{
						if(OPBA_Player[playerid][RacABTicks] == 0) 
						{
							KillTimer(OPBA_Player[playerid][RacRTPTimer]);
							OPBA_Player[playerid][RacRTPTimer] = SetTimerEx("OPBA_ResetTP", 50000, false, "i", playerid);
						}
						if(OPBA_Player[playerid][RacABTicks] < OPBA_AB_TICKS) OPBA_Player[playerid][RacABTicks]++;
						if(OPBA_Player[playerid][RacABTicks] == OPBA_AB_TICKS)
						{
							OPBA_Player[playerid][RacABTicks] = 0;
							#if defined OnPlayerBreakAir
								
								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
									{
										OPBA_PlayerDelayer{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										OPBA_WarnsIncrease{playerid}++;
										if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
									}
									else if(!OPBA_PlayerDelayer{playerid}) OnPlayerBreakAir(playerid, 2);
								#else
									OnPlayerBreakAir(playerid, 2);
								#endif

							#else
								SendClientMessageToAll(-1, OPBA_CheatString);
								SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
							#endif
						}
					}

				#else

					if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
					{
						#if defined OnPlayerBreakAir
							
							#if defined _FLaggersIncluded_
								if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
								{
									OPBA_PlayerDelayer{playerid} = true;
									SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
									OPBA_WarnsIncrease{playerid}++;
									if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
								}
								else if(!OPBA_PlayerDelayer{playerid}) OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
							#else
								OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 5);
							#endif
							
						#else
							SendClientMessageToAll(-1, OPBA_CheatString);
							SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
						#endif
					}
					else if(!IsPlayerInRangeOfPoint(playerid, OPBA_INVEHI_DETECTION, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
					{
						if(OPBA_Player[playerid][RacABTicks] == 0) 
						{
							KillTimer(OPBA_Player[playerid][RacRTPTimer]);
							OPBA_Player[playerid][RacRTPTimer] = SetTimerEx("OPBA_ResetTP", 50000, false, "i", playerid);
						}
						if(OPBA_Player[playerid][RacABTicks] < OPBA_AB_TICKS) OPBA_Player[playerid][RacABTicks]++;
						if(OPBA_Player[playerid][RacABTicks] == OPBA_AB_TICKS)
						{
							OPBA_Player[playerid][RacABTicks] = 0;
							#if defined OnPlayerBreakAir
								
								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
									{
										OPBA_PlayerDelayer{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										OPBA_WarnsIncrease{playerid}++;
										if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
									}
									else if(!OPBA_PlayerDelayer{playerid}) OnPlayerBreakAir(playerid, 2);
								#else
									OnPlayerBreakAir(playerid, 2);
								#endif
								
							#else
								SendClientMessageToAll(-1, OPBA_CheatString);
								SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
							#endif
						}
					}
				#endif
			}
			case 3:
			{
	 			if(OPBA_VOwner[OPBA_Vehicle] == INVALID_PLAYER_ID && !OPBA_Player[playerid][RacPassFall] && !OPBA_Player[playerid][RacVDeath])
				{
					#if !defined OPBA_CUSTOM_INTERIORS
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]) && OPBA_PInteriors[playerid][OpbaTempInt])
						{
							#if defined OnPlayerBreakAir
							
								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
									{
										OPBA_PlayerDelayer{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										OPBA_WarnsIncrease{playerid}++;
										if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
									}
									else if(!OPBA_PlayerDelayer{playerid}) OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
								#else
									OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
								#endif
							
							#else
								SendClientMessageToAll(-1, OPBA_CheatString);
								SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
							#endif
						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_MAX_PASS_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]) && OPBA_PInteriors[playerid][OpbaTempInt])
						{
							if(OPBA_Player[playerid][RacABTicks] == 0) 
							{
								KillTimer(OPBA_Player[playerid][RacRTPTimer]);
								OPBA_Player[playerid][RacRTPTimer] = SetTimerEx("OPBA_ResetTP", 50000, false, "i", playerid);
							}
							if(OPBA_Player[playerid][RacABTicks] < OPBA_AB_TICKS) OPBA_Player[playerid][RacABTicks]++;
							if(OPBA_Player[playerid][RacABTicks] == OPBA_AB_TICKS)
							{
								OPBA_Player[playerid][RacABTicks] = 0;
								#if defined OnPlayerBreakAir
								
									#if defined _FLaggersIncluded_
										if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
										{
											OPBA_PlayerDelayer{playerid} = true;
											SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
											OPBA_WarnsIncrease{playerid}++;
											if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
										}
										else if(!OPBA_PlayerDelayer{playerid}) OnPlayerBreakAir(playerid, 3);
									#else
										OnPlayerBreakAir(playerid, 3);
									#endif
								
								#else
									SendClientMessageToAll(-1, OPBA_CheatString);
									SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
								#endif
							}
						}
					#else
						if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
						{
							#if defined OnPlayerBreakAir
								
								#if defined _FLaggersIncluded_
									if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
									{
										OPBA_PlayerDelayer{playerid} = true;
										SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
										OPBA_WarnsIncrease{playerid}++;
										if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
									}
									else if(!OPBA_PlayerDelayer{playerid}) OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
								#else
									OPBA_Player[playerid][RacHoldCheck] = true, SetTimerEx("OPBA_CheckRealTP", OPBA_TP_DELAY_TIME, false, "ii", playerid, 6);
								#endif
								
							#else
								SendClientMessageToAll(-1, OPBA_CheatString);
								SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
							#endif
						}
						else if(!IsPlayerInRangeOfPoint(playerid, OPBA_MAX_PASS_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
						{
							if(OPBA_Player[playerid][RacABTicks] == 0) 
							{
								KillTimer(OPBA_Player[playerid][RacRTPTimer]);
								OPBA_Player[playerid][RacRTPTimer] = SetTimerEx("OPBA_ResetTP", 50000, false, "i", playerid);
							}
							if(OPBA_Player[playerid][RacABTicks] < OPBA_AB_TICKS) OPBA_Player[playerid][RacABTicks]++;
							if(OPBA_Player[playerid][RacABTicks] == OPBA_AB_TICKS)
							{
								OPBA_Player[playerid][RacABTicks] = 0;
								#if defined OnPlayerBreakAir
									
									#if defined _FLaggersIncluded_
										if( (IsPlayerLagging(playerid) || OPBA_WarnsIncrease{playerid} > 0) && !OPBA_PlayerDelayer{playerid})
										{
											OPBA_PlayerDelayer{playerid} = true;
											SetTimerEx("OPBA_PlayerRDelayer", 4100, false, "i", playerid);
											OPBA_WarnsIncrease{playerid}++;
											if(OPBA_WarnsIncrease{playerid} >= OPBA_MAX_LAG_WARNS) OPBA_WarnsIncrease{playerid} = 0, OPBA_TogglePlayerLag(playerid, true, 20000);
										}
										else if(!OPBA_PlayerDelayer{playerid}) OnPlayerBreakAir(playerid, 3);
									#else
										OnPlayerBreakAir(playerid, 3);
									#endif
									
								#else
									SendClientMessageToAll(-1, OPBA_CheatString);
									SetTimerEx("OPBA_DelayCall", 250, false, "i", playerid);
								#endif
							}
						}
					#endif
				}
			}
		}
	}
	
	if(!OPBA_Player[playerid][RacHoldCheck]) GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);
	return 1;
}

#if defined OnPlayerBreakAir
	public OPBA_CheckRealTP(playerid, tpcode)
	{
		if(tpcode == 4 && OPBA_IsPEnExCar(playerid)) return OPBA_Player[playerid][RacHoldCheck] = false;
		if(tpcode == 6 && OPBA_Player[playerid][RacPassengerTP]) return OPBA_Player[playerid][RacHoldCheck] = false;
		#if !defined OPBA_CUSTOM_INTERIORS
			if(!OPBA_Player[playerid][RacProtecInt] && !IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
			{
				OnPlayerBreakAir(playerid, tpcode);
			}
		#else
			if(!IsPlayerInRangeOfPoint(playerid, OPBA_TELEPORT_RANGE, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]))
			{
				OnPlayerBreakAir(playerid, tpcode);
			}
		#endif
		OPBA_Player[playerid][RacHoldCheck] = false;
		return 1;
	}
#endif

#if !defined OnPlayerBreakAir
	public OPBA_DelayCall(playerid)
	{
		#if defined OPBA_BAN_VERSION
			BanEx(playerid, "Airbreak");
		#else
			Kick(playerid);
		#endif
		return 1;
	}
#endif

public OPBA_SafeReset() 
{ 
	OPBA_memset(OPBA_VPassenger, INVALID_PLAYER_ID);
	OPBA_memset(OPBA_VOwner, INVALID_PLAYER_ID);
	new RacVeh;

	for(new i, j = GetPlayerPoolSize(); i <= j; i++) 
	{ 
		RacVeh = GetPlayerVehicleID(i);
		if(!IsPlayerConnected(i)) continue; 
		if(GetPlayerState(i)== PLAYER_STATE_DRIVER) 
		OPBA_VOwner[RacVeh]	 = i ; 
		if(GetPlayerState(i) == PLAYER_STATE_PASSENGER)
		OPBA_VPassenger[RacVeh] = i  ;
	} 

	return 1; 
} 

public OPBA_PreventAbuse(playerid)	return OPBA_Player[playerid][RacCarFall]		= 0,	OPBA_Player[playerid][RacAbuseP]	= true;
public OPBA_RCarFall(playerid)		return OPBA_Player[playerid][RacCarFall]		= 0,	KillTimer(OPBA_Player[playerid][RacPATimer]);
public OPBA_FixFallBug(playerid)	return OPBA_Player[playerid][RacFallBug]		= 0;
public OPBA_ResetPImuunity(playerid)return OPBA_Player[playerid][RacBreakImmunity]	= false;
public OPBA_SetPPos(playerid)		return GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]), OPBA_Player[playerid][RacSImu]	= true;
public OPBA_FixPassenger(playerid)	return OPBA_Player[playerid][RacPBFix]			= false;
public OPBA_PassengerRTP(playerid)	return OPBA_Player[playerid][RacPassengerTP]	= false;

public OPBA_ResetVDeath(playerid)	return OPBA_Player[playerid][RacVDeath]			= false;
public OPBA_RVRSPWN(playerid)		return OPBA_Player[playerid][RacVRspwn]			= false;
public OPBA_ResetTP(playerid)		return OPBA_Player[playerid][RacABTicks]		= 0;
public OPBA_RPFALL(playerid)		return OPBA_Player[playerid][RacPause]			= 0;
public OPBA_RPSURF(playerid)		return OPBA_Player[playerid][RacSurf]			= 0;

public OPBA_SetVehicleToRespawn(vehicleid)
{
	if(vehicleid < 0 || vehicleid > MAX_VEHICLES) return 0;
	if(OPBA_VOwner[vehicleid] != INVALID_PLAYER_ID)
	{
		OPBA_Player[ OPBA_VOwner[vehicleid] ][RacVRspwn] = true;
		SetTimerEx("OPBA_RVRSPWN", 1000, false, "i", OPBA_VOwner[vehicleid]);
	}
	else if(OPBA_VPassenger[vehicleid] != INVALID_PLAYER_ID)
	{
		OPBA_Player[ OPBA_VPassenger[vehicleid] ][RacPassengerTP] = true;
		SetTimerEx("OPBA_PassengerRTP", OPBA_RESET_TIME, false, "i", OPBA_VPassenger[vehicleid]);
	}

	return SetVehicleToRespawn(vehicleid);
}

public OPBA_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	KillTimer(OPBA_Player[playerid][RacProTimer]);
	OPBA_Player[playerid][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", playerid);
	OPBA_Player[playerid][RacBreakImmunity] = true;

	OPBA_Player[playerid][RacX] = x, OPBA_Player[playerid][RacY] = y, OPBA_Player[playerid][RacZ] = z;
	return SetPlayerPos(playerid, x, y, z);
}

public OPBA_TogglePlayerSpectating(playerid, bool:toggle)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	KillTimer(OPBA_Player[playerid][RacProTimer]);
	OPBA_Player[playerid][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", playerid);

	OPBA_Player[playerid][RacBreakImmunity] = true;
	return TogglePlayerSpectating(playerid, toggle);
}

public OPBA_SetPlayerPosFindZ(playerid, Float:x, Float:y, Float:z)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	KillTimer(OPBA_Player[playerid][RacProTimer]);
	OPBA_Player[playerid][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", playerid);
	OPBA_Player[playerid][RacBreakImmunity] = true;

	OPBA_Player[playerid][RacX] = x, OPBA_Player[playerid][RacY] = y, OPBA_Player[playerid][RacZ] = z;
	return SetPlayerPosFindZ(playerid, x, y, z);
}

public OPBA_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z)
{
	if(vehicleid > MAX_VEHICLES || vehicleid < 0) return 0;
	if(OPBA_VOwner[vehicleid] != INVALID_PLAYER_ID)
	{
		KillTimer(OPBA_Player[OPBA_VOwner[vehicleid]][RacProTimer]);
		OPBA_Player[OPBA_VOwner[vehicleid]][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", OPBA_VOwner[vehicleid]),
		OPBA_Player[OPBA_VOwner[vehicleid]][RacBreakImmunity] = true;

		OPBA_Player[OPBA_VOwner[vehicleid]][RacX] = x, OPBA_Player[OPBA_VOwner[vehicleid]][RacY] = y,
		OPBA_Player[OPBA_VOwner[vehicleid]][RacZ] = z;
		SetVehiclePos(vehicleid, x, y, z);
	}

	else if(OPBA_VPassenger[vehicleid] != INVALID_PLAYER_ID)
	{
		OPBA_Player[ OPBA_VPassenger[vehicleid] ][RacPassengerTP] = true;
		SetTimerEx("OPBA_PassengerRTP", 1000, false, "i", OPBA_VPassenger[vehicleid]);

		OPBA_Player[OPBA_VPassenger[vehicleid]][RacX] = x, OPBA_Player[OPBA_VPassenger[vehicleid]][RacY] = y,
		OPBA_Player[OPBA_VPassenger[vehicleid]][RacZ] = z;
		SetVehiclePos(vehicleid, x, y, z);
	}

	else SetVehiclePos(vehicleid, x, y, z);
	return 1;
}

public OPBA_PutPlayerInVehicle(playerid, vehicleid, seatid)
{
	if(playerid > MAX_PLAYERS || playerid < 0 || vehicleid < 0 || vehicleid > MAX_VEHICLES) return 0;
	KillTimer(OPBA_Player[playerid][RacProTimer]);
	OPBA_Player[playerid][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", playerid);

	OPBA_Player[playerid][RacBreakImmunity] = true;
	new Float:x , Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);

	OPBA_Player[playerid][RacX] = x, OPBA_Player[playerid][RacY] = y, OPBA_Player[playerid][RacZ] = z;
	return PutPlayerInVehicle(playerid, vehicleid, seatid);
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(OPBA_VOwner[vehicleid] != INVALID_PLAYER_ID && !ispassenger && !OPBA_Player[playerid][RacBreakImmunity])
	{
		KillTimer(OPBA_Player[playerid][RacProTimer]);
		OPBA_Player[playerid][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", playerid);
		OPBA_Player[playerid][RacBreakImmunity] = true;
	}

	#if defined OPBA_OnPlayerEnterVehicle
		return OPBA_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
	#else
		return 1;
	#endif
}

public OnVehicleDeath(vehicleid, killerid)
{
	if(OPBA_VPassenger[vehicleid] != INVALID_PLAYER_ID)
	{
		if(!OPBA_Player[ OPBA_VPassenger[vehicleid] ][RacVDeath])
		{
			OPBA_Player[ OPBA_VPassenger[vehicleid] ][RacVDeath] = true;
			SetTimerEx("OPBA_ResetVDeath", 1200, false, "i", OPBA_VPassenger[vehicleid]);
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
	KillTimer(OPBA_Player[playerid][RacProTimer]);
	OPBA_Player[playerid][RacProTimer] = SetTimerEx("OPBA_ResetPImuunity", OPBA_RESET_TIME, false, "i", playerid);
	OPBA_Player[playerid][RacBreakImmunity] = true;

	#if defined OPBA_OnPlayerExitVehicle
		return OPBA_OnPlayerExitVehicle(playerid, vehicleid);
	#else
		return 1;
	#endif
}

public OnGameModeInit()
{
	OP_SafeTimer = SetTimer("OPBA_SafeReset", 5000, true);

	#if defined OPBA_OnGameModeInit
		return OPBA_OnGameModeInit();
	#else
		return 1;
	#endif
}

public OnGameModeExit()
{
	#if defined _FLaggersIncluded_
		if(!OPBAG_LagInit)
			OPBA_LagInit();
	#endif

	KillTimer(OP_SafeTimer);

	#if defined OPBA_OnGameModeExit
		return OPBA_OnGameModeExit();
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	#if defined _FLaggersIncluded_
		if(OPBA_PLagTimer[playerid] != -1) 
		{
			KillTimer(OPBA_PLagTimer[playerid]);
			OPBA_PLagTimer[playerid] = -1;
		}
		GetPlayerIp(playerid, OPBA_PLagIP[playerid], 16);
	#endif
	OPBA_Player[playerid][RacPlayerInt] = -1;
	OPBA_Player[playerid][RacDetectTimer] = SetTimerEx("OPBA_AirbrakeTeleport", 300, true, "i", playerid);

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
			if(oldstate ==2) OPBA_VOwner[OPBA_TempCar] = INVALID_PLAYER_ID;
		}
		case 2:
		{
			OPBA_VOwner[OPBA_TempCar] = playerid;
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

	KillTimer(OPBA_Player[playerid][RacRTPTimer]);
	#if defined _FLaggersIncluded_
		OPBA_PlayerDelayer{playerid} = false;
		if(OPBA_PLagTimer[playerid] != -1) 
		{
			KillTimer(OPBA_PLagTimer[playerid]);
			OPBA_PLagTimer[playerid] = -1;
			if(reason != 2) 
			{
				#if defined BlockIpAddress
					UnBlockIpAddress(OPBA_PLagIP[playerid]);
				#else
					new OPBA_String[32];
					format(OPBA_String, sizeof(OPBA_String), "unbanip %s", OPBA_PLagIP[playerid]);
					SendRconCommand(OPBA_String);
				#endif
			}
		}
		OPBA_WarnsIncrease{playerid} = 0;
	#endif

	KillTimer(OPBA_Player[playerid][RacDetectTimer]);
	KillTimer(OPBA_Player[playerid][RacProTimer]);

	if(GetPlayerState(playerid) == 2) OPBA_VOwner[GetPlayerVehicleID(playerid)] = INVALID_PLAYER_ID;

	OPBA_Player[playerid][RacPassengerTP]	  = false;
	OPBA_Player[playerid][RacHoldCheck]		  = false;
	OPBA_Player[playerid][RacPassFall] 		  = false;
	OPBA_Player[playerid][RacVRspwn]		  = false;
	OPBA_Player[playerid][RacVDeath]		  = false;
	OPBA_Player[playerid][RacPassenger] 	  = false;
	OPBA_Player[playerid][RacPBFix] 		  = false;
	OPBA_Player[playerid][RacAbuseP]		  = false;
	OPBA_Player[playerid][RacBreakImmunity]   = false;
	OPBA_Player[playerid][RacSImu] 			  = false;	
	OPBA_Player[playerid][RacFallBug] 		  = 0;
	OPBA_Player[playerid][RacPause] 		  = 0;
	OPBA_Player[playerid][RacCarFall]		 = 0;
	OPBA_Player[playerid][RacABTicks] 		  = 0;
	OPBA_Player[playerid][RacPassFallAbuse]   = 0;
	OPBA_Player[playerid][RacSurf] 			  = 0;	

	#if !defined OPBA_CUSTOM_INTERIORS
		OPBA_Player[playerid][RacProtecInt] = false;
		for(new i; i < OPBA_MAX_INTERIORS; i++)
		{
			OPBA_PInteriors[playerid][i] = false;
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
	OPBA_Player[playerid][RacSImu]	= false;
	GetPlayerPos(playerid, OPBA_Player[playerid][RacX], OPBA_Player[playerid][RacY], OPBA_Player[playerid][RacZ]);
	SetTimerEx("OPBA_SetPPos", 4000, false, "i", playerid);

	#if !defined OPBA_CUSTOM_INTERIORS
		OPBA_Player[playerid][RacPlayerInt] = GetPlayerInterior(playerid);
		OPBA_PInteriors[playerid][GetPlayerInterior(playerid)] = true;
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
		KillTimer(OPBA_Player[playerid][RacFixTimer]);
		OPBA_PInteriors[playerid][oldinteriorid] = false;
		OPBA_Player[playerid][RacFixTimer] = SetTimerEx("OPBA_FixIBug", 1500, false, "ii", playerid, newinteriorid);

		#if defined OPBA_OnPlayerInteriorChange
			return OPBA_OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid);
		#else
			return 1;
		#endif
	}

	public OPBA_FixIBug(playerid, newinteriorid) return OPBA_PInteriors[playerid][newinteriorid] = true;
	public OPBA_FixInteriors(playerid) return OPBA_Player[playerid][RacProtecInt] = false;
#endif

public OnPlayerDeath(playerid, killerid, reason)
{
	OPBA_Player[playerid][RacSImu] = false;

	#if defined OPBA_OnPlayerDeath	
		return OPBA_OnPlayerDeath(playerid, killerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerUpdate(playerid)
{
	if(GetPlayerSurfingVehicleID(playerid) != INVALID_VEHICLE_ID && OPBA_Player[playerid][RacSurf] == 0)
	{
		OPBA_Player[playerid][RacSurf] =1;
	}
	else if( OPBA_Player[playerid][RacSurf] == 1 && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID)
	{
		OPBA_Player[playerid][RacSurf] =2;
		SetTimerEx("OPBA_RPSURF", 3000, false, "i", playerid);
	}
	if(GetPlayerState(playerid) == 3 && !OPBA_Player[playerid][RacPassenger])
		OPBA_Player[playerid][RacPassenger] = true;
	else if(GetPlayerState(playerid) != 3 && OPBA_Player[playerid][RacPassenger] )
	{
		OPBA_Player[playerid][RacPassenger] = false;
		OPBA_Player[playerid][RacPBFix] = true;
		SetTimerEx("OPBA_FixPassenger", 1200, false, "i", playerid);
	}
	#if !defined OPBA_CUSTOM_INTERIORS
		new OPBA_PInt;
		OPBA_PInt = GetPlayerInterior(playerid);
		if(OPBA_PInt != OPBA_Player[playerid][RacPlayerInt] && OPBA_Player[playerid][RacPlayerInt] != -1)
		{
			OPBA_Player[playerid][RacProtecInt] = true;
			KillTimer(OPBA_Player[playerid][RacIntTimer]);
			OPBA_Player[playerid][RacIntTimer] = SetTimerEx("OPBA_FixInteriors", 4500, false, "i", playerid);
			OPBA_Player[playerid][RacPlayerInt] = OPBA_PInt;
		}
	#endif
	OPBA_Player[playerid][RacPauseTicks] = GetTickCount();

	if(OPBA_PlayerFalling(playerid) && OPBA_Player[playerid][RacFallBug] == 0) OPBA_Player[playerid][RacFallBug] = 1;
	else if(!OPBA_PlayerFalling(playerid) && OPBA_Player[playerid][RacFallBug] == 1) OPBA_Player[playerid][RacFallBug] = 2, SetTimerEx("OPBA_FixFallBug", 2500, false, "i", playerid);
		
	#if defined OPBA_OnPlayerUpdate	
		return OPBA_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

static stock OPBA_IsPEnExCar(playerid)
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
static OPBA_GetPlayerSpeed(playerid)
{
	new Float:ST[4];

	if(IsPlayerInAnyVehicle(playerid))
	GetVehicleVelocity(GetPlayerVehicleID(playerid),ST[0],ST[1],ST[2]);

	else GetPlayerVelocity(playerid,ST[0],ST[1],ST[2]);
	ST[3] = floatsqroot(floatpower(floatabs(ST[0]), 2.0) + floatpower(floatabs(ST[1]), 2.0) + floatpower(floatabs(ST[2]), 2.0)) * 179.28625;
	return floatround(ST[3]);
}

static OPBA_PlayerFalling(playerid) 
{
	switch(GetPlayerAnimationIndex(playerid))
	{
		case 1130 , 1133: return 1;
	}
	return 0;
}

#if defined _FLaggersIncluded_
	forward OPBA_PlayerRDelayer(playerid);
	public OPBA_PlayerRDelayer(playerid) return OPBA_PlayerDelayer{playerid} = false;

	static OPBA_LagInit() 
	{
		for(new i, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			OnPlayerConnect(i);
		}
		OPBAG_LagInit = true;
		return 1;
	}

	static stock OPBA_TogglePlayerLag(playerid, bool:toggle, lag_delay = 3000) 
	{
		#if !defined BlockIpAddress
			new Opba_Temp_string[32];
		#endif
		
		if(toggle) 
		{
			if(OPBA_PLagTimer[playerid] != -1)
				return 0;

		#if defined BlockIpAddress
			BlockIpAddress(OPBA_PLagIP[playerid], lag_delay);
		#else
			format(Opba_Temp_string, sizeof(Opba_Temp_string), "banip %s", OPBA_PLagIP[playerid]);
			SendRconCommand(Opba_Temp_string);
		#endif
			OPBA_PLagTimer[playerid] = SetTimerEx("L_LagPlayer", lag_delay, true, "dd", playerid, lag_delay);
		}
		else 
		{
			if(OPBA_PLagTimer[playerid] == -1)
				return 0;
		
			KillTimer(OPBA_PLagTimer[playerid]);
			OPBA_PLagTimer[playerid] = -1;

		#if defined BlockIpAddress
			UnBlockIpAddress(OPBA_PLagIP[playerid]);
		#else
			format(Opba_Temp_string, sizeof(Opba_Temp_string), "unbanip %s", OPBA_PLagIP[playerid]);
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

static stock OPBA_memset(aArray[], iValue, iSize = sizeof(aArray)) {//by Slice
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
