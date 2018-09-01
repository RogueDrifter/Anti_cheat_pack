#include <rAgc>//Anti gun cheats -Rogue
#include <FLaggers>//Lag spike\desnyc detector- Rogue
#include <Opba>//Anti airbreak\teleport - Rogue
#include <rAct>//Anti car trolls - Rogue
#include <AntiJC>//Anti jetpack cheats- Rogue
#include <rEac>//Anti carswing\particlespam - Lorenc_
#include <rAmc>//Anti money cheats- Rogue
#include <rAGM>//Anti godmode -Rogue
#include <rAntiBuggers>//Depresses 5 abusing bugs triggered by cheats. -Rogue
#include <AFkill> //Anti fake kill -Rogue
#include <AntiFC>//Anti fake clients - Rogue
#include <rAsc>//Anti speed cheats- Rogue
#include <rVM>//Anti illegal car mods -Emmet_
#include <rAcs>//Anti car spam - Rogue
#include <rED> //Anti dialog spoof/crash- Emmet_ (easyDialog)
#include <AntiSpam>//Anti chat spam- Rogue
#include <AntiSba>//Anti slide bug abuse- Rogue

#if !defined FILTERSCRIPT 

#if defined RogueAC_OnGameModeInit
	forward RogueAC_OnGameModeInit();
#endif

public OnGameModeInit()
{
	print("	 [[--|| Rogue-AC V7.0 ||--]]	 ");
	print("	 [[--|| Rogue Anti cheat version 7.0 ||--]]	  ");
	print("	 [[--|| Successfully Loaded! ||--]]	  ");
	#if defined RogueAC_OnGameModeInit
		return RogueAC_OnGameModeInit();
	#else
		return 1;
	#endif
}

#if defined _ALS_OnGameModeInit
  #undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit RogueAC_OnGameModeInit

#endif
