/*
Anti fake client by Rogue 2018/3/26
-=-=-=
Last updated on Mar 30th.
=-=-=-=-=-=-=-=
OnPlayerFakeConnect(playerid);
-=-=
playerid = the cheater who connected with a fake client.
-=-=-=-=-=-=
*/

#if !defined FILTERSCRIPT 

#if defined _rAFC_Included_
	#endinput
#endif

#define _rAFC_Included_
#define AFC_NUMBER 30

#include <a_samp>

#if !defined gpci
	native gpci(playerid, serial[], len);
#endif

#if defined AFC_OnPlayerConnect
	forward AFC_OnPlayerConnect(playerid);
#endif

#if defined OnPlayerFakeConnect
	forward OnPlayerFakeConnect(playerid);
#endif

public OnPlayerConnect(playerid)
{
	if(AFC_IsPlayerBot(playerid))
	{
		CallLocalFunction("OnPlayerFakeConnect", "i", playerid);
	}

	#if defined AFC_OnPlayerConnect
		return AFC_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

static AFC_IsPlayerBot(playerid)
{  
	if(IsPlayerNPC(playerid)) return 0;
	new TempId[80], TempNumb;  
	gpci(playerid, TempId, sizeof(TempId));  
	for(new i = 0; i < strlen(TempId); i++)  
	{  
		if(TempId[i] >= '0' && TempId[i] <= '9')  TempNumb++;  
	}  
	return (TempNumb >= AFC_NUMBER || strlen(TempId) <= AFC_NUMBER) ? true : false;
}

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect AFC_OnPlayerConnect

#endif
