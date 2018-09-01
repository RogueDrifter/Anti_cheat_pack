/*
Anti spam by Rogue 2018/3/25.
----
Last Updated Mar 29th
-=-=-=-=-=-=-

Callbacks:
----------

OnPlayerSpamChat(playerid);
--
playerid = the spamer
-=-=-=-=-=--=-=-=
*/

#if !defined FILTERSCRIPT 

#if defined _rAntiSpammer_Included
	#endinput
#endif

#define _rAntiSpammer_Included

#include <a_samp>

#if defined OnPlayerSpamChat
	forward OnPlayerSpamChat(playerid);
#endif

#if defined Aspa_OnPlayerText
	forward Aspa_OnPlayerText(playerid, text[]);
#endif

#if defined Aspa_OnPlayerDisconnect
	forward Aspa_OnPlayerDisconnect(playerid, reason);
#endif

enum ASPA_PLAYER_DATA
{
	AspaTextone,
	AspaTexttwo,

	AspaRTimer,
	AspaWarns
}

static 
	Aspa_Player[MAX_PLAYERS][ASPA_PLAYER_DATA];

#define MAX_WAIT_TIME 500 //max waiting time in MS (half a second here) 

public OnPlayerText(playerid, text[]) 
{ 
	if(Aspa_Player[playerid][AspaTexttwo] == 0 && Aspa_Player[playerid][AspaTextone] ==0 ) Aspa_Player[playerid][AspaTextone] = GetTickCount(); 
	else if(Aspa_Player[playerid][AspaTextone] != 0 && Aspa_Player[playerid][AspaTexttwo] == 0) Aspa_Player[playerid][AspaTexttwo] = GetTickCount(); 
	
	else if(Aspa_Player[playerid][AspaTextone] != 0 && Aspa_Player[playerid][AspaTexttwo] != 0 && Aspa_Player[playerid][AspaTexttwo] - Aspa_Player[playerid][AspaTextone] < MAX_WAIT_TIME) 
	{ 
		Aspa_Player[playerid][AspaWarns]++;
		switch(Aspa_Player[playerid][AspaWarns])
		{
			case 1:
			{
				KillTimer(Aspa_Player[playerid][AspaRTimer]);
				Aspa_Player[playerid][AspaRTimer] = SetTimerEx("ASPA_RWarns", 1500, false, "i", playerid);
			}
			case 2:
			{
				KillTimer(Aspa_Player[playerid][AspaRTimer]);
				Aspa_Player[playerid][AspaRTimer] = SetTimerEx("ASPA_RWarns", 3000, false, "i", playerid);
			}
			case 3:
			{
				KillTimer(Aspa_Player[playerid][AspaRTimer]);
				Aspa_Player[playerid][AspaWarns] = 0;
				CallLocalFunction("OnPlayerSpamChat", "i", playerid);
			}
		}
		Aspa_Player[playerid][AspaTextone] = 0; 
		Aspa_Player[playerid][AspaTexttwo] = 0; 
		return 0; 
	} 
	else if(Aspa_Player[playerid][AspaTextone] != 0 && Aspa_Player[playerid][AspaTexttwo] != 0 && Aspa_Player[playerid][AspaTexttwo] - Aspa_Player[playerid][AspaTextone] >= MAX_WAIT_TIME) 
	{ 
		Aspa_Player[playerid][AspaTextone] =0; 
		Aspa_Player[playerid][AspaTexttwo] =0; 
	} 

	#if defined Aspa_OnPlayerText
		return Aspa_OnPlayerText(playerid, text);
	#else
		return 1;
	#endif
} 

public OnPlayerDisconnect(playerid, reason) 
{ 
	Aspa_Player[playerid][AspaTextone] = 0; 
	Aspa_Player[playerid][AspaTexttwo] = 0; 

	#if defined Aspa_OnPlayerDisconnect
		return Aspa_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}  

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect Aspa_OnPlayerDisconnect

#if defined _ALS_OnPlayerText
	#undef OnPlayerText
#else
	#define _ALS_OnPlayerText
#endif

#define OnPlayerText Aspa_OnPlayerText

#endif