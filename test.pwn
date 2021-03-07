#include "a_samp.inc"
#include "Rogue-AC.inc"


//OnPlayerViolate default example:
#define MAX_WARNS_AC 3 //Max warns before kick in severe case 1
#define AC_MESSAGE_COLOR -1

static s_playerWarnings[MAX_PLAYERS];
forward OnPlayerViolate(playerid, severity, violationCode, const violationName[]);
public OnPlayerViolate(playerid, severity, violationCode, const violationName[])
{
	new acString[128], name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof name);
	
	switch(severity)
	{
		case SEVERITY_CASE_ONE:
		{
			if(s_playerWarnings[playerid] < MAX_WARNS_AC) s_playerWarnings[playerid]++;
			else 
			{
				format(acString, sizeof acString, "Player %s was kicked after 3 warnings, reason: Code violation #%d", name, violationCode);
				Kick(playerid);
				SendClientMessageToAll(AC_MESSAGE_COLOR, acString);

			}
		}
		case SEVERITY_CASE_TWO:
		{
			format(acString, sizeof acString, "Player %s was kicked, reason: Code violation #%d", name, violationCode);
			Kick(playerid);
			SendClientMessageToAll(AC_MESSAGE_COLOR, acString);
		}
		case SEVERITY_CASE_THREE:
		{
			format(acString, sizeof acString, "Player %s was banned, reason: Code violation #%d", name, violationCode);
			BanEx(playerid, violationName);
			SendClientMessageToAll(AC_MESSAGE_COLOR, acString);
		}
	}
	return 1;
}
public OnPlayerDisconnect(playerid, reason)
{
	s_playerWarnings[playerid] = 0;
	return 1;
}