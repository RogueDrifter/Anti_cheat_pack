/*
Anti jetpack cheats by Rogue 2018/3/26.
-=-=-=-=
Last updated on April 18th.
-=-=-=-
OnPlayerJetpackCheat(playerid);
-=-=-=-=
playerid = the cheater who spoofed a jetpack.
-=-=-=-=-=--=-=-=
*/

#include <a_samp>

#if defined _rAntiJC_Included
	#endinput
#endif

#define _rAntiJC_Included

#if !defined FILTERSCRIPT

#if defined OnPlayerJetpackCheat
	forward OnPlayerJetpackCheat(playerid);
#endif

forward AJC_SetPlayerSpecialAction(playerid, actionid);

static bool:s_jetpackProtection[MAX_PLAYERS char];

#if defined AJC_OnPlayerUpdate
	forward AJC_OnPlayerUpdate(playerid);
#endif

#if defined AJC_OnPlayerDisconnect
	forward AJC_OnPlayerDisconnect(playerid, reason);
#endif

#if defined AJC_OnPlayerSpawn
	forward AJC_OnPlayerSpawn(playerid);
#endif

public AJC_SetPlayerSpecialAction(playerid, actionid)
{
	if(playerid > MAX_PLAYERS || playerid < 0) return 0;
	if(actionid == SPECIAL_ACTION_USEJETPACK) s_jetpackProtection{playerid} = true;
	return SetPlayerSpecialAction(playerid, actionid);
}

public OnPlayerDisconnect(playerid, reason)
{
	s_jetpackProtection{playerid} = false;
	#if defined AJC_OnPlayerDisconnect
		return AJC_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerUpdate(playerid)
{
	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK && !s_jetpackProtection{playerid})
	{
		s_jetpackProtection{playerid} = true;
		SetPlayerHealth(playerid, 0.0);
		#if defined OnPlayerJetpackCheat
			OnPlayerJetpackCheat(playerid);
		#endif
	}

	#if defined AJC_OnPlayerUpdate
		return AJC_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	s_jetpackProtection{playerid} = false;
	#if defined AJC_OnPlayerSpawn
		return AJC_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate AJC_OnPlayerUpdate

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn AJC_OnPlayerSpawn

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect AJC_OnPlayerDisconnect

#if defined _ALS_SetPlayerSpecialAction
  #undef SetPlayerSpecialAction
#else
    #define _ALS_SetPlayerSpecialAction
#endif

#define SetPlayerSpecialAction AJC_SetPlayerSpecialAction

#else

stock AJC_FSetPlayerSpecialAction(playerid, actionid)
	return CallRemoteFunction("AJC_SetPlayerSpecialAction", "ii", playerid, actionid);

#if defined _ALS_SetPlayerSpecialAction
  #undef SetPlayerSpecialAction
#else
    #define _ALS_SetPlayerSpecialAction
#endif

#define SetPlayerSpecialAction AJC_FSetPlayerSpecialAction

#endif
