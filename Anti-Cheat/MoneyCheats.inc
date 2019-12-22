/* 
Advanced anti cash cheat by RogueDrifter 2018/2/9 -=-==Last updated on Apr 18th
USAGE: Include in all scripts and use the following:
-=-=-=-=-=-=-=-=-=-=
Callback:
-=-=-=-=-=
OnPlayerCashCheat(playerid, oldcash, newcash, amount);
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
playerid = the player who used cheats to spoof cash.
oldcash = the original cash he had before using cheats.
newcash = the current amount of cash after he used cheats.
amount = the amount of cash he used cheats to spoof.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#if defined _rAntiCashCheat_
	#endinput
#endif

#define _rAntiCashCheat_

#include <a_samp>

#if !defined FILTERSCRIPT 

enum E_AMC_INFO
{
	bool:cashImmunity,

	detectingTimer,

	pausedTicks,
	actualCash

};

static 
	playerData[MAX_PLAYERS][E_AMC_INFO];

#if defined Amc_OnPlayerDisconnect
	forward Amc_OnPlayerDisconnect(playerid, reason);
#endif

#if defined Amc_OnPlayerConnect
	forward Amc_OnPlayerConnect(playerid);
#endif

#if defined Amc_OnPlayerUpdate
	forward Amc_OnPlayerUpdate(playerid);
#endif

forward Amc_GivePlayerMoney(playerid, amount);
forward Amc_ResetPlayerMoney(playerid);
forward Amc_GetPlayerMoney(playerid);

forward ResetImmunity(playerid);
forward CheckActualCash(playerid);

static bool:IsPlayerPaused(playerid)
	return (gettime() > (playerData[playerid][pausedTicks]+2));
	
public ResetImmunity(playerid) 
	return playerData[playerid][cashImmunity]= false;

public Amc_GetPlayerMoney(playerid) 
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	return playerData[playerid][actualCash];
}

public CheckActualCash(playerid)
{
	if(IsPlayerPaused(playerid)) return 0;

	new rAmcTempCash;
	rAmcTempCash = GetPlayerMoney(playerid);

	if(playerData[playerid][actualCash] < rAmcTempCash && !playerData[playerid][cashImmunity])
	{
		new rAmcSumUp;
		rAmcSumUp = rAmcTempCash - playerData[playerid][actualCash];
		if(rAmcSumUp <= 1000) return GivePlayerMoney(playerid, - rAmcSumUp);

		#if defined OnPlayerCashCheat
			OnPlayerCashCheat(playerid, playerData[playerid][actualCash], rAmcTempCash, rAmcSumUp);
		#endif

		GivePlayerMoney(playerid, - rAmcSumUp);
	}
	return 1;
}

public Amc_ResetPlayerMoney(playerid)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!playerData[playerid][cashImmunity]) SetTimerEx("ResetImmunity", 3000, false, "i", playerid);
	playerData[playerid][cashImmunity]= true;

	playerData[playerid][actualCash] = 0;
	return ResetPlayerMoney(playerid);
}

public Amc_GivePlayerMoney(playerid, amount)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!playerData[playerid][cashImmunity]) SetTimerEx("ResetImmunity", 3000, false, "i", playerid);
	playerData[playerid][cashImmunity]= true;

	playerData[playerid][actualCash] += amount;
	return GivePlayerMoney(playerid, amount);
}

public OnPlayerDisconnect(playerid, reason)
{
	playerData[playerid][cashImmunity]= false;
	KillTimer(playerData[playerid][detectingTimer]);
	
	#if defined Amc_OnPlayerDisconnect
		return Amc_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerUpdate(playerid)
{
	playerData[playerid][pausedTicks] = gettime();

	#if defined Amc_OnPlayerUpdate
		return Amc_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerConnect(playerid)
{
	Amc_ResetPlayerMoney(playerid);
	playerData[playerid][detectingTimer] = SetTimerEx("CheckActualCash", 15000, true, "i", playerid);

	#if defined Amc_OnPlayerConnect
		return Amc_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

#if defined OnPlayerCashCheat
	forward OnPlayerCashCheat(playerid, oldcash, newcash, amount);
#endif

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect Amc_OnPlayerDisconnect

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate Amc_OnPlayerUpdate

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect Amc_OnPlayerConnect

#if defined _ALS_GivePlayerMoney
  #undef GivePlayerMoney
#else
	#define _ALS_GivePlayerMoney
#endif

#define GivePlayerMoney Amc_GivePlayerMoney

#if defined _ALS_ResetPlayerMoney
  #undef ResetPlayerMoney
#else
	#define _ALS_ResetPlayerMoney
#endif

#define ResetPlayerMoney Amc_ResetPlayerMoney

#if defined _ALS_GetPlayerMoney
  #undef GetPlayerMoney
#else
	#define _ALS_GetPlayerMoney
#endif

#define GetPlayerMoney Amc_GetPlayerMoney

#else

stock Amc_FGetPlayerMoney(playerid)
	return CallRemoteFunction("Amc_GetPlayerMoney","i",playerid);

stock Amc_FGivePlayerMoney(playerid, money)
	return CallRemoteFunction("Amc_GivePlayerMoney","ii",playerid, money);

stock Amc_FResetPlayerMoney(playerid)
	return CallRemoteFunction("Amc_ResetPlayerMoney","i",playerid);

#if defined _ALS_GivePlayerMoney
  #undef GivePlayerMoney
#else
	#define _ALS_GivePlayerMoney
#endif

#define GivePlayerMoney Amc_FGivePlayerMoney

#if defined _ALS_ResetPlayerMoney
  #undef ResetPlayerMoney
#else
	#define _ALS_ResetPlayerMoney
#endif

#define ResetPlayerMoney Amc_FResetPlayerMoney

#if defined _ALS_GetPlayerMoney
  #undef GetPlayerMoney
#else
	#define _ALS_GetPlayerMoney
#endif

#define GetPlayerMoney Amc_FGetPlayerMoney

#endif
