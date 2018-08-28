/*
Quick minigame using my own tech lights lib for custom samp controlled lights

Dependancies: 
Tech_lights.inc     (By me)
ZCMD        (By Zeex)
Streamer (By Incognito)

*/

#include <a_samp>

#include <streamer>
#include <zcmd>

#define MAX_BOUNDARIES 16
#define MAX_RTLIGHTS 9
#define RTL_MAX_PLAYER_RANGE 5.0
#define RT_MAX_LIGHTS_CHECK 250

#include <Tech_Lights>

//Future updates
#define LEVER_ZERO_LIGHT 2
#define LEVER_ONE_LIGHT 1
#define LEVER_TWO_LIGHT 0
#define LEVER_THREE_LIGHT 6
#define LEVER_FOUR_LIGHT 8
#define LEVER_FIVE_LIGHT 7
#define LEVER_SIX_LIGHT 3
#define LEVER_SEVEN_LIGHT 4
#define LEVER_EIGHT_LIGHT 5

#define LIGHT_ZERO_LEVER 2
#define LIGHT_ONE_LEVER 1
#define LIGHT_TWO_LEVER 0
#define LIGHT_THREE_LEVER 6
#define LIGHT_FOUR_LEVER 7
#define LIGHT_FIVE_LEVER 8
#define LIGHT_SIX_LEVER 3
#define LIGHT_SEVEN_LEVER 5
#define LIGHT_EIGHT_LEVER 4
//End.

#define SECOND_STAGE_INTERVAL 30 //In seconds
#define THIRD_STAGE_INTERVAL 60//In seconds
#define END_STAGE_INTERVAL  90//In seconds
#define RT_EVENT_BLINKER_INTERVAL 3//In seconds

#define RT_X_OUT 0.0
#define RT_Y_OUT 0.0 
#define RT_Z_OUT 10.0
#define RT_EVENT_CASH 350

enum TECH_LIGHTS_EVENT_PLAYER
{
    bool:RT_EventIn,

    RT_StageOne_Timer,
    RT_StageTwo_Timer,
    RT_StageThree_Timer,
    RT_EVENT_Blinker,

    RT_PlayerPoints,
    RT_PlayerStage
}

new 
    RT_EventPlayer[MAX_PLAYERS][TECH_LIGHTS_EVENT_PLAYER],
    Levers[MAX_RTLIGHTS], Boundaries[MAX_BOUNDARIES],
    bool:RT_EVENT_STATUS
    ;

static CreateStageOne()
{
    //Lights type 0
    CreateLights(0, 0, 293.91925, 1750.07861, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 1, 293.39740, 1746.07971, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 2, 289.44562, 1746.37366, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 3, 285.14310, 1745.97229, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 4, 285.17328, 1749.85925, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 5, 284.79230, 1753.61194, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 6, 289.62296, 1749.96204, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 7, 289.45261, 1753.91528, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(0, 8, 294.04929, 1754.08264, 469.40671,   91.00000, 0.00000, 0.00000);
    return 1;
}

static CreateStageTwo()
{ 
    //Lights type 2
    CreateLights(2, 0, 293.91925, 1750.07861, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 1, 293.39740, 1746.07971, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 2, 289.44562, 1746.37366, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 3, 285.14310, 1745.97229, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 4, 285.17328, 1749.85925, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 5, 284.79230, 1753.61194, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 6, 289.62296, 1749.96204, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 7, 289.45261, 1753.91528, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(2, 8, 294.04929, 1754.08264, 469.40671,   91.00000, 0.00000, 0.00000);
    return 1;
}

static CreateStageThree()
{
    //Lights type 1
    CreateLights(1, 0, 293.91925, 1750.07861, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 1, 293.39740, 1746.07971, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 2, 289.44562, 1746.37366, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 3, 285.14310, 1745.97229, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 4, 285.17328, 1749.85925, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 5, 284.79230, 1753.61194, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 6, 289.62296, 1749.96204, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 7, 289.45261, 1753.91528, 469.40671,   91.00000, 0.00000, 0.00000);
    CreateLights(1, 8, 294.04929, 1754.08264, 469.40671,   91.00000, 0.00000, 0.00000);
    return 1;
}


static DestroyStageOne()
{
    DestroyAllLights(0);
    return 1;
}

static DestroyStageTwo()
{
    DestroyAllLights(2);
    return 1;
}

static DestroyStageThree()
{
    DestroyAllLights(1);
    return 1;
}

static EndEvent(playerid)
{
    if(RT_EventPlayer[playerid][RT_EventIn])
    {
        RT_EVENT_STATUS = false;
        RT_EventPlayer[playerid][RT_EventIn] = false;
        RT_EventPlayer[playerid][RT_PlayerStage] = 0;
        RT_EventPlayer[playerid][RT_PlayerPoints] = 0;
        KillAllTimers(playerid);
    }
    return 1;
}

static MovePlayerNextStage(playerid)
{
    if(RT_EventPlayer[playerid][RT_PlayerStage] == 1) KeepAllLightsOn(RT_EventPlayer[playerid][RT_PlayerStage] - 1, 0), ShowAllLights(RT_EventPlayer[playerid][RT_PlayerStage] - 1);
    else if(RT_EventPlayer[playerid][RT_PlayerStage] == 2) KeepAllLightsOn(RT_EventPlayer[playerid][RT_PlayerStage], 0), ShowAllLights(RT_EventPlayer[playerid][RT_PlayerStage]);
    if(RT_EventPlayer[playerid][RT_PlayerStage] <= 2) RT_EventPlayer[playerid][RT_PlayerStage]++;

    switch(RT_EventPlayer[playerid][RT_PlayerStage])
    {
        case 2:
        {
            CreateStageTwo();
            KeepAllLightsOn(RT_EventPlayer[playerid][RT_PlayerStage], 1);
            ShowAllLights(RT_EventPlayer[playerid][RT_PlayerStage]);
        }

        case 3:
        {
            CreateStageThree();
            KeepAllLightsOn(RT_EventPlayer[playerid][RT_PlayerStage] - 2, 1);
            ShowAllLights(RT_EventPlayer[playerid][RT_PlayerStage] - 2);
        }
    }
    return 1;
}

static SendPlayerOutEvent(playerid)
{
    new RT_EVENT_STRING[90];
    format(RT_EVENT_STRING, sizeof(RT_EVENT_STRING), "RT_EVENT: You've finished the event and gained %d with a score of %d", RT_EVENT_CASH+RT_EventPlayer[playerid][RT_PlayerPoints], RT_EventPlayer[playerid][RT_PlayerPoints] );
    SendClientMessage(playerid, -1, RT_EVENT_STRING);
    SetPlayerPos(playerid, RT_X_OUT, RT_Y_OUT, RT_Z_OUT);
    if(RT_EventPlayer[playerid][RT_PlayerPoints] >= 1) GivePlayerMoney(playerid, RT_EVENT_CASH * RT_EventPlayer[playerid][RT_PlayerPoints]);
    return 1;
}

static KillAllTimers(playerid)
{
    KillTimer(RT_EventPlayer[playerid][RT_StageOne_Timer]);
    KillTimer(RT_EventPlayer[playerid][RT_StageTwo_Timer]);
    KillTimer(RT_EventPlayer[playerid][RT_StageThree_Timer]);
    KillTimer(RT_EventPlayer[playerid][RT_EVENT_Blinker]);
    return 1;
}

static EndCurrentStage(playerid)
{
    switch(RT_EventPlayer[playerid][RT_PlayerStage])
    {
        case 1:
        {
            DestroyStageOne();
        }
        case 2:
        {
            DestroyStageTwo();
        }
        case 3:
        {
            DestroyStageThree();
        }
    }
    return 1;
}

public OnFilterScriptInit()
{
    //Levers
    Levers[0] = CreateDynamicObject(18886, 289.41461, 1745.21643, 470.06631,   182.00000, 0.00000, 54.00000); //LightID 2
    Levers[1] = CreateDynamicObject(18886, 293.58713, 1745.16284, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 1
    Levers[2] = CreateDynamicObject(18886, 293.87866, 1749.13684, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 0
    Levers[3] = CreateDynamicObject(18886, 289.46060, 1749.00635, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 6
    Levers[4] = CreateDynamicObject(18886, 294.10678, 1753.12378, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 8
    Levers[5] = CreateDynamicObject(18886, 289.42963, 1752.96680, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 7
    Levers[6] = CreateDynamicObject(18886, 285.09726, 1745.09302, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 3
    Levers[7] = CreateDynamicObject(18886, 285.07999, 1749.02759, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 4
    Levers[8] = CreateDynamicObject(18886, 284.95264, 1752.88257, 469.98630,   182.00000, 0.00000, 54.00000); //LightID 5

    //Boundaries
    Boundaries[0] = CreateDynamicObject(971, 287.14969, 1746.78040, 470.86960,   90.00000, 0.00000, 0.00000);
    Boundaries[1] = CreateDynamicObject(971, 291.48679, 1746.81604, 470.86960,   90.00000, 0.00000, 0.00000);
    Boundaries[2] = CreateDynamicObject(971, 291.47110, 1751.17456, 470.86960,   90.00000, 0.00000, 0.00000);
    Boundaries[3] = CreateDynamicObject(971, 287.18909, 1751.16614, 470.86960,   90.00000, 0.00000, 0.00000);
    Boundaries[4] = CreateDynamicObject(971, 287.18311, 1754.72534, 474.41809,   0.00000, 0.00000, 0.00000);
    Boundaries[5] = CreateDynamicObject(971, 295.91989, 1750.40234, 474.41809,   0.00000, 0.00000, 90.50000);
    Boundaries[6] = CreateDynamicObject(971, 287.18311, 1743.16528, 474.41809,   0.00000, 0.00000, 0.00000);
    Boundaries[7] = CreateDynamicObject(971, 291.48309, 1743.26526, 474.41809,   0.00000, 0.00000, 0.00000);
    Boundaries[8] = CreateDynamicObject(971, 291.40311, 1754.72534, 474.41809,   0.00000, 0.00000, 0.00000);
    Boundaries[9] = CreateDynamicObject(971, 295.91989, 1747.66235, 474.41809,   0.00000, 0.00000, 90.50000);
    Boundaries[10] = CreateDynamicObject(971, 282.79568, 1750.28467, 474.39810,   0.00000, 0.00000, 90.50000);
    Boundaries[11] = CreateDynamicObject(971, 282.81989, 1747.58228, 474.39810,   0.00000, 0.00000, 90.50000);
    Boundaries[12] = CreateDynamicObject(971, 287.28970, 1746.80042, 477.96960,   90.00000, 0.00000, 0.00000);
    Boundaries[13] = CreateDynamicObject(971, 291.57751, 1746.83862, 477.96960,   90.00000, 0.00000, 0.00000);
    Boundaries[14] = CreateDynamicObject(971, 291.44101, 1751.19092, 477.96960,   90.00000, 0.00000, 0.00000);
    Boundaries[15] = CreateDynamicObject(971, 287.20496, 1751.15149, 477.96960,   90.00000, 0.00000, 0.00000);
    return 1;
}

public OnFilterScriptExit()
{
    for(new i; i < MAX_BOUNDARIES; i++)
    {
        DestroyDynamicObject(Boundaries[i]);
    }

    for(new i; i < MAX_RTLIGHTS; i++)
    {
        DestroyDynamicObject(Levers[i]);
    }

    if(RT_EVENT_STATUS)
    {
        RT_EVENT_STATUS = false;
        DestroyStageOne();
        DestroyStageTwo();
        DestroyStageThree();
    }
    return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
    if(RT_EventPlayer[playerid][RT_EventIn])
    {
        EndCurrentStage(playerid);
        EndEvent(playerid);
    }
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    if(RT_EventPlayer[playerid][RT_EventIn])
    {
        EndCurrentStage(playerid);
        EndEvent(playerid);
    }
    return 1;
}

forward RT_SecondStage(playerid);
public RT_SecondStage(playerid)
{
    EndCurrentStage(playerid);
    MovePlayerNextStage(playerid);
    return 1;
}

forward RT_ThirdStage(playerid);
public RT_ThirdStage(playerid)
{
    EndCurrentStage(playerid);
    MovePlayerNextStage(playerid);
    return 1;
}

forward RT_EndStages(playerid);
public RT_EndStages(playerid)
{
    EndCurrentStage(playerid);
    SendPlayerOutEvent(playerid);
    EndEvent(playerid);
    return 1;
}

forward RT_BlinkLights(playerid);
public RT_BlinkLights(playerid)
{
    new RT_RandomLight = random(8);
    switch(RT_EventPlayer[playerid][RT_PlayerStage])
    {
        case 1:
        {
            if(!IsLightValid(RT_RandomLight, 0) || IsLightBlinking(RT_RandomLight, 0)) return 0;
            BlinkToDestroyLights(RT_RandomLight, 0, 3000);
        }
        case 2:
        {
            if(!IsLightValid(RT_RandomLight, 2) || IsLightBlinking(RT_RandomLight, 2)) return 0;
            BlinkToDestroyLights(RT_RandomLight, 2, 3000);
        }
        case 3:
        {
            if(!IsLightValid(RT_RandomLight, 1) || IsLightBlinking(RT_RandomLight, 1)) return 0;
            BlinkToDestroyLights(RT_RandomLight, 1, 3000);
        }
    }
    return 1;
}

CMD:joinlightsevent(playerid)
{
    if(RT_EVENT_STATUS) return SendClientMessage(playerid, -1, "RT_EVENT: The event is currently running, can't start another one.");
    
    SetPlayerTime(playerid, 0, 0);
    SetPlayerWeather(playerid, 5);
    SetPlayerPos(playerid, 289.41461, 1745.21643, 473.06631);
    SendClientMessage(playerid, -1, "RT_EVENT: The event has started.");

    RT_EventPlayer[playerid][RT_StageOne_Timer] = SetTimerEx("RT_SecondStage", SECOND_STAGE_INTERVAL*1000, false, "i", playerid);
    RT_EventPlayer[playerid][RT_StageTwo_Timer] = SetTimerEx("RT_ThirdStage", THIRD_STAGE_INTERVAL*1000, false, "i", playerid);
    RT_EventPlayer[playerid][RT_StageThree_Timer] = SetTimerEx("RT_EndStages", END_STAGE_INTERVAL*1000, false, "i", playerid);
    RT_EventPlayer[playerid][RT_EVENT_Blinker] = SetTimerEx("RT_BlinkLights", RT_EVENT_BLINKER_INTERVAL*1000, true, "i", playerid);

    RT_EVENT_STATUS = true;
    RT_EventPlayer[playerid][RT_EventIn] = true;
    RT_EventPlayer[playerid][RT_PlayerStage] = 1;
    
    CreateStageOne();
    KeepAllLightsOn(0, 1);
    ShowAllLights(0);
    return 1;
}

//Future updates
public OnLightGoOff(LightID, LightType)
{
    if(RT_EVENT_STATUS)
    {
        switch(LightID)
        {
            case 0:
            {
                
            }
            case 1:
            {

            }
            case 2:
            {

            }
            case 3:
            {

            }
            case 4:
            {

            }
            case 5:
            {

            }
            case 6:
            {

            }
            case 7:
            {

            }
            case 8:
            {

            }
        }
    }
    return 1;
}
//End.

public OnPlayerTriggerLight(playerid, LightID, LightType)
{
    if(RT_EventPlayer[playerid][RT_EventIn] && IsLightBlinking(LightID, LightType))
    {
        BlinkLights(LightID, LightType, 0);
        ShowLights(LightID, LightType);
        switch(RT_EventPlayer[playerid][RT_PlayerStage])
        {
            case 1:
            {
                BlinkLights(LightID, LightType, 0);
                RT_EventPlayer[playerid][RT_PlayerPoints]++;
                GameTextForPlayer(playerid, "+1 Point!", 1500, 3);
            }
            case 2:
            {
                BlinkLights(LightID, LightType, 0);
                RT_EventPlayer[playerid][RT_PlayerPoints] = RT_EventPlayer[playerid][RT_PlayerPoints] + 2;
                GameTextForPlayer(playerid, "+2 Points!", 1500, 3);
            }
            case 3:
            {
                BlinkLights(LightID, LightType, 0);
                RT_EventPlayer[playerid][RT_PlayerPoints] = RT_EventPlayer[playerid][RT_PlayerPoints] + 3;
                GameTextForPlayer(playerid, "+3 Point!", 1500, 3);
            }
        }
    }
    return 1;
}
