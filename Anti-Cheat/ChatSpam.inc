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

enum E_SPAM_PLAYER_DATA
{
	firstText, 
	secondText, 

	spamTimer, 
	spamWarnings
}

static 
	playerData[MAX_PLAYERS][E_SPAM_PLAYER_DATA];

#define MAX_WAIT_TIME 1 //max waiting time in MS (half a second here) 

public OnPlayerText(playerid, text[]) 
{ 
	if(playerData[playerid][secondText] == 0 && playerData[playerid][firstText] ==0 ) playerData[playerid][firstText] = gettime(); 
	else if(playerData[playerid][firstText] != 0 && playerData[playerid][secondText] == 0) playerData[playerid][secondText] = gettime(); 
	
	else if(playerData[playerid][firstText] != 0 && playerData[playerid][secondText] != 0 && playerData[playerid][secondText] - playerData[playerid][firstText] < MAX_WAIT_TIME) 
	{ 
		playerData[playerid][spamWarnings]++;
		switch(playerData[playerid][spamWarnings])
		{
			case 1:
			{
				KillTimer(playerData[playerid][spamTimer]);
				playerData[playerid][spamTimer] = SetTimerEx("ASPA_RWarns", 1500, false, "i", playerid);
			}
			case 2:
			{
				KillTimer(playerData[playerid][spamTimer]);
				playerData[playerid][spamTimer] = SetTimerEx("ASPA_RWarns", 3000, false, "i", playerid);
			}
			case 3:
			{
				KillTimer(playerData[playerid][spamTimer]);
				playerData[playerid][spamWarnings] = 0;
				CallLocalFunction("OnPlayerSpamChat", "i", playerid);
			}
		}
		playerData[playerid][firstText] = 0; 
		playerData[playerid][secondText] = 0; 
		return 0; 
	} 
	else if(playerData[playerid][firstText] != 0 && playerData[playerid][secondText] != 0 && playerData[playerid][secondText] - playerData[playerid][firstText] >= MAX_WAIT_TIME) 
	{ 
		playerData[playerid][firstText] =0; 
		playerData[playerid][secondText] =0; 
	} 

	#if defined Aspa_OnPlayerText
		return Aspa_OnPlayerText(playerid, text);
	#else
		return 1;
	#endif
} 

public OnPlayerDisconnect(playerid, reason) 
{ 
	playerData[playerid][firstText] = 0; 
	playerData[playerid][secondText] = 0; 

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