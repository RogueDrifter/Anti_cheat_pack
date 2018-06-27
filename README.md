# Anti_cheat_pack

[![sampctl](https://shields.southcla.ws/badge/sampctl-Anti_cheat_pack-2f2f2f.svg?style=for-the-badge)](https://github.com/RogueDrifter/Anti_cheat_pack)

>This is a pack of 13 anticheats and 3 helping systems which you can control through callbacks mentioned
>in the includes and the test.pwn file.
>
> UPDATED: June 27th - 2018
>
> `## Installation`: You simply include the files in your gamemode/fs depending
>on the include's setup (some require include in all scripts and only edit through
> the gamemode, others can be used in one script alone).
>
> `## Testing`: All can be done through the test.pwn folder which already
>contains all of the includes and their callbacks nottify you through `SCM` and `print` messages
>
> `## Version`: `7.0`

## Installation

Simply install to your project:

```bash
sampctl package install RogueDrifter/Anti_cheat_pack
```

Include in your code and begin using the library:

```pawn
#include <Anti_cheat_pack>
```

## Usage

> Use the callbacks provided to you by the includes depending on the type of it.
>
> The includes are 2 types as mentioned before, some edit only through gamemode and include everywhere.
>
> Others done through a single script whether its the GM/FS.
>
> If you're including the pack as a whole you'll need to only use the callbacks through the GM, you'll still need to include the Anti_cheat_pack everywhere.
>
> If you use separate includes you can edit through usage through the files.

## Testing

> Use the test.pwn file and check the server for client messages/log for printed messages.

To test, simply run the package:

```bash
sampctl package run
```

## Contains:

*Anti cheats that don't require include in all scripts
>AFkill  //Anti fake kill -Rogue

>AntiFC //Anti fake clients - Rogue

>rAsc //Anti speed cheats- Rogue

>rVM //Anti illegal car mods -Emmet_

>rAcs //Anti car spam - Rogue

>rED  //Anti dialog spoof/crash- Emmet_ (easyDialog)

*Includes that provide spam/bug protection (not worth a ban)
>AntiSpam //Anti chat spam- Rogue

>AntiSba //Anti slide bug abuse- Rogue

>Flaggers //Lag spike\desnyc detector- Rogue

*Anti cheats that do require include in all scripts and only edit through GM
>AntiJC //Anti jetpack cheats- Rogue

>Opba //Anti airbreak\teleport - Rogue

>rAgc //Anti gun cheats -Rogue

>rAct //Anti car trolls - Rogue

>rEac //Anti carswing\particlespam - Lorenc_

>rAmc //Anti money cheats- Rogue

>rAntiBuggers //Anti abusing sa-mp bugs triggered by cheats- Rogue


## How to update:

>Simply open your project and run the code:

```bash
sampctl package ensure
```

## Non sampctl users:
> Go get sampctl! 
>
> But for now:
>
> 1- Download the repository.
>
> 2- Take all of the .inc files and put them in your pawno/include.
>
> 3- Open all of your filterscripts and use #include <Anti_cheat_pack> right after #include a_samp.
>
> 4- Open your gamemode and type the same line as above^.
>
> 5- Keep your gamemode open and use the callbacks mentioned in the post as examples to your own desire.

> And that's about it! every time i announce a new version release just re-do those steps above!
