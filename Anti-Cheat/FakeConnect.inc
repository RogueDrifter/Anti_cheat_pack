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

static bool:IsPlayerBot(playerid)
{  
	if(IsPlayerNPC(playerid)) return false;
	new temporaryID[80], temporaryNumber;  
	gpci(playerid, temporaryID, sizeof(temporaryID));  
	for(new i; i < strlen(temporaryID); i++)  
	{  
		if(temporaryID[i] >= '0' && temporaryID[i] <= '9')  temporaryNumber++;  
	}  
	return (temporaryNumber >= AFC_NUMBER || strlen(temporaryID) <= AFC_NUMBER);
}

public OnPlayerConnect(playerid)
{
	if(IsPlayerBot(playerid))
	{
		#if defined OnPlayerFakeConnect
			OnPlayerFakeConnect(playerid);
		#endif
	}

	#if defined AFC_OnPlayerConnect
		return AFC_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect AFC_OnPlayerConnect

#endif
