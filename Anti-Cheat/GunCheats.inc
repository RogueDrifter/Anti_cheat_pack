/*
Anti weapons/ammo cheat by Rogue-=-=-= Created on 2018/3/26.
=-=-==--=-=-=-=-=
Last updated on June 27th.
-=-=-=-=-=-=-=-=-=-==-=-=-=-=-
OnPlayerGunCheat(playerid, weaponid, ammo, hacktype);
-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=
playerid = the cheater
weaponid = the weapon used
ammo 	 = the ammo used
hacktype 1 = weapon cheat 	   (spoofed weapon)
hacktype 2 = ammo cheat   	   (spoofed ammo  )
hacktype 3 = freeze ammo cheat (self explained)
hacktype 4 = Rapid fire 	   (also super fast cbugging)
-=-=-=-=-=-=-=-=-=-=
*/

#if defined _rAgc_Included_
	#endinput
#endif

#define _rAgc_Included_

#include <a_samp>

#if !defined FILTERSCRIPT 

forward rAgc_ResetPlayerWeapons(playerid);
forward rAgc_SetPlayerAmmo(playerid, weaponid, ammo);
forward rAgc_GivePlayerWeapon(playerid, weaponid, ammo);

forward HandleVariablesReset(handleid, handletype);
forward DetectCheaters(playerid);
forward DelayRapidShots(playerid, weapon, ammo);

#define rAgc_MAX_WEAPONIDS 		   47
#define RAGC_IMMU_TIME	   		   2000
#define RAGC_MAX_RAPID_TIME_NORMAL 200
#define RAGC_MAX_WARNINGS_FREEZE   4
#define RAGC_MAX_SPOOF_WARNS	   3

#if defined OnPlayerGunCheat
	forward OnPlayerGunCheat(playerid, weaponid, ammo, hacktype);
#endif

#if defined rAgc_OnPlayerConnect
	forward rAgc_OnPlayerConnect(playerid);
#endif

#if defined rAgc_OnPlayerDisconnect
	forward rAgc_OnPlayerDisconnect(playerid, reason);
#endif

#if defined rAgc_OnPlayerSpawn
	forward rAgc_OnPlayerSpawn(playerid);
#endif

#if defined rAgc_OnPlayerUpdate
	forward rAgc_OnPlayerUpdate(playerid);
#endif

#if defined rAgc_OnPlayerWeaponShot
	forward rAgc_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

enum E_WEAPONS_DATA
{
	basicPlayerAmmo, 
	basicPlayerWeapons, 

	newPlayerAmmo, 
	oldPlayerAmmo
}

enum E_PLAYER_DATA
{
	bool:resetWeaponsImmunity, 
	bool:changeAmmoImmunity, 

	ammoWarnings, 
	firstTrigger, 
	secondTrigger, 
	firstShot, 
	secondShot, 
	constantWeapon, 
	pausedTicks, 
	freezeWarnings, 

	rapidShotsTimer, 
	connectionTimer
}

static 
	weaponsData[MAX_PLAYERS][E_WEAPONS_DATA][rAgc_MAX_WEAPONIDS], 
	playerData[MAX_PLAYERS][E_PLAYER_DATA];

static bool:IsWeaponAmmoable(Weapon)
	return ( 22 <= Weapon <= 43 || 16 <= Weapon <= 18  );

static bool:IsWeaponValid(Weapon)
	return ( 0 <= Weapon <= 18 || 22 <= Weapon <= 46   );

static bool:IsWeaponSystemized(Weapon)
	return (Weapon == 46 || Weapon == 0 || Weapon == 40);

static bool:IsPlayerPaused(playerid)
	return (gettime() > (playerData[playerid][pausedTicks]+2));

static RAGC_GetTickDiff(newtick, oldtick)
{
	if(oldtick < 0 && newtick >= 0)
	{
		return newtick - oldtick;
	}
	else if(oldtick >= 0 && newtick < 0 || oldtick > newtick)
	{
		return (cellmax - oldtick + 1) - (cellmin - newtick);
	}
	return newtick - oldtick;
}

static bool:IsWeaponRapid(Weapon)
{
	switch(Weapon)
	{
		case 26, 27, 23, 22, 30, 31, 28, 29, 32, 34, 38: return true;
	}
	return false;
}

static GetWeaponSlot(weaponid) 
{ 
	switch(weaponid) 
	{ 
		case 0..1:   return 0;
		case 2..9:   return 1;
		case 22..24: return 2;
		case 25..27: return 3;
		case 28..29: return 4;
		case 32:	 return 4;
		case 30..31: return 5;
		case 33..34: return 6;
		case 35..38: return 7;
		case 16..18: return 8;
		case 39:	 return 8;
		case 41..43: return 9;
		case 10..15: return 10;
		case 44..46: return 11;
		case 40: 	 return 12;
	} 
	return 0; 
}


public OnPlayerConnect(playerid)
{
	rAgc_ResetPlayerWeapons(playerid);
	playerData[playerid][connectionTimer] = SetTimerEx("DetectCheaters", 1000, true, "i", playerid);
	#if defined rAgc_OnPlayerConnect
		return rAgc_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerSpawn(playerid)
{
	for(new s; s<rAgc_MAX_WEAPONIDS; s++)
	{
		weaponsData[playerid][newPlayerAmmo][s] = 0;
		weaponsData[playerid][oldPlayerAmmo][s] = 0;
	}

	rAgc_ResetPlayerWeapons(playerid);
	#if defined rAgc_OnPlayerSpawn
		return rAgc_OnPlayerSpawn(playerid);
	#else
		return 1;
	#endif
}

public OnPlayerDisconnect(playerid, reason)
{
	KillTimer(playerData[playerid][connectionTimer]);

	playerData[playerid][changeAmmoImmunity] = false;
	playerData[playerid][resetWeaponsImmunity]  = false;
	playerData[playerid][freezeWarnings] = 0;
	playerData[playerid][ammoWarnings] = 0;

	#if defined rAgc_OnPlayerDisconnect
		return rAgc_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	new rAgcTempWeap, rAgcTempAmmos;
	rAgcTempWeap = GetPlayerWeapon(playerid);
	rAgcTempAmmos= GetPlayerAmmo(playerid);

	if(playerData[playerid][secondTrigger] == 0 && playerData[playerid][firstTrigger] ==0 )
	{ //First call of the callback
		playerData[playerid][firstTrigger] = GetTickCount(); 
		playerData[playerid][firstShot] = rAgcTempAmmos;
		playerData[playerid][constantWeapon]  = rAgcTempWeap;
	}
	else if(playerData[playerid][firstTrigger] != 0 && playerData[playerid][secondTrigger] == 0)
	{ //Second call of the callback
		playerData[playerid][secondTrigger] = GetTickCount(); 
		playerData[playerid][secondShot] = rAgcTempAmmos;
	} 
	
	else if(playerData[playerid][firstTrigger] != 0 && playerData[playerid][secondTrigger] != 0 ) 
	{ //After the second call of the callback
	       
		if(RAGC_GetTickDiff(playerData[playerid][secondTrigger], playerData[playerid][firstTrigger]) > RAGC_MAX_RAPID_TIME_NORMAL && playerData[playerid][constantWeapon] == rAgcTempWeap && !IsWeaponRapid(rAgcTempWeap) && GetPlayerState(playerid) != 3)
		{//Anti rapid fire for slow weapons
			CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcTempWeap, rAgcTempAmmos, 4);
			playerData[playerid][firstTrigger] = 0; 
			playerData[playerid][secondTrigger]= 0; 
			return 0; 
		}
		else 
		{
			playerData[playerid][firstTrigger] =0; 
			playerData[playerid][secondTrigger]=0; 
		}

		if(playerData[playerid][firstShot] == playerData[playerid][secondShot] && playerData[playerid][constantWeapon] == rAgcTempWeap 
			&& IsWeaponAmmoable(rAgcTempWeap) && !IsWeaponRapid(rAgcTempWeap) && GetPlayerState(playerid) != 3)
		{//Anti ammo freeze for slow ammo weapons
			playerData[playerid][freezeWarnings]++;
			if(playerData[playerid][freezeWarnings] == RAGC_MAX_WARNINGS_FREEZE)
			{
				playerData[playerid][firstShot] =0; 
				playerData[playerid][secondShot]=0; 
				playerData[playerid][freezeWarnings] = 0;
				if(!IsPlayerPaused(playerid))
				{
					CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcTempWeap, rAgcTempAmmos, 3);
				}
				return 0;
			}
		}
		else if(playerData[playerid][constantWeapon] == rAgcTempWeap && IsWeaponAmmoable(rAgcTempWeap) && (IsWeaponRapid(rAgcTempWeap) || GetPlayerState(playerid) == 3))
		{//Anti ammo freeze for fast ammo weapons
			KillTimer(playerData[playerid][rapidShotsTimer]);
			playerData[playerid][rapidShotsTimer] = SetTimerEx("DelayRapidShots", 700, false, "iii", playerid, rAgcTempWeap, rAgcTempAmmos);
		}
		else 
		{
			playerData[playerid][firstShot] =0; 
			playerData[playerid][secondShot]=0; 
		}
	} 
	#if defined rAgc_OnPlayerWeaponShot
		return rAgc_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#else
		return 1;
	#endif
}

public OnPlayerUpdate(playerid)
{
	playerData[playerid][pausedTicks] = gettime();

	#if defined rAgc_OnPlayerUpdate
		return rAgc_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public rAgc_ResetPlayerWeapons(playerid)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!playerData[playerid][resetWeaponsImmunity]) SetTimerEx("HandleVariablesReset", RAGC_IMMU_TIME, false, "d", playerid, 0);
	playerData[playerid][resetWeaponsImmunity] = true;

	for(new s; s<rAgc_MAX_WEAPONIDS; s++)
	{
		if(weaponsData[playerid][basicPlayerWeapons][s] != 0)
		{
			weaponsData[playerid][basicPlayerWeapons][s] = 0;
			weaponsData[playerid][basicPlayerAmmo][s] = 0;
		}
	}

	ResetPlayerWeapons(playerid);
	return 1;
}

public rAgc_SetPlayerAmmo(playerid, weaponid, ammo)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!IsWeaponValid(weaponid) || (IsWeaponAmmoable(weaponid) && ammo ==0)) return 0;
	weaponsData[playerid][newPlayerAmmo][weaponid] = 0;
	weaponsData[playerid][oldPlayerAmmo][weaponid] = 0;

	if(!playerData[playerid][resetWeaponsImmunity]) SetTimerEx("HandleVariablesReset", RAGC_IMMU_TIME, false, "d", playerid, 0);
	playerData[playerid][resetWeaponsImmunity] = true;

	switch(IsWeaponAmmoable(weaponid))
	{
		case 0:
		{
			weaponsData[playerid][basicPlayerAmmo][weaponid] = 1;
			SetPlayerAmmo(playerid, weaponid, 1);
		}
		case 1:
		{
			weaponsData[playerid][basicPlayerAmmo][weaponid] = ammo;
			SetPlayerAmmo(playerid, weaponid, ammo);
		}
	}
	return 1;
}

public rAgc_GivePlayerWeapon(playerid, weaponid, ammo)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!IsWeaponValid(weaponid) || (IsWeaponAmmoable(weaponid) && ammo ==0)) return 0;
	weaponsData[playerid][newPlayerAmmo][weaponid] = 0;
	weaponsData[playerid][oldPlayerAmmo][weaponid] = 0;
	if(!playerData[playerid][resetWeaponsImmunity]) SetTimerEx("HandleVariablesReset", RAGC_IMMU_TIME, false, "d", playerid, 0);
	playerData[playerid][resetWeaponsImmunity] = true;
	switch(IsWeaponAmmoable(weaponid))
	{
		case 0:
		{
			if(weaponsData[playerid][basicPlayerWeapons][weaponid] == weaponid)
			{
				weaponsData[playerid][basicPlayerAmmo][weaponid] = 1;
				GivePlayerWeapon(playerid, weaponid, 1);
			}
			else
			{
				weaponsData[playerid][basicPlayerWeapons][weaponid] = weaponid;
				weaponsData[playerid][basicPlayerAmmo][weaponid] = 1;
				GivePlayerWeapon(playerid, weaponid, 1);
			}
		}
		case 1:
		{
			if(weaponsData[playerid][basicPlayerWeapons][weaponid] == weaponid)
			{
				if(!playerData[playerid][changeAmmoImmunity]) SetTimerEx("HandleVariablesReset", RAGC_IMMU_TIME, false, "d", playerid, 1);
				playerData[playerid][changeAmmoImmunity] = true;
				new rAgcFixWep, rAgcFixAmmo;
				GivePlayerWeapon(playerid, weaponid, ammo);
				GetPlayerWeaponData(playerid, GetWeaponSlot(weaponid), rAgcFixWep, rAgcFixAmmo);
				weaponsData[playerid][basicPlayerAmmo][weaponid] = rAgcFixAmmo + ammo;
			}
			else
			{
				weaponsData[playerid][basicPlayerWeapons][weaponid] = weaponid;
				weaponsData[playerid][basicPlayerAmmo][weaponid] = ammo;
				GivePlayerWeapon(playerid, weaponid, ammo);
			}
		}	
	}
	return 1;
}

public DelayRapidShots(playerid, weapon, ammo)
{
	if(playerData[playerid][firstShot] == playerData[playerid][secondShot])
	{
		playerData[playerid][freezeWarnings]++;
		if(playerData[playerid][freezeWarnings] == RAGC_MAX_WARNINGS_FREEZE)
		{
			playerData[playerid][freezeWarnings] = 0;
			/*if(!IsPlayerPaused(playerid))
			{
				if(GetWeaponSlot(weapon) != 0) CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, weapon, ammo, 3);
			}*/
		}
	}
	playerData[playerid][firstShot] =0; 
	playerData[playerid][secondShot]=0; 
	return 1;
}

public DetectCheaters(playerid)
{
	if(!IsPlayerPaused(playerid))
	{
		//Weapon cheat detector:
		new rAgcAmmo, rAgcWep;
		for(new s; s<rAgc_MAX_WEAPONIDS; s++)
		{
			if(!IsWeaponValid(s)) continue;
			GetPlayerWeaponData(playerid, GetWeaponSlot(s), rAgcWep, rAgcAmmo);
			if(weaponsData[playerid][basicPlayerWeapons][rAgcWep] != rAgcWep && !playerData[playerid][resetWeaponsImmunity] && !IsWeaponSystemized(rAgcWep))
			{
				rAgc_ResetPlayerWeapons(playerid);
				CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcWep, rAgcAmmo, 1);
				return 0;
			}
			//Ammo cheat detector:
			if(!playerData[playerid][changeAmmoImmunity] && weaponsData[playerid][basicPlayerAmmo][rAgcWep] < rAgcAmmo 
				&& IsWeaponAmmoable(rAgcWep))	
			{
				if(playerData[playerid][ammoWarnings] < RAGC_MAX_SPOOF_WARNS)
				{	
					playerData[playerid][ammoWarnings]++;
					SetPlayerAmmo(playerid, rAgcWep, weaponsData[playerid][basicPlayerAmmo][rAgcWep]);
				}
				else 
				{
					playerData[playerid][ammoWarnings] = 0;
			   		rAgc_ResetPlayerWeapons(playerid);
					CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcWep, rAgcAmmo, 2);
				}
				return 0;
			}
		}
	}
	return 1;
}

public HandleVariablesReset(handleid, handletype)
{
	switch(handletype)
	{
		case 0:
		{
			playerData[handleid][resetWeaponsImmunity]  = false;
		}
		case 1:
		{
			playerData[handleid][changeAmmoImmunity] = false;
		}
	}
	return 1;
}

#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect rAgc_OnPlayerDisconnect

#if defined _ALS_OnPlayerSpawn
  #undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif

#define OnPlayerSpawn rAgc_OnPlayerSpawn

#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect rAgc_OnPlayerConnect

#if defined _ALS_OnPlayerWeaponShot
  #undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot rAgc_OnPlayerWeaponShot

#if defined _ALS_OnPlayerUpdate
  #undef OnPlayerUpdate
#else
	#define _ALS_OnPlayerUpdate
#endif

#define OnPlayerUpdate rAgc_OnPlayerUpdate

#if defined _ALS_GivePlayerWeapon
  #undef GivePlayerWeapon
#else
	#define _ALS_GivePlayerWeapon
#endif

#define GivePlayerWeapon rAgc_GivePlayerWeapon

#if defined _ALS_ResetPlayerAmmo
  #undef ResetPlayerAmmo
#else
	#define _ALS_ResetPlayerAmmo
#endif

#define ResetPlayerAmmo rAgc_ResetPlayerAmmo

#if defined _ALS_SetPlayerAmmo
  #undef SetPlayerAmmo
#else
	#define _ALS_SetPlayerAmmo
#endif

#define SetPlayerAmmo rAgc_SetPlayerAmmo

#else

stock rAgc_FGivePlayerWeapon(playerid, weaponid, ammo)
	return CallRemoteFunction("rAgc_GivePlayerWeapon", "iii", playerid, weaponid, ammo);

stock rAgc_FResetPlayerAmmo(playerid)
	return CallRemoteFunction("rAgc_ResetPlayerAmmo", "i", playerid);

stock rAgc_FSetPlayerAmmo(playerid, slot, ammo)
	return CallRemoteFunction("rAgc_SetPlayerAmmo", "iii", playerid, slot, ammo);

#if defined _ALS_GivePlayerWeapon
  #undef GivePlayerWeapon
#else
	#define _ALS_GivePlayerWeapon
#endif

#define GivePlayerWeapon rAgc_FGivePlayerWeapon

#if defined _ALS_ResetPlayerAmmo
  #undef ResetPlayerAmmo
#else
	#define _ALS_ResetPlayerAmmo
#endif

#define ResetPlayerAmmo rAgc_FResetPlayerAmmo

#if defined _ALS_SetPlayerAmmo
  #undef SetPlayerAmmo
#else
	#define _ALS_SetPlayerAmmo
#endif

#define SetPlayerAmmo rAgc_FSetPlayerAmmo

#endif
