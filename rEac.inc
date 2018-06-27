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
 
enum EAC_PLAYER_DATA
{
    bool:EacCarSpeed,
 
    EacDTick,
    EacSpamC,
 
    Float:pEacX,
    Float:pEacY,
    Float:pEacZ,

    EacRTimer
}
 
static
    EAC_Player[MAX_PLAYERS][EAC_PLAYER_DATA];
 
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
 
forward EAC_ResetCarSW(playerid);
public  EAC_ResetCarSW(playerid) return EAC_Player[playerid][EacCarSpeed] = false;
 
public EAC_RepairVehicle(vehicleid)
{
    if(vehicleid <0 || vehicleid > MAX_VEHICLES) return 0;
    #if !defined foreach
    for(new i, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if(!IsPlayerConnected(i)) continue;
        if(GetPlayerVehicleID(i) == vehicleid)
        {
            EAC_Player[i][EacDTick] = 0;
            EAC_Player[i][EacSpamC] = 0;
        }
    }
    #else
    foreach(new i: Player)
    {
        if(GetPlayerVehicleID(i) == vehicleid)
        {
            EAC_Player[i][EacDTick] = 0;
            EAC_Player[i][EacSpamC] = 0;
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
            KillTimer(EAC_Player[i][EacRTimer]);
            EAC_Player[i][EacRTimer] = SetTimerEx("EAC_ResetCarSW", 2000, false, "i", i);
            EAC_Player[i][EacCarSpeed] = true;
        }
    }
    #else
    foreach(new i: Player)
    {
        if(GetPlayerVehicleID(i) == vehicleid)
        {
            KillTimer(EAC_Player[i][EacRTimer]);
            EAC_Player[i][EacRTimer] = SetTimerEx("EAC_ResetCarSW", 2000, false, "i", i);
            EAC_Player[i][EacCarSpeed] = true;
        }
    }
    #endif
    return SetVehicleVelocity(vehicleid, X, Y, Z);
}
 
public OnPlayerDisconnect(playerid, reason)
{
    EAC_Player[playerid][EacSpamC]    = 0;
    EAC_Player[playerid][EacDTick]    = 0;
    EAC_Player[playerid][EacCarSpeed] = false;
        #if defined EAC_OnPlayerDisconnect
    return EAC_OnPlayerDisconnect(playerid, reason);
        #else
    return 1;
        #endif
}
 
public OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
    static EacTires, EacLights;
    GetVehicleDamageStatus(vehicleid, EacLights, EacTires, EacLights, EacTires );
    if(GetPlayerState(playerid) == 2)
    {
        if(EacLights || EacTires) return 1;
        static EacTime;
        EacTime = GetTickCount();
 
        if(500 <= EacTime - EAC_Player[playerid][EacDTick] >= 0)
        {
            EAC_Player[playerid][EacSpamC] ++;
            if(EAC_Player[playerid][EacSpamC] >= 10)
            {
                EAC_Player[playerid][EacSpamC] = 0;
                #if defined OnPlayerParticleSpam
                OnPlayerParticleSpam(playerid, vehicleid);
                #endif
                SetPlayerHealth(playerid, 0.0);
                EAC_RepairVehicle(vehicleid);
                SetVehicleHealth(vehicleid, 999.000);
                return 1;
            }
        }
        else EAC_Player[playerid][EacSpamC] = 0;
        EAC_Player[playerid][EacDTick] = EacTime;
    }
        #if defined EAC_OnVehicleDamageStatusUpd
    return EAC_OnVehicleDamageStatusUpd(vehicleid, playerid);
        #else
    return 1;
        #endif
}
 
public OnPlayerUpdate(playerid)
{
    static Float:EacX,  Float:EacY,  Float:EacZ,
           Float:EacvX, Float:EacvY, Float:EacvZ, EacTempSCar;
    GetPlayerPos(playerid, EacX, EacY, EacZ);
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && EacTempSCar != GetPlayerVehicleID(playerid))
        EacTempSCar = GetPlayerVehicleID(playerid);

    if(EacX >= 99999.0 || EacY >= 99999.0 || EacZ >= 99999.0 || EacX <= -99999.0 || EacY <= -99999.0 || EacZ <= -99999.0)
    {
        if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) SetVehicleToRespawn(EacTempSCar);
        SendClientMessage(playerid, 0xa9c4e4ff, "Warning: Excessive X, Y, Z has been breached thus old location set.");
        SetPlayerPos(playerid, EAC_Player[playerid][pEacX], EAC_Player[playerid][pEacY], EAC_Player[playerid][pEacZ]);
    }
    else GetPlayerPos(playerid, EAC_Player[playerid][pEacX], EAC_Player[playerid][pEacY], EAC_Player[playerid][pEacZ]);

    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        GetVehicleVelocity(EacTempSCar, EacvX, EacvY, EacvZ);
        if(( EacvX > 3.0 || EacvY > 3.0 || EacvZ > 3.0 || EacvX < -3.0 || EacvY < -3.0 || EacvZ < -3.0 )
            && ( EacvX != EacX && EacvY != EacY && EacvZ != EacZ ) && !EAC_Player[playerid][EacCarSpeed])
        {
            #if defined OnPlayerCarSwing
            OnPlayerCarSwing(playerid, EacTempSCar);
            #endif
            SetPlayerHealth(playerid, 0.0);
            SetVehicleToRespawn(EacTempSCar);
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
