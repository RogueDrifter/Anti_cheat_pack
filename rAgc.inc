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

forward rAgc_ProtHandler(handleid, handletype);
forward rAgc_Detector(playerid);
forward rAgc_DelayRapid(playerid, weapon, ammo);

#define rAgc_MAX_WEAPONIDS 		   47
#define RAGC_IMMU_TIME	   		   2000
#define RAGC_MAX_RAPID_TIME_NORMAL 300
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

enum RAGC_WEAP_DATA
{
	rAgcPlayerAmmo,
	rAgcPlayerWeapons,

	rAgcNewAmmo,
	rAgcOldAmmo
}

enum RAGC_PLAYER_DATA
{
	bool:rAgcWepImmu,
	bool:rAgcAmmoImmu,

	rAgcASWarns,
	rAgcFirstTick,
	rAgcSecondTick,
	rAgcFirstShot,
	rAgcSecondShot,
	rAgcConstWep,
	rAgcUpdateTicks,
	rAgcAntibugTicks,

	rAgcRepeater,
	rAgcCheckTimer
}

static 
	rAgc_Weapons[MAX_PLAYERS][RAGC_WEAP_DATA][rAgc_MAX_WEAPONIDS],
	rAgc_Player[MAX_PLAYERS][RAGC_PLAYER_DATA];

public OnPlayerConnect(playerid)
{
	rAgc_ResetPlayerWeapons(playerid);
	rAgc_Player[playerid][rAgcCheckTimer] = SetTimerEx("rAgc_Detector", 1000, true, "i", playerid);
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
		rAgc_Weapons[playerid][rAgcNewAmmo][s] = 0;
		rAgc_Weapons[playerid][rAgcOldAmmo][s] = 0;
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
	KillTimer(rAgc_Player[playerid][rAgcCheckTimer]);

	rAgc_Player[playerid][rAgcAmmoImmu] = false;
	rAgc_Player[playerid][rAgcWepImmu]  = false;
	rAgc_Player[playerid][rAgcAntibugTicks] = 0;
	rAgc_Player[playerid][rAgcASWarns] = 0;

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

	if(rAgc_Player[playerid][rAgcSecondTick] == 0 && rAgc_Player[playerid][rAgcFirstTick] ==0 )
	{ //First call of the callback
		rAgc_Player[playerid][rAgcFirstTick] = GetTickCount(); 
		rAgc_Player[playerid][rAgcFirstShot] = rAgcTempAmmos;
		rAgc_Player[playerid][rAgcConstWep]  = rAgcTempWeap;
	}
	else if(rAgc_Player[playerid][rAgcFirstTick] != 0 && rAgc_Player[playerid][rAgcSecondTick] == 0)
	{ //Second call of the callback
		rAgc_Player[playerid][rAgcSecondTick] = GetTickCount(); 
		rAgc_Player[playerid][rAgcSecondShot] = rAgcTempAmmos;
	} 
	
	else if(rAgc_Player[playerid][rAgcFirstTick] != 0 && rAgc_Player[playerid][rAgcSecondTick] != 0 ) 
	{ //After the second call of the callback
		if(rAgc_Player[playerid][rAgcSecondTick] - rAgc_Player[playerid][rAgcFirstTick] < RAGC_MAX_RAPID_TIME_NORMAL && rAgc_Player[playerid][rAgcConstWep] == rAgcTempWeap && !rAgc_IsWepRapid(rAgcTempWeap) && GetPlayerState(playerid) != 3)
		{//Anti rapid fire for slow weapons
			CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcTempWeap, rAgcTempAmmos, 4);
			rAgc_Player[playerid][rAgcFirstTick] = 0; 
			rAgc_Player[playerid][rAgcSecondTick]= 0; 
			return 0; 
		}
		else 
		{
			rAgc_Player[playerid][rAgcFirstTick] =0; 
			rAgc_Player[playerid][rAgcSecondTick]=0; 
		}

		if(rAgc_Player[playerid][rAgcFirstShot] == rAgc_Player[playerid][rAgcSecondShot] && rAgc_Player[playerid][rAgcConstWep] == rAgcTempWeap 
			&& rAgc_IsWeaponAmmoable(rAgcTempWeap) && !rAgc_IsWepRapid(rAgcTempWeap) && GetPlayerState(playerid) != 3)
		{//Anti ammo freeze for slow ammo weapons
			rAgc_Player[playerid][rAgcAntibugTicks]++;
			if(rAgc_Player[playerid][rAgcAntibugTicks] == RAGC_MAX_WARNINGS_FREEZE)
			{
				rAgc_Player[playerid][rAgcFirstShot] =0; 
				rAgc_Player[playerid][rAgcSecondShot]=0; 
				rAgc_Player[playerid][rAgcAntibugTicks] = 0;
				if(!rAgc_IsPlayerPaused(playerid))
				{
					CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcTempWeap, rAgcTempAmmos, 3);
				}
				return 0;
			}
		}
		else if(rAgc_Player[playerid][rAgcConstWep] == rAgcTempWeap && rAgc_IsWeaponAmmoable(rAgcTempWeap) && (rAgc_IsWepRapid(rAgcTempWeap) || GetPlayerState(playerid) == 3))
		{//Anti ammo freeze for fast ammo weapons
			KillTimer(rAgc_Player[playerid][rAgcRepeater]);
			rAgc_Player[playerid][rAgcRepeater] = SetTimerEx("rAgc_DelayRapid", 700, false, "iii", playerid, rAgcTempWeap, rAgcTempAmmos);
		}
		else 
		{
			rAgc_Player[playerid][rAgcFirstShot] =0; 
			rAgc_Player[playerid][rAgcSecondShot]=0; 
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
	rAgc_Player[playerid][rAgcUpdateTicks] = GetTickCount();

	#if defined rAgc_OnPlayerUpdate
		return rAgc_OnPlayerUpdate(playerid);
	#else
		return 1;
	#endif
}

public rAgc_ResetPlayerWeapons(playerid)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!rAgc_Player[playerid][rAgcWepImmu]) SetTimerEx("rAgc_ProtHandler",RAGC_IMMU_TIME,false,"d",playerid, 0);
	rAgc_Player[playerid][rAgcWepImmu] = true;

	for(new s; s<rAgc_MAX_WEAPONIDS; s++)
	{
		if(rAgc_Weapons[playerid][rAgcPlayerWeapons][s] != 0)
		{
			rAgc_Weapons[playerid][rAgcPlayerWeapons][s] = 0;
			rAgc_Weapons[playerid][rAgcPlayerAmmo][s] = 0;
		}
	}

	ResetPlayerWeapons(playerid);
	return 1;
}

public rAgc_SetPlayerAmmo(playerid, weaponid, ammo)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!rAgc_IsWeaponValid(weaponid) || (rAgc_IsWeaponAmmoable(weaponid) && ammo ==0)) return 0;
	rAgc_Weapons[playerid][rAgcNewAmmo][weaponid] = 0;
	rAgc_Weapons[playerid][rAgcOldAmmo][weaponid] = 0;

	if(!rAgc_Player[playerid][rAgcWepImmu]) SetTimerEx("rAgc_ProtHandler", RAGC_IMMU_TIME, false, "d",playerid, 0);
	rAgc_Player[playerid][rAgcWepImmu] = true;

	switch(rAgc_IsWeaponAmmoable(weaponid))
	{
		case 0:
		{
			rAgc_Weapons[playerid][rAgcPlayerAmmo][weaponid] = 1;
			SetPlayerAmmo(playerid, weaponid, 1);
		}
		case 1:
		{
			rAgc_Weapons[playerid][rAgcPlayerAmmo][weaponid] = ammo;
			SetPlayerAmmo(playerid, weaponid, ammo);
		}
	}
	return 1;
}

public rAgc_GivePlayerWeapon(playerid, weaponid, ammo)
{
	if(playerid <0 || playerid > MAX_PLAYERS) return 0;
	if(!rAgc_IsWeaponValid(weaponid) || (rAgc_IsWeaponAmmoable(weaponid) && ammo ==0)) return 0;
	rAgc_Weapons[playerid][rAgcNewAmmo][weaponid] = 0;
	rAgc_Weapons[playerid][rAgcOldAmmo][weaponid] = 0;
	if(!rAgc_Player[playerid][rAgcWepImmu]) SetTimerEx("rAgc_ProtHandler", RAGC_IMMU_TIME, false, "d",playerid, 0);
	rAgc_Player[playerid][rAgcWepImmu] = true;
	switch(rAgc_IsWeaponAmmoable(weaponid))
	{
		case 0:
		{
			if(rAgc_Weapons[playerid][rAgcPlayerWeapons][weaponid] == weaponid)
			{
				rAgc_Weapons[playerid][rAgcPlayerAmmo][weaponid] = 1;
				GivePlayerWeapon(playerid, weaponid, 1);
			}
			else
			{
				rAgc_Weapons[playerid][rAgcPlayerWeapons][weaponid] = weaponid;
				rAgc_Weapons[playerid][rAgcPlayerAmmo][weaponid] = 1;
				GivePlayerWeapon(playerid, weaponid, 1);
			}
		}
		case 1:
		{
			if(rAgc_Weapons[playerid][rAgcPlayerWeapons][weaponid] == weaponid)
			{
				if(!rAgc_Player[playerid][rAgcAmmoImmu]) SetTimerEx("rAgc_ProtHandler",RAGC_IMMU_TIME,false,"d",playerid, 1);
				rAgc_Player[playerid][rAgcAmmoImmu] = true;
				new rAgcFixWep, rAgcFixAmmo;
				GivePlayerWeapon(playerid, weaponid, ammo);
				GetPlayerWeaponData(playerid, rAgc_GetWeaponSlot(weaponid), rAgcFixWep, rAgcFixAmmo);
				rAgc_Weapons[playerid][rAgcPlayerAmmo][weaponid] = rAgcFixAmmo + ammo;
			}
			else
			{
				rAgc_Weapons[playerid][rAgcPlayerWeapons][weaponid] = weaponid;
				rAgc_Weapons[playerid][rAgcPlayerAmmo][weaponid] = ammo;
				GivePlayerWeapon(playerid, weaponid, ammo);
			}
		}	
	}
	return 1;
}

public rAgc_DelayRapid(playerid, weapon, ammo)
{
	if(rAgc_Player[playerid][rAgcFirstShot] == rAgc_Player[playerid][rAgcSecondShot])
	{
		rAgc_Player[playerid][rAgcAntibugTicks]++;
		if(rAgc_Player[playerid][rAgcAntibugTicks] == RAGC_MAX_WARNINGS_FREEZE)
		{
			rAgc_Player[playerid][rAgcAntibugTicks] = 0;
			/*if(!rAgc_IsPlayerPaused(playerid))
			{
				if(rAgc_GetWeaponSlot(weapon) != 0) CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, weapon, ammo, 3);
			}*/
		}
	}
	rAgc_Player[playerid][rAgcFirstShot] =0; 
	rAgc_Player[playerid][rAgcSecondShot]=0; 
	return 1;
}

public rAgc_Detector(playerid)
{
	if(!rAgc_IsPlayerPaused(playerid))
	{
		//Weapon cheat detector:
		new rAgcAmmo,rAgcWep;
		for(new s; s<rAgc_MAX_WEAPONIDS; s++)
		{
			if(!rAgc_IsWeaponValid(s)) continue;
			GetPlayerWeaponData(playerid, rAgc_GetWeaponSlot(s), rAgcWep, rAgcAmmo);
			if(rAgc_Weapons[playerid][rAgcPlayerWeapons][rAgcWep] != rAgcWep && !rAgc_Player[playerid][rAgcWepImmu] && !rAgc_IsWeaponSys(rAgcWep))
			{
				rAgc_ResetPlayerWeapons(playerid);
				CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcWep, rAgcAmmo, 1);
				return 0;
			}
			//Ammo cheat detector:
			if(!rAgc_Player[playerid][rAgcAmmoImmu] && rAgc_Weapons[playerid][rAgcPlayerAmmo][rAgcWep] < rAgcAmmo 
				&& rAgc_IsWeaponAmmoable(rAgcWep))	
			{
				if(rAgc_Player[playerid][rAgcASWarns] < RAGC_MAX_SPOOF_WARNS)
				{	
					rAgc_Player[playerid][rAgcASWarns]++;
					SetPlayerAmmo(playerid, rAgcWep, rAgc_Weapons[playerid][rAgcPlayerAmmo][rAgcWep]);
				}
				else 
				{
					rAgc_Player[playerid][rAgcASWarns] = 0;
			   		rAgc_ResetPlayerWeapons(playerid);
					CallLocalFunction("OnPlayerGunCheat", "iiii", playerid, rAgcWep, rAgcAmmo, 2);
				}
				return 0;
			}
		}
	}
	return 1;
}

public rAgc_ProtHandler(handleid, handletype)
{
	switch(handletype)
	{
		case 0:
		{
			rAgc_Player[handleid][rAgcWepImmu]  = false;
		}
		case 1:
		{
			rAgc_Player[handleid][rAgcAmmoImmu] = false;
		}
	}
	return 1;
}

static rAgc_IsWeaponAmmoable(Weapon)
	return ( 22 <= Weapon <= 43 || 16 <= Weapon <= 18  );

static rAgc_IsWeaponValid(Weapon)
	return ( 0 <= Weapon <= 18 || 22 <= Weapon <= 46   );

static rAgc_IsWeaponSys(Weapon)
	return (Weapon == 46 || Weapon == 0 || Weapon == 40);

static rAgc_IsPlayerPaused(playerid)
	return (GetTickCount() > (rAgc_Player[playerid][rAgcUpdateTicks]+2000));

static rAgc_IsWepRapid(Weapon)
{
	switch(Weapon)
	{
		case 26, 27, 23, 22, 30, 31, 28, 29, 32, 34, 38: return 1;
	}
	return 0;
}

static rAgc_GetWeaponSlot(weaponid) 
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
