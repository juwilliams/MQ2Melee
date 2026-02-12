//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Project: MQ2Melee.cpp    | 2008-04-02: Updated by Wasted
// Author: s0rCieR          | 2008-11-08: Updated by Jobey
//                          | 2008-11-26: Updated by htw
//                          | 2009-02-21: Updated by pms (MoveUtils 9.x support)
//                          | 2010-11-01: Updated by pms (BagWindow support)
//                          | 2010-10-21: Updated by maskoi (House of Thule abilities)
//                          | 2011-11-15: Updated by maskoi (Veil of Alaris abilities)
//                          | 2012-12-02: Updated by Teichou (Rain of Fear abilities), with pet fix by Gomer) (Pet fix)
//                          | 2012-12-31: Updated by Teichou(with pet fix by Gomer) (Pet fix)
//                          | 2014-04-15: Updated by Cr4zyb4rd (-BagWindow +moveitem2)
//                          | 2014-04-19: Updated by Maskoi various fixes
//                          | 2014-05-05: Updated by Cr4zyb4rd rogue fixes
//                          | 2014-05-06: Updated by rswiders ranger and pet fixes
//                          | 2014-05-13: Updated by Cr4zyb4rd -BagWindow +moveitem2, equip/swapping fixes, rogue fixes
//                          | 2014-06-02: Updated by rswiders fix for doMELEE/doPETASSIST,ranger and pet fixes, added cleric yaulp for mana
//                          | 2014-11-17: Updated by Nayenia (The Darkened Sea abilities)
//                          | 2014-12-20: Updated by Maskoi  (The Darkened Sea abilities), Pet fixes, Added Ghold thanks eqmule
//                          | 2014-12-22: Updated by rswiders fix for 64bit getticks
//                          | 2015-02-10: Updated by Ctaylor22 addition of global downshitIf, and 60 slots for down/holy shits.
//                          | 2015-05-23: Updated by winnower berserker TDS fixes and improvements
//                          | 2015-07-14: Updated by Eqmule CombatabilityTimer change
//                          | 2016-07-24: Updated by Eqmule Added string safety
//                          | 2016-08-30: Updated by Eqmule GetAAbyId Level changes
//                          | 2017-02-15: Updated by rswiders XTarget changes
//                          | 2017-08-22: Updated by Eqmule downflag and holyflag now takes 0, 1 or 2, if its set to 2 it tells the plugin to only parse it if a macro IS running.
//                          | 2017-12-12: UPdated by rswiders (Ring of Scale abilities)
//                          | 2018-01-19: Updated by Saar (rest of the RoS abilities)
//                          | 2018-04-24: Updated by Eqmule (Slam Fix)
//                          | 2019-01-28: Updated by s0rcier down/holyflag=3 parse when no macro running. increase down/holy shits to 90. stickmode=2 (disable sticking)
//                          | 2019-02-01: Updated Immobile flags to return false for a litte after being summoned
//                          | 2019-02-07: Fixed Two Handed Bash skills... + saar zerker tbl disc...
//                          | 2019-11-14: Updated by Sic/CWTN Yaulp to default to "off"
//                          | 2019-12-29: Updated by ChatWithThisname-> Added Warrior, Berserker, Rogue discs for ToV. Rearranged information by class instead of alphabetically.
//                          | 2020-01-06: Updated by Sic - Added Paladin, Shadowknight, Ranger, Monk, Necro, and Beastlord ToV discs/spells
//                          | 2021-02-13: Updated by BigDorf - fix knights 2H Bash, Nov 2019 AA name change, "Two-Handed Bash" to "Improved Bash"
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// SHOW_ABILITY:    0=0ff, 1=Display every ability that plugin use.
// SHOW_ATTACKING:  0=0ff, 1=Display Attacking Target
// SHOW_CASTING:    0=0ff, 1=Display MQ2Cast Target
// SHOW_CONTROL:    0=0ff, 1=Display Pet Control
// SHOW_ENRAGING:   0=0ff, 1=Display Enrage/Infuriate
// SHOW_FEIGN:      0=0ff, 1=Display Fallen Detected
// SHOW_OVERRIDE:   0=0ff, 1=Display Override Warning
// SHOW_PROVOKING:  0=0ff, 1=Display Provoke/Aggro AA/Disc/Spell
// SHOW_STICKING:   0=0ff, 1=Display Stick Arguments
// SHOW_STUNNING    0=0ff, 1=Display Stunning AA/Disc/Spell
// SHOW_SWITCHING:  0=0ff, 1=Display Switch Melee/Range
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Distribution of this code in compile form without source code is prohibited.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
#define   PLUGIN_NAME  "MQ2Melee"   // Plugin Name
#define   PLUGIN_DATE   20190128    // Plugin Date

#define   SHOW_ABILITY         0
#define   SHOW_ATTACKING       1
#define   SHOW_CASTING         0
#define   SHOW_CONTROL         0
#define   SHOW_ENRAGING        0
#define   SHOW_FEIGN           1
#define   SHOW_OVERRIDE        0
#define   SHOW_PROVOKING       0
#define   SHOW_STICKING        0
#define   SHOW_STUNNING        0
#define   SHOW_SWITCHING       1

#define   NOID                -1
constexpr int delay = 250;

enum { Tiny, Small, Medium, Large, Giant, Huge }; // container sizes

enum {
    st_x          =0x0000,    // SpawnType: NONE
    st_cn         =0x0020,    // SpawnType: CORPSENPC
    st_cp         =0x0010,    // SpawnType: CORPSEPLAYER
    st_wn         =0x0008,    // SpawnType: PETNPC
    st_wp         =0x0004,    // SpawnType: PETPLAYER
    st_n          =0x0002,    // SpawnType: NPC
    st_p          =0x0001,    // SpawnType: PLAYER
};

#ifndef PLUGIN_API
#include <mq/Plugin.h>
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(8.802);
#include <map>
#include <string>
#include "../../Blech/Blech.h"
#endif PLUGIN_API

// TODO: Remove this include file, move functions in it to main
#include "moveitem.h"

//MoveUtils 11.x

fEQCommand fStickCommand;
bool* pbStickOn;
PLUGIN_API bool bMULoaded = false;
bool bMUPointers = false;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

bool      DebugReady     = false;           // Use for debugging Ability->Ready();
bool      BardClass      = false;           // Bard Class?
bool      BerserkerClass = false;           // Beserker Class?
bool      MonkClass      = false;           // Monk Class?
bool      RogueClass     = false;           // Rogue Class?
bool      Silenced       = false;           // Silenced?
long      BuffMax        = NUM_LONG_BUFFS;  // Maximum Number of Buffs
long      SongMax        = NUM_SHORT_BUFFS; // Maximum Number of Songs
long      GemsMax        = 12;              // Maximum Number of Gems
short     PET_BUTTONS    = 14;              // Number of buttons on Pet UI window

long      InvSlot        = NOID;            // slot # where item is found
CONTENTS* InvCont        = NULL;            // slot content pointer

bool      Sticking       = false;           // Stick Saved State On/Off?
char      StickArg[128]  = { 0 };           // Stick Saved Arguments

char      Reserved[MAX_STRING] = { 0 };     // string buffer
char      Workings[MAX_STRING] = { 0 };     // string buffer

using     Function = void(*)();
struct    infodata { long i, t; } *pinfodata;
//Rogue Strike Fix htw 2/20/2011
bool StrikeFail = false;
ULONGLONG PressDelay = 0;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// These definitions are used for various conditions in MQ2Melee.
#define   d_assassin1       4676      // Duelist Discipline lv 58 ROG
#define   d_assassin2       10898     // Assassin's Discipline lv 75 ROG TSS
#define   d_assassin3       10899     // Assassin Discipline Rk. II
#define   d_assassin4       10900     // Assassin Discipline Rk. III
#define   d_assassin5       29252     // Eradicator's Discipline lv 95 ROG VOA
#define   d_assassin6       29253     // Eradicator's Discipline Rk. II
#define   d_assassin7       29254     // Eradicator's Discipline Rk. III

#define   d_cleaverage1     5037      // Cleaving Rage Discipline 54
#define   d_cleaverage2     5043      // Cleaving Anger Discipline 65
#define   d_cleaverage3     27257     // Cleaving Acrimony Discipline lv 86 BER HoT
#define   d_cleaverage4     27258     // Cleaving Acrimony Discipline rk. II
#define   d_cleaverage5     27259     // Cleaving Acrimony Discipline rk. III

#define   i_disarm            16
#define   i_forage            27
#define   i_intimidation      71
#define   i_kick              30
#define   i_mend              32
#define   i_taunt             73

infodata
//Everyone
tstone =  { 5225   ,3 },        // disc: throw stone
btlleap = { 611    ,4 },        // aa: battle leap
#pragma region Nec/Mag/Bst
mendpet1 = { 58     ,4 },        // aa: mend companion
mendpet2 = { 418    ,4 },        // aa: replenish companion
#pragma endregion

#pragma region Potions
potfast0 = { 77789  ,7 },        // potion: Distillate of Divine Healing I
potfast1 = { 77790  ,7 },        // potion: Distillate of Divine Healing II
potfast2 = { 77791  ,7 },        // potion: Distillate of Divine Healing III
potfast3 = { 77792  ,7 },        // potion: Distillate of Divine Healing IV
potfast4 = { 77793  ,7 },        // potion: Distillate of Divine Healing V
potfast5 = { 77794  ,7 },        // potion: Distillate of Divine Healing VI
potfast6 = { 77795  ,7 },        // potion: Distillate of Divine Healing VII
potfast7 = { 77796  ,7 },        // potion: Distillate of Divine Healing VIII
potfast8 = { 77797  ,7 },        // potion: Distillate of Divine Healing IX
potfast9 = { 77798  ,7 },        // potion: Distillate of Divine Healing X
potfast10 = { 35930  ,7 },        // potion: Distillate of Divine Healing XI
potfast11 = { 35935  ,7 },        // potion: Distillate of Divine Healing XII
potfast12 = { 35940  ,7 },        // potion: Distillate of Divine Healing XIII
potfast13 = { 40554  ,7 },        // potion: Distillate of Divine Healing XIV
potfast14 = { 56941  ,7 },        // potion: Distillate of Divine Healing XV
potfast15 = { 64612  ,7 },        // potion: Distillate of Divine Healing XVI
potfast16 = { 135337 ,7 },        // potion: Distillate of Divine Healing XVII
potfast17 = { 93988  ,7 },        // potion: Distillate of Divine Healing XVIII
potfast18 = { 152388 ,7 },        // potion: Distillate of Divine Healing XIX
potfast19 = { 164279 ,7 },        // potion: Distillate of Divine Healing XX
potfast20 = { 166392 ,7 },        // potion: Distillate of Divine Healing XXI
potfast21 = { 159910 ,7 },        // potion: Distillate of Divine Healing XXII

potover0 = { 77779  ,7 },        // potion: Distillate of Celestial Healing I
potover1 = { 77780  ,7 },        // potion: Distillate of Celestial Healing II
potover2 = { 77781  ,7 },        // potion: Distillate of Celestial Healing III
potover3 = { 77782  ,7 },        // potion: Distillate of Celestial Healing IV
potover4 = { 77783  ,7 },        // potion: Distillate of Celestial Healing V
potover5 = { 77784  ,7 },        // potion: Distillate of Celestial Healing VI
potover6 = { 77785  ,7 },        // potion: Distillate of Celestial Healing VII
potover7 = { 77786  ,7 },        // potion: Distillate of Celestial Healing VIII
potover8 = { 77787  ,7 },        // potion: Distillate of Celestial Healing IX
potover9 = { 77788  ,7 },        // potion: Distillate of Celestial Healing X
potover10 = { 35931  ,7 },        // potion: Distillate of Celestial Healing XI
potover11 = { 35936  ,7 },        // potion: Distillate of Celestial Healing XII
potover12 = { 35941  ,7 },        // potion: Distillate of Celestial Healing XIII
potover13 = { 40555  ,7 },        // potion: Distillate of Celestial Healing XIV
potover14 = { 56942  ,7 },        // potion: Distillate of Celestial Healing XV
potover15 = { 64613  ,7 },        // potion: Distillate of Celestial Healing XVI
potover16 = { 135338 ,7 },        // potion: Distillate of Celestial Healing XVII
potover17 = { 93989  ,7 },        // potion: Distillate of Celestial Healing XVIII
potover18 = { 152389 ,7 },        // potion: Distillate of Celestial Healing XIX
potover19 = { 164280 ,7 },        // potion: Distillate of Celestial Healing XX
potover20 = { 166393 ,7 },        // potion: Distillate of Celestial Healing XXI
potover21 = { 159911 ,7 },        // potion: Distillate of Celestial Healing XXII

#pragma endregion

#pragma region Pal/Shd
steely1 = { 19137  ,5 },        //spell Steely Stance lv 84 pal/shd UF
steely2 = { 19138  ,5 },        //spell Steely Stance Rk. II
steely3 = { 19139  ,5 },        //spell Steely Stance Rk. III
steely4 = { 25270  ,5 },        //spell Stubborn Stance lv 89 pal/shd HoT
steely5 = { 25271  ,5 },        //spell Stubborn Stance Rk. II
steely6 = { 25272  ,5 },        //spell Stubborn Stance Rk. III
steely7 = { 28314  ,5 },        //spell Stoic Stance lv 94 pal/shd VoA
steely8 = { 28315  ,5 },        //spell Stoic Stance Rk. II
steely9 = { 28316  ,5 },        //spell Stoic Stance Rk. III
steely10 = { 34320  ,5 },        //spell Steadfast Stance lv 99 pal/shd RoF
steely11 = { 34321  ,5 },        //spell Steadfast Stance Rk. II
steely12 = { 34322  ,5 },        //spell Steadfast Stance Rk. III
steely13 = { 43289  ,5 },        //spell Staunch Stance lv 104 pal/shd TDS
steely14 = { 43290  ,5 },        //spell Staunch Stance Rk. II
steely15 = { 43291  ,5 },        //spell Staunch Stance Rk. III
steely16 = { 55320  ,5 },        // spell: Defiant Stance Lv 109 Pal/Shd RoS
steely17 = { 55321  ,5 },        // spell: Defiant Stance Rk. II
steely18 = { 55322  ,5 },        // spell: Defiant Stance Rk. III
steely19 = { 58781  ,5 },        // spell: Stormwall Stance
steely20 = { 58782  ,5 },        // spell: Stormwall Stance Rk. II
steely21 = { 58783  ,5 },        // spell: Stormwall Stance Rk. III
steely22 = { 62293  ,5 },        // spell: Adamant Stance Lv 119 Pal/Shd ToL
steely23 = { 62294  ,5 },        // spell: Adamant Stance Rk. II
steely24 = { 62295  ,5 },        // spell: Adamant Stance Rk. III

withstand1 = { 19131  ,3 },        // disc: Withstand Lv 83 pal/sk UF
withstand2 = { 19132  ,3 },        // disc: Withstand Rk. II
withstand3 = { 19133  ,3 },        // disc: Withstand Rk. III
withstand4 = { 25264  ,3 },        // disc: Defy Lv 88 pal/sk HoT
withstand5 = { 25265  ,3 },        // disc: Defy Rk. II
withstand6 = { 25266  ,3 },        // disc: Defy Rk. III
withstand7 = { 28308  ,3 },        // disc: Renounce Lv 93 pal/sk VoA
withstand8 = { 28309  ,3 },        // disc: Renounce Rk. II
withstand9 = { 28310  ,3 },        // disc: Renounce Rk. III
withstand10 = { 34314  ,3 },        // disc: Reprove Lv 98 pal/sk VoA
withstand11 = { 34315  ,3 },        // disc: Reprove Rk. II
withstand12 = { 34316  ,3 },        // disc: Reprove Rk. III
withstand13 = { 43283  ,3 },        // disc: Repel Lv 103 pal/sk TDS
withstand14 = { 43284  ,3 },        // disc: Repel Rk. II
withstand15 = { 43285  ,3 },        // disc: Repel Rk. III
withstand16 = { 55317  ,3 },        // disc: Spurn Lv 108 Pal/Sk RoS
withstand17 = { 55318  ,3 },        // disc: Spurn Rk. II
withstand18 = { 55319  ,3 },        // dics: Spurn Rk. III
withstand19 = { 58778  ,3 },        // disc: Thwart
withstand20 = { 58779  ,3 },        // disc: Thwart Rk. II
withstand21 = { 58780  ,3 },        // dics: Thwart Rk. III
withstand22 = { 62290  ,3 },        // dics: Repudiate Lv 118 Pal/Sk ToL
withstand23 = { 62291  ,3 },        // dics: Repudiate Rk. II
withstand24 = { 62292  ,3 },        // dics: Repudiate Rk. III
#pragma endregion Pal/Shd Updated for ToL

#pragma region Abilities
sbkstab = { 8      ,2 },        // skill: backstab
sbash = { 10     ,2 },        // skill: bash
sbegging = { 67     ,2 },        // skill: begging
sdisarm = { 16     ,2 },        // skill: disarm
sdrpunch = { 21     ,2 },        // skill: dragon punch
sestrike = { 23     ,2 },        // skill: eagle strike
sfeign = { 25     ,2 },        // skill: feign death
sflykick = { 26     ,2 },        // skill: flying kick
sforage = { 27     ,2 },        // skill: forage
sfrenzy = { 74     ,2 },        // skill: frenzy
shide = { 29     ,2 },        // skill: hide
sintim = { 71     ,2 },        // skill: intimidation
skick = { 30     ,2 },        // skill: kick
smend = { 32     ,2 },        // skill: mend
sppocket = { 48     ,2 },        // skill: pick pockets
srndkick = { 38     ,2 },        // skill: round kick
ssensetr = { 62     ,2 },        // skill: sense trap
sslam = { 111    ,2 },        // skill: slam
ssneak = { 42     ,2 },        // skill: sneak
staunt = { 73     ,2 },        // skill: taunt
stigclaw = { 52     ,2 },        // skill: tigerclaw
#pragma endregion Abilities

#pragma region Warrior - Class 1
callchal = { 552    ,4 },        // aa: call of challenge

commanding = { 8000   ,3 },        // disc: commanding voice

defense1 =  { 22556  ,3 },        // disc: Bracing Defense Lv 85 war UF
defense2 =  { 22557  ,3 },        // disc: Bracing Defense Rk. II
defense3 =  { 22558  ,3 },        // disc: Bracing Defense Rk. III
defense4 =  { 25051  ,3 },        // disc: Staunch Defense Lc 90 war HoT
defense5 =  { 25052  ,3 },        // disc: Staunch Defense Rk. II
defense6 =  { 25053  ,3 },        // disc: Staunch Defense Rk. III
defense7 =  { 28066  ,3 },        // disc: Stalwart Defense Lv 95 war VoA
defense8 =  { 28067  ,3 },        // disc: Stalwart Defense Rk. II
defense9 =  { 28068  ,3 },        // disc: Stalwart Defense Rk. III
defense10 = { 34042  ,3 },        // disc: Steadfast Defense Lv 100 war RoF
defense11 = { 34043  ,3 },        // disc: Steadfast Defense Rk. II
defense12 = { 34044  ,3 },        // disc: Steadfast Defense Rk. III
defense13 = { 43060  ,3 },        // disc: Stout Defense Lv 105 war TDS
defense14 = { 43061  ,3 },        // disc: Stout Defense Rk. II
defense15 = { 43062  ,3 },        // disc: Stout Defense Rk. III
defense16 = { 55057  ,3 },        // disc: Resolute Defense Lv 110 War RoS
defense17 = { 55058  ,3 },        // disc: Resolute Defense Rk. II
defense18 = { 55059  ,3 },        // disc: Resolute Defense Rk. III
defense19 = { 58557  ,3 },        // Disc: Courageous Defense - Level 115 - ToV
defense20 = { 58558  ,3 },
defense21 = { 58559  ,3 },
defense22 = { 62060  ,3 },        // Disc: Primal Defense Lv 120 War ToL
defense23 = { 62061  ,3 },        // Disc: Primal Defense Rk. II
defense24 = { 62062  ,3 },        // Disc: Primal Defense Rk. III

fieldarm1 =  { 19917  ,3 },        // disc: Field Armorer Lv 85 war UF
fieldarm2 =  { 19918  ,3 },        // disc: Field Armorer Rk. II
fieldarm3 =  { 19919  ,3 },        // disc: Field Armorer Rk. III
fieldarm4 =  { 25036  ,3 },        // disc: Field Outfitter Lv 90 war HoT
fieldarm5 =  { 25037  ,3 },        // disc: Field Outfitter Rk. II
fieldarm6 =  { 25038  ,3 },        // disc: Field Outfitter Rk. III
fieldarm7 =  { 28051  ,3 },        // disc: Field Defender Lv 95 war VoA
fieldarm8 =  { 28052  ,3 },        // disc: Field Defender Rk. II
fieldarm9 =  { 28053  ,3 },        // disc: Field Defender Rk. III
fieldarm10 = { 34036  ,3 },        // disc: Field Guardian Lv 100 war RoF
fieldarm11 = { 34037  ,3 },        // disc: Field Guardian Rk. II
fieldarm12 = { 34038  ,3 },        // disc: Field Guardian Rk. III
fieldarm13 = { 43057  ,3 },        // disc: Field Protector Lv 105 war TDS
fieldarm14 = { 43058  ,3 },        // disc: Field Protector Rk. II
fieldarm15 = { 43059  ,3 },        // disc: Field Protector Rk. III
fieldarm16 = { 55054  ,3 },        // disc: Field Champion Lv 110 War RoS
fieldarm17 = { 55055  ,3 },        // disc: Field Champion Rk. II
fieldarm18 = { 55056  ,3 },        // disc: Field Champion Rk. III
fieldarm19 = { 58554  ,3 },        // Disc: Paragon Champion - Level 115 - ToV
fieldarm20 = { 58555  ,3 },
fieldarm21 = { 58556  ,3 },
fieldarm22 = { 62057  ,3 },        // disc: Full Moon's Champion Lv 120 War ToL
fieldarm23 = { 62058  ,3 },        // disc: Full Moon's Champion Rk. II
fieldarm24 = { 62059  ,3 },        // disc: Full Moon's Champion Rk. III

gutpunch = { 3732   ,4 },        // aa: gut punch

kneestrike = { 801    ,4 },        // aa: knee strike

prowar1 =  { 4608   ,3 },        // disc: provoke
prowar2 =  { 4681   ,3 },        // disc: bellow
prowar3 =  { 4682   ,3 },        // disc: berate
prowar4 =  { 4697   ,3 },        // disc: incite
prowar5 =  { 5015   ,3 },        // disc: bellow of the mastruq
prowar6 =  { 5016   ,3 },        // disc: ancient: chaos cry
prowar7 =  { 6173   ,3 },        // disc: bazu bellow
prowar8 =  { 10974  ,3 },        // disc: scowl
prowar9 =  { 10975  ,3 },        // disc: scowl rk ii
prowar10 = { 10976  ,3 },        // disc: scowl rk iii
prowar11 = { 15360  ,3 },        // disc: sneer
prowar12 = { 15361  ,3 },        // disc: sneer rk ii
prowar13 = { 15362  ,3 },        // disc: sneer rk iii
prowar14 = { 19537  ,3 },        // disc: bazu bluster Lv 81 war UF
prowar15 = { 19538  ,3 },        // disc: bazu bluster rk. ii
prowar16 = { 19539  ,3 },        // disc: bazu bluster rk. iii
prowar17 = { 19531  ,3 },        // disc: jeer rk i Lv 85 war UF
prowar18 = { 19532  ,3 },        // disc: jeer rk ii
prowar19 = { 19533  ,3 },        // disc: jeer rk iii
prowar20 = { 25018  ,3 },        // disc: bazu roar Lv 86 war HoT
prowar21 = { 25019  ,3 },        // disc: bazu roar rk. ii
prowar22 = { 25020  ,3 },        // disc: bazu roar rk. iii
prowar23 = { 25045  ,3 },        // disc: scoff Lv 90 war HoT
prowar24 = { 25046  ,3 },        // disc: scoff rk. ii
prowar25 = { 25047  ,3 },        // disc: scoff rk. iii
prowar26 = { 28021  ,3 },        // disc: Grendlaen Roar Lv 91 war VoA
prowar27 = { 28022  ,3 },        // disc: Grendlaen Roar rk. ii
prowar28 = { 28023  ,3 },        // disc: Grendlaen Roar rk. iii
prowar29 = { 28060  ,3 },        // disc: Scorn Lv 95 war VoA
prowar30 = { 28061  ,3 },        // disc: Scorn rk. ii
prowar31 = { 28062  ,3 },        // disc: Scorn rk. iii
prowar32 = { 34015  ,3 },        // disc: Krondal's Roar Lv 96 war RoF
prowar33 = { 34016  ,3 },        // disc: Krondal's Roar rk. ii
prowar34 = { 34017  ,3 },        // disc: Krondal's Roar rk. iii
prowar35 = { 34027  ,3 },        // disc: Ridicule Lv 98 war RoF
prowar36 = { 34028  ,3 },        // disc: Ridicule rk. ii
prowar37 = { 34029  ,3 },        // disc: Ridicule rk. iii
prowar38 = { 43021  ,3 },        // disc: Cyclone Roar
prowar39 = { 43022  ,3 },        // disc: Cyclone Roar rk. ii
prowar40 = { 43023  ,3 },        // disc: Cyclone Roar rk. iii
prowar41 = { 43033  ,3 },        // disc: Insult Lv 103 war TDS
prowar42 = { 43034  ,3 },        // disc: Insult rk. ii
prowar43 = { 43035  ,3 },        // disc: Insult rk. iii
prowar44 = { 55027  ,3 },        // disc: Slander Lv 108 War RoS
prowar45 = { 55028  ,3 },        // disc: Slander Rk. II
prowar46 = { 55029  ,3 },        // disc: Slander Rk. III
prowar47 = { 55009  ,3 },        // disc: Kluzen's Roar Lv 106 war RoS
prowar48 = { 55010  ,3 },        // disc: Kluzen's Roar Rk. II
prowar49 = { 55011  ,3 },        // disc: Kluzen's Roar Rk. III
prowar50 = { 58509  ,3 },        // Disc: Kragek's Roar - Level 111 - ToV
prowar51 = { 58510  ,3 },
prowar52 = { 58511  ,3 },
prowar53 = { 62012  ,3 },        // disc: Namdrows' Roar Lv 116 war ToL
prowar54 = { 62013  ,3 },        // disc: Namdrows' Roar Rk II
prowar55 = { 62014  ,3 },        // disc: Namdrows' Roar Rk III

opstrke1 =  { 15375  ,3 },        // disc: Opportunistic Strike Lv 78 war
opstrke2 =  { 15376  ,3 },        // disc: Opportunistic Strike rk ii
opstrke3 =  { 15377  ,3 },        // disc: Opportunistic Strike rk iii
opstrke4 =  { 25027  ,3 },        // disc: Strategic Strike lv 88 war HoT
opstrke5 =  { 25028  ,3 },        // disc: Strategic Strike rk ii
opstrke6 =  { 25029  ,3 },        // disc: Strategic Strike rk iii
opstrke7 =  { 28036  ,3 },        // disc: Vital Strike Lv 93 war VoA
opstrke8 =  { 28037  ,3 },        // disc: Vital Strike rk ii
opstrke9 =  { 28038  ,3 },        // disc: Vital Strike rk iii
opstrke10 = { 43045  ,3 },        // disc: Calculated Strike Lv 104 war TDS
opstrke11 = { 43046  ,3 },        // disc: Calculated Strike rk ii
opstrke12 = { 43046  ,3 },        // disc: Calculated Strike rk iii
opstrke13 = { 55045  ,3 },        // disc: Cunning Strike Lv 109 War RoS
opstrke14 = { 55046  ,3 },        // disc: Cunning Strike Rk. II
opstrke15 = { 55047  ,3 },        // disc: Cunning Strike Rk. III
opstrke16 = { 58545  ,3 },        // Disc: Precision Strike - Level 114 - ToV
opstrke17 = { 58546  ,3 },
opstrke18 = { 58547  ,3 },
opstrke19 = { 62051  ,3 },        // disc: Exploitive Strike Lv 119 War ToL
opstrke20 = { 62052  ,3 },        // disc: Exploitive Strike Rk. II
opstrke21 = { 62053  ,3 },        // disc: Exploitive Strike Rk. III

throat1 = { 10968  ,3 },        // disc: throat jab
throat2 = { 10969  ,3 },        // disc: throat jab rk ii
throat3 = { 10970  ,3 },        // disc: throat jab rk iii
#pragma endregion Updated to ToL

#pragma region Cleric - Class 2
yaulp = { 489    ,4 },        // aa: yaulp
#pragma endregion Updated to ToV

#pragma region Paladin - Class 3
honor1 =  { 10173  ,5 },        // spell: challenge for honor
honor2 =  { 10174  ,5 },        // spell: challenge for honor rk ii
honor3 =  { 10175  ,5 },        // spell: challenge for honor rk iii
honor4 =  { 14954  ,5 },        // spell: trial for honor
honor5 =  { 14955  ,5 },        // spell: trial for honor rk ii
honor6 =  { 14956  ,5 },        // spell: trial for honor rk iii
honor7 =  { 19068  ,5 },        // spell: Charge for Honor Lv 85 pal UF
honor8 =  { 19069  ,5 },        // spell: Charge for Honor rk ii
honor9 =  { 19070  ,5 },        // spell: Charge for Honor rk iii
honor10 = { 25297  ,5 },        // spell: Confrontation for Honor Lv 90 pal HoT
honor11 = { 25298  ,5 },        // spell: Confrontation for Honor rk ii
honor12 = { 25299  ,5 },        // spell: Confrontation for Honor rk iii
honor13 = { 28347  ,5 },        // spell: Provocation for Honor Lv 95 pal VoA
honor14 = { 28348  ,5 },        // spell: Provocation for Honor rk ii
honor15 = { 28349  ,5 },        // spell: Provocation for Honor rk iii
honor16 = { 34350  ,5 },        // spell: Demand for Honor Lv 97 pal RoF
honor17 = { 34351  ,5 },        // spell: Demand for Honor rk. ii
honor18 = { 34352  ,5 },        // spell: Demand for Honor rk. iii
honor19 = { 43322  ,5 },        // spell: Impose for Honor Lv 102 pal TDS
honor20 = { 43323  ,5 },        // spell: Impose for Honor rk. ii
honor21 = { 43324  ,5 },        // spell: Impose for Honor rk. iii
honor22 = { 55353  ,5 },        // spell: Refute for Honor Lv 107 Pal RoS
honor23 = { 55354  ,5 },        // spell: Refute for Honor Rk. II
honor24 = { 55355  ,5 },        // spell: Refute for Honor Rk. III
honor25 = { 58811  ,5 },        // spell: Protest for Honor
honor26 = { 58812  ,5 },        // spell: Protest for Honor Rk. II
honor27 = { 58813  ,5 },        // spell: Protest for Honor Rk. III
honor28 = { 62325  ,5 },        // spell: Parlay for Honor Lv 117 Pal ToL
honor29 = { 62326  ,5 },        // spell: Parlay for Honor Rk. II
honor30 = { 62327  ,5 },        // spell: Parlay for Honor Rk. III

layhand = { 6001   ,4 },        // aa: lay on hands

rightidg1 =  { 25345  ,3 },        // disc: Righteous Indignation Lv 88 pal HoT
rightidg2 =  { 25346  ,3 },        // disc: Righteous Indignation Rk. II
rightidg3 =  { 25347  ,3 },        // disc: Righteous Indignation Rk. III
rightidg4 =  { 28398  ,3 },        // disc: Righteous Vexation Lv 93 pal VoA
rightidg5 =  { 28399  ,3 },        // disc: Righteous Vexation Rk. II
rightidg6 =  { 28400  ,3 },        // disc: Righteous Vexation Rk. III
rightidg7 =  { 34401  ,3 },        // disc: Righteous Umbrage Lv 98 pal RoF
rightidg8 =  { 34402  ,3 },        // disc: Righteous Umbrage Rk. II
rightidg9 =  { 34403  ,3 },        // disc: Righteous Umbrage Rk. III
rightidg10 = { 55389  ,3 },        // disc: Righteous Condemnation Lv 108 pal RoS
rightidg11 = { 55390  ,3 },        // disc: Righteous Condemnation Rk. II
rightidg12 = { 55391  ,3 },        // disc: Righteous Condemnation Rk. III
rightidg13 = { 58838  ,3 },        // disc: Righteous Antipathy
rightidg14 = { 58839  ,3 },        // disc: Righteous Antipathy Rk. II
rightidg15 = { 58840  ,3 },        // disc: Righteous Antipathy Rk. III
rightidg16 = { 62358  ,3 },        // disc: Righteous Censure Lv 118 pal ToL
rightidg17 = { 62359  ,3 },        // disc: Righteous Censure Rk. II
rightidg18 = { 62360  ,3 },        // disc: Righteous Censure Rk. III

stunaas1 = { 73     ,4 },        // aa: divine stun
stunaas2 = { 702    ,4 },        // aa: hand of disruption
stunaas3 = { 3826   ,4 },        // aa: force of disruption

stunpal1 =  { 216    ,5 },        // spell: stun
stunpal2 =  { 123    ,5 },        // spell: holy might
stunpal3 =  { 3975   ,5 },        // spell: force of akera
stunpal4 =  { 3245   ,5 },        // spell: force of akilae
stunpal5 =  { 4977   ,5 },        // spell: ancient force of chaos
stunpal6 =  { 5284   ,5 },        // spell: force of piety
stunpal7 =  { 5299   ,5 },        // spell: ancient force of jeron
stunpal8 =  { 10158  ,5 },        // spell: sacred force
stunpal9 =  { 10159  ,5 },        // spell: sacred force rk. ii
stunpal10 = { 10160  ,5 },        // spell: sacred force rk. iii
stunpal11 = { 11851  ,5 },        // spell: force of prexus
stunpal12 = { 11852  ,5 },        // spell: force of prexus rk. ii
stunpal13 = { 11853  ,5 },        // spell: force of prexus rk. iii
stunpal14 = { 14942  ,5 },        // spell: solemn force
stunpal15 = { 14943  ,5 },        // spell: solemn force rk. ii
stunpal16 = { 14944  ,5 },        // spell: solemn force rk. iii
stunpal17 = { 14984  ,5 },        // spell: Force of Timorous
stunpal18 = { 14985  ,5 },        // spell: Force of Timorous  rk. ii
stunpal19 = { 14986  ,5 },        // spell: Force of Timorous  rk. iii
stunpal20 = { 19056  ,5 },        // spell: Devout Force Lv 81 pal UF
stunpal21 = { 19057  ,5 },        // spell: Devout Force rk. ii
stunpal22 = { 19058  ,5 },        // spell: Devout Force rk. iii
stunpal23 = { 19098  ,5 },        // spell: Force of the Crying Seas Lv 85 pal UF
stunpal24 = { 19099  ,5 },        // spell: Force of the Crying Seas rk. ii
stunpal25 = { 19100  ,5 },        // spell: Force of the Crying Seas rk. iii
stunpal26 = { 25282  ,5 },        // spell: Earnest Force Lv 86 pal HoT
stunpal27 = { 25283  ,5 },        // spell: Earnest Force rk. ii
stunpal28 = { 25284  ,5 },        // spell: Earnest Force rk. iii
stunpal29 = { 25375  ,5 },        // spell: Force of Marr Lv 90 pal Hot
stunpal30 = { 25376  ,5 },        // spell: Force of Marr rk. ii
stunpal31 = { 25377  ,5 },        // spell: Force of Marr rk. iii
stunpal32 = { 28326  ,5 },        // spell: Zealous Force Lv 91 pal VoA
stunpal33 = { 28327  ,5 },        // spell: Zealous Force rk. ii
stunpal34 = { 28328  ,5 },        // spell: Zealous Force rk. iii
stunpal35 = { 28446  ,5 },        // spell: Force of Oseka Lv 95 pal VoA
stunpal36 = { 28447  ,5 },        // spell: Force of Oseka rk. ii
stunpal37 = { 28448  ,5 },        // spell: Force of Oseka rk. iii
stunpal38 = { 34332  ,5 },        // spell: Reverent Force Lv 96 pal VoA
stunpal39 = { 34333  ,5 },        // spell: Reverent Force rk. ii
stunpal40 = { 34334  ,5 },        // spell: Reverent Force rk. iii
stunpal41 = { 34452  ,5 },        // spell: Force of the Iceclad Lv 100 pal RoF
stunpal42 = { 34453  ,5 },        // spell: Force of the Iceclad rk. ii
stunpal43 = { 34454  ,5 },        // spell: Force of the Iceclad rk .iii
stunpal44 = { 43412  ,5 },        // spell: Force of the Darkened Sea Lv 105 pal TDS
stunpal45 = { 43413  ,5 },        // spell: Force of the Darkened Sea rk. ii
stunpal46 = { 43414  ,5 },        // spell: Force of the Darkened Sea rk .iii
stunpal47 = { 55479  ,5 },        // spell: Force of the Timorous Deep Lv 110 Pal RoS
stunpal48 = { 55480  ,5 },        // spell: Force of the Timorous Deep Rk. II
stunpal49 = { 55481  ,5 },        // spell: Force of the Timorous Deep Rk. III
stunpal50 = { 58919  ,5 },        // spell: Force of the Grotto
stunpal51 = { 55420  ,5 },        // spell: Force of the Grotto Rk. II
stunpal52 = { 55421  ,5 },        // spell: Force of the Grotto Rk. III
stunpal53 = { 62450  ,5 },        // spell: Force of the Umbra Lv 120 Pal ToL
stunpal54 = { 62451  ,5 },        // spell: Force of the Umbra Rk. II
stunpal55 = { 62452  ,5 },        // spell: Force of the Umbra Rk. III
#pragma endregion Paladin - Class 3 Updated for ToL

#pragma region Ranger - Class 4
bladesrng1 = { 40105  ,3 },        // disc: storm of blades rk i
bladesrng2 = { 40106  ,3 },        // disc: storm of blades rk ii
bladesrng3 = { 40107  ,3 },        // disc: storm of blades rk iii
bladesrng4 = { 40108  ,3 },        // disc: focused storm of blades rk i
bladesrng5 = { 40109  ,3 },        // disc: focused storm of blades rk ii
bladesrng6 = { 40110  ,3 },        // disc: focused storm of blades rk iii
bladesrng7 = { 43457  ,3 },        // disc: Squall of Blades
bladesrng8 = { 43458  ,3 },        // disc: Squall of Blades Rk. II
bladesrng9 = { 43459  ,3 },        // disc: Squall of Blades Rk. III
bladesrng10 = { 50083  ,3 },        // disc: Focused Squall of Blades
bladesrng11 = { 50084  ,3 },        // disc: Focused Squall of Blades Rk. II
bladesrng12 = { 50085  ,3 },        // disc: Focused Squall of Blades Rk. III
bladesrng13 = { 55527  ,3 },        // disc: Gale of Blades
bladesrng14 = { 55528  ,3 },        // disc: Gale of Blades Rk. II
bladesrng15 = { 55529  ,3 },        // disc: Gale of Blades Rk. III
bladesrng16 = { 57918  ,3 },        // disc: Focused Gale of Blades
bladesrng17 = { 57919  ,3 },        // disc: Focused Gale of Blades Rk. II
bladesrng18 = { 57920  ,3 },        // disc: Focused Gale of Blades Rk. III
bladesrng19 = { 58958  ,3 },        // disc: Blizzard of Blades
bladesrng20 = { 58959  ,3 },        // disc: Blizzard of Blades Rk. II
bladesrng21 = { 58960  ,3 },        // disc: Blizzard of Blades Rk. III
// bladesrng22 = { 61155  ,3 },        // disc: Focused Blizzard of Blades
// bladesrng23 = { 61156  ,3 },        // disc: Focused Blizzard of Blades Rk. II
// bladesrng24 = { 61157  ,3 },        // disc: Focused Blizzard of Blades Rk. III
bladesrng25 = { 62475  ,3 },        // disc: Tempest of Blades Lv 116 Rng ToL
bladesrng26 = { 62476  ,3 },        // disc: Tempest of Blades Rk. II
bladesrng27 = { 62477  ,3 },        // disc: Tempest of Blades Rk. III

enragingkick1 =  { 28506  ,3 },        // disc: Enraging Crescent lv 92 Voa rng
enragingkick2 =  { 28507  ,3 },        // disc: Enraging Crescent Kicks Rk. III
enragingkick3 =  { 28508  ,3 },        // disc: Enraging Crescent Kicks Rk. III
enragingkick4 =  { 34527  ,3 },        // disc: Enraging Heel Kicks lv 97 Rof rng
enragingkick5 =  { 34528  ,3 },        // disc: Enraging Heel Kicks Rk. III
enragingkick6 =  { 34529  ,3 },        // disc: Enraging Heel Kicks Rk. III
enragingkick7 =  { 43463  ,3 },        // disc: Enraging Cut Kicks lv 102 TDS rng
enragingkick8 =  { 43464  ,3 },        // disc: Enraging Cut Kicks Rk. III
enragingkick9 =  { 43465  ,3 },        // disc: Enraging Cut Kicks Rk. III
enragingkick10 = { 55539  ,3 },        // disc: Enraging Wheel Kicks Lv 107 Rng RoS
enragingkick11 = { 55540  ,3 },        // disc: Enraging Wheel Kicks Rk. II
enragingkick12 = { 55541  ,3 },        // disc: Enraging Wheel Kicks Rk. III
enragingkick13 = { 58970  ,3 },        // disc: Enraging Axe Kicks
enragingkick14 = { 58971  ,3 },        // disc: Enraging Axe Kicks Rk. II
enragingkick15 = { 58972  ,3 },        // disc: Enraging Axe Kicks Rk. III
enragingkick16 = { 62485  ,3 },        // disc: Enraging Roundhouse Kicks Lv 117 Rng ToL
enragingkick17 = { 62486  ,3 },        // disc: Enraging Roundhouse Rk. II
enragingkick18 = { 62487  ,3 },        // disc: Enraging Roundhouse Rk. III

jltkicks1 = { 10086  ,3 },        // disc: jolting kicks
jltkicks2 = { 10087  ,3 },        // disc: jolting kicks rk ii
jltkicks3 = { 10088  ,3 },        // disc: jolting kicks rk iii
jltkicks4 = { 15020  ,3 },        // disc: Jolting Snapkicks
jltkicks5 = { 15021  ,3 },        // disc: Jolting Snapkicks rk ii
jltkicks6 = { 15022  ,3 },        // disc: Jolting Snapkicks rk iii
jltkicks7 = { 19152  ,3 },        // disc: Jolting Frontkicks Lv 82 rng UF
jltkicks8 = { 19153  ,3 },        // disc: Jolting Frontkicks rk ii
jltkicks9 = { 19154  ,3 },        // disc: Jolting Frontkicks rk iii
jltkicks10 = { 25432  ,3 },        // disc: Jolting Hook kicks Lv Lv 87 rng HoT
jltkicks11 = { 25433  ,3 },        // disc: Jolting Hook kicks rk ii
jltkicks12 = { 25434  ,3 },        // disc: Jolting Hook kicks rk iii
jltkicks13 = { 28509  ,3 },        // disc: Jolting Crescent kicks lv 92 rng VoA
jltkicks14 = { 28510  ,3 },        // disc: Jolting Crescent kicks rk ii
jltkicks15 = { 28511  ,3 },        // disc: Jolting Crescent kicks rk iii
jltkicks16 = { 34530  ,3 },        // disc: Jolting Heel Kicks lv 97 rng RoF
jltkicks17 = { 34531  ,3 },        // disc: Jolting Heel Kicks rk ii
jltkicks18 = { 34532  ,3 },        // disc: Jolting Heel Kicks rk iii
jltkicks19 = { 43466  ,3 },        // disc: Jolting Cut Kicks lv 102 rng TDS
jltkicks20 = { 43467  ,3 },        // disc: Jolting Cut Kicks rk ii
jltkicks21 = { 43468  ,3 },        // disc: Jolting Cut Kicks rk iii
jltkicks22 = { 55542  ,3 },        // disc: Jolting Wheel Kicks Lv 107 Rng RoS
jltkicks23 = { 55543  ,3 },        // disc: Jolting Wheel Kicks Rk. II
jltkicks24 = { 55544  ,3 },        // disc: Jolting Wheel Kicks Rk. III
jltkicks25 = { 55542  ,3 },        // disc: Jolting Axe Kicks
jltkicks26 = { 55543  ,3 },        // disc: Jolting Axe Kicks Rk. II
jltkicks27 = { 55544  ,3 },        // disc: Jolting Axe Kicks Rk. III
jltkicks28 = { 62482  ,3 },        // disc: Jolting Roundhouse Kicks Lv 117 Rng ToL
jltkicks29 = { 62483  ,3 },        // disc: Jolting Roundhouse Kicks Rk. II
jltkicks30 = { 62484  ,3 },        // disc: Jolting Roundhouse Kicks Rk. III

joltrng1 = { 1741   ,5 },        // spell: jolt
joltrng2 = { 1296   ,5 },        // spell: cinder jolt
#pragma endregion Ranger - Class 4 Updated for ToL

#pragma region ShadowKnight - Class 5
feignid =  { 420    ,4 },        // aa: imitate death
feigndp =  { 428    ,4 },        // aa: death peace
feigns1 =  { 366    ,5 },        // spell: feign death
feigns2 =  { 3685   ,5 },        // spell: comatose
feigns3 =  { 1460   ,5 },        // spell: death peace
feigns4 =  { 10306  ,5 },        // spell: last breath
feigns5 =  { 10307  ,5 },        // spell: last breath rk ii
feigns6 =  { 10308  ,5 },        // spell: last breath rk iii
feigns7 =  { 15223  ,5 },        // spell: rigor mortis
feigns8 =  { 15224  ,5 },        // spell: rigor mortis rk ii
feigns9 =  { 15225  ,5 },        // spell: rigor mortis rk iii
feigns10 = { 15190  ,5 },        // spell: last gasp
feigns11 = { 15191  ,5 },        // spell: last gasp rk ii
feigns12 = { 15192  ,5 },        // spell: last gasp rk iii
feigns13 = { 19343  ,5 },        // spell: Final Breath Lv 85 sk UF
feigns14 = { 19344  ,5 },        // spell: Final Breath rk ii
feigns15 = { 19345  ,5 },        // spell: Final Breath rk iii
feigns16 = { 25662  ,5 },        // spell: Last Breath Lv 90 sk Hot
feigns17 = { 25663  ,5 },        // spell: Last Breath rk ii
feigns18 = { 25664  ,5 },        // spell: Last Breath rk iii
feigns19 = { 28760  ,5 },        // spell: Final Gasp Lv 95 sk Voa
feigns20 = { 28761  ,5 },        // spell: Final Gasp rk ii
feigns21 = { 28762  ,5 },        // spell: Final Gasp rk iii
feigns22 = { 34775  ,5 },        // spell: Terminal Breath Lv 100 sk RoF
feigns23 = { 34776  ,5 },        // spell: Terminal Breath Rk. II
feigns24 = { 34777  ,5 },        // spell: Terminal Breath Rk. III

gblade1 = { 28687  ,3 },        // disc: Gouging Blade - SK Lv 92 VoA
gblade2 = { 28688  ,3 },        // disc: Gouging Blade Rk. II
gblade3 = { 28689  ,3 },        // disc: Gouging Blade Rk. III
gblade4 = { 34714  ,3 },        // disc: Gashing Blade - SK Lv 97 RoF
gblade5 = { 34715  ,3 },        // disc: Gashing Blade Rk. II
gblade6 = { 34716  ,3 },        // disc: Gashing Blade Rk. III
gblade7 = { 43637  ,3 },        // disc: Lacerating Blade - SK Lv 102 RoF
gblade8 = { 43638  ,3 },        // disc: Lacerating Blade Rk. II
gblade9 = { 43639  ,3 },        // disc: Lacerating Blade Rk. III
gblade10 = { 55731  ,3 },        // disc: Wounding Blade Lv 107 Shd RoS
gblade11 = { 55732  ,3 },        // disc: Wounding Blade Rk. II
gblade12 = { 55733  ,3 },        // disc: Wounding Blade Rk. III
gblade13 = { 59147  ,3 },        // disc: Rending Blade
gblade14 = { 59148  ,3 },        // disc: Rending Blade Rk. II
gblade15 = { 59149  ,3 },        // disc: Rending Blade Rk. III
gblade16 = { 62625  ,3 },        // disc: Grisly Blade Lv 117 Shd ToL
gblade17 = { 62626  ,3 },        // disc: Grisly Blade Rk. II
gblade18 = { 62627  ,3 },        // disc: Grisly Blade Rk. III

harmtouch = { 6000   ,4 },        // aa: harmtouch

power1 = { 10260  ,5 },        // spell: challenge for power
power2 = { 10261  ,5 },        // spell: challenge for power rk. ii
power3 = { 10262  ,5 },        // spell: challenge for power rk. iii
power4 = { 15163  ,5 },        // spell: trial for power
power5 = { 15164  ,5 },        // spell: trial for power rk. ii
power6 = { 15165  ,5 },        // spell: trial for power rk. iii
power7 = { 19316  ,5 },        // spell: Charge for Power Lv 82 shd UF
power8 = { 19317  ,5 },        // spell: Charge for Power rk. ii
power9 = { 19318  ,5 },        // spell: Charge for Power rk. iii
power10 = { 25586  ,5 },        // spell: Confrontation for Power Lv 87 shd HoT
power11 = { 25587  ,5 },        // spell: Confrontation for Power rk. ii
power12 = { 25588  ,5 },        // spell: Confrontation for Powerr rk. iii
power13 = { 28663  ,5 },        // spell: Provocation for Power Lv 92 shd VoA
power14 = { 28664  ,5 },        // spell: Provocation for Power rk. ii
power15 = { 28665  ,5 },        // spell: Provocation for Power rk .iii
power16 = { 34693  ,5 },        // spell: Demand for Power  Lv 97 shd RoF
power17 = { 34694  ,5 },        // spell: Demand for Power rk. ii
power18 = { 34695  ,5 },        // spell: Demand for Power rk. iii
power19 = { 43619  ,5 },        // spell: Impose for Power  Lv 102 shd TDS
power20 = { 43620  ,5 },        // spell: Impose for Power rk. ii
power21 = { 43621  ,5 },        // spell: Impose for Power rk. iii
power22 = { 55713  ,5 },        // spell: Refute for Power Lv 107 Shd RoS
power23 = { 55714  ,5 },        // spell: Refute for Power Rk. II
power24 = { 55715  ,5 },        // spell: Refute for Power Rk. III
power25 = { 59129  ,5 },        // spell: Protest for Power
power26 = { 59130  ,5 },        // spell: Protest for Power Rk. II
power27 = { 59131  ,5 },        // spell: Protest for Power Rk. III
power28 = { 62634  ,5 },        // spell: Parlay for Power Lv 117 Shd ToL
power29 = { 62635  ,5 },        // spell: Parlay for Power Rk. II
power30 = { 62636  ,5 },        // spell: Parlay for Power Rk. III

terror1 = { 1221   ,5 },        // spell: terror of darkness
terror2 = { 1222   ,5 },        // spell: terror of shadows
terror3 = { 1223   ,5 },        // spell: terror of death
terror4 = { 1224   ,5 },        // spell: terror of terris
terror5 = { 3405   ,5 },        // spell: terror of thule
terror6 = { 5329   ,5 },        // spell: terror of discord
terror7 = { 10257  ,5 },        // spell: terror of vergalid
terror8 = { 10258  ,5 },        // spell: terror of vergalid rk. ii
terror9 = { 10259  ,5 },        // spell: terror of vergalid rk. iii
terror10 = { 15160  ,5 },        // spell: terror of the Soulbleeder
terror11 = { 15161  ,5 },        // spell: terror of the Soulbleeder rk. ii
terror12 = { 15162  ,5 },        // spell: terror of the Soulbleeder rk. iii
terror13 = { 19313  ,5 },        // spell: Terror of Jelvalak Lv 81 sk UF
terror14 = { 19314  ,5 },        // spell: Terror of Jelvalak rk. ii
terror15 = { 19315  ,5 },        // spell: Terror of Jelvalak rk. iii
terror16 = { 25580  ,5 },        // spell: Terror of Rerekalen lv 86 sk HoT
terror17 = { 25581  ,5 },        // spell: Terror of Rerekalen rk. ii
terror18 = { 25582  ,5 },        // spell: Terror of Rerekalen rk. iii
terror19 = { 28657  ,5 },        // spell: Terror of Desalin Lv 91 sk VoA
terror20 = { 28658  ,5 },        // spell: Terror of Desalin rk. ii
terror21 = { 28659  ,5 },        // spell: Terror of Desalin rk. iii
terror22 = { 34687  ,5 },        // spell: Terror of Poira Lv 96 sk RoF
terror23 = { 34688  ,5 },        // spell: Terror of Poira rk. ii
terror24 = { 34689  ,5 },        // spell: Terror of Poira rk. iii
terror25 = { 43607  ,5 },        // spell: Terror of Narus Lv 101 sk TDS
terror26 = { 43608  ,5 },        // spell: Terror of Narus rk. ii
terror27 = { 43609  ,5 },        // spell: Terror of Narus rk. iii
terror28 = { 55689  ,5 },        // spell: Terror of Kra'Du Lv 106 Sk RoS
terror29 = { 55690  ,5 },        // spell: Terror of Kra'Du Rk. II
terror30 = { 55691  ,5 },        // spell: Terror of Kra'Du Rk. III
terror31 = { 59105  ,5 },        // spell: Terror of Mirenilla
terror32 = { 59106  ,5 },        // spell: Terror of Mirenilla Rk. II
terror33 = { 59107  ,5 },        // spell: Terror of Mirenilla Rk. III
terror34 = { 62592  ,5 },        // spell: Terror of Ander Lv 116 Sk ToL
terror35 = { 62593  ,5 },        // spell: Terror of Ander Rk. II
terror36 = { 62594  ,5 },        // spell: Terror of Ander Rk. III

#pragma endregion ShadowKnight - Class 5 Updated for ToL

#pragma region Druid - Class 6
#pragma endregion - Empty

#pragma region Monk - Class 7
cloud1 = { 25914  ,3 },        // disc: Cloud of Fists Lv 87 mnk HoT
cloud2 = { 25915  ,3 },        // disc: Cloud of Fists rk. ii
cloud3 = { 25916  ,3 },        // disc: Cloud of Fists rk. iii
cloud4 = { 40229  ,3 },        // disc: Phantom Partisan Lv 100 mnk HoT
cloud5 = { 40230  ,3 },        // disc: Phantom Partisan rk. ii
cloud6 = { 40231  ,3 },        // disc: Phantom Partisan rk. iii
cloud7 = { 50183  ,3 },        // disc: Phantom Pummeling
cloud8 = { 50184  ,3 },        // disc: Phantom Pummeling Rk. II
cloud9 = { 50185  ,3 },        // disc: Phantom Pummeling Rk. III
cloud10 = { 61271  ,3 },        // disc: Phantom Fisticuffs Lv 115 mnk CoV
cloud11 = { 61272  ,3 },        // disc: Phantom Fisticuffs Rk. II
cloud12 = { 61273  ,3 },        // disc: Phantom Fisticuffs Rk. III

fistswu = { 8002   ,3 },        // disc: fists of wu

leop1 =  { 6752   ,3 },        // disc: leopard claw
leop2 =  { 6727   ,3 },        // disc: dragon fang
leop3 =  { 10944  ,3 },        // disc: clawstriker flurry
leop4 =  { 10945  ,3 },        // disc: clawstriker flurry rk ii
leop5 =  { 10946  ,3 },        // disc: clawstriker flurry rk iii
leop6 =  { 14796  ,3 },        // disc: wheel of fists
leop7 =  { 14797  ,3 },        // disc: wheel of fists rk ii
leop8 =  { 14798  ,3 },        // disc: wheel of fists rk iii
leop9 =  { 18901  ,3 },        // disc: whorl of fists Lv 84 mnk UF
leop10 = { 18902  ,3 },        // disc: whorl of fists rk ii
leop11 = { 18903  ,3 },        // disc: whorl of fists rk iii
leop12 = { 25926  ,3 },        // disc: Six-Step Pattern Lv 89 mnk Hot
leop13 = { 25927  ,3 },        // disc: Six-Step Pattern rk. ii
leop14 = { 25928  ,3 },        // disc: Six-Step Pattern rk iii
leop15 = { 29033  ,3 },        // disc: Seven-Step Pattern Lv 94 mnk VoA
leop16 = { 29034  ,3 },        // disc: Seven-Step Pattern rk .ii
leop17 = { 29035  ,3 },        // disc: Seven-Step Pattern rk iii
leop18 = { 35074  ,3 },        // disc: Eight-Step Pattern Lv 96 mnk RoF
leop19 = { 35075  ,3 },        // disc: Eight-Step Pattern Rk. II
leop20 = { 35076  ,3 },        // disc: Eight-Step Pattern Rk. III
leop21 = { 43974  ,3 },        // disc: Torrent of Fists Lv 104 mnk TDS
leop22 = { 43975  ,3 },        // disc: Torrent of Fists Rk. II
leop23 = { 43976  ,3 },        // disc: Torrent of Fists Rk. III
leop24 = { 56099  ,3 },        // disc: Firestorm of Fists Lv 110 Mnk RoS
leop25 = { 56100  ,3 },        // disc: Firestorm of Fists Rk. II
leop26 = { 56101  ,3 },        // disc: Firestorm of Fists Rk. III
leop27 = { 59454  ,3 },        // disc: Barrage of Fists
leop28 = { 59455  ,3 },        // disc: Barrage of Fists Rk. II
leop29 = { 59456  ,3 },        // disc: Barrage of Fists Rk. III
leop30 = { 62964  ,3 },        // disc: Buffeting of Fists Lv 120 Mnk ToL
leop31 = { 62965  ,3 },        // disc: Buffeting of Fists Rk. II
leop32 = { 62966  ,3 },        // disc: Buffeting of Fists Rk. III

monkey1 = { 22525  ,3 },        // disc: Drunken Monkey Style Lv 85 mnk UF
monkey2 = { 22526  ,3 },        // disc: Drunken Monkey Style rk .ii
monkey3 = { 22527  ,3 },        // disc: Drunken Monkey Style rk iii

stunmnk1 = { 469    ,4 },        // aa: stunning kick
stunmnk2 = { 600    ,4 },        // aa: resounding kick

synergy1 =  { 18895  ,3 },        // disc: Calanin's Synergy Lv 81 mnk UF
synergy2 =  { 18896  ,3 },        // disc: Calanin's Synergy Rk. II
synergy3 =  { 18897  ,3 },        // disc: Calanin's Synergy Rk. III
synergy4 =  { 25907  ,3 },        // disc: Dreamwalker's Synergy Lv 86 mnk HoT
synergy5 =  { 25908  ,3 },        // disc: Dreamwalker's Synergy Rk. II
synergy6 =  { 25909  ,3 },        // disc: Dreamwalker's Synergy Rk. III
synergy7 =  { 29002  ,3 },        // disc: Veilwalker's Synergy Lv 91 mnk VoA
synergy8 =  { 29003  ,3 },        // disc: Veilwalker's Synergy Rk. II
synergy9 =  { 29004  ,3 },        // disc: Veilwalker's Synergy Rk. III
synergy10 = { 35043  ,3 },        // disc: Shadewalker's Synergy mnk RoF
synergy11 = { 35044  ,3 },        // disc: Shadewalker's Synergy Rk. II
synergy12 = { 35045  ,3 },        // disc: Shadewalker's Synergy Rk. III
synergy13 = { 43943  ,3 },        // disc: Doomwalker's Synergy mnk Lv 101 TDS
synergy14 = { 43944  ,3 },        // disc: Doomwalker's Synergy Rk. II
synergy15 = { 43945  ,3 },        // disc: Doomwalker's Synergy Rk. III
synergy16 = { 56058  ,3 },        // disc: Firewalker's Synergy Lv 106 Mnk RoS
synergy17 = { 56059  ,3 },        // disc: Firewalker's Synergy Rk. II
synergy18 = { 56060  ,3 },        // disc: Firewalker's Synergy Rk. III
synergy19 = { 59428  ,3 },        // disc: Icewalker's Synergy
synergy20 = { 59429  ,3 },        // disc: Icewalker's Synergy Rk. II
synergy21 = { 59430  ,3 },        // disc: Icewalker's Synergy Rk. III
synergy22 = { 62920  ,3 },        // disc: Bloodwalker's Synergy Lv 116 Mnk ToL
synergy23 = { 62921  ,3 },        // disc: Bloodwalker's Synergy Rk. II
synergy24 = { 62922  ,3 },        // disc: Bloodwalker's Synergy Rk. III

vigmnk1 = { 19826  ,3 },        // disc: Vigorous Shuriken
vigmnk2 = { 19827  ,3 },        // disc: Vigorous Shuriken Rk. II
vigmnk3 = { 19828  ,3 },        // disc: Vigorous Shuriken Rk. III

#pragma endregion Monk - Class 7 Updated for ToL

#pragma region Bard - Class 8
boastful = { 199    ,4 },        // aa: boastful bellow

selos = { 8205   ,4 },        // aa: selos

#pragma endregion Updated to ToV

#pragma region Rogue - Class 9
assault1 =  { 22540  ,3 },        // disc: Assault Lv 85 rog UF
assault2 =  { 22541  ,3 },        // disc: Assault Rk. II
assault3 =  { 22542  ,3 },        // disc: Assault Rk. III
assault4 =  { 26142  ,3 },        // disc: Battery Lv 90 rog HoT
assault5 =  { 26143  ,3 },        // disc: Battery Rk. II
assault6 =  { 26144  ,3 },        // disc: Battery Rk. III
assault7 =  { 29243  ,3 },        // disc: Onslaught Lv 95 rog VoA
assault8 =  { 29244  ,3 },        // disc: Onslaught Rk. II
assault9 =  { 29245  ,3 },        // disc: Onslaught Rk. III
assault10 = { 35299  ,3 },        // disc: Incursion Lv 100 rog RoF
assault11 = { 35300  ,3 },        // disc: Incursion Rk. II
assault12 = { 35301  ,3 },        // disc: Incursion Rk. III
assault13 = { 44172  ,3 },        // disc: Barrage Lv 105 rog TDS
assault14 = { 44173  ,3 },        // disc: Barrage Rk. II
assault15 = { 44174  ,3 },        // disc: Barrage Rk. III
assault16 = { 56324  ,3 },        // disc: Fellstrike Lv 110 rog RoS
assault17 = { 56325  ,3 },        // disc: Fellstrike Rk. II
assault18 = { 56326  ,3 },        // disc: Fellstrike Rk. III
assault19 = { 59646  ,3 },        // Disc: Blitzstrike - Level 115 - TOV
assault20 = { 59647  ,3 },
assault21 = { 59648  ,3 },
assault22 = { 63170  ,3 },        // Disc: Shadowstrike Lv 120 rog ToL
assault23 = { 63171  ,3 },        // Disc: Shadowstrike Rk. II
assault24 = { 63172  ,3 },        // Disc: Shadowstrike Rk. III

bleed1 =  { 19247  ,3 },        // disc: bleed Lv 83 rog UF
bleed2 =  { 19248  ,3 },        // disc: bleed Rk. II
bleed3 =  { 19249  ,3 },        // disc: bleed Rk. III
bleed4 =  { 26127  ,3 },        // disc: Wound Lv 88 rog HoT
bleed5 =  { 26128  ,3 },        // disc: Wound Rk. II
bleed6 =  { 26129  ,3 },        // disc: Wound Rk. III
bleed7 =  { 29228  ,3 },        // disc: Lacerate Lv 93 rog VoA
bleed8 =  { 29229  ,3 },        // disc: Lacerate Rk. II
bleed9 =  { 29230  ,3 },        // disc: Lacerate Rk. III
bleed10 = { 35284  ,3 },        // disc: Gash Lv 98 rog RoF
bleed11 = { 35285  ,3 },        // disc: Gash Rk. II
bleed12 = { 35286  ,3 },        // disc: Gash Rk. III
bleed13 = { 44151  ,3 },        // disc: Hack Lv 103 rog TDS
bleed14 = { 44152  ,3 },        // disc: Hack Rk. II
bleed15 = { 44153  ,3 },        // disc: Hack Rk. III
bleed16 = { 56303  ,3 },        // disc: Slice Lv 108 rog RoS
bleed17 = { 56304  ,3 },        // disc: Slice Rk. II
bleed18 = { 56305  ,3 },        // disc: Slice Rk. III
bleed19 = { 59625  ,3 },        // Disc: Slash - Level 113 - TOV
bleed20 = { 59626  ,3 },
bleed21 = { 59627  ,3 },
bleed22 = { 63132  ,3 },        // disc: Lance Lv 118 rog ToL
bleed23 = { 63133  ,3 },        // disc: Lance Rk. II
bleed24 = { 63134  ,3 },        // disc: Lance Rk. III

escape = { 102    ,4 },        // aa: escape

jugular1 =  { 15121  ,3 },        // disc: Jugular Slash Lv 77 ROG
jugular2 =  { 15122  ,3 },        // disc: Jugular Slash Rk. II
jugular3 =  { 15123  ,3 },        // disc: Jugular Slash Rk. III
jugular4 =  { 19268  ,3 },        // disc: Jugular Slice Lv 82 ROG
jugular5 =  { 19269  ,3 },        // disc: Jugular Slice Rk. II
jugular6 =  { 19270  ,3 },        // disc: Jugular Slice Rk. III
jugular7 =  { 26115  ,3 },        // disc: Jugular Sever Lv 87 ROG HoT
jugular8 =  { 26116  ,3 },        // disc: Jugular Sever Rk. II
jugular9 =  { 26117  ,3 },        // disc: Jugular Sever Rk. III
jugular10 = { 29210  ,3 },        // disc: Jugular Gash Lv 92 ROG VoA
jugular11 = { 29211  ,3 },        // disc: Jugular Gash Rk. II
jugular12 = { 29212  ,3 },        // disc: Jugular Gash Rk. III
jugular13 = { 35263  ,3 },        // disc: Jugular Lacerate Lv 97 ROG RoF
jugular14 = { 35264  ,3 },        // disc: Jugular Lacerate Rk. II
jugular15 = { 35265  ,3 },        // disc: Jugular Lacerate Rk. III
jugular16 = { 44136  ,3 },        // disc: Jugular Hack Lv 102 ROG TDS
jugular17 = { 44137  ,3 },        // disc: Jugular Hack Rk. II
jugular18 = { 44138  ,3 },        // disc: Jugular Hack Rk. III
jugular19 = { 56285  ,3 },        // disc: Jugular Strike Lv 107 Rog RoS
jugular20 = { 56286  ,3 },        // disc: Jugular Strike Rk. II
jugular21 = { 56287  ,3 },        // disc: Jugular Strike Rk. III
jugular22 = { 59607  ,3 },        // Disc: Jugular Cut - Level 112 - ToV
jugular23 = { 59608  ,3 },
jugular24 = { 59609  ,3 },
jugular25 = { 63120  ,3 },        // disc: Jugular Rend Lv 117 Rog ToL
jugular26 = { 63121  ,3 },        // disc: Jugular Rend Rk. II
jugular27 = { 63122  ,3 },        // disc: Jugular Rend Rk. III

knifeplay1 = { 40297  ,3 },        // disc: Knifeplay Discipline rog 97 Rof
knifeplay2 = { 40298  ,3 },        // disc: Knifeplay Disciplinen Rk. II
knifeplay3 = { 40299  ,3 },        // disc: Knifeplay Discipline Rk. III

pinpoint1 =  { 11925  ,3 },        // disc: Pinpoint Vulnerability Lv 74 ROG
pinpoint2 =  { 11926  ,3 },        // disc: Pinpoint Vulnerability Rk. II
pinpoint3 =  { 11927  ,3 },        // disc: Pinpoint Vulnerability Rk. III
pinpoint4 =  { 15115  ,3 },        // disc: Pinpoint Weaknesses Lv 79 ROG
pinpoint5 =  { 15116  ,3 },        // disc: Pinpoint Weaknesses Rk. II
pinpoint6 =  { 15117  ,3 },        // disc: Pinpoint Weaknesses Rk. III
pinpoint7 =  { 19262  ,3 },        // disc: Pinpoint Vitals Lv 84 ROG
pinpoint8 =  { 19263  ,3 },        // disc: Pinpoint Vitals Rk. II
pinpoint9 =  { 19264  ,3 },        // disc: Pinpoint Vitals Rk. III
pinpoint10 = { 26139  ,3 },        // disc: Pinpoint Flaws Lv 89 ROG HoT
pinpoint11 = { 26140  ,3 },        // disc: Pinpoint Flaws Rk. II
pinpoint12 = { 26141  ,3 },        // disc: Pinpoint Flaws Rk. III
pinpoint13 = { 29240  ,3 },        // disc: Pinpoint Liabilities Lv 94 ROG VoA
pinpoint14 = { 29241  ,3 },        // disc: Pinpoint Liabilities Rk. II
pinpoint15 = { 29242  ,3 },        // disc: Pinpoint Liabilities Rk. III
pinpoint16 = { 35296  ,3 },        // disc: Pinpoint Deficiencies Lv 99 ROG RoF
pinpoint17 = { 35297  ,3 },        // disc: Pinpoint Deficiencies Rk. II
pinpoint18 = { 35298  ,3 },        // disc: Pinpoint Deficiencies Rk. III
pinpoint19 = { 56306  ,3 },        // Disc: Pinpoint Shortcomings Rk. III
pinpoint20 = { 56307  ,3 },
pinpoint21 = { 56308  ,3 },
pinpoint22 = { 59628  ,3 },        // Disc: Pinpoint Defects - Level 114 - ToV
pinpoint23 = { 59629  ,3 },
pinpoint24 = { 59630  ,3 },

strike1 =  { 4659   ,3 },        // disc: sneak attack
strike2 =  { 4685   ,3 },        // disc: thief's vengeance
strike3 =  { 4686   ,3 },        // disc: assassin strike
strike4 =  { 5017   ,3 },        // disc: kyv strike
strike5 =  { 5018   ,3 },        // disc: ancient chaos strike
strike6 =  { 6174   ,3 },        // disc: daggerfall
strike7 =  { 8470   ,3 },        // disc: razor arc
strike8 =  { 15133  ,3 },        // disc: swiftblade
strike9 =  { 15134  ,3 },        // disc: swiftblade rk. ii
strike10 = { 15135  ,3 },        // disc: swiftblade rk. iii
strike11 = { 19280  ,3 },        // disc: Daggerlunge Lv 85 rog UF
strike12 = { 19281  ,3 },        // disc: Daggerlunge rk. ii
strike13 = { 19282  ,3 },        // disc: Daggerlunge rk. iii
strike14 = { 26148  ,3 },        // disc: Daggerswipe Lv 90 rog HoT
strike15 = { 26149  ,3 },        // disc: Daggerswipe rk. ii
strike16 = { 26150  ,3 },        // disc: Daggerswipe rk. iii
strike17 = { 29249  ,3 },        // disc: Daggerstrike Lv 95 rog VoA
strike18 = { 29250  ,3 },        // disc: Daggerstrike rk. ii
strike19 = { 29251  ,3 },        // disc: Daggerstrike rk. iii
strike20 = { 35305  ,3 },        // disc: Daggerthrust Lv 100 rog RoF
strike21 = { 35306  ,3 },        // disc: Daggerthrust rk. ii
strike22 = { 35307  ,3 },        // disc: Daggerthrust rk. iii
strike23 = { 44178  ,3 },        // disc: Daggergash Lv 105 rog TDS
strike24 = { 44179  ,3 },        // disc: Daggergash rk. ii
strike25 = { 44180  ,3 },        // disc: Daggergash rk. iii
strike26 = { 56330  ,3 },        // disc: Daggerslice Lv 110 Rog RoS
strike27 = { 56331  ,3 },        // disc: Daggerslice Rk. II
strike28 = { 56332  ,3 },        // disc: Daggerslice Rk. III
strike29 = { 59652  ,3 },        // Disc: Daggerslash - Level 115 - ToV
strike30 = { 59653  ,3 },
strike31 = { 59654  ,3 },

thiefeye1 = { 8001   ,3 },        // disc: thief's eye
thiefeye2 = { 40294  ,3 },        // disc: Thief's Vision
thiefeye3 = { 40295  ,3 },        // disc: Thief's Vision rk. II
thiefeye4 = { 40296  ,3 },        // disc: Thief's Vision rk. III

vigrog1 =  { 19871  ,3 },        // disc: Vigorous Dagger-Throw
vigrog2 =  { 19872  ,3 },        // disc: Vigorous Dagger-Throw Rk. II
vigrog3 =  { 19873  ,3 },        // disc: Vigorous Dagger-Throw Rk. III
vigrog4 =  { 26124  ,3 },        // disc: Vigorous Dagger-Strike
vigrog5 =  { 26125  ,3 },        // disc: Vigorous Dagger-Strike Rk. II
vigrog6 =  { 26126  ,3 },        // disc: Vigorous Dagger-Strike Rk. III
vigrog7 =  { 29225  ,3 },        // disc: Energetic Dagger-Strike
vigrog8 =  { 29226  ,3 },        // disc: Energetic Dagger-Strike Rk. II
vigrog9 =  { 29227  ,3 },        // disc: Energetic Dagger-Strike Rk. III
vigrog10 = { 35281  ,3 },        // disc: Energetic Dagger-Throw 98 Rog RoF
vigrog11 = { 35282  ,3 },        // disc: Energetic Dagger-Throw Rk. II
vigrog12 = { 35283  ,3 },        // disc: Energetic Dagger-Throw Rk. III
vigrog13 = { 44148  ,3 },        // disc: Exuberant Dagger-Throw 103 Rog TDS
vigrog14 = { 44149  ,3 },        // disc: Exuberant Dagger-Throw Rk. II
vigrog15 = { 44150  ,3 },        // disc: Exuberant Dagger-Throw Rk. III
vigrog16 = { 56300  ,3 },        // disc: Forceful Dagger-Throw 108 Rog RoS
vigrog17 = { 56301  ,3 },        // disc: Forceful Dagger-Throw Rk. II
vigrog18 = { 56302  ,3 },        // disc: Forceful Dagger-Throw Rk. III
vigrog19 = { 59622  ,3 },        // Disc: Powerful Dagger-Throw - Level 113 - ToV
vigrog20 = { 59622  ,3 },
vigrog21 = { 59622  ,3 },
vigrog22 = { 63135  ,3 },        // disc: Precise Dagger-Throw 118 Rog ToL
vigrog23 = { 63136  ,3 },        // disc: Precise Dagger-Throw Rk. II
vigrog24 = { 63137  ,3 },        // disc: Precise Dagger-Throw Rk. III

twisted = { 670    ,4 },        // aa: twisted shank
#pragma endregion Updated to ToL

#pragma region Shaman - Class 10
#pragma endregion - Empty

#pragma region Necromancer - Class 11
feign_n1 = { 25662  ,5 },        // spell: Scapegoat
feign_n2 = { 25663  ,5 },        // spell: Scapegoat rk ii
feign_n3 = { 25664  ,5 },        // spell: Scapegoat rk iii
feign_n4 = { 29571  ,5 },        // spell: Unwitting Sacrifice
feign_n5 = { 29572  ,5 },        // spell: Unwitting Sacrifice Rk. II
feign_n6 = { 29573  ,5 },        // spell: Unwitting Sacrifice Rk. III
feign_n7 = { 31616  ,5 },        // spell: Conscripted Sacrifice
feign_n8 = { 31617  ,5 },        // spell: Conscripted Sacrifice Rk. II
feign_n9 = { 31618  ,5 },        // spell: Conscripted Sacrifice Rk. III
#pragma endregion - Updated to ToV

#pragma region Wizard - Class 12
#pragma endregion - Empty

#pragma region Mage - Class 13
#pragma endregion - Empty

#pragma region Enchanter - Class 14
#pragma endregion - Empty

#pragma region Beastlord - Class 15
asp = { 986    ,4 },        // aa: bite of the asp

bvivi1 =  { 27098  ,3 },        // disc: Bestial Vivisection lv 86 BST HoT
bvivi2 =  { 27099  ,3 },        // disc: Bestial Vivisection Rk. II
bvivi3 =  { 27100  ,3 },        // disc: Bestial Vivisection Rk. III
bvivi4 =  { 30238  ,3 },        // disc: Bestial Rending lv 91 BST VoA
bvivi5 =  { 30239  ,3 },        // disc: Bestial Rending Rk. II
bvivi6 =  { 30240  ,3 },        // disc: Bestial Rending Rk. III
bvivi7 =  { 36319  ,3 },        // disc: Bestial Evulsing lv 96 BST RoF
bvivi8 =  { 36320  ,3 },        // disc: Bestial Evulsing Rk. II
bvivi9 =  { 36321  ,3 },        // disc: Bestial Evulsing Rk. III
bvivi10 = { 57335  ,3 },        // disc: Bestial Savagery Lv 106 Bst RoS
bvivi11 = { 57336  ,3 },        // disc: Bestial Savagery Rk. II
bvivi12 = { 57337  ,3 },        // disc: Bestial Savagery Rk. III
bvivi13 = { 64068  ,3 },        // disc: Bestial Fierceness Lv 116 Bst ToL
bvivi14 = { 64069  ,3 },        // disc: Bestial Fierceness Rk. II
bvivi15 = { 64070  ,3 },        // disc: Bestial Fierceness Rk. III

cstrike = { 11080  ,4 },        // aa: Chameleon Strike

feign_bst = { 11073  ,4 },        // aa: playing possum

feral1 = { 247    ,4 },        // aa: feral swipe

fclaw1 =  { 27119  ,3 },        // disc: flurry of claws lv 87 BST HoT
fclaw2 =  { 27120  ,3 },        // disc: flurry of claws rk. ii
fclaw3 =  { 27121  ,3 },        // disc: flurry of claws rk. iii
fclaw4 =  { 30271  ,3 },        // disc: Tumult of claws lv 92 BST Voa
fclaw5 =  { 30272  ,3 },        // disc: Tumult of claws rk. ii
fclaw6 =  { 30273  ,3 },        // disc: Tumult of claws rk. iii
fclaw7 =  { 36346  ,3 },        // disc: Clamor of Claws lv 97 BST Rof
fclaw8 =  { 36347  ,3 },        // disc: Clamor of Claws rk. ii
fclaw9 =  { 36348  ,3 },        // disc: Clamor of Claws rk. iii
fclaw10 = { 40516  ,3 },        // disc: Focused Clamor of Claws lv 98 BST Rof
fclaw11 = { 40517  ,3 },        // disc: Focused Clamor of Claws rk. ii
fclaw12 = { 40518  ,3 },        // disc: Focused Clamor of Claws rk. iii
fclaw13 = { 45103  ,3 },        // disc: Tempest of Claws lv 102 BST TDS
fclaw14 = { 45104  ,3 },        // disc: Tempest of Claws rk. ii
fclaw15 = { 45105  ,3 },        // disc: Tempest of Claws rk. iii
fclaw16 = { 57386  ,3 },        // disc: Storm of Claws Lv 107 Bst RoS
fclaw17 = { 57387  ,3 },        // disc: Storm of Claws Rk. II
fclaw18 = { 57388  ,3 },        // disc: Storm of Claws Rk. III
fclaw19 = { 60616  ,3 },        // disc: Maelstrom of Claws
fclaw20 = { 60617  ,3 },        // disc: Maelstrom of Claws Rk. II
fclaw21 = { 60618  ,3 },        // disc: Maelstrom of Claws Rk. III
fclaw22 = { 64111  ,3 },        // disc: Eruption of Claws Lv 117 Bst ToL
fclaw23 = { 64112  ,3 },        // disc: Eruption of Claws Rk. II
fclaw24 = { 64113  ,3 },        // disc: Eruption of Claws Rk. III

gorillasmash = { 988    ,4 },        // aa: gorilla smash

rake1 =  { 8782   ,3 },        // disc: rake
rake2 =  { 14158  ,3 },        // disc: harrow
rake3 =  { 14159  ,3 },        // disc: harrow rk. ii
rake4 =  { 14160  ,3 },        // disc: harrow rk. iii
rake5 =  { 18170  ,3 },        // disc: foray Lv 85 bst UF
rake6 =  { 18171  ,3 },        // disc: foray rk. ii
rake7 =  { 18172  ,3 },        // disc: foray rk. iii
rake8 =  { 27219  ,3 },        // disc: rush Lv 90 bst Hot
rake9 =  { 27220  ,3 },        // disc: rush rk. ii
rake10 = { 27221  ,3 },        // disc: rush rk. iii
rake11 = { 30368  ,3 },        // disc: Barrage Lv 95 bst Hot
rake12 = { 30369  ,3 },        // disc: Barrage rk. ii
rake13 = { 30370  ,3 },        // disc: Barrage rk. iii
rake14 = { 36425  ,3 },        // disc: Pummel Lv 100 bst RoF
rake15 = { 36426  ,3 },        // disc: Pummel rk. ii
rake16 = { 36427  ,3 },        // disc: Pummel rk. iii
rake17 = { 45164  ,3 },        // disc: Maul Lv 104 bst TDS
rake18 = { 45165  ,3 },        // disc: Maul rk. ii
rake19 = { 45166  ,3 },        // disc: Maul rk. iii
rake20 = { 57450  ,3 },        // disc: Mangle Lv 109 Bst RoS
rake21 = { 57451  ,3 },        // disc: Mangle Rk. II
rake22 = { 57452  ,3 },        // disc: Mangle Rk. III
rake23 = { 60678  ,3 },        // disc: Batter
rake24 = { 60679  ,3 },        // disc: Batter Rk. II
rake25 = { 60680  ,3 },        // disc: Batter Rk. III
rake26 = { 64151  ,3 },        // disc: Clobber Lv 119 Bst ToL
rake27 = { 64152  ,3 },        // disc: Clobber Rk. II
rake28 = { 64153  ,3 },        // disc: Clobber Rk. III

joltbst1 = { 362    ,4 },        // aa: roar of thunder

ravens = { 987    ,4 },        // aa: raven's claw
#pragma endregion Beastlord - Class 15 Updated for ToL

#pragma region Berserker - Class 16
bloodlust1 =  { 22506  ,3 },        // Disc: Shared Bloodlust Lv 85 ber UF
bloodlust2 =  { 22507  ,3 },        // Disc: Shared Bloodlust  Rk. II
bloodlust3 =  { 22508  ,3 },        // Disc: Shared Bloodlust  Rk. III
bloodlust4 =  { 27317  ,3 },        // Disc: Shared Brutality Lv 90 ber HoT
bloodlust5 =  { 27318  ,3 },        // Disc: Shared Brutality Rk. II
bloodlust6 =  { 27319  ,3 },        // Disc: Shared Brutality Rk. III
bloodlust7 =  { 30475  ,3 },        // Disc: Shared Savagery Lv 95 ber VoA
bloodlust8 =  { 30476  ,3 },        // Disc: Shared Savagery Rk. II
bloodlust9 =  { 30477  ,3 },        // Disc: Shared Savagery Rk. III
bloodlust10 = { 36541  ,3 },        // Disc: Shared Viciousness Lv 100 ber RoF
bloodlust11 = { 36542  ,3 },        // Disc: Shared Viciousness Rk. II
bloodlust12 = { 36543  ,3 },        // Disc: Shared Viciousness Rk. III
bloodlust13 = { 45278  ,3 },        // Disc: Shared Cruelty Lv 105 ber TDS
bloodlust14 = { 45279  ,3 },        // Disc: Shared Cruelty Rk. II
bloodlust15 = { 45280  ,3 },        // Disc: Shared Cruelty Rk. III
bloodlust16 = { 57558  ,3 },        // Disc: Shared Ruthlessness Lv 110 Ber RoS
bloodlust17 = { 57559  ,3 },        // Disc: Shared Ruthlessness Rk. II
bloodlust18 = { 57560  ,3 },        // Disc: Shared Ruthlessness Rk. III
bloodlust19 = { 60781  ,3 },        //Disc: Shared Svagery - Level 115 - TOV
bloodlust20 = { 60782  ,3 },
bloodlust21 = { 60783  ,3 },
bloodlust22 = { 64268  ,3 },        // Disc: Shared Violence Lv 120 Ber ToL
bloodlust23 = { 64269  ,3 },        // Disc: Shared Violence Rk. II
bloodlust24 = { 64270  ,3 },        // Disc: Shared Violence Rk. III

cripple1 =  { 4928   ,3 },        // Disc: leg strike
cripple2 =  { 4929   ,3 },        // Disc: leg cut
cripple3 =  { 4930   ,3 },        // Disc: leg slice
cripple4 =  { 8205   ,3 },        // Disc: crippling strike
cripple5 =  { 10908  ,3 },        // Disc: tendon cleave
cripple6 =  { 10909  ,3 },        // Disc: tendon cleave rk. ii
cripple7 =  { 10910  ,3 },        // Disc: tendon cleave rk. iii
cripple8 =  { 14177  ,3 },        // Disc: tendon sever
cripple9 =  { 14178  ,3 },        // Disc: tendon sever rk. ii
cripple10 = { 14179  ,3 },        // Disc: tendon sever rk. iii
cripple11 = { 18198  ,3 },        // Disc: tendon shear Lv 81 ber UF
cripple12 = { 18199  ,3 },        // Disc: tendon shear rk. ii
cripple13 = { 18200  ,3 },        // Disc: tendon shear rk. iii
cripple14 = { 27263  ,3 },        // Disc: tendon lacerate Lc 86 ber HoT
cripple15 = { 27264  ,3 },        // Disc: tendon lacerate rk. ii
cripple16 = { 27265  ,3 },        // Disc: tendon lacerate rk. iii
cripple17 = { 30412  ,3 },        // Disc: Tendon Slash Lv 91 ber VoA
cripple18 = { 30413  ,3 },        // Disc: Tendon Slash rk. ii
cripple19 = { 30414  ,3 },        // Disc: Tendon Slash rk. iii
cripple20 = { 36505  ,3 },        // Disc: Tendon Gash Lv 96 ber RoF
cripple21 = { 36506  ,3 },        // Disc: Tendon Gash Rk. II
cripple22 = { 36507  ,3 },        // Disc: Tendon Gash Rk. III
cripple23 = { 45224  ,3 },        // Disc: Tendon Tear Lv 101 ber TDS
cripple24 = { 45225  ,3 },        // Disc: Tendon Tear Rk. II
cripple25 = { 45226  ,3 },        // Disc: Tendon Tear Rk. III
cripple26 = { 57516  ,3 },        // Disc: Tendon Rupture Lv 106 Ber RoS
cripple27 = { 57517  ,3 },        // Disc: Tendon Rupture Rk. II
cripple28 = { 57518  ,3 },        // Disc: Tendon Rupture Rk. III
cripple29 = { 60730  ,3 },        // Disc: Tendon Rip - Level 111 - TOV
cripple30 = { 60731  ,3 },
cripple31 = { 60732  ,3 },
cripple32 = { 64205  ,3 },        // Disc: Tendon Shred Lv 116 Ber ToL
cripple33 = { 64206  ,3 },        // Disc: Tendon Shred Lv 116 Ber ToL
cripple34 = { 64207  ,3 },        // Disc: Tendon Shred Lv 116 Ber ToL

cryhavoc1 = { 8003   ,3 },        // Disc: cry havoc
cryhavoc2 = { 36556  ,3 },        // Disc: Cry Carnage lev 98 Rof
cryhavoc3 = { 36557  ,3 },        // Disc: Cry Carnage Rk. II
cryhavoc4 = { 36558  ,3 },        // Disc: Cry Carnage Rk. III

joltber1 =  { 4934   ,3 },        // Disc: diversive strike
joltber2 =  { 4935   ,3 },        // Disc: distracting strike
joltber3 =  { 4936   ,3 },        // Disc: confusing strike
joltber4 =  { 6171   ,3 },        // Disc: baffling strike
joltber5 =  { 10920  ,3 },        // Disc: jarring strike
joltber6 =  { 10921  ,3 },        // Disc: jarring strike rk ii
joltber7 =  { 10922  ,3 },        // Disc: jarring strike rk iii
joltber8 =  { 14186  ,3 },        // Disc: jarring smash
joltber9 =  { 14187  ,3 },        // Disc: jarring smash rk ii
joltber10 = { 14188  ,3 },        // Disc: jarring smash rk iii
joltber11 = { 18207  ,3 },        // Disc: Jarring Clash Lev 85 ber UF
joltber12 = { 18208  ,3 },        // Disc: Jarring Clash rk ii
joltber13 = { 18209  ,3 },        // Disc: Jarring Clash rk iii
joltber14 = { 27290  ,3 },        // Disc: Jarring Slam Lv 89 ber HoT
joltber15 = { 27291  ,3 },        // Disc: Jarring Slam rk ii
joltber16 = { 27292  ,3 },        // Disc: Jarring Slam rk iii
joltber17 = { 30445  ,3 },        // Disc: Jarring Blow Lv 94 ber VoA
joltber18 = { 30446  ,3 },        // Disc: Jarring Blow rk ii
joltber19 = { 30447  ,3 },        // Disc: Jarring Blow rk iii
joltber20 = { 36526  ,3 },        // Disc: Jarring Crush Lv 99 ber RoF
joltber21 = { 36527  ,3 },        // Disc: Jarring Crush rk ii
joltber22 = { 36528  ,3 },        // Disc: Jarring Crush rk iii
joltber23 = { 45266  ,3 },        // Disc: Jarring Smite Lv 104 ber TDS
joltber24 = { 45267  ,3 },        // Disc: Jarring Smite rk ii
joltber25 = { 45268  ,3 },        // Disc: Jarring Smite rk iii
joltber26 = { 57549  ,3 },        // Disc: Jarring Jolt Lv 109 Ber RoS
joltber27 = { 57550  ,3 },        // Disc: Jarring Jolt Rk. II
joltber28 = { 57551  ,3 },        // Disc: Jarring Jolt Rk. III
joltber29 = { 60772  ,3 },        // Disc: Jarring Strike - Level 114 - TOV
joltber30 = { 60773  ,3 },
joltber31 = { 60774  ,3 },
joltber32 = { 64250  ,3 },        // Disc: Jarring Impact Lv 119 Ber ToL
joltber33 = { 64251  ,3 },        // Disc: Jarring Impact Rk. II
joltber34 = { 64252  ,3 },        // Disc: Jarring Impact Rk. III

opfrenzy1 =  { 16918  ,3 },        // Disc: Overpowering Frenzy lv 81 ber UF
opfrenzy2 =  { 16919  ,3 },        // Disc: Overpowering Frenzy Rk. II
opfrenzy3 =  { 16920  ,3 },        // Disc: Overpowering Frenzy Rk. III
opfrenzy4 =  { 27260  ,3 },        // Disc: Overwhelming Frenzy  Lv 86 ber Hot
opfrenzy5 =  { 27261  ,3 },        // Disc: Overwhelming Frenzy Rk. II
opfrenzy6 =  { 27262  ,3 },        // Disc: Overwhelming Frenzy Rk. III
opfrenzy7 =  { 30409  ,3 },        // Disc: Conquering Frenzy  Lv 91 ber VoA
opfrenzy8 =  { 30410  ,3 },        // Disc: Conquering Frenzy Rk. II
opfrenzy9 =  { 30411  ,3 },        // Disc: Conquering Frenzy Rk. III
opfrenzy10 = { 36502  ,3 },        // Disc: Vanquishing Frenzy  Lv 96 ber RoF
opfrenzy11 = { 36503  ,3 },        // Disc: Vanquishing Frenzy Rk. II
opfrenzy12 = { 36504  ,3 },        // Disc: Vanquishing Frenzy Rk. III
opfrenzy13 = { 45221  ,3 },        // Disc: Demolishing Frenzy  Lv 101 ber TDS
opfrenzy14 = { 45222  ,3 },        // Disc: Demolishing Frenzy Rk. II
opfrenzy15 = { 45223  ,3 },        // Disc: Demolishing Frenzy Rk. III
opfrenzy16 = { 57513  ,3 },        // Disc: Mangling Frenzy - Level 106 - RoS
opfrenzy17 = { 57514  ,3 },        // Disc: Mangling Frenzy Rk. II
opfrenzy18 = { 57515  ,3 },        // Disc: Mangling Frenzy Rk. III
opfrenzy19 = { 60727  ,3 },        // Disc: Vindicating Frenzy - Level 111 - ToV
opfrenzy20 = { 60728  ,3 },
opfrenzy21 = { 60729  ,3 },
opfrenzy22 = { 64214  ,3 },        // Disc: Oppressing Frenzy Lv 116 Ber ToL
opfrenzy23 = { 64215  ,3 },        // Disc: Oppressing Frenzy Rk. II
opfrenzy24 = { 64216  ,3 },        // Disc: Oppressing Frenzy Rk. III

rallos1 =  { 19741  ,3 },        // Disc: Axe of Rallos Lv 85 ber UF
rallos2 =  { 19742  ,3 },        // Disc: Axe of Rallos Rk. II
rallos3 =  { 19743  ,3 },        // Disc: Axe of Rallos Rk. III
rallos4 =  { 27293  ,3 },        // Disc: Axe of Graster Lv 90 ber HoT
rallos5 =  { 27294  ,3 },        // Disc: Axe of Graster Rk. II
rallos6 =  { 27295  ,3 },        // Disc: Axe of Graster Rk. III
rallos7 =  { 30448  ,3 },        // Disc: Axe of Illdaera Lv 95 ber VoA
rallos8 =  { 30449  ,3 },        // Disc: Axe of Illdaera Rk. II
rallos9 =  { 30450  ,3 },        // Disc: Axe of Illdaera Rk. III
rallos10 = { 30658  ,3 },        // Disc: Axe of Zurel Lv 100 ber VoA
rallos11 = { 30659  ,3 },        // Disc: Axe of Zurel Rk. II
rallos12 = { 30660  ,3 },        // Disc: Axe of Zurel Rk. III
rallos13 = { 45284  ,3 },        // Disc: Axe of Numicia Lv 105 ber TDS
rallos14 = { 45285  ,3 },        // Disc: Axe of Numicia Rk. II
rallos15 = { 45286  ,3 },        // Disc: Axe of Numicia Rk. III
rallos16 = { 50371  ,3 },        // Disc: Axe of the Aeons ber 103 eok
rallos17 = { 50372  ,3 },        // Disc: Axe of the Aeons Rk. II
rallos18 = { 50373  ,3 },        // Disc: Axe of the Aeons Rk. III
rallos19 = { 58273  ,3 },        // Disc: Axe of Empyr 107 ber TBL
rallos20 = { 58274  ,3 },        // Disc: Axe of Empyr Rk. II
rallos21 = { 58275  ,3 },        // Disc: Axe of Empyr Rk. III
rallos22 = { 57564  ,3 },        // Disc: Axe of Rekatok Lv 110 Ber RoS
rallos23 = { 57565  ,3 },        // Disc: Axe of Rekatok Rk. II
rallos24 = { 57566  ,3 },        // Disc: Axe of Rekatok Rk. III
rallos25 = { 60787  ,3 },        // Disc: Axe of Derakor - Level 115 ToV
rallos26 = { 60788  ,3 },        // Disc: Axe of Derakor - Level 115 ToV
rallos27 = { 60789  ,3 },        // Disc: Axe of Derakor - Level 115 ToV
rallos28 = { 64274  ,3 },        // Disc: Axe of Xin Diabo Lv 120 Ber ToL
rallos29 = { 64275  ,3 },        // Disc: Axe of Xin Diabo Rk. II
rallos30 = { 64276  ,3 },        // Disc: Axe of Xin Diabo Rk. III

slapface1 =  { 27269  ,3 },        // Disc: slap in the face Lv 87 ber HoT
slapface2 =  { 27270  ,3 },        // Disc: slap in the face rk. ii
slapface3 =  { 27271  ,3 },        // Disc: slap in the face rk. iii
slapface4 =  { 30418  ,3 },        // Disc: Kick in the Teeth Lv 92 ber VoA
slapface5 = { 30419  ,3 },        // Disc: Kick in the Teeth rk. ii
slapface6 = { 30420  ,3 },        // Disc: Kick in the Teeth rk. iii
slapface7 = { 36508  ,3 },        // Disc: Punch in the Throat  Lv 97 ber RoF
slapface8 = { 36509  ,3 },        // Disc: Punch in the Throat rk. ii
slapface9 = { 36510  ,3 },        // Disc: Punch in the Throat rk. iii
slapface10 = { 45227  ,3 },        // Disc: Kick in the Shins  Lv 102 ber TDS
slapface11 = { 45228  ,3 },        // Disc: Kick in the Shins rk. ii
slapface12 = { 45229  ,3 },        // Disc: Kick in the Shins rk. iii
slapface13 = { 57519  ,3 },        // Disc: Sucker Punch Lv 107 Ber RoS
slapface14 = { 57520  ,3 },        // Disc: Sucker Punch Rk. II
slapface15 = { 57521  ,3 },        // Disc: Sucker Punch Rk. III
slapface16 = { 60734  ,3 },        // Disc: Rabbit Punch - Level 112 - ToV
slapface17 = { 60735  ,3 },
slapface18 = { 60736  ,3 },
slapface19 = { 64220  ,3 },        // Disc: Swift Punch Lv 117 Ber ToL
slapface20 = { 64221  ,3 },        // Disc: Swift Punch Rk. II
slapface21 = { 64222  ,3 },        // Disc: Swift Punch Rk. III

stunber1 = { 4931   ,3 },        // Disc: head strike
stunber2 = { 4932   ,3 },        // Disc: head pummel
stunber3 = { 4933   ,3 },        // Disc: head crush
stunber4 = { 6170   ,3 },        // Disc: mind strike
stunber5 = { 10917  ,3 },        // Disc: temple blow
stunber6 = { 10918  ,3 },        // Disc: temple blow rk. ii
stunber7 = { 10919  ,3 },        // Disc: temple blow rk. iii
stunber8 = { 14183  ,3 },        // Disc: temple strike
stunber9 = { 14184  ,3 },        // Disc: temple strike rk. ii
stunber10 = { 14185  ,3 },        // Disc: temple strike rk. iii
stunber11 = { 18204  ,3 },        // Disc: Temple Bash Lv 83 ber UF
stunber12 = { 18205  ,3 },        // Disc: Temple Bash rk. ii
stunber13 = { 18206  ,3 },        // Disc: Temple Bash rk. iii
stunber14 = { 27281  ,3 },        // Disc: Temple Chop Lv 88 ber HoT
stunber15 = { 27282  ,3 },        // Disc: Temple Chop rk. ii
stunber16 = { 27283  ,3 },        // Disc: Temple Chop rk. iii
stunber17 = { 30430  ,3 },        // Disc: Temple Smash Lv 93 ber VoA
stunber18 = { 30431  ,3 },        // Disc: Temple Smash rk. ii
stunber19 = { 30432  ,3 },        // Disc: Temple Smash rk. iii
stunber20 = { 36520  ,3 },        // Disc: Temple Crush Lv 98 ber RoF
stunber21 = { 36521  ,3 },        // Disc: Temple Crush rk. ii
stunber22 = { 36522  ,3 },        // Disc: Temple Crush rk. iii
stunber23 = { 45251  ,3 },        // Disc: Temple Demolish Lv 103 ber TDS
stunber24 = { 45252  ,3 },        // Disc: Temple Demolish rk. ii
stunber25 = { 45253  ,3 },        // Disc: Temple Demolish rk. iii
stunber26 = { 57534  ,3 },        // Disc: Temple Slam Lv 108 Ber RoS
stunber27 = { 57535  ,3 },        // Disc: Temple Slam Rk. II
stunber28 = { 57536  ,3 },        // Disc: Temple Slam Rk. III
stunber29 = { 60760  ,3 },        // Disc: Temple Crack - Level 113 - TOV
stunber30 = { 60761  ,3 },
stunber31 = { 60762  ,3 },
stunber32 = { 64244  ,3 },        // Disc: Temple Shatter Lv 118 Ber ToL
stunber33 = { 64245  ,3 },        // Disc: Temple Shatter Rk. II
stunber34 = { 64246  ,3 },        // Disc: Temple Shatter Rk. II

volley1 = { 6754   ,3 },        // Disc: rage volley
volley2 = { 6729   ,3 },        // Disc: destroyer's volley
volley3 = { 10926  ,3 },        // Disc: giant slayer's volley
volley4 = { 10927  ,3 },        // Disc: giant slayer's volley rk ii
volley5 = { 10928  ,3 },        // Disc: giant slayer's volley rk iii
volley6 = { 11928  ,3 },        // Disc: annihilator's volley
volley7 = { 11929  ,3 },        // Disc: annihilator's volley rk ii
volley8 = { 11930  ,3 },        // Disc: annihilator's volley rk iii
volley9 = { 14195  ,3 },        // Disc: decimator's volley
volley10 = { 14196  ,3 },        // Disc: decimator's volley rk ii
volley11 = { 14197  ,3 },        // Disc: decimator's volley rk iii
volley12 = { 18216  ,3 },        // Disc: Eradicator's Volley Lv 84 ber UF
volley13 = { 18217  ,3 },        // Disc: Eradicator's Volley Rk. II
volley14 = { 18218  ,3 },        // Disc: Eradicator's Volley Rk. III
volley15 = { 27287  ,3 },        // Disc: Savage Volley Lv 89 ber HoT
volley16 = { 27288  ,3 },        // Disc: Savage Volley Rk. II
volley17 = { 27289  ,3 },        // Disc: Savage Volley Rk. III
volley18 = { 30442  ,3 },        // Disc: Sundering Volley Lv 94 ber VoA
volley19 = { 30443  ,3 },        // Disc: Sundering Volley Rk. II
volley20 = { 30444  ,3 },        // Disc: Sundering Volley Rk. III
volley21 = { 36523  ,3 },        // Disc: Brutal Volley Lv 99 ber RoF
volley22 = { 36524  ,3 },        // Disc: Brutal Volley Rk. II
volley23 = { 36525  ,3 },        // Disc: Brutal Volley Rk. III
volley24 = { 45263  ,3 },        // Disc: Demolishing Volley Lv 104 ber TDS
volley25 = { 45264  ,3 },        // Disc: Demolishing Volley Rk. II
volley26 = { 45265  ,3 },        // Disc: Demolishing Volley Rk. III
volley27 = { 57546  ,3 },        // Disc: Mangling Volley Lv 109 Ber RoS
volley28 = { 57547  ,3 },        // Disc: Mangling Volley Rk. II
volley29 = { 57548  ,3 },        // Disc: Mangling Volley Rk. III
volley30 = { 60769  ,3 },        // Disc: Vindicating Volley - Level 114 - ToV
volley31 = { 60770  ,3 },
volley32 = { 60771  ,3 },
volley33 = { 64253  ,3 },        // Disc: Pulverizing Volley Lv 119 Ber ToL
volley34 = { 64254  ,3 },        // Disc: Pulverizing Volley Rk. II
volley35 = { 64255  ,3 },        // Disc: Pulverizing Volley Rk. III

vigber1 = { 19753  ,3 },        // Disc: Vigorous Axe Throw Lv 83 ber UF
vigber2 = { 19754  ,3 },        // Disc: Vigorous Axe Throw Rk. II
vigber3 = { 19755  ,3 },        // Disc: Vigorous Axe Throw Rk. III
vigber4 = { 27278  ,3 },        // Disc: Energetic Axe Throw Lv 88 ber HoT
vigber5 = { 27279  ,3 },        // Disc: Energetic Axe Throw Rk. II
vigber6 = { 27280  ,3 },        // Disc: Energetic Axe Throw Rk. III
vigber7 = { 30427  ,3 },        // Disc: Spirited Axe Throw Lv 93 ber VoA
vigber8 = { 30428  ,3 },        // Disc: Spirited Axe Throw Rk. II
vigber9 = { 30429  ,3 },        // Disc: Spirited Axe Throw Rk. III
vigber10 = { 36517  ,3 },        // Disc: Brutal Axe Throw Lv 98 ber RoF
vigber11 = { 36518  ,3 },        // Disc: Brutal Axe Throw Rk. II
vigber12 = { 36519  ,3 },        // Disc: Brutal Axe Throw Rk. III
vigber13 = { 45248  ,3 },        // Disc: Demolishing Axe Throw Lv 103 ber TDS
vigber14 = { 45249  ,3 },        // Disc: Demolishing Axe Throw Rk. II
vigber15 = { 45250  ,3 },        // Disc: Demolishing Axe Throw Rk. III
vigber16 = { 57531  ,3 },        // Disc: Mangling Axe Throw    Lv 108 Ber RoS
vigber17 = { 57532  ,3 },        // Disc: Mangling Axe Throw Rk. II
vigber18 = { 57533  ,3 },        // Disc: Mangling Axe Throw Rk. III
vigber19 = { 60757  ,3 },        // Disc: Vindicating Axe Throw - Level
vigber20 = { 60758  ,3 },
vigber21 = { 60759  ,3 },
vigber22 = { 64247  ,3 },        // Disc: Maiming Axe Throw  Lv 118 Ber ToL
vigber23 = { 64248  ,3 },        // Disc: Maiming Axe Throw  Rk. II
vigber24 = { 64249  ,3 };        // Disc: Maiming Axe Throw  Rk. II
#pragma endregion Updated to ToL


#define DECLARE_ABILITY_OPTION( __var, __key, __help, __default, __show) const char* __var[]  = {\
                                                                                  __key, \
                                                                                  __help, \
                                                                                  __default, \
                                                                                  __show, \
                                                                                  };
#define REGISTER_ABILITY_OPTION( __var, __func, __ability ) MapInsert(&CmdListe, Option(__var[0], __var[1], __var[2], __var[3], __func, __ability));

DECLARE_ABILITY_OPTION(pDEBUG, "debug", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[debug].Length},1,0]}");
DECLARE_ABILITY_OPTION(pAGGRO, "aggro", "[ON/OFF]?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD]},1,0]}", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pAGGRP, "aggropri", "[ID] Primary (Aggro)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pAGGRS, "aggrosec", "[ID] Offhand (Aggro)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]} && ${meleemvi[aggropri]},1,0]}");
DECLARE_ABILITY_OPTION(pARROW, "arrow", "[ID] item?", "0", "${If[${meleemvi[plugin]} && (${Me.Skill[archery]} || ${Me.Skill[throwing]}),1,0]}");
DECLARE_ABILITY_OPTION(pBASHS, "bash", "[#] Bash 0=0ff", "${If[${Me.Skill[bash]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[bash]},1,0]}");
DECLARE_ABILITY_OPTION(pBKOFF, "backoff", "[#] Life% Below? 0=0ff", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pBOWID, "bow", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[archery]},1,0]}");
DECLARE_ABILITY_OPTION(pBTLLP, "battleleap", "[ON/OFF]?", "${If[${Me.AltAbility[Battle Leap]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Battle Leap]},1,0]}");
DECLARE_ABILITY_OPTION(pBGING, "begging", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[begging]},1,0]}");
DECLARE_ABILITY_OPTION(pDISRM, "disarm", "[ON/OFF]?", "${If[${Me.Skill[disarm]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[disarm]},1,0]}");
DECLARE_ABILITY_OPTION(pKICKS, "kick", "[ON/OFF]?", "${If[${Me.Skill[kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[kick]},1,0]}");
DECLARE_ABILITY_OPTION(pSTUN0, "stun0", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[stunning]},1,0]}");
DECLARE_ABILITY_OPTION(pSTUN1, "stun1", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[stunning]},1,0]}");
DECLARE_ABILITY_OPTION(pSTUNS, "stunning", "[#] Target Life% Below? 0=0ff", "0", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pDRPNC, "dragonpunch", "[ON/OFF]?", "${If[${Me.Skill[dragon punch]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[dragon punch]},1,0]}");
DECLARE_ABILITY_OPTION(pEAGLE, "eaglestrike", "[ON/OFF]?", "${If[${Me.Skill[eagle strike]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[eagle strike]},1,0]}");
DECLARE_ABILITY_OPTION(pERAGE, "enrage", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pFACES, "facing", "[ON/OFF] Face Target (Range)?", "1", "${If[${meleemvi[plugin]} && ${meleemvi[range]},1,0]}");
DECLARE_ABILITY_OPTION(pHIDES, "hide", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[hide]},1,0]}");
DECLARE_ABILITY_OPTION(pFEIGN, "feigndeath", "[#] Life% Below? 0=0ff", "${If[${Select[${Me.Class.ShortName},BST,SHD,NEC,MNK]},30,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},BST,SHD,NEC,MNK]},1,0]}");
//DECLARE_ABILITY_OPTION(pFKICK, "ferociouskick", "[ON/OFF]?", "${If[${Me.AltAbility[ferocious kick]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[ferocious kick]},1,0]}");
DECLARE_ABILITY_OPTION(pFLYKC, "flyingkick", "[ON/OFF]?", "${If[${Me.Skill[flying kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[flying kick]},1,0]}");
DECLARE_ABILITY_OPTION(pFORAG, "forage", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[forage]},1,0]}");
DECLARE_ABILITY_OPTION(pHFAST, "pothealfast", "[#] MyLife% Below? 0=0ff (FAST)", "${If[${meleemvi[idpothealfast]},30,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[idpothealfast]},1,0]}");
DECLARE_ABILITY_OPTION(pHOVER, "pothealover", "[#] MyLife% Below? 0=0ff (OVER)", "${If[${meleemvi[idpothealover]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[idpothealover]},1,0]}");
DECLARE_ABILITY_OPTION(pINFUR, "infuriate", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pINTIM, "intimidation", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[intimidation]},1,0]}");
DECLARE_ABILITY_OPTION(pJOLTS, "jolt", "Every [#] of Hits,0=0ff", "0", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},BER,BST,RNG]},1,0]}");
DECLARE_ABILITY_OPTION(pMELEE, "melee", "[ON/OFF] Melee Mode? 0=0ff", "${If[${Select[${Me.Class.ShortName},WAR,PAL,RNG,SHD,MNK,BRD,ROG,BST,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]},1,0]}");
DECLARE_ABILITY_OPTION(pMELEP, "meleepri", "[ID] Primary (Melee)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pMELES, "meleesec", "[ID] Offhand (Melee)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]} && ${meleemvi[meleepri]}");
DECLARE_ABILITY_OPTION(pPETAS, "petassist", "[ON/OFF] Assist Me?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPETDE, "petdelay", "[#] # Sec Delay Before Engaging?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},0,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPETENG, "petengagehps", "[#] TargetCurrentHPS% Below? 0=0ff", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},98,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPETMN, "petmend", "[#] Mend Pet Life % Below 0=0ff?", "${If[${Me.AltAbility[mend companion]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Me.AltAbility[mend companion]},1,0]}");
DECLARE_ABILITY_OPTION(pPETRN, "petrange", "[#] Target/Pet in this range?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},75,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPLUGS, "plugin", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVK0, "provoke0", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVK1, "provoke1", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVKE, "provokeend", "[#] Stop when Target Life% Below?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVKM, "provokemax", "[#] Counter? ,1=try once, 0=0ff", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVKO, "provokeonce", "[ON/OFF]?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pRANGE, "range", "[#] Max Range? 0=0ff", "0", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pRESUM, "resume", "[#] Life% Above? 100=0ff", "75", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pRKICK, "roundkick", "[ON/OFF]?", "${If[${Me.Skill[round kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[round kick]},1,0]}");
DECLARE_ABILITY_OPTION(pSENSE, "sensetraps", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[sense traps]},1,0]}");
DECLARE_ABILITY_OPTION(pSHIEL, "shield", "[ID] item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[bash]},0,0]}");
DECLARE_ABILITY_OPTION(pSLAMS, "slam", "[ON/OFF]?", "${If[${Select[${Me.Race.ID},2,9,10]},1,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Race.ID},2,9,10]},1,0]}"); // 2=barbarian 9=troll 10=ogre
DECLARE_ABILITY_OPTION(pSNEAK, "sneak", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[sneak]},1,0]}");
DECLARE_ABILITY_OPTION(pSTAND, "standup", "[ON/OFF] Authorize to StandUp?", "0", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKD, "stickdelay", "[#] Sec to Wait Target in Range?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKKB, "stickbreak", "0=Normal, 1=Allow BreakOnKB", "1", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKM, "stickmode", "[0-2] 0=Built-In 1=From INI 2=do not stick?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKNR, "sticknorange", "0=Normal, 1=No Range Check", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKR, "stickrange", "[#] Target in Range? 0=0ff", "${If[${Stick.Status.NotEqual[NULL]},75,0]}", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]} && ${meleemvi[stickrange]},1,0]}");
DECLARE_ABILITY_OPTION(pSTRIKM, "strikemode", "[ON/OFF] Use strikecmd from ini?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pTAUNT, "taunt", "[ON/OFF]?", "${If[${Me.Skill[taunt]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]} && ${Me.Skill[taunt]},1,0]}");
DECLARE_ABILITY_OPTION(pTHROW, "throwstone", "[#] Endu% Above? 0=0ff", "0", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[throw stone]},1,0]}");
DECLARE_ABILITY_OPTION(pTIGER, "tigerclaw", "[ON/OFF]?", "${If[${Me.Skill[tiger claw]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[tiger claw]},1,0]}");


#pragma region Downshit
DECLARE_ABILITY_OPTION(pDWNF0, "downflag0", "[ON/OFF] downflag0?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit0].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF1, "downflag1", "[ON/OFF] downflag1?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit1].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF2, "downflag2", "[ON/OFF] downflag2?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit2].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF3, "downflag3", "[ON/OFF] downflag3?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit3].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF4, "downflag4", "[ON/OFF] downflag4?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit4].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF5, "downflag5", "[ON/OFF] downflag5?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit5].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF6, "downflag6", "[ON/OFF] downflag6?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit6].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF7, "downflag7", "[ON/OFF] downflag7?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit7].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF8, "downflag8", "[ON/OFF] downflag8?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit8].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF9, "downflag9", "[ON/OFF] downflag9?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit9].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF10, "downflag10", "[ON/OFF] downflag10?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit10].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF11, "downflag11", "[ON/OFF] downflag11?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit11].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF12, "downflag12", "[ON/OFF] downflag12?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit12].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF13, "downflag13", "[ON/OFF] downflag13?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit13].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF14, "downflag14", "[ON/OFF] downflag14?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit14].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF15, "downflag15", "[ON/OFF] downflag15?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit15].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF16, "downflag16", "[ON/OFF] downflag16?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit16].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF17, "downflag17", "[ON/OFF] downflag17?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit17].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF18, "downflag18", "[ON/OFF] downflag18?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit18].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF19, "downflag19", "[ON/OFF] downflag19?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit19].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF20, "downflag20", "[ON/OFF] downflag20?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit20].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF21, "downflag21", "[ON/OFF] downflag21?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit21].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF22, "downflag22", "[ON/OFF] downflag22?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit22].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF23, "downflag23", "[ON/OFF] downflag23?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit23].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF24, "downflag24", "[ON/OFF] downflag24?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit24].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF25, "downflag25", "[ON/OFF] downflag25?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit25].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF26, "downflag26", "[ON/OFF] downflag26?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit26].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF27, "downflag27", "[ON/OFF] downflag27?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit27].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF28, "downflag28", "[ON/OFF] downflag28?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit28].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF29, "downflag29", "[ON/OFF] downflag29?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit29].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF30, "downflag30", "[ON/OFF] downflag30?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit30].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF31, "downflag31", "[ON/OFF] downflag31?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit31].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF32, "downflag32", "[ON/OFF] downflag32?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit32].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF33, "downflag33", "[ON/OFF] downflag33?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit33].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF34, "downflag34", "[ON/OFF] downflag34?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit34].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF35, "downflag35", "[ON/OFF] downflag35?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit35].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF36, "downflag36", "[ON/OFF] downflag36?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit36].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF37, "downflag37", "[ON/OFF] downflag37?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit37].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF38, "downflag38", "[ON/OFF] downflag38?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit38].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF39, "downflag39", "[ON/OFF] downflag39?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit39].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF40, "downflag40", "[ON/OFF] downflag40?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit40].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF41, "downflag41", "[ON/OFF] downflag41?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit41].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF42, "downflag42", "[ON/OFF] downflag42?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit42].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF43, "downflag43", "[ON/OFF] downflag43?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit43].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF44, "downflag44", "[ON/OFF] downflag44?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit44].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF45, "downflag45", "[ON/OFF] downflag45?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit45].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF46, "downflag46", "[ON/OFF] downflag46?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit46].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF47, "downflag47", "[ON/OFF] downflag47?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit47].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF48, "downflag48", "[ON/OFF] downflag48?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit48].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF49, "downflag49", "[ON/OFF] downflag49?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit49].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF50, "downflag50", "[ON/OFF] downflag50?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit50].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF51, "downflag51", "[ON/OFF] downflag51?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit51].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF52, "downflag52", "[ON/OFF] downflag52?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit52].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF53, "downflag53", "[ON/OFF] downflag53?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit53].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF54, "downflag54", "[ON/OFF] downflag54?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit54].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF55, "downflag55", "[ON/OFF] downflag55?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit55].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF56, "downflag56", "[ON/OFF] downflag56?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit56].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF57, "downflag57", "[ON/OFF] downflag57?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit57].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF58, "downflag58", "[ON/OFF] downflag58?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit58].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF59, "downflag59", "[ON/OFF] downflag59?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit59].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF60, "downflag60", "[ON/OFF] downflag60?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit60].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF61, "downflag61", "[ON/OFF] downflag61?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit61].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF62, "downflag62", "[ON/OFF] downflag62?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit62].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF63, "downflag63", "[ON/OFF] downflag63?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit63].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF64, "downflag64", "[ON/OFF] downflag64?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit64].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF65, "downflag65", "[ON/OFF] downflag65?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit65].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF66, "downflag66", "[ON/OFF] downflag66?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit66].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF67, "downflag67", "[ON/OFF] downflag67?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit67].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF68, "downflag68", "[ON/OFF] downflag68?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit68].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF69, "downflag69", "[ON/OFF] downflag69?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit69].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF70, "downflag70", "[ON/OFF] downflag70?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit70].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF71, "downflag71", "[ON/OFF] downflag71?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit71].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF72, "downflag72", "[ON/OFF] downflag72?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit72].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF73, "downflag73", "[ON/OFF] downflag73?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit73].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF74, "downflag74", "[ON/OFF] downflag74?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit74].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF75, "downflag75", "[ON/OFF] downflag75?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit75].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF76, "downflag76", "[ON/OFF] downflag76?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit76].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF77, "downflag77", "[ON/OFF] downflag77?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit77].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF78, "downflag78", "[ON/OFF] downflag78?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit78].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF79, "downflag79", "[ON/OFF] downflag79?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit79].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF80, "downflag80", "[ON/OFF] downflag80?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit80].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF81, "downflag81", "[ON/OFF] downflag81?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit81].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF82, "downflag82", "[ON/OFF] downflag82?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit82].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF83, "downflag83", "[ON/OFF] downflag83?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit83].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF84, "downflag84", "[ON/OFF] downflag84?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit84].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF85, "downflag85", "[ON/OFF] downflag85?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit85].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF86, "downflag86", "[ON/OFF] downflag86?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit86].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF87, "downflag87", "[ON/OFF] downflag87?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit87].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF88, "downflag88", "[ON/OFF] downflag88?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit88].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF89, "downflag89", "[ON/OFF] downflag89?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit89].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF90, "downflag90", "[ON/OFF] downflag90?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit90].Length},1,0]}");
#pragma endregion

#pragma region Holyshit
DECLARE_ABILITY_OPTION(pHOLF0, "holyflag0", "[ON/OFF] holyflag0?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit0].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF1, "holyflag1", "[ON/OFF] holyflag1?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit1].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF2, "holyflag2", "[ON/OFF] holyflag2?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit2].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF3, "holyflag3", "[ON/OFF] holyflag3?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit3].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF4, "holyflag4", "[ON/OFF] holyflag4?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit4].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF5, "holyflag5", "[ON/OFF] holyflag5?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit5].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF6, "holyflag6", "[ON/OFF] holyflag6?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit6].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF7, "holyflag7", "[ON/OFF] holyflag7?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit7].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF8, "holyflag8", "[ON/OFF] holyflag8?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit8].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF9, "holyflag9", "[ON/OFF] holyflag9?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit9].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF10, "holyflag10", "[ON/OFF] holyflag10?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit10].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF11, "holyflag11", "[ON/OFF] holyflag11?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit11].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF12, "holyflag12", "[ON/OFF] holyflag12?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit12].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF13, "holyflag13", "[ON/OFF] holyflag13?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit13].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF14, "holyflag14", "[ON/OFF] holyflag14?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit14].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF15, "holyflag15", "[ON/OFF] holyflag15?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit15].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF16, "holyflag16", "[ON/OFF] holyflag16?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit16].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF17, "holyflag17", "[ON/OFF] holyflag17?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit17].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF18, "holyflag18", "[ON/OFF] holyflag18?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit18].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF19, "holyflag19", "[ON/OFF] holyflag19?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit19].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF20, "holyflag20", "[ON/OFF] holyflag20?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit20].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF21, "holyflag21", "[ON/OFF] holyflag21?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit21].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF22, "holyflag22", "[ON/OFF] holyflag22?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit22].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF23, "holyflag23", "[ON/OFF] holyflag23?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit23].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF24, "holyflag24", "[ON/OFF] holyflag24?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit24].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF25, "holyflag25", "[ON/OFF] holyflag25?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit25].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF26, "holyflag26", "[ON/OFF] holyflag26?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit26].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF27, "holyflag27", "[ON/OFF] holyflag27?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit27].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF28, "holyflag28", "[ON/OFF] holyflag28?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit28].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF29, "holyflag29", "[ON/OFF] holyflag29?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit29].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF30, "holyflag30", "[ON/OFF] holyflag30?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit30].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF31, "holyflag31", "[ON/OFF] holyflag31?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit31].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF32, "holyflag32", "[ON/OFF] holyflag32?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit32].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF33, "holyflag33", "[ON/OFF] holyflag33?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit33].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF34, "holyflag34", "[ON/OFF] holyflag34?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit34].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF35, "holyflag35", "[ON/OFF] holyflag35?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit35].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF36, "holyflag36", "[ON/OFF] holyflag36?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit36].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF37, "holyflag37", "[ON/OFF] holyflag37?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit37].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF38, "holyflag38", "[ON/OFF] holyflag38?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit38].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF39, "holyflag39", "[ON/OFF] holyflag39?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit39].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF40, "holyflag40", "[ON/OFF] holyflag40?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit40].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF41, "holyflag41", "[ON/OFF] holyflag41?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit41].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF42, "holyflag42", "[ON/OFF] holyflag42?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit42].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF43, "holyflag43", "[ON/OFF] holyflag43?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit43].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF44, "holyflag44", "[ON/OFF] holyflag44?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit44].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF45, "holyflag45", "[ON/OFF] holyflag45?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit45].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF46, "holyflag46", "[ON/OFF] holyflag46?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit46].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF47, "holyflag47", "[ON/OFF] holyflag47?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit47].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF48, "holyflag48", "[ON/OFF] holyflag48?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit48].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF49, "holyflag49", "[ON/OFF] holyflag49?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit49].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF50, "holyflag50", "[ON/OFF] holyflag50?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit50].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF51, "holyflag51", "[ON/OFF] holyflag51?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit51].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF52, "holyflag52", "[ON/OFF] holyflag52?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit52].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF53, "holyflag53", "[ON/OFF] holyflag53?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit53].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF54, "holyflag54", "[ON/OFF] holyflag54?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit54].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF55, "holyflag55", "[ON/OFF] holyflag55?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit55].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF56, "holyflag56", "[ON/OFF] holyflag56?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit56].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF57, "holyflag57", "[ON/OFF] holyflag57?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit57].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF58, "holyflag58", "[ON/OFF] holyflag58?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit58].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF59, "holyflag59", "[ON/OFF] holyflag59?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit59].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF60, "holyflag60", "[ON/OFF] holyflag60?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit60].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF61, "holyflag61", "[ON/OFF] holyflag61?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit61].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF62, "holyflag62", "[ON/OFF] holyflag62?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit62].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF63, "holyflag63", "[ON/OFF] holyflag63?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit63].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF64, "holyflag64", "[ON/OFF] holyflag64?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit64].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF65, "holyflag65", "[ON/OFF] holyflag65?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit65].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF66, "holyflag66", "[ON/OFF] holyflag66?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit66].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF67, "holyflag67", "[ON/OFF] holyflag67?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit67].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF68, "holyflag68", "[ON/OFF] holyflag68?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit68].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF69, "holyflag69", "[ON/OFF] holyflag69?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit69].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF70, "holyflag70", "[ON/OFF] holyflag70?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit70].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF71, "holyflag71", "[ON/OFF] holyflag71?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit71].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF72, "holyflag72", "[ON/OFF] holyflag72?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit72].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF73, "holyflag73", "[ON/OFF] holyflag73?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit73].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF74, "holyflag74", "[ON/OFF] holyflag74?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit74].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF75, "holyflag75", "[ON/OFF] holyflag75?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit75].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF76, "holyflag76", "[ON/OFF] holyflag76?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit76].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF77, "holyflag77", "[ON/OFF] holyflag77?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit77].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF78, "holyflag78", "[ON/OFF] holyflag78?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit78].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF79, "holyflag79", "[ON/OFF] holyflag79?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit79].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF80, "holyflag80", "[ON/OFF] holyflag80?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit80].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF81, "holyflag81", "[ON/OFF] holyflag81?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit81].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF82, "holyflag82", "[ON/OFF] holyflag82?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit82].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF83, "holyflag83", "[ON/OFF] holyflag83?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit83].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF84, "holyflag84", "[ON/OFF] holyflag84?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit84].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF85, "holyflag85", "[ON/OFF] holyflag85?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit85].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF86, "holyflag86", "[ON/OFF] holyflag86?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit86].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF87, "holyflag87", "[ON/OFF] holyflag87?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit87].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF88, "holyflag88", "[ON/OFF] holyflag88?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit88].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF89, "holyflag89", "[ON/OFF] holyflag89?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit89].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF90, "holyflag90", "[ON/OFF] holyflag90?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit90].Length},1,0]}");
#pragma endregion

#pragma region Paladin/Shadowknight
DECLARE_ABILITY_OPTION(pWITHS, "withstand", "[#] Endu% Above? 0=Off", "${If[${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.CombatAbility[withstand]} || ${Me.CombatAbility[withstand rk. ii]} || ${Me.CombatAbility[withstand rk. iii]} || ${Me.CombatAbility[defy]} || ${Me.CombatAbility[defy rk. ii]} || ${Me.CombatAbility[defy rk. iii]} || ${Me.CombatAbility[Reprove]} || ${Me.CombatAbility[Reprove rk. ii]} || ${Me.CombatAbility[Reprove rk. iii]} || ${Me.CombatAbility[Repel]} || ${Me.CombatAbility[Repel rk. ii]} || ${Me.CombatAbility[Repel rk. iii]} || ${Me.CombatAbility[Spurn]} || ${Me.CombatAbility[Spurn Rk. II]} || ${Me.CombatAbility[Spurn Rk. III]} || ${Me.CombatAbility[Thwart]} || ${Me.CombatAbility[Thwart Rk. II]} || ${Me.CombatAbility[Thwart Rk. III]}),20,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.CombatAbility[withstand]} || ${Me.CombatAbility[withstand rk. ii]} || ${Me.CombatAbility[withstand rk. iii]} || ${Me.CombatAbility[defy]} || ${Me.CombatAbility[defy rk. ii]} || ${Me.CombatAbility[defy rk. iii]} || ${Me.CombatAbility[Reprove]} || ${Me.CombatAbility[Reprove rk. ii]} || ${Me.CombatAbility[Reprove rk. iii]} || ${Me.CombatAbility[Repel]} || ${Me.CombatAbility[Repel rk. ii]} || ${Me.CombatAbility[Repel rk. iii]} || ${Me.CombatAbility[Spurn]} || ${Me.CombatAbility[Spurn Rk. II]} || ${Me.CombatAbility[Spurn Rk. III]} || ${Me.CombatAbility[Thwart]} || ${Me.CombatAbility[Thwart Rk. II]} || ${Me.CombatAbility[Thwart Rk. III]} || ${Me.CombatAbility[Repudiate]} || ${Me.CombatAbility[Repudiate Rk. II]} || ${Me.CombatAbility[Repudiate Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCHFOR, "challengefor", "[ON/OFF]?", "${If[${Me.Spell[Challenge for Honor].ID} || ${Me.Spell[trial for honor].ID} || ${Me.Spell[charge for honor].ID} || ${Me.Spell[challenge for power].ID} || ${Me.Spell[trial for power].ID} || ${Me.Spell[charge for honor].ID} || ${Me.Spell[confrontation for power].ID} || ${Me.Spell[confrontation for honor].ID} || ${Me.Spell[Provocation for honor].ID} || ${Me.Spell[Provocation for power].ID} || ${Me.Spell[Demand for Power].ID} || ${Me.Spell[Demand for Honor].ID} || ${Me.Spell[Impose for Power].ID} || ${Me.Spell[Impose for Honor].ID} || ${Me.Spell[Refute for Power].ID} || ${Me.Spell[Refute for Honor].ID} || ${Me.Spell[Protest for Power].ID} || ${Me.Spell[Protest for Honor].ID} || ${Me.Spell[Parlay for Power].ID} || ${Me.Spell[Parlay for Honor].ID},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && (${Me.Spell[challenge for honor].ID} || ${Me.Spell[trial for honor].ID} || ${Me.Spell[charge for honor].ID} || ${Me.Spell[challenge for power].ID} || ${Me.Spell[trial for power].ID} || ${Me.Spell[charge for honor].ID} || ${Me.Spell[confrontation for power].ID} || ${Me.Spell[confrontation for honor].ID} || ${Me.Spell[Provocation for honor].ID} || ${Me.Spell[Provocation for power].ID} || ${Me.Spell[Demand for Power].ID} || ${Me.Spell[Demand for Honor].ID} || ${Me.Spell[Impose for Power].ID} || ${Me.Spell[Impose for Honor].ID} || ${Me.Spell[Refute for Power].ID} || ${Me.Spell[Refute for Honor].ID} || ${Me.Spell[Protest for Power].ID} || ${Me.Spell[Protest for Power].ID}),1,0]}");
DECLARE_ABILITY_OPTION(pSTEEL, "steely", "[ON/OFF]", "${If[${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.Spell[Steely Stance].ID} || ${Me.Spell[Stubborn Stance].ID} || ${Me.Spell[Stoic Stance].ID} || ${Me.Spell[Steadfast Stance].ID} || ${Me.Spell[Staunch Stance].ID} || ${Me.Spell[Defiant Stance].ID} || ${Me.Spell[Stormwall Stance].ID}),0,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.Spell[Steely Stance].ID} || ${Me.Spell[Stubborn Stance].ID} || ${Me.Spell[Stoic Stance].ID} || ${Me.Spell[Steadfast Stance].ID} || ${Me.Spell[Staunch Stance].ID} || ${Me.Spell[Defiant Stance].ID} || ${Me.Spell[Stormwall Stance].ID} || ${Me.Spell[Adamant Stance].ID} || ${Me.Spell[Adamant Stance Rk. II].ID} || ${Me.Spell[Adamant Stance Rk. III].ID}),1,0]}");
#pragma endregion Paladin/Shadowknight Updated for ToV

#pragma region Warrior - Class 1
DECLARE_ABILITY_OPTION(pCALLC, "callchallenge", "[ON/OFF]?", "${If[${Me.AltAbility[call of challenge]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[call of challenge]},1,0]}");
DECLARE_ABILITY_OPTION(pCOMMG, "commanding", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[commanding voice]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[commanding voice]},1,0]}");
DECLARE_ABILITY_OPTION(pDEFEN, "defense", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bracing Defense]} || ${Me.CombatAbility[Bracing Defense Rk. II]} || ${Me.CombatAbility[Bracing Defense Rk. III]} || ${Me.CombatAbility[Staunch Defense]} || ${Me.CombatAbility[Staunch Defense Rk. II]} || ${Me.CombatAbility[Staunch Defense Rk. III]} || ${Me.CombatAbility[Stalwart Defense]} || ${Me.CombatAbility[Stalwart Defense Rk. II]} || ${Me.CombatAbility[Stalwart Defense Rk. III]} || ${Me.CombatAbility[Steadfast Defense]} || ${Me.CombatAbility[Steadfast Defense Rk. II]} || ${Me.CombatAbility[Steadfast Defense Rk. III]} || ${Me.CombatAbility[Stout Defense]} || ${Me.CombatAbility[Stout Defense Rk. II]} || ${Me.CombatAbility[Stout Defense Rk. III]} || ${Me.CombatAbility[Resolute Defense]} || ${Me.CombatAbility[Resolute Defense Rk. II]} || ${Me.CombatAbility[Resolute Defense Rk. III]} || ${Me.CombatAbility[Courageous Defense]} || ${Me.CombatAbility[Courageous Defense Rk. II]} || ${Me.CombatAbility[Courageous Defense Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bracing Defense]} || ${Me.CombatAbility[Bracing Defense Rk. II]} || ${Me.CombatAbility[Bracing Defense Rk. III]} || ${Me.CombatAbility[Staunch Defense]} || ${Me.CombatAbility[Staunch Defense Rk. II]} || ${Me.CombatAbility[Staunch Defense Rk. III]} || ${Me.CombatAbility[Stalwart Defense]} || ${Me.CombatAbility[Stalwart Defense Rk. II]} || ${Me.CombatAbility[Stalwart Defense Rk. III]} || ${Me.CombatAbility[Steadfast Defense]} || ${Me.CombatAbility[Steadfast Defense Rk. II]} || ${Me.CombatAbility[Steadfast Defense Rk. III]} || ${Me.CombatAbility[Stout Defense]} || ${Me.CombatAbility[Stout Defense Rk. II]} || ${Me.CombatAbility[Stout Defense Rk. III]} || ${Me.CombatAbility[Resolute Defense]} || ${Me.CombatAbility[Resolute Defense Rk. II]} || ${Me.CombatAbility[Resolute Defense Rk. III]} || ${Me.CombatAbility[Courageous Defense]} || ${Me.CombatAbility[Courageous Defense Rk. II]} || ${Me.CombatAbility[Courageous Defense Rk. III]} || ${Me.CombatAbility[Primal Defense]}|| ${Me.CombatAbility[Primal Defense Rk. II]} || ${Me.CombatAbility[Primal Defense Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pFIELD, "fieldarm", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Field Armorer]} || ${Me.CombatAbility[Field Armorer Rk. II]} || ${Me.CombatAbility[Field Armorer Rk. III]} || ${Me.CombatAbility[Field Outfitter]} || ${Me.CombatAbility[Field Outfitter Rk. II]} || ${Me.CombatAbility[Field Outfitter Rk. III]} || ${Me.CombatAbility[Field Defender]} || ${Me.CombatAbility[Field Defender Rk. II]} || ${Me.CombatAbility[Field Defender Rk. III]} || ${Me.CombatAbility[Field Guardian]} || ${Me.CombatAbility[Field Guardian Rk. II]} || ${Me.CombatAbility[Field Guardian Rk. III]} || ${Me.CombatAbility[Field Protector]} || ${Me.CombatAbility[Field Protector Rk. II]} || ${Me.CombatAbility[Field Protector Rk. III]} || ${Me.CombatAbility[Field Champion]} || ${Me.CombatAbility[Field Champion Rk. II]} || ${Me.CombatAbility[Field Champion Rk. III]} || ${Me.CombatAbility[Paragon Champion]} || ${Me.CombatAbility[Paragon Champion Rk. II]} || ${Me.CombatAbility[Paragon Champion Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Field Armorer]} || ${Me.CombatAbility[Field Armorer Rk. II]} || ${Me.CombatAbility[Field Armorer Rk. III]} || ${Me.CombatAbility[Field Outfitter]} || ${Me.CombatAbility[Field Outfitter Rk. II]} || ${Me.CombatAbility[Field Outfitter Rk. III]} || ${Me.CombatAbility[Field Defender]} || ${Me.CombatAbility[Field Defender Rk. II]} || ${Me.CombatAbility[Field Defender Rk. III]} || ${Me.CombatAbility[Field Guardian]} || ${Me.CombatAbility[Field Guardian Rk. II]} || ${Me.CombatAbility[Field Guardian Rk. III]} || ${Me.CombatAbility[Field Protector]} || ${Me.CombatAbility[Field Protector Rk. II]} || ${Me.CombatAbility[Field Protector Rk. III]} || ${Me.CombatAbility[Field Champion]} || ${Me.CombatAbility[Field Champion Rk. II]} || ${Me.CombatAbility[Field Champion Rk. III]} || ${Me.CombatAbility[Paragon Champion]} || ${Me.CombatAbility[Paragon Champion Rk. II]} || ${Me.CombatAbility[Full Moon's Champion]} || ${Me.CombatAbility[Full Moon's Champion Rk. II]} || ${Me.CombatAbility[Full Moon's Champion Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pGTPUN, "gutpunch", "[ON/OFF]?", "${If[${Me.AltAbility[gut punch]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[gut punch]},1,0]}");
DECLARE_ABILITY_OPTION(pKNEES, "kneestrike", "[ON/OFF]?", "${If[${Me.AltAbility[knee strike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[knee strike]},1,0]}");
//Where is Prowar (provoke)
DECLARE_ABILITY_OPTION(pOSTRK, "opportunisticstrike", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Opportunistic Strike]} || ${Me.CombatAbility[Opportunistic Strike Rk. II]} || ${Me.CombatAbility[Opportunistic Strike Rk. III]} || ${Me.CombatAbility[Strategic Strike]} || ${Me.CombatAbility[Strategic Strike Rk. II]} || ${Me.CombatAbility[Strategic Strike Rk. III]} || ${Me.CombatAbility[Vital Strike]} || ${Me.CombatAbility[Vital Strike Rk. II]} || ${Me.CombatAbility[Vital Strike Rk. III]} || ${Me.CombatAbility[Calculated Strike]} || ${Me.CombatAbility[Calculated Strike Rk. II]} || ${Me.CombatAbility[Calculated Strike Rk. III]} || ${Me.CombatAbility[Cunning Strike]} || ${Me.CombatAbility[Cunning Strike Rk. II]} || ${Me.CombatAbility[Cunning Strike Rk. III]} || ${Me.CombatAbility[Precision Strike]} || ${Me.CombatAbility[Precision Strike Rk. II]} || ${Me.CombatAbility[Precision Strike Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Opportunistic Strike]} || ${Me.CombatAbility[Opportunistic Strike Rk. II]} || ${Me.CombatAbility[Opportunistic Strike Rk. III]} || ${Me.CombatAbility[Strategic Strike]} || ${Me.CombatAbility[Strategic Strike Rk. II]} || ${Me.CombatAbility[Strategic Strike Rk. III]} || ${Me.CombatAbility[Vital Strike]} || ${Me.CombatAbility[Vital Strike Rk. II]} || ${Me.CombatAbility[Vital Strike Rk. III]} || ${Me.CombatAbility[Calculated Strike]} || ${Me.CombatAbility[Calculated Strike Rk. II]} || ${Me.CombatAbility[Calculated Strike Rk. III]} || ${Me.CombatAbility[Cunning Strike]} || ${Me.CombatAbility[Cunning Strike Rk. II]} || ${Me.CombatAbility[Cunning Strike Rk. III]} || ${Me.CombatAbility[Precision Strike]} || ${Me.CombatAbility[Precision Strike Rk. II]} || ${Me.CombatAbility[Precision Strike Rk. III]}  || ${Me.CombatAbility[Exploitive Strike]} || ${Me.CombatAbility[Exploitive Strike Rk. II]} || ${Me.CombatAbility[Exploitive Strike Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pTTJAB, "throatjab", "[ON/OFF]?", "${If[${Me.AltAbility[throat jab]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[throat jab]},1,0]}");

#pragma endregion

#pragma region Cleric - Class 2
DECLARE_ABILITY_OPTION(pYAULP, "yaulp", "[ON/OFF]?", "${If[${Me.AltAbility[yaulp]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[yaulp]},1,0]}");
#pragma endregion

#pragma region Paladin - Class 3
//Challenge for power/honor is listed for both paladin/shadowknight
DECLARE_ABILITY_OPTION(pLHAND, "layhand", "[#] MyLife% Below? 0=0ff", "${If[${Me.AltAbility[Lay on Hands]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Lay on Hands]} && ${Me.Class.ShortName.Equal[PAL]},1,0]}");
DECLARE_ABILITY_OPTION(pRGHTI, "rightidg", "[#] Endu% Above? 0=Off", "0", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Righteous Indignation]} || ${Me.CombatAbility[Righteous Indignation rk. ii]} || ${Me.CombatAbility[Righteous Indignation rk. iii]} || ${Me.CombatAbility[Righteous Vexation]} || ${Me.CombatAbility[Righteous Vexation rk. ii]} || ${Me.CombatAbility[Righteous Vexation rk. iii]} || ${Me.CombatAbility[Righteous Umbrage]} || ${Me.CombatAbility[Righteous Umbrage rk. ii]} || ${Me.CombatAbility[Righteous Umbrage rk. iii]} || ${Me.CombatAbility[Righteous Condemnation]} || ${Me.CombatAbility[Righteous Condemnation Rk. II]} || ${Me.CombatAbility[Righteous Condemnation Rk. III]} || ${Me.CombatAbility[Righteous Antipathy]} || ${Me.CombatAbility[Righteous Antipathy Rk. II]} || ${Me.CombatAbility[Righteous Antipathy Rk. III]} || ${Me.CombatAbility[Righteous Censure]} || ${Me.CombatAbility[Righteous Censure Rk. II]} || ${Me.CombatAbility[Righteous Censure Rk. III]}),1,0]}");

//Stunaa missing?
//stunpal missing?

#pragma endregion

#pragma region Ranger - Class 4
DECLARE_ABILITY_OPTION(pSBLADES, "stormblades", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[focused storm of blades]} || ${Me.CombatAbility[focused storm of blades rk. ii]} || ${Me.CombatAbility[focused storm of blades rk. iii]} || ${Me.CombatAbility[Squall of Blades]} || ${Me.CombatAbility[Squall of Blades Rk. II]} || ${Me.CombatAbility[Squall of Blades Rk. III]} || ${Me.CombatAbility[Focused Squall of Blades]} || ${Me.CombatAbility[Focused Squall of Blades Rk. II]} || ${Me.CombatAbility[Focused Squall of Blades Rk. III]} || ${Me.CombatAbility[Gale of Blades]} || ${Me.CombatAbility[Gale of Blades Rk. II]} || ${Me.CombatAbility[Gale of Blades Rk. III]} || ${Me.CombatAbility[Focused Gale of Blades]} || ${Me.CombatAbility[Focused Gale of Blades Rk. II]} || ${Me.CombatAbility[Focused Gale of Blades Rk. III]} || ${Me.CombatAbility[Blizzard of Blades]} || ${Me.CombatAbility[Blizzard of Blades Rk. II]} || ${Me.CombatAbility[Blizzard of Blades Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[focused storm of blades]} || ${Me.CombatAbility[focused storm of blades rk. ii]} || ${Me.CombatAbility[focused storm of blades rk. iii]} || ${Me.CombatAbility[Squall of Blades]} || ${Me.CombatAbility[Squall of Blades Rk. II]} || ${Me.CombatAbility[Squall of Blades Rk. III]} || ${Me.CombatAbility[Focused Squall of Blades]} || ${Me.CombatAbility[Focused Squall of Blades Rk. II]} || ${Me.CombatAbility[Focused Squall of Blades Rk. III]} || ${Me.CombatAbility[Gale of Blades]} || ${Me.CombatAbility[Gale of Blades Rk. II]} || ${Me.CombatAbility[Gale of Blades Rk. III]} || ${Me.CombatAbility[Focused Gale of Blades]} || ${Me.CombatAbility[Focused Gale of Blades Rk. II]} || ${Me.CombatAbility[Focused Gale of Blades Rk. III]} || ${Me.CombatAbility[Blizzard of Blades]} || ${Me.CombatAbility[Blizzard of Blades Rk. II]} || ${Me.CombatAbility[Blizzard of Blades Rk. III]}  || ${Me.CombatAbility[Tempest of Blades]} || ${Me.CombatAbility[Tempest of Blades Rk. II]} || ${Me.CombatAbility[Tempest of Blades Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pERKCK, "enragingkick", "[#] Life% Below? 0=0ff", "${If[${Me.CombatAbility[Enraging Crescent Kicks]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. II]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. III]} || ${Me.CombatAbility[Enraging Heel Kicks]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. II]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. III]} || ${Me.CombatAbility[Enraging Cut Kicks]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. II]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. III]} || ${Me.CombatAbility[Enraging Axe Kicks]} || ${Me.CombatAbility[Enraging Axe Kicks Rk. II]} || ${Me.CombatAbility[Enraging Axe Kicks Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && (${Me.CombatAbility[Enraging Crescent Kicks]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. II]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. III]} || ${Me.CombatAbility[Enraging Heel Kicks]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. II]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. III]} || ${Me.CombatAbility[Enraging Cut Kicks]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. II]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. III]} || ${Me.CombatAbility[Enraging Axe Kicks]} || ${Me.CombatAbility[Enraging Axe Kicks Rk. II]} || ${Me.CombatAbility[Enraging Axe Kicks Rk. III]} || ${Me.CombatAbility[Enraging Roundhouse Kicks]} || ${Me.CombatAbility[Enraging Roundhouse Kicks Rk. II]} || ${Me.CombatAbility[Enraging Roundhouse Kicks Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pJKICK, "jltkicks", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[jolting kicks]} || ${Me.CombatAbility[jolting kicks rk. ii]} || ${Me.CombatAbility[jolting kicks rk. iii]} || ${Me.CombatAbility[Jolting Snapkicks]} || ${Me.CombatAbility[Jolting Snapkicks rk. ii]} || ${Me.CombatAbility[Jolting Snapkicks rk. iii]} || ${Me.CombatAbility[Jolting Frontkicks]} || ${Me.CombatAbility[Jolting Frontkicks rk. ii]} || ${Me.CombatAbility[Jolting Frontkicks rk. iii]} || ${Me.CombatAbility[Jolting Hook kicks]} || ${Me.CombatAbility[Jolting Hook kicks rk. ii]} || ${Me.CombatAbility[Jolting Hook kicks rk. iii]} || ${Me.CombatAbility[Jolting Crescent kicks]} || ${Me.CombatAbility[Jolting Crescent kicks rk. ii]} || ${Me.CombatAbility[Jolting Crescent kicks rk. iii]} || ${Me.CombatAbility[Jolting Heel Kicks]} || ${Me.CombatAbility[Jolting Heel Kicks rk. ii]} || ${Me.CombatAbility[Jolting Heel Kicks rk. iii]} || ${Me.CombatAbility[Jolting Cut Kicks]} || ${Me.CombatAbility[Jolting Cut Kicks rk. ii]} || ${Me.CombatAbility[Jolting Cut Kicks rk. iii]} || ${Me.CombatAbility[Jolting Wheel Kicks]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. II]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. III]} || ${Me.CombatAbility[Jolting Axe Kicks]} || ${Me.CombatAbility[Jolting Axe Kicks Rk. II]} || ${Me.CombatAbility[Jolting Axe Kicks Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[jolting kicks]} || ${Me.CombatAbility[jolting kicks rk. ii]} || ${Me.CombatAbility[jolting kicks rk. iii]} || ${Me.CombatAbility[Jolting Snapkicks]} || ${Me.CombatAbility[Jolting Snapkicks rk. ii]} || ${Me.CombatAbility[Jolting Snapkicks rk. iii]} || ${Me.CombatAbility[Jolting Frontkicks]} || ${Me.CombatAbility[Jolting Frontkicks rk. ii]} || ${Me.CombatAbility[Jolting Frontkicks rk. iii]} || ${Me.CombatAbility[Jolting Hook kicks]} || ${Me.CombatAbility[Jolting Hook kicks rk. ii]} || ${Me.CombatAbility[Jolting Hook kicks rk. iii]} || ${Me.CombatAbility[Jolting Crescent kicks]} || ${Me.CombatAbility[Jolting Crescent kicks rk. ii]} || ${Me.CombatAbility[Jolting Crescent kicks rk. iii]} || ${Me.CombatAbility[Jolting Heel Kicks]} || ${Me.CombatAbility[Jolting Heel Kicks rk. ii]} || ${Me.CombatAbility[Jolting Heel Kicks rk. iii]} || ${Me.CombatAbility[Jolting Cut Kicks]} || ${Me.CombatAbility[Jolting Cut Kicks rk. ii]} || ${Me.CombatAbility[Jolting Cut Kicks rk. iii]} || ${Me.CombatAbility[Jolting Wheel Kicks]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. II]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. III]} || ${Me.CombatAbility[Jolting Axe Kicks]} || ${Me.CombatAbility[Jolting Axe Kicks Rk. II]} || ${Me.CombatAbility[Jolting Axe Kicks Rk. III]} || ${Me.CombatAbility[Jolting Roundhouse Kicks]} || ${Me.CombatAbility[Jolting Roundhouse Kicks Rk. II]} || ${Me.CombatAbility[Jolting Roundhouse Kicks Rk. III]}),1,0]}");

#pragma endregion

#pragma region Shadow Knight - Class 5
DECLARE_ABILITY_OPTION(pGBLDE, "gblade", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Gouging Blade]} || ${Me.CombatAbility[Gouging Blade Rk. II]} || ${Me.CombatAbility[Gouging Blade Rk. III]} || ${Me.CombatAbility[Gashing Blade]} || ${Me.CombatAbility[Gashing Blade Rk. II]} || ${Me.CombatAbility[Gashing Blade Rk. III]} || ${Me.CombatAbility[Lacerating Blade]} || ${Me.CombatAbility[Lacerating Blade Rk. II]} || ${Me.CombatAbility[Lacerating Blade Rk. III]} || ${Me.CombatAbility[Wounding Blade]} || ${Me.CombatAbility[Wounding Blade Rk. II]} || ${Me.CombatAbility[Wounding Blade Rk. III]} || ${Me.CombatAbility[Rending Blade]} || ${Me.CombatAbility[Rending Blade Rk. II]} || ${Me.CombatAbility[Rending Blade Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Gouging Blade]} || ${Me.CombatAbility[Gouging Blade Rk. II]} || ${Me.CombatAbility[Gouging Blade Rk. III]} || ${Me.CombatAbility[Gashing Blade]} || ${Me.CombatAbility[Gashing Blade Rk. II]} || ${Me.CombatAbility[Gashing Blade Rk. III]} || ${Me.CombatAbility[Lacerating Blade]} || ${Me.CombatAbility[Lacerating Blade Rk. II]} || ${Me.CombatAbility[Lacerating Blade Rk. III]} || ${Me.CombatAbility[Wounding Blade]} || ${Me.CombatAbility[Wounding Blade Rk. II]} || ${Me.CombatAbility[Wounding Blade Rk. III]} || ${Me.CombatAbility[Rending Blade]} || ${Me.CombatAbility[Rending Blade Rk. II]} || ${Me.CombatAbility[Rending Blade Rk. III]} || ${Me.CombatAbility[Grisly Blade]} || ${Me.CombatAbility[Grisly Blade Rk. II]} || ${Me.CombatAbility[Grisly Blade Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pHARMT, "harmtouch", "[ON/OFF]?", "${If[${Me.AltAbility[harm touch].ID},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[harm touch]} && ${Me.Class.ShortName.Equal[SHD]},1,0]}");
//Challenge for power/honor is listed for both paladin/shadowknight
//Where is Terror of line?
#pragma endregion

#pragma region Druid - Class 6
#pragma endregion - Empty

#pragma region Monk - Class 7
DECLARE_ABILITY_OPTION(pCFIST, "cloud", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Cloud of Fists]} || ${Me.CombatAbility[Cloud of Fists Rk. II]} || ${Me.CombatAbility[Cloud of Fists Rk. III]} || ${Me.CombatAbility[Phantom Partisan]} || ${Me.CombatAbility[Phantom Partisan Rk. II]} || ${Me.CombatAbility[Phantom Partisan Rk. III]}|| ${Me.CombatAbility[Phantom Pummeling]} || ${Me.CombatAbility[Phantom Pummeling Rk. II]} || ${Me.CombatAbility[Phantom Pummeling Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Cloud of Fists]} || ${Me.CombatAbility[Cloud of Fists Rk. II]} || ${Me.CombatAbility[Cloud of Fists Rk. III]} || ${Me.CombatAbility[Phantom Partisan]} || ${Me.CombatAbility[Phantom Partisan Rk. II]} || ${Me.CombatAbility[Phantom Partisan Rk. III]}|| ${Me.CombatAbility[Phantom Pummeling]} || ${Me.CombatAbility[Phantom Pummeling Rk. II]} || ${Me.CombatAbility[Phantom Pummeling Rk. III]} || ${Me.CombatAbility[Phantom Fisticuffs]} || ${Me.CombatAbility[Phantom Fisticuffs Rk. II]} || ${Me.CombatAbility[Phantom Fisticuffs Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pFALLS, "falls", "[ON/OFF] Auto-Feign?", "0", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.Class.ShortName.Equal[MNK]},1,0]}");
DECLARE_ABILITY_OPTION(pFISTS, "fistsofwu", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[fists of wu]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[fists of wu]},1,0]}");
DECLARE_ABILITY_OPTION(pLCLAW, "leopardclaw", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[leopard claw]} || ${Me.CombatAbility[dragon fang]} || ${Me.CombatAbility[clawstriker's flurry]} || ${Me.CombatAbility[clawstriker's flurry rk. ii]} || ${Me.CombatAbility[clawstriker's flurry rk. iii]} || ${Me.CombatAbility[wheel of fists]} || ${Me.CombatAbility[wheel of fists rk. ii]} || ${Me.CombatAbility[wheel of fists rk. iii]} || ${Me.CombatAbility[Six-Step Pattern]} || ${Me.CombatAbility[Six-Step Pattern rk. ii]} || ${Me.CombatAbility[Six-Step Pattern rk. iii]} || ${Me.CombatAbility[Seven-Step Pattern]} || ${Me.CombatAbility[Seven-Step Pattern rk. ii]} || ${Me.CombatAbility[Seven-Step Pattern rk. iii]} || ${Me.CombatAbility[Eight-Step Pattern]} || ${Me.CombatAbility[Eight-Step Pattern Rk. II]} || ${Me.CombatAbility[Eight-Step Pattern Rk. III]} || ${Me.CombatAbility[Torrent of Fists]} || ${Me.CombatAbility[Torrent of Fists Rk. II]} || ${Me.CombatAbility[Torrent of Fists Rk. III]} || ${Me.CombatAbility[Firestorm of Fists]} || ${Me.CombatAbility[Firestorm of Fists Rk. II]} || ${Me.CombatAbility[Firestorm of Fists Rk. III]} || ${Me.CombatAbility[Barrage of Fists]} || ${Me.CombatAbility[Barrage of Fists Rk. II]} || ${Me.CombatAbility[Barrage of Fists Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[leopard claw]} || ${Me.CombatAbility[dragon fang]} || ${Me.CombatAbility[clawstriker's flurry]} || ${Me.CombatAbility[clawstriker's flurry rk. ii]} || ${Me.CombatAbility[clawstriker's flurry rk. iii]} || ${Me.CombatAbility[wheel of fists]} || ${Me.CombatAbility[wheel of fists rk. ii]} || ${Me.CombatAbility[wheel of fists rk. iii]} || ${Me.CombatAbility[Six-Step Pattern]} || ${Me.CombatAbility[Six-Step Pattern rk. ii]} || ${Me.CombatAbility[Six-Step Pattern rk. iii]} || ${Me.CombatAbility[Seven-Step Pattern]} || ${Me.CombatAbility[Seven-Step Pattern rk. ii]} || ${Me.CombatAbility[Seven-Step Pattern rk. iii]} || ${Me.CombatAbility[Eight-Step Pattern]} || ${Me.CombatAbility[Eight-Step Pattern Rk. II]} || ${Me.CombatAbility[Eight-Step Pattern Rk. III]} || ${Me.CombatAbility[Torrent of Fists]} || ${Me.CombatAbility[Torrent of Fists Rk. II]} || ${Me.CombatAbility[Torrent of Fists Rk. III]} || ${Me.CombatAbility[Firestorm of Fists]} || ${Me.CombatAbility[Firestorm of Fists Rk. II]} || ${Me.CombatAbility[Firestorm of Fists Rk. III]} || ${Me.CombatAbility[Barrage of Fists]} || ${Me.CombatAbility[Barrage of Fists Rk. II]} || ${Me.CombatAbility[Barrage of Fists Rk. III]} || ${Me.CombatAbility[Buffeting of Fists]} || ${Me.CombatAbility[Buffeting of Fists Rk. II]} || ${Me.CombatAbility[Buffeting of Fists Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pMENDS, "mend", "[#] MyLife% Below? 0=0ff", "${If[${Me.Skill[mend]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.Skill[mend]},1,0]}");
DECLARE_ABILITY_OPTION(pDMONK, "monkey", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Drunken Monkey Style]} || ${Me.CombatAbility[Drunken Monkey Style rk. ii]} || ${Me.CombatAbility[Drunken Monkey Style rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Drunken Monkey Style]} || ${Me.CombatAbility[Drunken Monkey Style rk. ii]} || ${Me.CombatAbility[Drunken Monkey Style rk. iii]}),1,0]}");
//stunmnk missing?
DECLARE_ABILITY_OPTION(pSYNGY, "synergy", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Calanin's Synergy]} || ${Me.CombatAbility[Calanin's Synergy Rk. II]} || ${Me.CombatAbility[Calanin's Synergy Rk. III]} || ${Me.CombatAbility[Dreamwalker's Synergy]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. II]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. III]} || ${Me.CombatAbility[Veilwalker's Synergy]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. II]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. III]} || ${Me.CombatAbility[Shadewalker's Synergy]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. II]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. III]} || ${Me.CombatAbility[Doomwalker's Synergy]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. II]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. III]} || ${Me.CombatAbility[Firewalker's Synergy]} || ${Me.CombatAbility[Firewalker's Synergy Rk. II]} || ${Me.CombatAbility[Firewalker's Synergy Rk. III]} || ${Me.CombatAbility[Icewalker's Synergy]} || ${Me.CombatAbility[Icewalker's Synergy Rk. II]} || ${Me.CombatAbility[Icewalker's Synergy Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Calanin's Synergy]} || ${Me.CombatAbility[Calanin's Synergy Rk. II]} || ${Me.CombatAbility[Calanin's Synergy Rk. III]} || ${Me.CombatAbility[Dreamwalker's Synergy]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. II]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. III]} || ${Me.CombatAbility[Veilwalker's Synergy]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. II]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. III]} || ${Me.CombatAbility[Shadewalker's Synergy]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. II]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. III]} || ${Me.CombatAbility[Doomwalker's Synergy]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. II]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. III]} || ${Me.CombatAbility[Firewalker's Synergy]} || ${Me.CombatAbility[Firewalker's Synergy Rk. II]} || ${Me.CombatAbility[Firewalker's Synergy Rk. III]} || ${Me.CombatAbility[Icewalker's Synergy]} || ${Me.CombatAbility[Icewalker's Synergy Rk. II]} || ${Me.CombatAbility[Icewalker's Synergy Rk. III]} || ${Me.CombatAbility[Bloodwalker's Synergy]} || ${Me.CombatAbility[Bloodwalker's Synergy Rk. II]} || ${Me.CombatAbility[Bloodwalker's Synergy Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pVIGSN, "vigshuriken", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Shuriken]} || ${Me.CombatAbility[Vigorous Shuriken Rk. II]} || ${Me.CombatAbility[Vigorous Shuriken Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Shuriken]} || ${Me.CombatAbility[Vigorous Shuriken Rk. II]} || ${Me.CombatAbility[Vigorous Shuriken Rk. III]}),1,0]}");

#pragma endregion

#pragma region Bard - Class 8
DECLARE_ABILITY_OPTION(pBBLOW, "boastful", "[ON/OFF]?", "${If[${Me.AltAbility[boastful bellow]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[boastful bellow]},1,0]}");
DECLARE_ABILITY_OPTION(pSELOK, "selos", "[ON/OFF]?", "${If[${Me.AltAbility[selo's kick]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[selo's kick]},1,0]}");

#pragma endregion

#pragma region Rogue - Class 9
DECLARE_ABILITY_OPTION(pASSAS, "assassinate", "Sneak/Hide/Behind/Strike/Stab [ON/OFF]?", "${If[${Me.Skill[backstab]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[backstab]},1,0]}");
DECLARE_ABILITY_OPTION(pASSLT, "assault", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Assault]} || ${Me.CombatAbility[Assault Rk. II]} || ${Me.CombatAbility[Assault Rk. III]} || ${Me.CombatAbility[Battery]} || ${Me.CombatAbility[Battery Rk. II]} || ${Me.CombatAbility[Battery Rk. III]} || ${Me.CombatAbility[Onslaught]} || ${Me.CombatAbility[Onslaught Rk. II]} || ${Me.CombatAbility[Onslaught Rk. III]} || ${Me.CombatAbility[Incursion]} || ${Me.CombatAbility[Incursion Rk. II]} || ${Me.CombatAbility[Incursion Rk. III]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage Rk. II]} || ${Me.CombatAbility[Barrage Rk. III]} || ${Me.CombatAbility[Fellstrike]} || ${Me.CombatAbility[Fellstrike Rk. II]} || ${Me.CombatAbility[Fellstrike Rk. III]} || ${Me.CombatAbility[Blitzstrike]} || ${Me.CombatAbility[Blitzstrike Rk. II]} || ${Me.CombatAbility[Blitzstrike Rk. III]},60,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Assault]} || ${Me.CombatAbility[Assault Rk. II]} || ${Me.CombatAbility[Assault Rk. III]} || ${Me.CombatAbility[Battery]} || ${Me.CombatAbility[Battery Rk. II]} || ${Me.CombatAbility[Battery Rk. III]} || ${Me.CombatAbility[Onslaught]} || ${Me.CombatAbility[Onslaught Rk. II]} || ${Me.CombatAbility[Onslaught Rk. III]} || ${Me.CombatAbility[Incursion]} || ${Me.CombatAbility[Incursion Rk. II]} || ${Me.CombatAbility[Incursion Rk. III]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage Rk. II]} || ${Me.CombatAbility[Barrage Rk. III]} || ${Me.CombatAbility[Fellstrike]} || ${Me.CombatAbility[Fellstrike Rk. II]} || ${Me.CombatAbility[Fellstrike Rk. III]} || ${Me.CombatAbility[Blitzstrike]} || ${Me.CombatAbility[Blitzstrike Rk. II]} || ${Me.CombatAbility[Blitzstrike Rk. III]} || ${Me.CombatAbility[Shadowstrike]} || ${Me.CombatAbility[Shadowstrike Rk. II]} || ${Me.CombatAbility[Shadowstrike Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pBSTAB, "backstab", "[ON/OFF]?", "${If[${Me.Skill[backstab]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[backstab]},1,0]}");
DECLARE_ABILITY_OPTION(pBLEED, "bleed", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bleed]} || ${Me.CombatAbility[Bleed Rk. II]} || ${Me.CombatAbility[Bleed Rk. III]} || ${Me.CombatAbility[Wound]} || ${Me.CombatAbility[Wound Rk. II]} || ${Me.CombatAbility[Wound Rk. III]} || ${Me.CombatAbility[Lacerate]} || ${Me.CombatAbility[Lacerate Rk. II]} || ${Me.CombatAbility[Lacerate Rk. III]} || ${Me.CombatAbility[Gash]} || ${Me.CombatAbility[Gash Rk. II]} || ${Me.CombatAbility[Gash Rk. III]} || ${Me.CombatAbility[Hack]} || ${Me.CombatAbility[Hack Rk. II]} || ${Me.CombatAbility[Hack Rk. III]} || ${Me.CombatAbility[Slice]} || ${Me.CombatAbility[Slice Rk. II]} || ${Me.CombatAbility[Slice Rk. III]} || ${Me.CombatAbility[Slash]} || ${Me.CombatAbility[Slash Rk. II]} || ${Me.CombatAbility[Slash Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bleed]} || ${Me.CombatAbility[Bleed Rk. II]} || ${Me.CombatAbility[Bleed Rk. III]}|| ${Me.CombatAbility[Wound]} || ${Me.CombatAbility[Wound Rk. II]} || ${Me.CombatAbility[Wound Rk. III]} || ${Me.CombatAbility[Lacerate]} || ${Me.CombatAbility[Lacerate Rk. II]} || ${Me.CombatAbility[Lacerate Rk. III]} || ${Me.CombatAbility[Gash]} || ${Me.CombatAbility[Gash Rk. II]} || ${Me.CombatAbility[Gash Rk. III]} || ${Me.CombatAbility[Hack]} || ${Me.CombatAbility[Hack Rk. II]} || ${Me.CombatAbility[Hack Rk. III]} || ${Me.CombatAbility[Slice]} || ${Me.CombatAbility[Slice Rk. II]} || ${Me.CombatAbility[Slice Rk. III]} || ${Me.CombatAbility[Slash]} || ${Me.CombatAbility[Slash Rk. II]} || ${Me.CombatAbility[Slash Rk. III]} || ${Me.CombatAbility[Lance]} || ${Me.CombatAbility[Lance Rk. II]} || ${Me.CombatAbility[Lance Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pESCAP, "escape", "[#] Life% Below? 0=0ff", "${If[${Me.AltAbility[escape]},20,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.AltAbility[escape]},1,0]}");
DECLARE_ABILITY_OPTION(pEVADE, "evade", "[#] [ON/OFF]?", "${If[${Me.Skill[hide]} && ${Me.Class.ShortName.Equal[ROG]},1,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.Skill[hide]} && ${Me.Class.ShortName.Equal[ROG]},1,0]}");
DECLARE_ABILITY_OPTION(pJUGUL, "jugular", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Jugular Slash]} || ${Me.CombatAbility[Jugular Slash rk. ii]} || ${Me.CombatAbility[Jugular Slash rk. iii]} || ${Me.CombatAbility[Jugular Slice]} || ${Me.CombatAbility[Jugular Slice rk. ii]} || ${Me.CombatAbility[Jugular Slice rk. iii]} || ${Me.CombatAbility[Jugular Sever]} || ${Me.CombatAbility[Jugular Sever rk. ii]} || ${Me.CombatAbility[Jugular Sever rk. iii]} || ${Me.CombatAbility[Jugular Gash]} || ${Me.CombatAbility[Jugular Gash rk. ii]} || ${Me.CombatAbility[Jugular Gash rk. iii]} || ${Me.CombatAbility[Jugular Lacerate]} || ${Me.CombatAbility[Jugular Lacerate rk. ii]} || ${Me.CombatAbility[Jugular Lacerate rk. iii]} || ${Me.CombatAbility[Jugular Hack]} || ${Me.CombatAbility[Jugular Hack Rk. II]} || ${Me.CombatAbility[Jugular Hack Rk. III]} || ${Me.CombatAbility[Jugular Strike]} || ${Me.CombatAbility[Jugular Strike Rk. II]} || ${Me.CombatAbility[Jugular Strike Rk. III]} || ${Me.CombatAbility[Jugular Cut]} || ${Me.CombatAbility[Jugular Cut Rk. II]} || ${Me.CombatAbility[Jugular Cut Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Jugular Slash]} || ${Me.CombatAbility[Jugular Slash rk. ii]} || ${Me.CombatAbility[Jugular Slash rk. iii]} || ${Me.CombatAbility[Jugular Slice]} || ${Me.CombatAbility[Jugular Slice rk. ii]} || ${Me.CombatAbility[Jugular Slice rk. iii]} || ${Me.CombatAbility[Jugular Sever]} || ${Me.CombatAbility[Jugular Sever rk. ii]} || ${Me.CombatAbility[Jugular Sever rk. iii]} || ${Me.CombatAbility[Jugular Gash]} || ${Me.CombatAbility[Jugular Gash rk. ii]} || ${Me.CombatAbility[Jugular Gash rk. iii]} || ${Me.CombatAbility[Jugular Lacerate]} || ${Me.CombatAbility[Jugular Lacerate rk. ii]} || ${Me.CombatAbility[Jugular Lacerate rk. iii]} || ${Me.CombatAbility[Jugular Hack]} || ${Me.CombatAbility[Jugular Hack Rk. II]} || ${Me.CombatAbility[Jugular Hack Rk. III]} || ${Me.CombatAbility[Jugular Strike]} || ${Me.CombatAbility[Jugular Strike Rk. II]} || ${Me.CombatAbility[Jugular Strike Rk. III]} || ${Me.CombatAbility[Jugular Cut]} || ${Me.CombatAbility[Jugular Cut Rk. II]} || ${Me.CombatAbility[Jugular Cut Rk. III]} || ${Me.CombatAbility[Jugular Rend]} || ${Me.CombatAbility[Jugular Rend Rk. II]} || ${Me.CombatAbility[Jugular Rend Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pKNFPL, "knifeplay", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Knifeplay]} || ${Me.CombatAbility[Knifeplay Rk. II]} || ${Me.CombatAbility[Knifeplay Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Knifeplay]} || ${Me.CombatAbility[Knifeplay Rk. II]} || ${Me.CombatAbility[Knifeplay Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pPICKP, "pickpocket", "[ON/OFF]?", "${If[${Me.Skill[pick pockets]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[pick pockets]},1,0]}");
DECLARE_ABILITY_OPTION(pPINPT, "pinpoint", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Pinpoint Vulnerability]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. ii]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. iii]} || ${Me.CombatAbility[Pinpoint Weaknesses]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. ii]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. iii]} || ${Me.CombatAbility[Pinpoint Vitals]} || ${Me.CombatAbility[Pinpoint Vitals rk. ii]} || ${Me.CombatAbility[Pinpoint Vitals rk. iii]} || ${Me.CombatAbility[Pinpoint Flaws]} || ${Me.CombatAbility[Pinpoint Flaws rk. ii]} || ${Me.CombatAbility[Pinpoint Flaws rk. iii]} || ${Me.CombatAbility[Pinpoint Liabilities]} || ${Me.CombatAbility[Pinpoint Liabilities rk. ii]} || ${Me.CombatAbility[Pinpoint Liabilities rk. iii]} || ${Me.CombatAbility[Pinpoint Deficiencies]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. ii]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. iii]} || ${Me.CombatAbility[Pinpoint Shortcomings]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. II]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. III]} || ${Me.CombatAbility[Pinpoint Defects]} || ${Me.CombatAbility[Pinpoint Defects Rk. II]} || ${Me.CombatAbility[Pinpoint Defects Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Pinpoint Vulnerability]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. ii]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. iii]} || ${Me.CombatAbility[Pinpoint Weaknesses]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. ii]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. iii]} || ${Me.CombatAbility[Pinpoint Vitals]} || ${Me.CombatAbility[Pinpoint Vitals rk. ii]} || ${Me.CombatAbility[Pinpoint Vitals rk. iii]} || ${Me.CombatAbility[Pinpoint Flaws]} || ${Me.CombatAbility[Pinpoint Flaws rk. ii]} || ${Me.CombatAbility[Pinpoint Flaws rk. iii]} || ${Me.CombatAbility[Pinpoint Liabilities]} || ${Me.CombatAbility[Pinpoint Liabilities rk. ii]} || ${Me.CombatAbility[Pinpoint Liabilities rk. iii]} || ${Me.CombatAbility[Pinpoint Deficiencies]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. ii]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. iii]} || ${Me.CombatAbility[Pinpoint Shortcomings]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. II]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. III]} || ${Me.CombatAbility[Pinpoint Defects]} || ${Me.CombatAbility[Pinpoint Defects Rk. II]} || ${Me.CombatAbility[Pinpoint Defects Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pPOKER, "poker", "[ID] item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[backstab]},1,0]}");
DECLARE_ABILITY_OPTION(pSTRIK, "strike", "Use best sneak attack disc [ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[backstab]} && ${meleemvi[idstrike]},1,0]}");
DECLARE_ABILITY_OPTION(pTHIEF, "thiefeye", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[thief's eyes]} || ${Me.CombatAbility[Thief's Vision]} || ${Me.CombatAbility[Thief's Vision Rk. II]} || ${Me.CombatAbility[Thief's Vision Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[thief's eyes]} || ${Me.CombatAbility[Thief's Vision]} || ${Me.CombatAbility[Thief's Vision Rk. II]} || ${Me.CombatAbility[Thief's Vision Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pVIGDR, "vigdagger", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Dagger-Throw]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. III]} || ${Me.CombatAbility[Vigorous Dagger-Strike]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Strike]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Throw]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. III]} || ${Me.CombatAbility[Exuberant Dagger-Throw]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. II]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. III]} || ${Me.CombatAbility[Forceful Dagger-Throw]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. II]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. III]} || ${Me.CombatAbility[Powerful Dagger-Throw]} || ${Me.CombatAbility[Powerful Dagger-Throw Rk. II]} || ${Me.CombatAbility[Powerful Dagger-Throw Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Dagger-Throw]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. III]} || ${Me.CombatAbility[Vigorous Dagger-Strike]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Strike]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Throw]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. III]} || ${Me.CombatAbility[Exuberant Dagger-Throw]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. II]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. III]} || ${Me.CombatAbility[Forceful Dagger-Throw]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. II]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. III]} || ${Me.CombatAbility[Powerful Dagger-Throw]} || ${Me.CombatAbility[Powerful Dagger-Throw Rk. II]} || ${Me.CombatAbility[Powerful Dagger-Throw Rk. III]} || ${Me.CombatAbility[Precise Dagger-Throw]} || ${Me.CombatAbility[Precise Dagger-Throw Rk. II]} || ${Me.CombatAbility[Precise Dagger-Throw Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pTWIST, "twistedshank", "[ON/OFF]?", "${If[${Me.AltAbility[twisted shank]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.AltAbility[twisted shank]},1,0]}");

#pragma endregion

#pragma region Shaman - Class 10

#pragma endregion - Empty

#pragma region Necromancer - Class 11
//feign_n1 scapegoat missing??
#pragma endregion - Empty

#pragma region Wizard - Class 12

#pragma endregion - Empty

#pragma region Mage - Class 13

#pragma endregion - Empty

#pragma region Enchanter - Class 14

#pragma endregion - Empty

#pragma region - Beastlord - Class 15
DECLARE_ABILITY_OPTION(pBTASP, "asp", "[ON/OFF]?", "${If[${Me.AltAbility[bite of the asp]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[bite of the asp]},1,0]}");
DECLARE_ABILITY_OPTION(pBVIVI, "bvivi", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bestial Vivisection]} || ${Me.CombatAbility[Bestial Vivisection Rk. II]} || ${Me.CombatAbility[Bestial Vivisection Rk. III]} || ${Me.CombatAbility[Bestial Rending]} || ${Me.CombatAbility[Bestial Rending Rk. II]} || ${Me.CombatAbility[Bestial Rending Rk. III]} || ${Me.CombatAbility[Bestial Evulsing]} || ${Me.CombatAbility[Bestial Evulsing Rk. II]} || ${Me.CombatAbility[Bestial Evulsing Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bestial Vivisection]} || ${Me.CombatAbility[Bestial Vivisection Rk. II]} || ${Me.CombatAbility[Bestial Vivisection Rk. III]} || ${Me.CombatAbility[Bestial Rending]} || ${Me.CombatAbility[Bestial Rending Rk. II]} || ${Me.CombatAbility[Bestial Rending Rk. III]} || ${Me.CombatAbility[Bestial Evulsing]} || ${Me.CombatAbility[Bestial Evulsing Rk. II]} || ${Me.CombatAbility[Bestial Evulsing Rk. III]} || ${Me.CombatAbility[Bestial Fierceness]} || ${Me.CombatAbility[Bestial Fierceness Rk. II]} || ${Me.CombatAbility[Bestial Fierceness Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCHAMS, "cstrike", "[ON/OFF]?", "${If[${Me.AltAbility[Chameleon Strike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Chameleon Strike]},1,0]}");
//Feign Beast?? Where for art thou Feign Beast?
DECLARE_ABILITY_OPTION(pFERAL, "feralswipe", "[ON/OFF]?", "${If[${Me.AltAbility[feral swipe]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[feral swipe]},1,0]}");
DECLARE_ABILITY_OPTION(pFCLAW, "fclaw", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[flurry of claws]} || ${Me.CombatAbility[flurry of claws rk. ii]} || ${Me.CombatAbility[flurry of claws rk. iii]} || ${Me.CombatAbility[tumult of claws]} || ${Me.CombatAbility[tumult of claws rk. ii]} || ${Me.CombatAbility[tumult of claws rk. iii]} || ${Me.CombatAbility[clamor of claws]} || ${Me.CombatAbility[clamor of claws rk. ii]} || ${Me.CombatAbility[clamor of claws rk. iii]} || ${Me.CombatAbility[tempest of claws]} || ${Me.CombatAbility[tempest of claws rk. ii]} || ${Me.CombatAbility[tempest of claws rk. iii]} || ${Me.CombatAbility[Storm of claws]} || ${Me.CombatAbility[Storm of claws Rk. II]} || ${Me.CombatAbility[Storm of claws Rk. III]} || ${Me.CombatAbility[Maelstrom of Claws]} || ${Me.CombatAbility[Maelstrom of Claws Rk. II]} || ${Me.CombatAbility[Maelstrom of Claws Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[flurry of claws]} || ${Me.CombatAbility[flurry of claws rk. ii]} || ${Me.CombatAbility[flurry of claws rk. iii]} || ${Me.CombatAbility[tumult of claws]} || ${Me.CombatAbility[tumult of claws rk. ii]} || ${Me.CombatAbility[tumult of claws rk. iii]} || ${Me.CombatAbility[clamor of claws]} || ${Me.CombatAbility[clamor of claws rk. ii]} || ${Me.CombatAbility[clamor of claws rk. iii]} || ${Me.CombatAbility[tempest of claws]} || ${Me.CombatAbility[tempest of claws rk. ii]} || ${Me.CombatAbility[tempest of claws rk. iii]} || ${Me.CombatAbility[Storm of claws]} || ${Me.CombatAbility[Storm of claws Rk. II]} || ${Me.CombatAbility[Storm of claws Rk. III]} || ${Me.CombatAbility[Maelstrom of Claws]} || ${Me.CombatAbility[Maelstrom of Claws Rk. II]} || ${Me.CombatAbility[Maelstrom of Claws Rk. III]} || ${Me.CombatAbility[Eruption of Claws]} || ${Me.CombatAbility[Eruption of Claws Rk. II]} || ${Me.CombatAbility[Eruption of Claws Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pGORSM, "gorillasmash", "[ON/OFF]?", "${If[${Me.AltAbility[gorilla smash]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[gorilla smash]},1,0]}");
DECLARE_ABILITY_OPTION(pRAKES, "rake", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[rake]} || ${Me.CombatAbility[harrow]} || ${Me.CombatAbility[harrow rk. ii]} || ${Me.CombatAbility[harrow rk. iii]} || ${Me.CombatAbility[foray]} || ${Me.CombatAbility[foray rk. ii]} || ${Me.CombatAbility[foray rk. iii]} || ${Me.CombatAbility[rush]} || ${Me.CombatAbility[rush rk. ii]} || ${Me.CombatAbility[rush rk. iii]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage rk. ii]} || ${Me.CombatAbility[Barrage rk. iii]} || ${Me.CombatAbility[Pummel]} || ${Me.CombatAbility[Pummel rk. ii]} || ${Me.CombatAbility[Pummel rk. iii]} || ${Me.CombatAbility[Maul]} || ${Me.CombatAbility[Maul rk. ii]} || ${Me.CombatAbility[Maul rk. iii]} || ${Me.CombatAbility[Mangle]} || ${Me.CombatAbility[Mangle Rk. II]} || ${Me.CombatAbility[Mangle Rk. III]} || ${Me.CombatAbility[Batter]} || ${Me.CombatAbility[Batter Rk. II]} || ${Me.CombatAbility[Batter Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[rake]} || ${Me.CombatAbility[harrow]} || ${Me.CombatAbility[harrow rk. ii]} || ${Me.CombatAbility[harrow rk. iii]} || ${Me.CombatAbility[foray]} || ${Me.CombatAbility[foray rk. ii]} || ${Me.CombatAbility[foray rk. iii]} || ${Me.CombatAbility[rush]} || ${Me.CombatAbility[rush rk. ii]} || ${Me.CombatAbility[rush rk. iii]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage rk. ii]} || ${Me.CombatAbility[Barrage rk. iii]} || ${Me.CombatAbility[Pummel]} || ${Me.CombatAbility[Pummel rk. ii]} || ${Me.CombatAbility[Pummel rk. iii]} || ${Me.CombatAbility[Maul]} || ${Me.CombatAbility[Maul rk. ii]} || ${Me.CombatAbility[Maul rk. iii]} || ${Me.CombatAbility[Mangle]} || ${Me.CombatAbility[Mangle Rk. II]} || ${Me.CombatAbility[Mangle Rk. III]} || ${Me.CombatAbility[Batter]} || ${Me.CombatAbility[Batter Rk. II]} || ${Me.CombatAbility[Batter Rk. III]}  || ${Me.CombatAbility[Clobber]} || ${Me.CombatAbility[Clobber Rk. II]} || ${Me.CombatAbility[Clobber Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRAVEN, "ravens", "[ON/OFF]?", "${If[${Me.AltAbility[raven's claw]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[raven's claw]},1,0]}");

#pragma endregion

#pragma region Berserker - Class 16
DECLARE_ABILITY_OPTION(pBLUST, "bloodlust", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Shared Bloodlust]} || ${Me.CombatAbility[Shared Bloodlust Rk. II]} || ${Me.CombatAbility[Shared Bloodlust Rk. III]} || ${Me.CombatAbility[Shared Brutality]} || ${Me.CombatAbility[Shared Brutality Rk. II]} || ${Me.CombatAbility[Shared Brutality Rk. III]} || ${Me.CombatAbility[Shared Savagery]} || ${Me.CombatAbility[Shared Savagery Rk. II]} || ${Me.CombatAbility[Shared Savagery Rk. III]} || ${Me.CombatAbility[Shared Viciousness]} || ${Me.CombatAbility[Shared Viciousness Rk. II]} || ${Me.CombatAbility[Shared Viciousness Rk. III]} || ${Me.CombatAbility[Shared Cruelty]} || ${Me.CombatAbility[Shared Cruelty Rk. II]} || ${Me.CombatAbility[Shared Cruelty Rk. III]} || ${Me.CombatAbility[Shared Ruthlessness]} || ${Me.CombatAbility[Shared Ruthlessness Rk. II]} || ${Me.CombatAbility[Shared Ruthlessness Rk. III]} || ${Me.CombatAbility[Shared Atavism]} || ${Me.CombatAbility[Shared Atavism Rk. II]} || ${Me.CombatAbility[Shared Atavism Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Shared Bloodlust]} || ${Me.CombatAbility[Shared Bloodlust Rk. II]} || ${Me.CombatAbility[Shared Bloodlust Rk. III]} || ${Me.CombatAbility[Shared Brutality]} || ${Me.CombatAbility[Shared Brutality Rk. II]} || ${Me.CombatAbility[Shared Brutality Rk. III]} || ${Me.CombatAbility[Shared Savagery]} || ${Me.CombatAbility[Shared Savagery Rk. II]} || ${Me.CombatAbility[Shared Savagery Rk. III]} || ${Me.CombatAbility[Shared Viciousness]} || ${Me.CombatAbility[Shared Viciousness Rk. II]} || ${Me.CombatAbility[Shared Viciousness Rk. III]} || ${Me.CombatAbility[Shared Cruelty]} || ${Me.CombatAbility[Shared Cruelty Rk. II]} || ${Me.CombatAbility[Shared Cruelty Rk. III]} || ${Me.CombatAbility[Shared Ruthlessness]} || ${Me.CombatAbility[Shared Ruthlessness Rk. II]} || ${Me.CombatAbility[Shared Ruthlessness Rk. III]} || ${Me.CombatAbility[Shared Atavism]} || ${Me.CombatAbility[Shared Atavism Rk. II]} || ${Me.CombatAbility[Shared Atavism Rk. III]} || ${Me.CombatAbility[Shared Violence]} || ${Me.CombatAbility[Shared Violence Rk. II]} || ${Me.CombatAbility[Shared Violence Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCRIPS, "cripple", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[leg strike]} || ${Me.CombatAbility[leg cut]} || ${Me.CombatAbility[leg slice]} || ${Me.CombatAbility[crippling strike]} || ${Me.CombatAbility[tendon cleave]} || ${Me.CombatAbility[tendon cleave rk. ii]} || ${Me.CombatAbility[tendon cleave rk. iii]} || ${Me.CombatAbility[tendon sever]} || ${Me.CombatAbility[tendon sever rk. ii]} || ${Me.CombatAbility[tendon sever rk. iii]} || ${Me.CombatAbility[tendon shear]} || ${Me.CombatAbility[tendon shear rk. ii]} || ${Me.CombatAbility[tendon shear rk. iii]} || ${Me.CombatAbility[tendon lacerate]} || ${Me.CombatAbility[tendon lacerate rk. ii]} || ${Me.CombatAbility[tendon lacerate rk. iii]} || ${Me.CombatAbility[tendon Slash]} || ${Me.CombatAbility[tendon Slash rk. ii]} || ${Me.CombatAbility[tendon Slash rk. iii]} || ${Me.CombatAbility[Tendon Gash]} || ${Me.CombatAbility[Tendon Gash Rk. II]} || ${Me.CombatAbility[Tendon Gash Rk. III]} || ${Me.CombatAbility[Tendon Tear]} || ${Me.CombatAbility[Tendon Tear Rk. II]} || ${Me.CombatAbility[Tendon Tear Rk. III]} || ${Me.CombatAbility[Tendon Rupture]} || ${Me.CombatAbility[Tendon Rupture Rk. II]} || ${Me.CombatAbility[Tendon Rupture Rk. III]} || ${Me.CombatAbility[Tendon Rip]} || ${Me.CombatAbility[Tendon Rip Rk. II]} || ${Me.CombatAbility[Tendon Rip Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[leg strike]} || ${Me.CombatAbility[leg cut]} || ${Me.CombatAbility[leg slice]} || ${Me.CombatAbility[crippling strike]} || ${Me.CombatAbility[tendon cleave]} || ${Me.CombatAbility[tendon cleave rk. ii]} || ${Me.CombatAbility[tendon cleave rk. iii]} || ${Me.CombatAbility[tendon sever]} || ${Me.CombatAbility[tendon sever rk. ii]} || ${Me.CombatAbility[tendon sever rk. iii]} || ${Me.CombatAbility[tendon shear]} || ${Me.CombatAbility[tendon shear rk. ii]} || ${Me.CombatAbility[tendon shear rk. iii]} || ${Me.CombatAbility[tendon lacerate]} || ${Me.CombatAbility[tendon lacerate rk. ii]} || ${Me.CombatAbility[tendon lacerate rk. iii]} || ${Me.CombatAbility[tendon Slash]} || ${Me.CombatAbility[tendon Slash rk. ii]} || ${Me.CombatAbility[tendon Slash rk. iii]} || ${Me.CombatAbility[Tendon Gash]} || ${Me.CombatAbility[Tendon Gash Rk. II]} || ${Me.CombatAbility[Tendon Gash Rk. III]} || ${Me.CombatAbility[Tendon Tear]} || ${Me.CombatAbility[Tendon Tear Rk. II]} || ${Me.CombatAbility[Tendon Tear Rk. III]} || ${Me.CombatAbility[Tendon Rupture]} || ${Me.CombatAbility[Tendon Rupture Rk. II]} || ${Me.CombatAbility[Tendon Rupture Rk. III]} || ${Me.CombatAbility[Tendon Rip]} || ${Me.CombatAbility[Tendon Rip Rk. II]} || ${Me.CombatAbility[Tendon Rip Rk. III]} || ${Me.CombatAbility[Tendon Shred]} || ${Me.CombatAbility[Tendon Shred Rk. II]} || ${Me.CombatAbility[Tendon Shred Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCRYHC, "cryhavoc", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[cry havoc]} || ${Me.CombatAbility[Cry Carnage]} || ${Me.CombatAbility[Cry Carnage Rk. II]} || ${Me.CombatAbility[Cry Carnage Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[cry havoc]} || ${Me.CombatAbility[Cry Carnage]} || ${Me.CombatAbility[Cry Carnage Rk. II]} || ${Me.CombatAbility[Cry Carnage Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pFRENZ, "frenzy", "[ON/OFF]?", "${If[${Me.Skill[frenzy]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[frenzy]} && ${Me.Skill[frenzy]},1,0]}");
//Where is jolt?
//Where is stun?
DECLARE_ABILITY_OPTION(pSLAPF, "slapface", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Slap in the Face]} || ${Me.CombatAbility[Slap in the Face rk. ii]} || ${Me.CombatAbility[Slap in the Face rk. iii]} || ${Me.CombatAbility[Kick in the Teeth]} || ${Me.CombatAbility[Kick in the Teeth rk. ii]} || ${Me.CombatAbility[Kick in the Teeth rk. iii]} || ${Me.CombatAbility[Punch in the Throat]} || ${Me.CombatAbility[Punch in the Throat rk. ii]} || ${Me.CombatAbility[Punch in the Throat rk. iii]} || ${Me.CombatAbility[Kick in the Shins]} || ${Me.CombatAbility[Kick in the Shins rk. ii]} || ${Me.CombatAbility[Kick in the Shins rk. iii]} || ${Me.CombatAbility[Sucker Punch]} || ${Me.CombatAbility[Sucker Punch Rk. II]} || ${Me.CombatAbility[Sucker Punch Rk. III]} || ${Me.CombatAbility[Rabbit Punch]} || ${Me.CombatAbility[Rabbit Punch Rk. II]} || ${Me.CombatAbility[Rabbit Punch Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Slap in the Face]} || ${Me.CombatAbility[Slap in the Face rk. ii]} || ${Me.CombatAbility[Slap in the Face rk. iii]} || ${Me.CombatAbility[Kick in the Teeth]} || ${Me.CombatAbility[Kick in the Teeth rk. ii]} || ${Me.CombatAbility[Kick in the Teeth rk. iii]} || ${Me.CombatAbility[Punch in the Throat]} || ${Me.CombatAbility[Punch in the Throat rk. ii]} || ${Me.CombatAbility[Punch in the Throat rk. iii]} || ${Me.CombatAbility[Kick in the Shins]} || ${Me.CombatAbility[Kick in the Shins rk. ii]} || ${Me.CombatAbility[Kick in the Shins rk. iii]} || ${Me.CombatAbility[Sucker Punch]} || ${Me.CombatAbility[Sucker Punch Rk. II]} || ${Me.CombatAbility[Sucker Punch Rk. III]} || ${Me.CombatAbility[Swift Punch]} || ${Me.CombatAbility[Swift Punch Rk. II]} || ${Me.CombatAbility[Swift Punch Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRALLO, "rallos", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Axe of Rallos]} || ${Me.CombatAbility[Axe of Rallos Rk. II]} || ${Me.CombatAbility[Axe of Rallos Rk. III]} || ${Me.CombatAbility[Axe of Graster]} || ${Me.CombatAbility[Axe of Graster Rk. II]} || ${Me.CombatAbility[Axe of Graster Rk. III]} || ${Me.CombatAbility[Axe of Illdaera]} || ${Me.CombatAbility[Axe of Illdaera Rk. II]} || ${Me.CombatAbility[Axe of Illdaera Rk. III]} || ${Me.CombatAbility[Axe of Zurel]} || ${Me.CombatAbility[Axe of Zurel Rk. II]} || ${Me.CombatAbility[Axe of Zurel Rk. III]} || ${Me.CombatAbility[Axe of Numicia]} || ${Me.CombatAbility[Axe of Numicia Rk. II]} || ${Me.CombatAbility[Axe of Numicia Rk. III]} || ${Me.CombatAbility[Axe of the Aeons]} || ${Me.CombatAbility[Axe of the Aeons Rk. II]} || ${Me.CombatAbility[Axe of the Aeons Rk. III]} || ${Me.CombatAbility[Axe of Rekatok]} || ${Me.CombatAbility[Axe of Rekatok Rk. II]} || ${Me.CombatAbility[Axe of Rekatok Rk. III]} || ${Me.CombatAbility[Axe of Empyr]} || ${Me.CombatAbility[Axe of Empyr Rk. II]} || ${Me.CombatAbility[Axe of Empyr Rk. III]} || ${Me.CombatAbility[Axe of Derakor]} || ${Me.CombatAbility[Axe of Derakor Rk. II]} || ${Me.CombatAbility[Axe of Derakor Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Axe of Rallos]} || ${Me.CombatAbility[Axe of Rallos Rk. II]} || ${Me.CombatAbility[Axe of Rallos Rk. III]} || ${Me.CombatAbility[Axe of Graster]} || ${Me.CombatAbility[Axe of Graster Rk. II]} || ${Me.CombatAbility[Axe of Graster Rk. III]} || ${Me.CombatAbility[Axe of Illdaera]} || ${Me.CombatAbility[Axe of Illdaera Rk. II]} || ${Me.CombatAbility[Axe of Illdaera Rk. III]} ${Me.CombatAbility[Axe of Illdaera Rk. III]} || ${Me.CombatAbility[Axe of Zurel]} || ${Me.CombatAbility[Axe of Zurel Rk. II]} || ${Me.CombatAbility[Axe of Zurel Rk. III]} || ${Me.CombatAbility[Axe of Numicia]} || ${Me.CombatAbility[Axe of Numicia Rk. II]} || ${Me.CombatAbility[Axe of Numicia Rk. III]} || ${Me.CombatAbility[Axe of the Aeons]} || ${Me.CombatAbility[Axe of the Aeons Rk. II]} || ${Me.CombatAbility[Axe of the Aeons Rk. III]} || ${Me.CombatAbility[Axe of Rekatok]} || ${Me.CombatAbility[Axe of Rekatok Rk. II]} || ${Me.CombatAbility[Axe of Rekatok Rk. III]} || ${Me.CombatAbility[Axe of Empyr]} || ${Me.CombatAbility[Axe of Empyr Rk. II]} || ${Me.CombatAbility[Axe of Empyr Rk. III]} || ${Me.CombatAbility[Axe of Derakor]} || ${Me.CombatAbility[Axe of Derakor Rk. II]} || ${Me.CombatAbility[Axe of Derakor Rk. III]} || ${Me.CombatAbility[Axe of Xin Diabo]} || ${Me.CombatAbility[Axe of Xin Diabo Rk. II]} || ${Me.CombatAbility[Axe of Xin Diabo Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRAVOL, "ragevolley", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[rage volley]} || ${Me.CombatAbility[destroyer's volley]} || ${Me.CombatAbility[giantslayer's volley]} || ${Me.CombatAbility[giantslayer's volley rk. ii]} || ${Me.CombatAbility[giantslayer's volley rk. iii]} || ${Me.CombatAbility[annihilator's volley]} || ${Me.CombatAbility[annihilator's volley rk. ii]} || ${Me.CombatAbility[annihilator's volley rk. iii]} || ${Me.CombatAbility[decimator's volley]} || ${Me.CombatAbility[decimator's volley rk. ii]} || ${Me.CombatAbility[decimator's volley rk. iii]} || ${Me.CombatAbility[Eradicator's Volley]} || ${Me.CombatAbility[Eradicator's Volley rk. ii]} || ${Me.CombatAbility[Eradicator's Volley rk. iii]} || ${Me.CombatAbility[Savage Volley]} || ${Me.CombatAbility[Savage Volley rk. ii]} || ${Me.CombatAbility[Savage Volley rk. iii]} || ${Me.CombatAbility[Sundering Volley]} || ${Me.CombatAbility[Sundering Volley rk. ii]} || ${Me.CombatAbility[Sundering Volley rk. iii]} || ${Me.CombatAbility[Brutal Volley]} || ${Me.CombatAbility[Brutal Volley rk. ii]} || ${Me.CombatAbility[Brutal Volley rk. iii]} || ${Me.CombatAbility[Demolishing Volley]} || ${Me.CombatAbility[Demolishing Volley rk. ii]} || ${Me.CombatAbility[Demolishing Volley rk. iii]} || ${Me.CombatAbility[Mangling Volley]} || ${Me.CombatAbility[Mangling Volley Rk. II]} || ${Me.CombatAbility[Mangling Volley Rk. III]} || ${Me.CombatAbility[Vindicating Volley]} || ${Me.CombatAbility[Vindicating Volley Rk. II]} || ${Me.CombatAbility[Vindicating Volley Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[rage volley]} || ${Me.CombatAbility[destroyer's volley]} || ${Me.CombatAbility[giantslayer's volley]} || ${Me.CombatAbility[giantslayer's volley rk. ii]} || ${Me.CombatAbility[giantslayer's volley rk. iii]} || ${Me.CombatAbility[annihilator's volley]} || ${Me.CombatAbility[annihilator's volley rk. ii]} || ${Me.CombatAbility[annihilator's volley rk. iii]} || ${Me.CombatAbility[decimator's volley]} || ${Me.CombatAbility[decimator's volley rk. ii]} || ${Me.CombatAbility[decimator's volley rk. iii]} || ${Me.CombatAbility[Eradicator's Volley]} || ${Me.CombatAbility[Eradicator's Volley rk. ii]} || ${Me.CombatAbility[Eradicator's Volley rk. iii]} || ${Me.CombatAbility[Savage Volley]} || ${Me.CombatAbility[Savage Volley rk. ii]} || ${Me.CombatAbility[Savage Volley rk. iii]} || ${Me.CombatAbility[Sundering Volley]} || ${Me.CombatAbility[Sundering Volley rk. ii]} || ${Me.CombatAbility[Sundering Volley rk. iii]} || ${Me.CombatAbility[Brutal Volley]} || ${Me.CombatAbility[Brutal Volley rk. ii]} || ${Me.CombatAbility[Brutal Volley rk. iii]} || ${Me.CombatAbility[Demolishing Volley]} || ${Me.CombatAbility[Demolishing Volley rk. ii]} || ${Me.CombatAbility[Demolishing Volley rk. iii]} || ${Me.CombatAbility[Mangling Volley]} || ${Me.CombatAbility[Mangling Volley Rk. II]} || ${Me.CombatAbility[Mangling Volley Rk. III]} || ${Me.CombatAbility[Vindicating Volley]} || ${Me.CombatAbility[Vindicating Volley Rk. II]} || ${Me.CombatAbility[Vindicating Volley Rk. III]} || ${Me.CombatAbility[Pulverizing Volley]} || ${Me.CombatAbility[Pulverizing Volley Rk. II]} || ${Me.CombatAbility[Pulverizing Volley Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pOFREN, "opfrenzy", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Overpowering Frenzy]} || ${Me.CombatAbility[Overpowering Frenzy Rk. II]} || ${Me.CombatAbility[Overpowering Frenzy Rk. III]} || ${Me.CombatAbility[Overwhelming Frenzy]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. II]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. III]} || ${Me.CombatAbility[Conquering Frenzy]} || ${Me.CombatAbility[Conquering Frenzy Rk. II]} || ${Me.CombatAbility[Conquering Frenzy Rk. III]} || ${Me.CombatAbility[Vanquishing Frenzy]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. II]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. III]} || ${Me.CombatAbility[Demolishing Frenzy]} || ${Me.CombatAbility[Demolishing Frenzy Rk. II]} || ${Me.CombatAbility[Demolishing Frenzy Rk. III]} || ${Me.CombatAbility[Mangling Frenzy]} || ${Me.CombatAbility[Mangling Frenzy Rk. II]} || ${Me.CombatAbility[Mangling Frenzy Rk. III]} || ${Me.CombatAbility[Vindicating Frenzy]} || ${Me.CombatAbility[Vindicating Frenzy Rk. II]} || ${Me.CombatAbility[Vindicating Frenzy Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Overpowering Frenzy]} || ${Me.CombatAbility[Overpowering Frenzy Rk. II]} || ${Me.CombatAbility[Overpowering Frenzy Rk. III]} || ${Me.CombatAbility[Overwhelming Frenzy]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. II]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. III]} || ${Me.CombatAbility[Conquering Frenzy]} || ${Me.CombatAbility[Conquering Frenzy Rk. II]} || ${Me.CombatAbility[Conquering Frenzy Rk. III]} || ${Me.CombatAbility[Vanquishing Frenzy]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. II]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. III]} || ${Me.CombatAbility[Demolishing Frenzy]} || ${Me.CombatAbility[Demolishing Frenzy Rk. II]} || ${Me.CombatAbility[Demolishing Frenzy Rk. III]} || ${Me.CombatAbility[Mangling Frenzy]} || ${Me.CombatAbility[Mangling Frenzy Rk. II]} || ${Me.CombatAbility[Mangling Frenzy Rk. III]} || ${Me.CombatAbility[Vindicating Frenzy]} || ${Me.CombatAbility[Vindicating Frenzy Rk. II]} || ${Me.CombatAbility[Vindicating Frenzy Rk. III]} || ${Me.CombatAbility[Oppressing Frenzy]} || ${Me.CombatAbility[Oppressing Frenzy Rk. II]} || ${Me.CombatAbility[Oppressing Frenzy Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pVIGAX, "vigaxe", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Axe Throw]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. II]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. III]} || ${Me.CombatAbility[Energetic Axe Throw]} || ${Me.CombatAbility[Energetic Axe Throw Rk. II]} || ${Me.CombatAbility[Energetic Axe Throw Rk. III]} || ${Me.CombatAbility[Spirited Axe Throw]} || ${Me.CombatAbility[Spirited Axe Throw Rk. II]} || ${Me.CombatAbility[Spirited Axe Throw Rk. III]} || ${Me.CombatAbility[Brutal Axe Throw]} || ${Me.CombatAbility[Brutal Axe Throw Rk. II]} || ${Me.CombatAbility[Brutal Axe Throw Rk. III]} || ${Me.CombatAbility[Demolishing Axe Throw]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. II]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. III]} || ${Me.CombatAbility[Mangling Axe Throw]} || ${Me.CombatAbility[Mangling Axe Throw Rk. II]} || ${Me.CombatAbility[Mangling Axe Throw Rk. III]} || ${Me.CombatAbility[Vindicating Axe Throw]} || ${Me.CombatAbility[Vindicating Axe Throw Rk. II]} || ${Me.CombatAbility[Vindicating Axe Throw Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Axe Throw]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. II]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. III]} || ${Me.CombatAbility[Energetic Axe Throw]} || ${Me.CombatAbility[Energetic Axe Throw Rk. II]} || ${Me.CombatAbility[Energetic Axe Throw Rk. III]} || ${Me.CombatAbility[Spirited Axe Throw]} || ${Me.CombatAbility[Spirited Axe Throw Rk. II]} || ${Me.CombatAbility[Spirited Axe Throw Rk. III]} || ${Me.CombatAbility[Brutal Axe Throw]} || ${Me.CombatAbility[Brutal Axe Throw Rk. II]} || ${Me.CombatAbility[Brutal Axe Throw Rk. III]} || ${Me.CombatAbility[Demolishing Axe Throw]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. II]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. III]} || ${Me.CombatAbility[Mangling Axe Throw]} || ${Me.CombatAbility[Mangling Axe Throw Rk. II]} || ${Me.CombatAbility[Mangling Axe Throw Rk. III]} || ${Me.CombatAbility[Vindicating Axe Throw]} || ${Me.CombatAbility[Vindicating Axe Throw Rk. II]} || ${Me.CombatAbility[Vindicating Axe Throw Rk. III]} || ${Me.CombatAbility[Maiming Axe Throw]} || ${Me.CombatAbility[Maiming Axe Throw Rk. II]} || ${Me.CombatAbility[Maiming Axe Throw Rk. III]}),1,0]}");
#pragma endregion

const char* UI_PetBack = "back";
const char* UI_PetAttk = "attack";

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

unsigned long AACheck(unsigned long id);
unsigned long AAPoint(unsigned long index);
int AAReady(unsigned long index);
long Aggroed(unsigned long id);
FLOAT AngularDistance(float h1, float h2);
DOUBLE AngularHeading(PSPAWNINFO t, PSPAWNINFO s);
int CACheck(unsigned long id);
int CAPress(unsigned long id);
int Casting(char* command);
int CursorEmpty();
long Discipline();
int Equip(unsigned long ID, long SlotID);
int Equipped(unsigned long id);
long Evaluate(const char* zFormat, ...);
void MeleeReset();
long OkayToEquip(long Size = NOID);
int SKCheck(unsigned long id);
int SKReady(unsigned long id);
int SKPress(unsigned long id);
long SpawnMask(PSPAWNINFO x);
int SpellCheck(unsigned long id);
long SpellGemID(unsigned long ID, long slotid = NOID);
int SpellReady(unsigned long ID, long SlotID = NOID);
int Stick(const char* command);
long Unequip(long SlotID);
//void WinClick(CXWnd *Wnd, char* ScreenID, char* ClickNotification, unsigned long KeyState);

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
long doDEBUG;

void Announce(unsigned long Wanted, const char* Format, ...) {
    char Output[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, Format); vsprintf_s(Output, Format, vaList);
    if (Output[0]) {
        bool Result = false; auto pPlugin = pPlugins;
        while (pPlugin) {
            fMQIncomingChat Request = (fMQIncomingChat)GetProcAddress(pPlugin->hModule, "OnMeleeChat");
            if (Request) Result |= Request(Output, Wanted);
            pPlugin = pPlugin->pNext;
        }
        if (!Result && ( Wanted || doDEBUG ) ) WriteChatf("%s", Output);
    }
}

static int WinState(CXWnd* Wnd) {
    return Wnd && Wnd->IsVisible();
}

static int TargetType(unsigned long mask) {
    return SpawnMask(pTarget)&mask;
}

static int TargetID(unsigned long ID) {
    if (ID && pTarget)
        return ID == pTarget->SpawnID;
    return false;
}

static float SpeedRun(SPAWNINFO* x) {
    if (x && x->Mount)
        return x->Mount->SpeedRun;
    if (x)
        return x->SpeedRun;
    return 0.0f;
}

static int SpawnType(SPAWNINFO* x, unsigned long mask) {
    return SpawnMask(x) & mask;
}

static SPAWNINFO* SpawnMe() {
    return pControlledPlayer;
}

static SPAWNINFO* SpawnMount() {
    return pLocalPlayer ? pLocalPlayer->Mount : nullptr;
}

static SPAWNINFO* SpawnPet() {
    return pLocalPlayer && pLocalPlayer->PetID > 0 ? GetSpawnByID(pLocalPlayer->PetID) : nullptr;
}

static unsigned long StandState() {
    return SpawnMe() ? SpawnMe()->StandState : 0;
}

static int Stackable(CONTENTS* Item) {
    return Item && !Item->IsContainer() && Item->IsStackable();
}

static long StackUnit(CONTENTS* Item) {
    return !Stackable(Item) ? 1 : Item->StackCount;
}

static int IsStunned() {
    return pLocalPC && pLocalPC->Stunned;
}

static int IsStanding() {
    if (SpawnMount()) return true;
    return StandState() == STANDSTATE_STAND;
}

static int IsSneaking() {
    return (SpawnMe() && SpawnMe()->Sneak);
}

static int IsInvisible() {
    return SpawnMe() && SpawnMe()->HideMode;
}

static int IsGrouped() {
    return pLocalPC && pLocalPC->pGroupInfo;
}

static int IsFeigning() {
    return StandState() == STANDSTATE_FEIGN;
}

static int InRange(PSPAWNINFO a, PSPAWNINFO b, float d) {
    if (!a || !b) return false;
    return (DistanceToSpawn(a, b) <= d);
}

static int InGame() {
    return (gbInZone && gGameState == GAMESTATE_INGAME && SpawnMe() && GetPcProfile() && pLocalPC && pLocalPC->Stunned != 3);
}

static CXWnd* XMLChild(CXWnd* window, char* screenid) {
    if (window) return window->GetChildItem(screenid);
    return nullptr;
}

static int XMLEnabled(CXWnd* window) {
    return (window && window->IsEnabled());
}

static CONTENTS* ContAmmo() {
    if (PcProfile* Me = GetPcProfile())
        return Me->GetInventorySlot(InvSlot_Ammo).get();
    return nullptr;
}

static CONTENTS* ContPrimary() {
    if (PcProfile* Me = GetPcProfile())
        return Me->GetInventorySlot(InvSlot_Primary).get();
    return nullptr;
}

static CONTENTS* ContRange() {
    if (PcProfile* Me = GetPcProfile())
        return Me->GetInventorySlot(InvSlot_Range).get();
    return nullptr;
}

static CONTENTS* ContSecondary() {
    if (PcProfile* Me = GetPcProfile())
        return Me->GetInventorySlot(InvSlot_Secondary).get();
    return nullptr;
}

static int PokerType(CONTENTS* item) {
    return item && item->GetItemClass() == ItemClass_Piercing;
}

static int ShieldType(CONTENTS* item) {
    return item && item->GetItemClass() == ItemClass_Shield;
}

static int TwohandType(CONTENTS* item) {
    if (item) {
        switch (item->GetItemClass()) {
        case ItemClass_2HSlashing:
        case ItemClass_2HBlunt:
        case ItemClass_2HPiercing:
            return true;
        default: break;
        }
    }
    return false;
}

unsigned long AACheck(unsigned long id) {
    int level = -1;
    if (pLocalPlayer) {
        level = pLocalPlayer->Level;
    }
    if (pAltAdvManager)
    {
        if (PcProfile* Me = GetPcProfile())
        {
            if (id)
            {
                for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++)
                {
                    if (long AAIndex = Me->AAList[nAbility].AAIndex)
                    {
                        if (PALTABILITY ability = GetAAByIdWrapper(AAIndex, level))
                        {
                            if (ability->ID == id)
                            {
                                return AAIndex;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

unsigned long AAPoint(unsigned long index) {
    if (index)
    {
        if (PcProfile* Me = GetPcProfile())
        {
            for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++)
            {
                if (index == Me->AAList[nAbility].AAIndex)
                {
                    return Me->AAList[nAbility].PointsSpent;
                }
            }
        }
    }
    return 0;
}

int AAReady(unsigned long index) {
    int result = 0;
    int level = -1;
    if (pLocalPlayer) {
        level = pLocalPlayer->Level;
    }
    if (pAltAdvManager)
    {
        if (index)
            if (PALTABILITY ability = GetAAByIdWrapper(index, level))
            {
                //unsigned long i = 0;
                //i = pAltAdvManager->GetCalculatedTimer(pLocalPC, ability);
                //DebugSpew("ability timer: %d", i);
                if (pAltAdvManager->GetCalculatedTimer((PcClient*)pLocalPC, ability) > 0)
                {
                    pAltAdvManager->IsAbilityReady((PcClient*)pLocalPC, ability, &result);
                    //DebugSpew("result: %d", result);
                }
            }
    }
    return (result < 0);
}

long Aggroed(unsigned long id) {
    if (PSPAWNINFO self = SpawnMe())
        if (PSPAWNINFO kill = GetSpawnByID(id))
            if (PSPAWNINFO targ = pTarget) {
                if (targ == kill && self->SpawnID == self->TargetOfTarget)  return  1; // im on hott
                if (fabs(AngularHeading(kill, self))<8.0f)       return  1; // it's facing me
                if (FindSpeed(kill)>0.0f && kill->HPCurrent<20) return -1; // it's moving
                if (InRange(self, targ, 25.0f))                   return -1; // close enough
            }
    return 0;
}

FLOAT AngularDistance(float h1, float h2) {
    if (h1 == h2) return 0.0;
    if (fabs(h1 - h2) > 256.0) * (h1 < h2 ? &h1 : &h2) += 512.0;
    return (fabs(h1 - h2) > 256.0) ? (h2 - h1) : (h1 - h2);
}

DOUBLE AngularHeading(PSPAWNINFO t, PSPAWNINFO s) {
    double Head = t->Heading - (float)atan2(s->X - t->X, s->Y - t->Y) * 256.0 / PI;
    if (Head > 256.0f) Head -= 512.0f; else if (Head < -256.0f) Head += 512.0f;
    return Head;
}

int CACheck(unsigned long id) {
    if (PcProfile* Me = GetPcProfile())
        if (id)
        {
            for (unsigned long nCombat = 0; nCombat < NUM_COMBAT_ABILITIES; nCombat++)
            {
                if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombat))
                {
                    if (id == Me->CombatAbilities[nCombat])
                    {
                        return true;
                    }
                }
            }
        }
    return false;
}

int CAPress(unsigned long id) {
    pLocalPC->DoCombatAbility(id);
    return true;
}

int Casting(char* command) {
    typedef void(__cdecl *fCALL)(PSPAWNINFO, char*);
    if (auto request = (fCALL)GetPluginProc("mq2cast", "CastCommand")) {
        Announce(SHOW_CASTING, "%s::Casting [\ay%s\ax].", PLUGIN_NAME, command);
        request(NULL, command);
        return true;
    }
    return false;
}

void Command(const char* zFormat, ...) {
    CHAR zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
    vsprintf_s(zOutput, zFormat, vaList);
    if (zOutput[0]) EzCommand(zOutput);
}

int CursorEmpty() {
    if (PcProfile* Me = GetPcProfile())
        if (!Me->GetInventorySlot(InvSlot_Cursor))
            if (!Me->CursorPlat)
                if (!Me->CursorGold)
                    if (!Me->CursorSilver)
                        if (!Me->CursorCopper)
                            return true;
    return false;
}

long Discipline() {
    PSPELL spell = GetSpellByName(pCombatAbilityWnd->GetChildItem("CAW_CombatEffectLabel")->GetWindowText().c_str());
    return (spell) ? spell->ID : 0;
}

int Equipped(unsigned long id) {
    if (id)
        for (int i = 0; i < InvSlot_FirstBagSlot; i++)
            if (CONTENTS* Cont = GetPcProfile()->GetInventorySlot(i))
                if (id == GetItemFromContents(Cont)->ItemNumber) return true;
    return false;
}

long Evaluate(const char* zFormat, ...) {
    char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
    vsprintf_s(zOutput, zFormat, vaList); if (!zOutput[0]) return 1;
    //DebugSpewAlways("E[%s]",zOutput);
    ParseMacroData(zOutput, sizeof(zOutput));
    //DebugSpewAlways("R[%s]",zOutput);
    return atoi(zOutput);
}

long ItemTimer(CONTENTS* pItem) {
    if (GetItemFromContents(pItem)->Clicky.TimerID != 0xFFFFFFFF) return GetItemTimer(pItem);
    if (GetItemFromContents(pItem)->Clicky.SpellID != 0xFFFFFFFF) return 0;
    return 999999;
}

int SKCheck(unsigned long id) {
    if (id<100 && (pSkillMgr->pSkill[id]->Activated && GetPcProfile()->Skill[id]))     return true;
    if (id>100 && id<128 && GetPcProfile()->Skill[id] != 0xFF && strlen(szSkills[id])>3) return true;
    return false;
}

//VoA skills fix 11/20/2011 - ieatacid
int SKReady(unsigned long id) {
    if (id < 100 || id == 111 || id == 114 || id == 115 || id == 116)
    {
        return pSkillMgr->IsAvailable(id);
    }
    if (id == 105 || id == 107) return LoH_HT_Ready();
    return false;
}

int SKPress(unsigned long id) {
    if (pLocalPC) {
        pLocalPC->UseSkill((unsigned char)id, pLocalPC->me);
        return true;
    }
    return false;
}

long SpawnMask(PSPAWNINFO x) {
    if (!x)                         return st_x;
    if (x->Type == SPAWN_PLAYER)    return st_p;
    if (x->Type == SPAWN_CORPSE)    return x->Deity ? st_cp : st_cn;
    if (x->Type != SPAWN_NPC)       return st_x;
    if (strstr(x->Name, "s_Mount")) return st_x;
    if (!x->MasterID)               return st_n;
    PSPAWNINFO m = GetSpawnByID(x->MasterID);
    return (!m || m->Type != SPAWN_PLAYER) ? st_wn : st_wp;
}

int SpellCheck(unsigned long ID) {
    if (ID)
        if (PcProfile* Me = GetPcProfile())
            for (unsigned long nSlot = 0; nSlot < NUM_BOOK_SLOTS; nSlot++)
                if (ID == Me->SpellBook[nSlot])
                    return true;
    return false;
}

long SpellGemID(unsigned long ID, long SlotID) {
    if (PcProfile* Me = GetPcProfile()) {
        if (SlotID != NOID && ID == Me->MemorizedSpells[SlotID]) return SlotID;
        for (long GEM = 0; GEM < GemsMax; GEM++)
            if (ID == Me->MemorizedSpells[GEM])
                return GEM;
    }
    return NOID;
}

int SpellReady(unsigned long ID, long SlotID) {
    if (pCastSpellWnd)
        if (PcProfile* Me = GetPcProfile()) {
            unsigned long GemID = (SlotID != NOID) ? SlotID : SpellGemID(ID);
            if (GemID < (unsigned long)GemsMax)
                if (Me->MemorizedSpells[GemID] == ID)
                    if (pCastSpellWnd->SpellSlots[GemID]->spellicon != NOID)
                        if (BardClass || pCastSpellWnd->SpellSlots[GemID]->spellstate != 1)
                            return true;
        }
    return false;
}

bool HandleMoveUtils(void)
{
    bMUPointers = false;
    fStickCommand = (fEQCommand)GetPluginProc("mq2moveutils", "StickCommand");
    pbStickOn = (bool *)GetPluginProc("mq2moveutils", "bStickOn");
    if (fStickCommand && pbStickOn)
    {
        bMUPointers = true;
        return true;
    }
    return false;
}

unsigned char PetButtonEnabled(const char* pszButtonName)
{
    for (int i = 0; i < PET_BUTTONS; i++)
    {
        if (auto pButton = pPetInfoWnd->pButton[i])
            if (ci_equals(pButton->GetWindowText(), pszButtonName))
                return pButton->IsEnabled();
    }
    return 0;
}

int Stick(const char* command)
{
    char szPasser[MAX_STRING] = { 0 };
    //typedef void (__cdecl *fCALL)(PSPAWNINFO, char*);
    //if (fCALL request = (fCALL)PluginEntry("mq2moveutils", "StickCommand"))
    if (bMULoaded && bMUPointers)
    {
        if (command[0])
        {
            Announce(SHOW_STICKING, "%s::Sticking [\ay%s\ax].", PLUGIN_NAME, command);
            fStickCommand(SpawnMe(), command);
            sprintf_s(szPasser, "/stick %s", command);
            //DebugSpew("MQ2Melee Stick call: %s", command);
        }
        else if (*pbStickOn)
        {
            fStickCommand(SpawnMe(), "off");
            sprintf_s(szPasser, "/stick off");
            //DebugSpew("MQ2Melee Stick call: off");
        }
        Sticking = *pbStickOn;
        //Sticking = (command[0]) ? true : false;
        strcpy_s(StickArg, *pbStickOn ? command : "OFF");
        return true;
    }
    return false;
}

unsigned long TimeSince(unsigned long Timer) {
    if (Timer) return (unsigned long)clock() - Timer;
    return 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// item related

bool FitInPack(long Size)
{
    //long pSIZE = 20;
    //long pSLOT = 0;
    unsigned char ucSlot = 0;
    for (ucSlot = InvSlot_FirstBagSlot; ucSlot <= GetHighestAvailableBagSlot(); ucSlot++)
    {
        if (CONTENTS* pInvSlot = GetPcProfile()->GetInventorySlot(ucSlot))
        {
            if (TypePack(pInvSlot) && GetItemFromContents(pInvSlot)->Combine != ContainerType_Quiver
                && Size <= GetItemFromContents(pInvSlot)->SizeCapacity)// && (!pSLOT || GetItemFromContents(pInvSlot)->SizeCapacity < pSIZE))
            {
                if (pInvSlot->GetHeldItems().IsEmpty())
                    return true;
                for (const ItemPtr& pItem : pInvSlot->GetHeldItems())
                {
                    if (!pItem)
                        return true;
                }
            }
        }
    }
    return false;
}

long OkayToEquip(long Size)
{
    if (!CursorEmpty() || IsCasting()) return false;
    if (Size != NOID) return FitInPack(Size);
    return true;
}

long Unequip(long SlotID)
{
    if (SlotID < InvSlot_NumInvSlots)
    {
        CONTENTS* uCONT = GetPcProfile()->GetInventorySlot(SlotID);
        if (!uCONT) return true;

        CItemLocation cUnequipTo;
        if (!PackFind(&cUnequipTo, uCONT)) return false;

        char szCommand[MAX_STRING] = { 0 };
        sprintf_s(szCommand, "/shiftkey /itemnotify %s leftmouseup", szItemSlot[SlotID]);
        EzCommand(szCommand);
        sprintf_s(szCommand, "/ctrlkey /itemnotify %s leftmouseup", szItemSlot[cUnequipTo.InvSlot]);
        EzCommand(szCommand);

        return true;
    }
    return false;
}

int Equip(unsigned long ID, long SlotID)
{
    if (!(SlotID < InvSlot_NumInvSlots)) return false;                   // invalid destination slot id for equipping item to
    if (!OkayToEquip())      return false;                         // can't equip item right casting or cursor not free

    auto dCONT = GetPcProfile()->GetInventorySlot(SlotID);

    // if the ID requested to equip has the same ID in the target slot, no action to take, objective already met.
    if (dCONT != nullptr && GetItemFromContents(dCONT)->ItemNumber == ID) {
        return true;
    }

    char szTempItem[25] = { 0 };
    sprintf_s(szTempItem, "%d", ID);
    CItemLocation cMoveItem;
    CItemLocation cUnequipTo;

    if (!ItemFind(&cMoveItem, szTempItem, InvSlot_FirstBagSlot, GetHighestAvailableBagSlot() + 1)) // assume that equipping item is in a backpack first
    {
        if (!ItemFind(&cMoveItem, szTempItem, 0, InvSlot_FirstBagSlot)) // wasnt found, check if already equipped somewhere
        {
            return false; // wasnt found, can't equip something we dont have
        }
    }
    if (SlotID == cMoveItem.InvSlot) return true; // item already in place

    // check class, level, deity and race to see if we have rights to equip this items.
    CONTENTS* fITEM = cMoveItem.pBagSlot;
    if (!(GetItemFromContents(fITEM)->Classes&(1 << ((GetPcProfile()->Class) - 1))))                                     return false;
    if (GetItemFromContents(fITEM)->RequiredLevel > GetPcProfile()->Level)                                               return false;
    if (GetItemFromContents(fITEM)->Deity && !(GetItemFromContents(fITEM)->Deity&(1 << (GetPcProfile()->Deity - 200))))  return false;
    int MyRace = GetPcProfile()->Race;
    switch (MyRace)
    {
        case 128: MyRace = 12;    break;
        case 130: MyRace = 13;    break;
        case 330: MyRace = 14;    break;
        case 522: MyRace = 15;    break;
        default:  MyRace--;
    }
    if (!(GetItemFromContents(fITEM)->Races&(1 << MyRace))) return false;
    if (SlotID == InvSlot_Primary && TwohandType(fITEM) && ContSecondary())  if (!Unequip(InvSlot_Secondary)) return false;
    if (SlotID == InvSlot_Secondary && TwohandType(ContPrimary()))           if (!Unequip(InvSlot_Primary))   return false;

    // if wearing something
    if (dCONT != nullptr && cMoveItem.InvSlot >= NUM_INV_SLOTS)
    {
        // search bags, if bags cant fit it, return false
        if (!PackFind(&cUnequipTo, dCONT))
        {
            return false;
        }
    }

    CHAR szCommand[MAX_STRING] = { 0 };
    if (cMoveItem.BagSlot != -1) {
        sprintf_s(szCommand, "/shiftkey /itemnotify in %s %d leftmouseup", szItemSlot[cMoveItem.InvSlot], cMoveItem.BagSlot + 1);
        EzCommand(szCommand);
    }
    else {
        sprintf_s(szCommand, "/shiftkey /itemnotify %s leftmouseup", szItemSlot[cMoveItem.InvSlot]);
        EzCommand(szCommand);
    }
    sprintf_s(szCommand, "/shiftkey /itemnotify %s leftmouseup", szItemSlot[SlotID]);
    EzCommand(szCommand);
    EzCommand("/autoinventory");
    return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class Ability {
    CHAR   COMM[16];    // Ability Cast Command
    unsigned long  REUSE;       // Ability Reuse Time
    unsigned long  READY;       // Ability Ready
    long   INDEX;       // Ability Index
    PSPELL EFFECT;      // Ability Spell Effect
    enum { UNKNOWN, ITEM, SKILL, DISC, AA, SPELL, CLICKY, POTION };


public:
    long   ID;          // Ability ID
    long   TYPE;        // Ability Type
    CHAR   NAME[128];   // Ability Name
    int   SEARCH;      // Ability Searched

    int Avail() {
        SEARCH = 1;
        EFFECT = 0;
        if (ID > NOID) {
            if (TYPE == SKILL || TYPE == UNKNOWN) {
                if (SKCheck(ID)) {
                    strcpy_s(NAME, szSkills[ID]);
                    REUSE = (ID == i_forage) ? 101750 : delay * 3;
                    TYPE = SKILL;
                    return true;
                }
            }
            if (TYPE == DISC || TYPE == UNKNOWN) {
                if (CACheck(ID)) {
                    if (PSPELL spell = GetSpellByID(ID)) {
                        strcpy_s(NAME, spell->Name);
                        EFFECT = spell;
                        REUSE = spell->CastTime + spell->RecastTime + delay * 6;
                        TYPE = DISC;
                        return true;
                    }
                }
            }
            if (TYPE == AA || TYPE == UNKNOWN) {
                int level = -1;
                if (pLocalPlayer) {
                    level = pLocalPlayer->Level;
                }
                if (long AAIndex = AACheck(ID)) {
                    if (PALTABILITY ability = GetAAByIdWrapper(AAIndex, level)) {
                        if (PSPELL spell = GetSpellByID(ability->SpellID)) {
                            strcpy_s(NAME, pCDBStr->GetString(ability->nName, eAltAbilityName, NULL));
                            sprintf_s(COMM, "%d|ALT", ID);
                            EFFECT = spell;
                            REUSE = pAltAdvManager->GetCalculatedTimer(pLocalPC, ability) * 1000 + spell->CastTime + delay * 3;
                            INDEX = AAIndex;
                            TYPE = AA;
                            return true;
                        }
                    }
                }
            }
            if (TYPE == SPELL || TYPE == UNKNOWN) {
                if (SpellCheck(ID)) {
                    if (PSPELL spell = GetSpellByID(ID)) {
                        strcpy_s(NAME, spell->Name);
                        sprintf_s(COMM, "%d|GEM5", ID);
                        EFFECT = spell;
                        REUSE = spell->CastTime + spell->RecastTime + delay * 3;
                        TYPE = SPELL;
                        return true;
                    }
                }
            }
            if (TYPE == POTION || TYPE == CLICKY || TYPE == ITEM || TYPE == UNKNOWN)
            {
                char szTempItem[25] = { 0 };
                sprintf_s(szTempItem, "%d", ID);
                CItemLocation cFindItem;
                if (ItemFind(&cFindItem, szTempItem))
                {
                    if (CONTENTS* find = cFindItem.pBagSlot)
                    {
                        INDEX = cFindItem.InvSlot;
                        strcpy_s(NAME, GetItemFromContents(find)->Name);
                        TYPE = ITEM;
                        if (PSPELL spell = GetSpellByID(GetItemFromContents(find)->Clicky.SpellID))
                        {
                            sprintf_s(COMM, "%d|ITEM", ID);
                            EFFECT = spell;
                            REUSE = GetItemFromContents(find)->Clicky.TimerID * 1000 + GetItemFromContents(find)->Clicky.CastTime + delay * 3;
                            TYPE = (GetItemFromContents(find)->ItemType == 21) ? POTION : CLICKY;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool Found() {
        if (!SEARCH) Avail();
        return (ID > 0 && TYPE != UNKNOWN) ? true : false;
    }

    long Check(const std::string& test) {
        if (!Found())                        return 0x01;  // Ability Not Found
        if ((unsigned long)clock() <= READY) return 0x02;  // Ability Not Refreshed
        if (TYPE == SKILL)
        {
            if (SpawnMount())
            {
                switch (ID)
                {
                case i_disarm:        break;
                case i_forage:        break;
                case i_intimidation:  break;
                case i_kick:          break;
                case i_mend:          break;
                case i_taunt:         break;
                default:              return 0x03;  // Ability Do Not Work on Mount
                }
            }
            if (!SKReady(ID))         return 0x13;  // Ability Not Ready
        }
        else if (EFFECT)
        {
            if (!IsStanding())        return 0x05;  // Not Standing
            if (IsStunned())          return 0x06;  // Stunned
            if (TYPE > DISC)
            {
                if (IsInvisible())    return 0x04;  // Will Break Invisiblity
                if (Silenced)         return 0x07;  // Silenced
            }
            if (!BardClass && IsCasting())         return 0x08;  // already casting
            if (WinState((CXWnd*)pSpellBookWnd)) return 0x09;  // spellbook open
            if (TYPE != AA)
            {
                //DebugSpew("EFFECT->ReuseTimerIndex Name: %s  ID: %d Type: %dVal: %d", NAME, ID, TYPE, EFFECT->ReuseTimerIndex);
                if (((unsigned long)pLocalPC->GetCombatAbilityTimer(EFFECT->ReuseTimerIndex, EFFECT->SpellGroup) - (unsigned long)time(NULL)) < 0) return 0x16; // discipline timer not ready
            }
            // TODO:  There's no reason to check for zero since 0 < 0 is false.  There's no reason to cast either unless you're trying to round, in which case do that instead.
            if ((long)EFFECT->ReagentID[0]>0 && (long)CountItemByID(EFFECT->ReagentID[0]) < (long)EFFECT->ReagentCount[0])        return 0x0A;  // out of reagent
            if (EFFECT->EnduranceCost && (int)GetPcProfile()->Endurance < EFFECT->EnduranceCost)                                  return 0x0B;  // out of endurance
            if (EFFECT->ManaCost && (int)GetPcProfile()->Mana < EFFECT->ManaCost)                                                 return 0x0C;  // out of mana
            if (!EFFECT->SpellType)
            {
                if (!pTarget)                                                                                                     return 0x0D;  // no target
                float SpellRange = (EFFECT->Range) ? EFFECT->Range : EFFECT->AERange;
                if (SpellRange && !InRange(SpawnMe(), pTarget, SpellRange))                                      return 0x0E;  // out of range
            }
            else if (EFFECT->DurationCap>0)
            {
                if (EFFECT->DurationWindow)
                {
                    for (int s = 0; s < SongMax; s++)
                    {
                        if (PSPELL buff = GetSpellByID(GetPcProfile()->GetTempEffect(s).SpellID))
                        {
                            if (EFFECT->ID == buff->ID)        return 0x0F; // already have
                            if (!BuffStackTest(EFFECT, buff)) return 0x10; // not stacking
                        }
                    }
                }
                else
                {
                    for (int b = 0; b<BuffMax; b++)
                    {
                        if (PSPELL buff = GetSpellByID(GetPcProfile()->GetEffect(b).SpellID))
                        {
                            if (EFFECT->ID == buff->ID)        return 0x0F; // already have
                            if (!BuffStackTest(EFFECT, buff)) return 0x10; // not stacking
                        }
                    }
                }
            }
            //mq2cast
            if (TYPE > DISC) {
                if (FindMQ2DataType("Cast")) {
                    if (!Evaluate("${If[${Cast.Ready[%s]},1,0]}", COMM)) {
                        return 0x11; // mq2cast not ready
                    }
                }
                else {
                    if (!Evaluate("${Me.SpellReady}"))
                    {
                        return 0x11;
                    }
                }
            }
            if (TYPE == AA) {
                if (!AAReady(INDEX))       return 0x13;  // Ability Not Ready
            }
            else if (TYPE == SPELL) {
                if (!pCastSpellWnd)        return 0x12;  // No Casting Spell Bar
                INDEX = SpellGemID(ID, INDEX);
                if (!SpellReady(ID, INDEX)) return 0x13;  // Ability Not Ready
            }
            else if (TYPE == POTION || TYPE == CLICKY)
            {
                //CONTENTS* find=ItemLocate(ID,0,NUM_INV_SLOTS,INDEX);
                char szTempItem[25] = { 0 };
                sprintf_s(szTempItem, "%d", ID);
                CItemLocation cFindItem;
                if (ItemFind(&cFindItem, szTempItem))
                {
                    CONTENTS* find = cFindItem.pBagSlot;
                    INDEX = cFindItem.InvSlot;
                    //INDEX=InvSlot;
                    if (!find || !find->Charges || ItemTimer(find)) return 0x13;  // Ability Not Ready
                    if (!CursorEmpty())        return 0x14;  // Cursor Not Empty
                }
            }
        }
        if (!test.empty() && !Evaluate((char*)test.c_str())) return 0x15; // User Condition Abort
        return 0x00;
    }

    int Ready(const std::string& test) {
        long Result = Check(test);
        if (DebugReady && Result) {
            const char* Message = "";
            switch (Result) {
            case 0x01:  Message = "NOT FOUND";                break;
            case 0x02:  Message = "NOT REFRESHED";            break;
            case 0x03:  Message = "NOT WORKING ON MOUNT";     break;
            case 0x04:  Message = "WILL BREAK INVISIBILITY";  break;
            case 0x05:  Message = "NOT STANDING";             break;
            case 0x06:  Message = "STUNNED";                  break;
            case 0x07:  Message = "SILENCED";                 break;
            case 0x08:  Message = "ALREADY CASTING";          break;
            case 0x09:  Message = "SPELLBOOK OPEN";           break;
            case 0x0A:  Message = "OUT OF REAGENT";           break;
            case 0x0B:  Message = "OUT OF ENDURANCE";         break;
            case 0x0C:  Message = "OUT OF MANA";              break;
            case 0x0D:  Message = "NO TARGET";                break;
            case 0x0E:  Message = "OUT OF RANGE";             break;
            case 0x0F:  Message = "ALREADY BUFFED WITH THIS"; break;
            case 0x10:  Message = "BUFF NOT STACKING";        break;
            case 0x11:  Message = "MQ2CAST NOT READY/FOUND";  break;
            case 0x12:  Message = "NO SPELL BAR";             break;
            case 0x13:  Message = "ABILITY NOT READY";        break;
            case 0x14:  Message = "CURSOR NOT EMPTY";         break;
            case 0x15:  Message = "USER CONDITION ABORT";     break;
            case 0x16:  Message = "TIMER NOT READY";          break;
            }
            if (ID && TYPE) Announce(DebugReady, "Ability[%d][%d][%s] <<%s>>.", ID, TYPE, NAME, Message);
        }
        return (!Result);
    }

    int Press() {
        int Casted = false;
        bool bFound = Found();
        if (bFound && (unsigned long)clock() > READY) {
            Announce(SHOW_ABILITY, "%s::Activate [\ay%s\ax].", PLUGIN_NAME, NAME);
            if (TYPE == SKILL)     Casted = SKPress(ID);
            else if (TYPE == DISC) Casted = CAPress(ID);
            else if (TYPE >= AA)   Casted = Casting(COMM);
            if (Casted) READY = (unsigned long)clock() + REUSE;
        }
        else
        {
            Announce(SHOW_ABILITY, "%s::Ability Not %s [\ay%s\ax].", PLUGIN_NAME, bFound ? "Ready" : "Found", NAME);
        }
        return Casted;
    }

    void Setup(long id, long type) {
        ID = id;            // Ability ID?
        TYPE = type;        // Ability Type?
        NAME[0] = 0;        // Ability Name?
        COMM[0] = 0;        // Ability Command?
        SEARCH = false;     // Ability Searched?
        READY = 0;          // Ability Ready Time
        INDEX = NOID;       // Ability Index
        EFFECT = NULL;      // Ability Spell Effect
    }

    Ability(long id, long type) {
        Setup(id, type);
    }

    Ability() {
        Setup(0, UNKNOWN);
    }
};

class Option {
public:
    // TODO: Give these sane names and make them std::string (lose the pointers)
    const char*  OptionKey;  // key?
    const char*  OptionHelp;  // help?
    const char*  OptionDefault;  // default?
    const char*  OptionShow;  // show?
    std::string* OptionCondition;  // condition?
    Ability*     OptionAbility;  // ability?
    long*        OptionValue;  // value?
    Function     OptionFunction;  // function?
    int          OptionUpdate;  // update?

    Option(const char* key, const char* help, const char* default_, const char* show, Function func, std::string* condition) {
        OptionKey = key;
        OptionHelp = help;
        OptionDefault = default_;
        OptionShow = show;
        OptionFunction = func;
        OptionUpdate = false;
        OptionCondition = condition;
        OptionAbility = nullptr;
        OptionValue = nullptr;
    }

    Option(const char* key, const char* help, const char* default_, const char* show, Function func, Ability* ability) {
        OptionKey = key;
        OptionHelp = help;
        OptionDefault = default_;
        OptionShow = show;
        OptionFunction = func;
        OptionUpdate = false;
        OptionCondition = nullptr;
        OptionAbility = ability;
        OptionValue = nullptr;
    }

    Option(const char* key, const char* help, const char* default_, const char* show, Function func, long* value) {
        OptionKey = key;
        OptionHelp = help;
        OptionDefault = default_;
        OptionShow = show;
        OptionFunction = func;
        OptionUpdate = false;
        OptionCondition = nullptr;
        OptionAbility = nullptr;
        OptionValue = value;
    }

    void Write() {
        if (OptionKey[0] && !OptionCondition && Evaluate(OptionShow)) {
            long value = (OptionAbility) ? OptionAbility->ID : (OptionValue) ? *OptionValue : 0;
            if (value > 0) WriteChatf("%s::%s (\ag%d\ax) \ay%s\ax.", PLUGIN_NAME, OptionKey, value, OptionHelp);
            else           WriteChatf("%s::%s (\ar0\ax) \ay%s\ax.", PLUGIN_NAME, OptionKey, OptionHelp);
        }
    }

    void Setup(char* value) {
        if (OptionCondition) *OptionCondition = value;
        else if (OptionAbility) OptionAbility->Setup(atol(value), 0);
        else if (OptionValue) {
            if (!_stricmp("false", value) || !_stricmp("off", value))    *OptionValue = 0;
            else if (!_stricmp("true", value) || !_stricmp("on", value)) *OptionValue = 1;
            else *OptionValue = atol(value);
        }
        if (OptionFunction) this->OptionFunction();
    }

    void *Value() {
        if (OptionAbility) return &OptionAbility->ID;
        if (OptionValue) return OptionValue;
        if (OptionCondition) return OptionCondition;
        return nullptr;
    }

    long Ready() {
        if (OptionAbility) return OptionAbility->Ready("");
        return NOID;
    }

    void Reset() {
        if (OptionKey[0]) {
            if (OptionCondition) *OptionCondition = OptionDefault;
            else {
                strcpy_s(Reserved, OptionDefault);
                if (Reserved[0])
                    ParseMacroData(Reserved, sizeof(Reserved));
                CHAR szTemp[2048] = { 0 };
                strcpy_s(szTemp, Reserved);
                long value = 0;
                try {
                    value = atol(szTemp);
                }
                catch (...) {
                    MessageBox(NULL, "Exception in MQ2Melee::Reset", "Debug", MB_OK);
                    //throw();
                }
                if (OptionAbility)
                    OptionAbility->Setup(value, 0);
                else if (OptionValue)
                    *OptionValue = value;

            }
            if (OptionFunction)
                this->OptionFunction();
        }
    }
};

typedef std::map<std::string, Option> Liste;    // declare a type so more easy to refer
Liste     CmdListe;                   // settings from command or ini
Liste     IniListe;                   // settings from ini only
Liste     VarListe;                   // settings from var liste

CHAR      section[256];               // ini section

long      doAGGRO,
doASP,
doASSASSINATE,
doASSAULT,
doBACKOFF,
doBACKSTAB,
doBASH,
doBATTLELEAP,
doBEGGING,
doBLEED,
doBLOODLUST,
doBOASTFUL,
doBVIVI,
doCALLCHALLENGE,
doCLOUD,
doCHALLENGEFOR,
doCOMMANDING,
doCRIPPLE,
doCRYHAVOC,
doCSTRIKE,
doDEFENSE,
doDISARM,
doDOWNFLAG[91],
doDRAGONPUNCH,
doEAGLESTRIKE,
doENRAGE,
doESCAPE,
doENRAGINGKICK,
doEVADE,
doEYEGOUGE,
doFACING,
doFALLS,
doFEIGNDEATH,
doFERALSWIPE,
//doFEROCIOUSKICK,
doFIELDARM,
doFISTSOFWU,
doFCLAW,
doFLYINGKICK,
doFORAGE,
doFRENZY,
doGBLADE,
doGORILLASMASH,
doGUTPUNCH,
doHARMTOUCH,
doHIDE,
doHOLYFLAG[91],
doINFURIATE,
doINTIMIDATION,
doJLTKICKS,
doJUGULAR,
doJOLT,
doKICK,
doKNEESTRIKE,
doKNIFEPLAY,
doLAYHAND,
doLEOPARDCLAW,
doMELEE,
doMEND,
doMONKEY,
doOPFRENZY,
doOPPORTUNISTICSTRIKE,
doPETASSIST,
doPETDELAY,
doPETMEND,
doPETRANGE,
doPETENGAGEHPS,
doPICKPOCKET,
doPINPOINT,
doPOTHEALFAST,
doPOTHEALOVER,
doPROVOKEEND,
doPROVOKEMAX,
doPROVOKEONCE,
doRAGEVOLLEY,
doRAKE,
doRALLOS,
doRANGE,
doRAVENS,
doRESUME,
doRIGHTIND,
doROUNDKICK,
doSELOS,
doSENSETRAP,
doSKILL,
doSLAM,
doSLAPFACE,
doSNEAK,
doSTAB,
doSTAND,
doSTEELY,
doSTICKBREAK,
doSTICKDELAY,
doSTICKMODE,
doSTICKNORANGE,
doSTICKRANGE,
doSTORMBLADES,
doSTRIKE,
doSTRIKEMODE,
doSTUNNING,
doSYNERGY,
doTAUNT,
doTHIEFEYE,
doTHROATJAB,
doTHROWSTONE,
doTIGERCLAW,
doTWISTEDSHANK,
doVIGAXE,
doVIGDAGGER,
doVIGSHURIKEN,
doWITHSTAND,
doYAULP;

long      elARROWS,
elAGGROPRI,
elAGGROSEC,
elMELEEPRI,
elMELEESEC,
elPOKER,
elRANGED,
elSHIELD;

std::string    ifASP,
ifASSAULT,
ifBACKSTAB,
ifBASH,
ifBATTLELEAP,
ifBEGGING,
ifBOASTFUL,
ifBLEED,
ifBLOODLUST,
ifBVIVI,
ifCALLCHALLENGE,
ifCLOUD,
ifCHALLENGEFOR,
ifCOMMANDING,
ifCRIPPLE,
ifCRYHAVOC,
ifCSTRIKE,
ifDEFENSE,
ifDISARM,
ifDRAGONPUNCH,
ifEAGLESTRIKE,
ifEVADE,
ifENRAGINGKICK,
ifEYEGOUGE,
ifFALLS,
ifFERALSWIPE,
//ifFEROCIOUSKICK,
ifFIELDARM,
ifFISTSOFWU,
ifFCLAW,
ifFLYINGKICK,
ifFORAGE,
ifFRENZY,
ifGBLADE,
ifGORILLASMASH,
ifGUTPUNCH,
ifHARMTOUCH,
ifHIDE,
ifINTIMIDATION,
ifJLTKICKS,
ifJOLT,
ifJUGULAR,
ifKICK,
ifKNEESTRIKE,
ifKNIFEPLAY,
ifLAYHAND,
ifLEOPARDCLAW,
ifMEND,
ifMONKEY,
ifOPFRENZY,
ifOPPORTUNISTICSTRIKE,
ifPICKPOCKET,
ifPINPOINT,
ifPOTHEALFAST,
ifPOTHEALOVER,
ifPROVOKE,
ifRAGEVOLLEY,
ifRAKE,
ifRALLOS,
ifRAVENS,
ifRIGHTIND,
ifROUNDKICK,
ifSELOS,
ifSENSETRAP,
ifSLAM,
ifSLAPFACE,
ifSNEAK,
ifSTEELY,
ifSTORMBLADES,
ifSTRIKE,
ifSTUNNING,
ifSYNERGY,
ifTAUNT,
ifTHIEFEYE,
ifTHROATJAB,
ifTHROWSTONE,
ifTIGERCLAW,
ifTWISTEDSHANK,
ifVIGAXE,
ifVIGDAGGER,
ifVIGSHURIKEN,
ifWITHSTAND,
ifYAULP,
DOWNSHIT[91],
HOLYSHIT[91],
StickCMD,
StrikeCMD,
HOLYSHITIF,
DOWNSHITIF;

Ability  idASP,
idASSAULT,
idBACKSTAB,
idBASH,
idBATTLELEAP,
idBEGGING,
idBLEED,
idBLOODLUST,
idBOASTFUL,
idBVIVI,
idCALLCHALLENGE,
idCLOUD,
idCSTRIKE,
idCHALLENGEFOR,
idCOMMANDING,
idCRIPPLE,
idCRYHAVOC,
idDEFENSE,
idDISARM,
idDRAGONPUNCH,
idEAGLESTRIKE,
idESCAPE,
idENRAGINGKICK,
idEYEGOUGE,
idFEIGN[2],
idFERALSWIPE,
//idFEROCIOUSKICK,
idFIELDARM,
idFISTSOFWU,
idFCLAW,
idFLYINGKICK,
idFORAGE,
idFRENZY,
idGBLADE,
idGORILLASMASH,
idGUTPUNCH,
idHARMTOUCH,
idHIDE,
idINTIMIDATION,
idJLTKICKS,
idJOLT,
idJUGULAR,
idKICK,
idKNEESTRIKE,
idKNIFEPLAY,
idLAYHAND,
idLEOPARDCLAW,
idMEND,
idMONKEY,
idOPFRENZY,
idOPPORTUNISTICSTRIKE,
idPETMEND,
idPETASSIST,
idPETDELAY,
idPETENGAGEHPS,
idPETRANGE,
idPICKPOCKET,
idPINPOINT,
idPOTHEALFAST,
idPOTHEALOVER,
idPROVOKE[2],
idRAGEVOLLEY,
idRAKE,
idRALLOS,
idRAVENS,
idRIGHTIND,
idROUNDKICK,
idSELOS,
idSENSETRAP,
idSLAM,
idSLAPFACE,
idSNEAK,
idSTEELY,
idSTORMBLADES,
idSTRIKE,
idSTUN[2],
idSYNERGY,
idTAUNT,
idTHIEFEYE,
idTHROATJAB,
idTHROWSTONE,
idTIGERCLAW,
idTWISTEDSHANK,
idVIGAXE,
idVIGDAGGER,
idVIGSHURIKEN,
idWITHSTAND,
idYAULP;

unsigned long     Shrouded    = false;        // True when shrouded.
bool      Binded              = false;        // Attack Key is Binded?
bool      Loaded              = false;        // Loaded?
bool      Moving              = false;        // Moving?
bool      Immobile            = false;        // Immobilized?
bool      AutoFire            = false;        // True when autofire is on.
bool      ClassCan2HBash      = false;        // True for PAL SHD and WAR
bool      HaveBash            = false;        // Have Two Hand Bash?
bool      HaveHold            = false;        // Have Pet Hold?
bool      HaveGHold           = false;        // Have Pet GHold?
unsigned long     BrokenFD    = 0;            // Timer for Broken Feign Death

float     Travel              = 0.0f;         // Travel Speed?
long      Health              = 0;            // Current Health

unsigned long     MeleeTime   = 0;            // Melee Pulse Timer
long      MeleeTarg           = 0;            // Melee Target ID
long      MeleeType           = 0;            // Melee Target Type
long      MeleeFlee           = 0;            // Melee Target Fleeing?
int64_t      MeleeLife           = 0;         // Melee Target Life %
long      MeleeCast           = 0;            // Melee Target Cast ?
long      MeleeSize           = 0;            // Melee Name Size
char      MeleeName[64]       = { 0 };        // Melee Name

double    MeleeSpeed          = 0.0f;         // Melee Target Speed
double    MeleeBack           = 0.0f;         // Melee Target Angle Back
double    MeleeView           = 0.0f;         // Melee Target Angle View
double    MeleeDist           = 0.0f;         // Melee Distance to Target
double    MeleeKill           = 0.0f;         // Melee Distance to Use Ability

long      onEVENT             = false;        // Ranged=0x8000,Begging=0x2000,PickPocket=0x1000,Feign=0x0040,Hide=0x0020,Backoff=0x0010,Infuriate=0x0002,Enrage=0x0001
long      onSTICK             = false;        // Do Stick? (turn false when stick command is issue)
long      onBELOW             = false;        // Below Flag? (turn false when no more provoke counter)
long      onCHALLENGEFOR      = false;        // Challenge Flag? (turn to false when use once)
long      onENRAGINGKICK      = false;        // Challenge Flag? (turn to false when use once)

unsigned long     NPC_TYPE    = 0x000A;       // NPC TYPE
char      MeleeKey[32];                       // Plugin Melee Key
char      RangeKey[32];                       // Plugin Range Key
unsigned long     SETBINDINGS = 1;            // Set Bindings?

unsigned long     PetInDist   = 0;            // Pet Target in Range
unsigned long     PetOnAttk   = 0;            // Pet Seen Attacking TimeStamp?
unsigned long     PetOnHold   = 0;            // Pet Hold?
unsigned long     PetOnWait   = 0;            // Pet Wait Assist Delay TimeStamp
unsigned long     PetTarget   = 0;            // Pet Target ID

unsigned long     TimerAttk   = 0;            // Timer Attk
unsigned long     TimerBack   = 0;            // Timer BackOff/Escape/Feign
unsigned long     TimerMove   = 0;            // Timer Move
unsigned long     TimerLife   = 0;            // Timer Life (Target his dieing)
unsigned long     TimerFace   = 0;            // Face Time Stamp when started
unsigned long     TimerStik   = 0;            // Stik Time Stamp when started
unsigned long     TimerStun   = 0;            // Timer Stun

long      SwingHits           = 0;            // Total Hits
long      TakenHits           = 0;            // Under Hits

long      doHOLY              = 0;            // Holy Shits while meleeing?
long      doDOWN              = 0;            // Down Shits while downtime?

Blech    *pMeleeEvent         = 0;            // blech event list
bool      IdlingArray[256];                   // animation array while idle
bool      AttackArray[256];                   // animation array while attacking

long      SaveList[50];                       // saved event list
long      SaveIndx;                           // saved event counters

unsigned long     HiddenTimer = 0;            // Last TimeStamp for Hide
unsigned long     SilentTimer = 0;            // Last TimeStamp for Sneak

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class MQ2MeleeType *pMeleeTypes = 0;
class MQ2MeleeType : public MQ2Type {
    long isKill;
    char Tempos[MAX_STRING];
public:
    enum Information {
        Enable = 1,
        Combat = 2,
        Casted = 3,
        Engage = 4,
        Status = 5,
        Target = 6,
        DiscID = 7,
        Enrage = 8,
        Infuriate = 9,
        AggroMode = 10,
        MeleeMode = 11,
        RangeMode = 12,
        BackAngle = 13,
        ViewAngle = 14,
        Immobilize = 15,
        Ammunition = 16,
        BackStabbing = 17,
        GotAggro = 18,
        Hidden = 19,
        Silent = 20,
        NumHits = 21,
        XTaggro = 22,
    };
    MQ2MeleeType() :MQ2Type("Melee") {
        TypeMember(Enable);
        TypeMember(Combat);
        TypeMember(Casted);
        TypeMember(Status);
        TypeMember(Target);
        TypeMember(DiscID);
        TypeMember(GotAggro);
        TypeMember(AggroMode);
        TypeMember(MeleeMode);
        TypeMember(RangeMode);
        TypeMember(Enrage);
        TypeMember(Infuriate);
        TypeMember(BackAngle);
        TypeMember(ViewAngle);
        TypeMember(Immobilize);
        TypeMember(Ammunition);
        TypeMember(BackStabbing);
        TypeMember(Hidden);
        TypeMember(Silent);
        TypeMember(NumHits);
        TypeMember(XTaggro);
    }

    virtual bool MQ2MeleeType::GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest) override {
        auto pMember = MQ2MeleeType::FindMember(Member);
        isKill = false; if (doSKILL) if (MeleeTarg) isKill = true;
        if (pMember)
        {
            switch ((Information)pMember->ID)
            {
                case Enable:
                    Dest.DWord = doSKILL;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case Combat:
                    Dest.DWord = isKill;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case Casted:
                    Dest.Int = (isKill && MeleeCast) ? labs((unsigned long)clock() - MeleeCast) : 60000;
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case Status:
                    Tempos[0] = 0;
                    if (isKill) strcat_s(Tempos, "ENGAGED ");
                    else strcat_s(Tempos, "WAITING ");
                    if (pEverQuestInfo->bAutoAttack) strcat_s(Tempos, "MELEE ");
                    else if (onEVENT & 0x8000) strcat_s(Tempos, "RANGE ");
                    if (onEVENT & 0x0001) strcat_s(Tempos, "ENRAGE ");
                    if (onEVENT & 0x0002) strcat_s(Tempos, "INFURIATE ");
                    if (onEVENT & 0x0010) strcat_s(Tempos, "BACKING ");
                    if (onEVENT & 0x0020) strcat_s(Tempos, "ESCAPING ");
                    if (onEVENT & 0x0040) strcat_s(Tempos, "FEIGNING ");
                    if (onEVENT & 0x0200) strcat_s(Tempos, "EVADING ");
                    if (onEVENT & 0x0400) strcat_s(Tempos, "FALLING ");
                    if (onEVENT & 0x1000) strcat_s(Tempos, "STEALING ");
                    if (onEVENT & 0x2000) strcat_s(Tempos, "BEGGING ");
                    Dest.Type = mq::datatypes::pStringType;
                    Dest.Ptr = &Tempos[0];
                    return true;
                case Target:
                    Dest.Int = isKill ? MeleeTarg : 0;
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case DiscID:
                    Dest.DWord = Discipline();
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case GotAggro:
                    Dest.DWord = (Aggroed(MeleeTarg) > 0);
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case AggroMode:
                    Dest.DWord = doAGGRO;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case MeleeMode:
                    Dest.DWord = doMELEE;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case RangeMode:
                    Dest.DWord = doRANGE;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case Enrage:
                    Dest.DWord = onEVENT & 0x0001;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case Infuriate:
                    Dest.DWord = onEVENT & 0x0002;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case BackAngle:
                    Dest.Float = pTarget ? AngularDistance(pTarget->Heading, SpawnMe()->Heading) : 0.0f;
                    Dest.Type = mq::datatypes::pFloatType;
                    return true;
                case ViewAngle:
                    Dest.Float = pTarget ? (float)AngularHeading(SpawnMe(), pTarget) : 0.0f;
                    Dest.Type = mq::datatypes::pFloatType;
                    return true;
                case Immobilize:
                    Dest.DWord = Immobile;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case Ammunition:
                    Dest.DWord = CountItemByID(elARROWS);
                    if (CONTENTS* r = GetPcProfile()->GetInventorySlot(InvSlot_Ammo))
                        if (r->GetID() != elARROWS)
                            if (r->GetItemClass() == ItemClass_Thrown || r->GetItemClass() == ItemClass_Ammo || r->GetItemClass() == ItemClass_Arrow)
                                Dest.DWord = CountItemByID(r->GetID());
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case BackStabbing:
                    Dest.DWord = doBACKSTAB;
                    Dest.Type = mq::datatypes::pBoolType;
                    return true;
                case Hidden:
                    Dest.Int = TimeSince(HiddenTimer);
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case Silent:
                    Dest.Int = TimeSince(SilentTimer);
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case NumHits:
                    Dest.DWord = SwingHits;
                    Dest.Type = mq::datatypes::pIntType;
                    return true;
                case XTaggro:
                {
                    Dest.DWord = true;
                    Dest.Type = mq::datatypes::pBoolType;
                    if (pLocalPC) {
                        ExtendedTargetList* etl = pLocalPC->pXTargetMgr;
                        DWORD x = 0;
                        for (int n = 0; n < etl->XTargetSlots.Count; n++)
                        {
                            ExtendedTargetSlot& xts = pLocalPC->pXTargetMgr->XTargetSlots[n];
                            if (xts.xTargetType == XTARGET_AUTO_HATER && xts.XTargetSlotStatus)
                            {
                                x++;
                            }
                        }
                        if (x > 1) {
                            if (pAggroInfo) {
                                for (int i = 0; i < etl->XTargetSlots.Count; i++) {
                                    ExtendedTargetSlot& xts = etl->XTargetSlots[i];
                                    if (DWORD spID = xts.SpawnID) {
                                        if (PSPAWNINFO pSpawn = GetSpawnByID(spID)) {
                                            if (pTarget && pTarget->SpawnID == pSpawn->SpawnID)
                                                continue;
                                            if (pSpawn->Type == SPAWN_NPC && xts.xTargetType == XTARGET_AUTO_HATER) {
                                                DWORD agropct = pAggroInfo->aggroData[AD_xTarget1 + i].AggroPct;
                                                //WriteChatf("Checking aggro on %s its %d",xta->pXTargetData[i].Name,agropct);
                                                if (agropct < 100) {
                                                    Dest.DWord = false;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return true;
            }
        }
        strcpy_s(Tempos, "NULL");
        Dest.Type = mq::datatypes::pStringType;
        Dest.Ptr = &Tempos[0];
        return true;
    }
    bool ToString(MQVarPtr VarPtr, char* Destination) override {
        strcpy_s(Destination, MAX_STRING, "TRUE");
        return true;
    }
};

bool DataMelee(const char* Index, MQTypeVar &Dest) {
    Dest.Type = pMeleeTypes;
    Dest.DWord = 1;
    return true;
}

bool datameleemvb(const char* Index, MQTypeVar &Dest) {
    Dest.Type = mq::datatypes::pIntType;
    Dest.Int = NOID;
    Liste::iterator c;
    if (VarListe.end() != (c = VarListe.find(Index)))
        Dest.Int = (*c).second.Ready();
    return true;
}

bool datameleemvi(const char* Index, MQTypeVar &Dest) {
    Dest.Type = mq::datatypes::pIntType;
    Dest.DWord = 0;
    Liste::iterator c;
    if (CmdListe.end() != (c = CmdListe.find(Index))) {
        if (long *OptionValue = (long*)(*c).second.Value()) Dest.DWord = *OptionValue;
        return true;
    }
    if (VarListe.end() != (c = VarListe.find(Index))) {
        if (long *OptionValue = (long*)(*c).second.Value()) Dest.DWord = *OptionValue;
        return true;
    }
    return true;
}

bool datameleemvs(const char* Index, MQTypeVar &Dest) {
    Dest.Type = mq::datatypes::pStringType;
    Dest.Ptr = &Workings;
    Liste::iterator c = IniListe.find(Index);
    if (IniListe.end() != c) {
        if (std::string *OptionShow = (std::string*)(*c).second.Value())
            strcpy_s(Workings, OptionShow->c_str());
    }
    else Workings[0] = 0;
    return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void AbilityFind(Ability *thisone, infodata *first, ...) {
    infodata *c = first;
    va_list  marker;
    va_start(marker, first);
    while (c) {
        thisone->Setup(c->i, c->t);
        if (thisone->Avail()) break;
        thisone->ID = 0;
        c = va_arg(marker, infodata *);
    }
    va_end(marker);
}

void AttackON() {
    if (pEverQuestInfo->bAutoAttack || onEVENT & 0xFFF7 || IsFeigning() || !doMELEE || !TargetID(MeleeTarg)) return;
    EzCommand("/attack on");
    TimerAttk = (unsigned long)clock();
}

void AttackOFF() {
    if (pEverQuestInfo->bAutoAttack) EzCommand("/attack off");
}

bool BashCheck() {
    if (ShieldType(ContSecondary()))
        return true;

    if (ClassCan2HBash) {                                 // class is Pal/Shd/War?
        PCONTENTS pContPri = ContPrimary();
        if (pContPri  && TwohandType(pContPri) ) {        // Primary is a 2H?
            if (HaveBash)                                 // Have the 2H bash aa?
                return true;

            PITEMINFO pItem = GetItemFromContents(pContPri);
            if ( pItem && pItem->Worn.SpellID == 2835 )   // Knights epic1.0 have worn spell effect: Two-Hand Bash, ID 2835
                return true;
        }
    }

    return (elSHIELD && CountItemByID(elSHIELD) && OkayToEquip(Giant));
}

void BashPress() {
    long savedpri = 0; long savedoff = 0; int got2hand = false;
    if (CONTENTS* pri = ContPrimary()) {
        got2hand = TwohandType(pri);
        savedpri = GetItemFromContents(pri)->ItemNumber;
    }
    if (CONTENTS* off = ContSecondary()) savedoff = GetItemFromContents(off)->ItemNumber;
    if (elSHIELD && CountItemByID(elSHIELD) && OkayToEquip(Giant)) Equip(elSHIELD, InvSlot_Secondary);
    if (ShieldType(ContSecondary()) || (got2hand && HaveBash)) idBASH.Press();
    if (savedoff) Equip(savedoff, InvSlot_Secondary);
    if (savedpri) Equip(savedpri, InvSlot_Primary);
}

bool isAAPurchased(const char* AAName) {
    if (!pLocalPlayer)
        return false;

    if(const ALTABILITY* pAltAbility = GetAAByIdWrapper(GetAAIndexByName(AAName), pLocalPlayer->Level))
    {
        return pAltAbility->CurrentRank > 0;
    }
    return false;
}

void Configure() {
    PcProfile* pChar2 = GetPcProfile();
    if (!pChar2)
        return;
    long Class = pChar2->Class;
    long Races = pChar2->Race;
    long Level = pChar2->Level;
    int SOValue = 0;
    sprintf_s(INIFileName, "%s\\%s_%s.ini", gPathConfig, GetServerShortName(), pLocalPC->Name);
    sprintf_s(section, "%s_%d_%s_%s", PLUGIN_NAME, Level, pEverQuest->GetRaceDesc(Races), pEverQuest->GetClassDesc(Class));
    Shrouded = pChar2->Shrouded; if (!Shrouded) section[strlen(PLUGIN_NAME)] = 0;
    BuffMax = GetCharMaxBuffSlots();
    HaveHold = GetAAIndexByName("Pet Discipline") ? true : false;
    if (HaveHold) {
        if (int pdindex = GetAAIndexByName("Pet Discipline")) {
            if (int pointsspentinpd = AAPoint(pdindex)) {
                if (pointsspentinpd >= 6) {
                    HaveGHold = true;
                }
            }
        }
    }

    if (Class == Warrior)
    {
        HaveBash = isAAPurchased("Two-Handed Bash");
        ClassCan2HBash = true;
    }
    else if (Class == Paladin || Class == Shadowknight)
    {
        HaveBash = isAAPurchased("Improved Bash");
        ClassCan2HBash = true;
    }

    BardClass = false;
    BerserkerClass = false;
    MonkClass = false;
    RogueClass = false;
    char keys[MAX_STRING * 5];
    char temp[MAX_STRING];
    Liste::iterator c, i;
    Liste::iterator ec = CmdListe.end();
    Liste::iterator ei = IniListe.end();
    for (c = CmdListe.begin(); c != ec; c++) (*c).second.Reset();
    for (i = IniListe.begin(); i != ei; i++) (*i).second.Reset();

    idASP.Setup(0, 0);
    idASSAULT.Setup(0, 0);
    idBACKSTAB.Setup(0, 0);
    idBASH.Setup(0, 0);
    idBATTLELEAP.Setup(0, 0);
    idBEGGING.Setup(0, 0);
    idBLEED.Setup(0, 0);
    idBLOODLUST.Setup(0, 0);
    idBOASTFUL.Setup(0, 0);
    idBVIVI.Setup(0, 0);
    idCALLCHALLENGE.Setup(0, 0);
    idCLOUD.Setup(0, 0);
    idCHALLENGEFOR.Setup(0, 0);
    idCOMMANDING.Setup(0, 0);
    idCRIPPLE.Setup(0, 0);
    idCRYHAVOC.Setup(0, 0);
    idCSTRIKE.Setup(0, 0);
    idDEFENSE.Setup(0, 0);
    idDISARM.Setup(0, 0);
    idDRAGONPUNCH.Setup(0, 0);
    idEAGLESTRIKE.Setup(0, 0);
    idESCAPE.Setup(0, 0);
    idENRAGINGKICK.Setup(0, 0);
    idEYEGOUGE.Setup(0, 0);
    idFCLAW.Setup(0, 0);
    idFEIGN[0].Setup(0, 0);
    idFEIGN[1].Setup(0, 0);
    idFIELDARM.Setup(0, 0);
    idFISTSOFWU.Setup(0, 0);
    //idFEROCIOUSKICK.Setup(0, 0);
    idFLYINGKICK.Setup(0, 0);
    idFORAGE.Setup(0, 0);
    idFRENZY.Setup(0, 0);
    idGBLADE.Setup(0, 0);
    idGORILLASMASH.Setup(0, 0);
    idGUTPUNCH.Setup(0, 0);
    idHARMTOUCH.Setup(0, 0);
    idHIDE.Setup(0, 0);
    idINTIMIDATION.Setup(0, 0);
    idJLTKICKS.Setup(0, 0);
    idJOLT.Setup(0, 0);
    idJUGULAR.Setup(0, 0);
    idKICK.Setup(0, 0);
    idKNEESTRIKE.Setup(0, 0);
    idKNIFEPLAY.Setup(0, 0);
    idLAYHAND.Setup(0, 0);
    idLEOPARDCLAW.Setup(0, 0);
    idMEND.Setup(0, 0);
    idMONKEY.Setup(0, 0);
    idOPFRENZY.Setup(0, 0);
    idOPPORTUNISTICSTRIKE.Setup(0, 0);
    idPETASSIST.Setup(0, 0);
    idPETDELAY.Setup(0, 0);
    idPETRANGE.Setup(0, 0);
    idPETMEND.Setup(0, 0);
    idPICKPOCKET.Setup(0, 0);
    idPINPOINT.Setup(0, 0);
    idPOTHEALFAST.Setup(0, 0);
    idPOTHEALOVER.Setup(0, 0);
    idPROVOKE[0].Setup(0, 0);
    idPROVOKE[1].Setup(0, 0);
    idRAGEVOLLEY.Setup(0, 0);
    idRAKE.Setup(0, 0);
    idRALLOS.Setup(0, 0);
    idRAVENS.Setup(0, 0);
    idRIGHTIND.Setup(0, 0);
    idROUNDKICK.Setup(0, 0);
    idSELOS.Setup(0, 0);
    idSENSETRAP.Setup(0, 0);
    idSLAM.Setup(0, 0);
    idSLAPFACE.Setup(0, 0);
    idSNEAK.Setup(0, 0);
    idSTEELY.Setup(0, 0);
    idSTORMBLADES.Setup(0, 0);
    idSTRIKE.Setup(0, 0);
    idSTUN[0].Setup(0, 0);
    idSTUN[1].Setup(0, 0);
    idSYNERGY.Setup(0, 0);
    idTAUNT.Setup(0, 0);
    idTHIEFEYE.Setup(0, 0);
    idTHROATJAB.Setup(0, 0);
    idTHROWSTONE.Setup(0, 0);
    idTIGERCLAW.Setup(0, 0);
    idTWISTEDSHANK.Setup(0, 0);
    idVIGAXE.Setup(0, 0);
    idVIGDAGGER.Setup(0, 0);
    idVIGSHURIKEN.Setup(0, 0);
    idWITHSTAND.Setup(0, 0);
    idYAULP.Setup(0, 0);

    AbilityFind(&idBASH, &sbash, 0);
    AbilityFind(&idBEGGING, &sbegging, 0);
    AbilityFind(&idDISARM, &sdisarm, 0);
    AbilityFind(&idFORAGE, &sforage, 0);
    AbilityFind(&idHIDE, &shide, 0);
    AbilityFind(&idINTIMIDATION, &sintim, 0);
    AbilityFind(&idKICK, &skick, 0);
    AbilityFind(&idMEND, &smend, 0);
    AbilityFind(&idSENSETRAP, &ssensetr, 0);
    AbilityFind(&idSLAM, &sslam, 0);
    AbilityFind(&idSNEAK, &ssneak, 0);
    AbilityFind(&idTAUNT, &staunt, 0);
    AbilityFind(&idBATTLELEAP, &btlleap, 0);
    AbilityFind(&idTHROWSTONE, &tstone, 0);

    AbilityFind(&idPOTHEALOVER, &potover21, &potover20, &potover19, &potover18, &potover17, &potover16, &potover15, &potover14, &potover13, &potover12, &potover11, &potover10, &potover9, &potover8, &potover7, &potover6, &potover5, &potover4, &potover3, &potover2, &potover1, &potover0, 0);
    AbilityFind(&idPOTHEALFAST, &potfast21, &potfast20, &potfast19, &potfast18, &potfast17, &potfast16, &potfast15, &potfast14, &potfast13, &potfast12, &potfast11, &potfast10, &potfast9, &potfast8, &potfast7, &potfast6, &potfast5, &potfast4, &potfast3, &potfast2, &potfast1, &potfast0, 0);
    doSTAB = 0;
    switch (Class) {
    case  Warrior: // WAR
        AbilityFind(&idPROVOKE[1], &prowar55, &prowar54, &prowar53, &prowar52, &prowar51, &prowar50, &prowar49, &prowar48, &prowar47, &prowar46, &prowar45, &prowar44, &prowar43, &prowar42, &prowar41, &prowar40, &prowar39, &prowar38, &prowar37, &prowar36, &prowar35, &prowar34, &prowar33, &prowar32, &prowar31, &prowar30, &prowar29, &prowar28, &prowar27, &prowar26, &prowar25, &prowar24, &prowar23, &prowar22, &prowar21, &prowar20, &prowar19, &prowar18, &prowar17, &prowar16, &prowar15, &prowar14, &prowar13, &prowar12, &prowar11, &prowar10, &prowar9, &prowar8, &prowar7, &prowar6, &prowar5, &prowar4, &prowar3, &prowar2, &prowar1, 0);
        AbilityFind(&idOPPORTUNISTICSTRIKE, &opstrke21, &opstrke20, &opstrke19, &opstrke18, &opstrke17, &opstrke16, &opstrke15, &opstrke14, &opstrke13, &opstrke12, &opstrke11, &opstrke10, &opstrke9, &opstrke8, &opstrke7, &opstrke6, &opstrke5, &opstrke4, &opstrke3, &opstrke2, &opstrke1, 0);
        AbilityFind(&idFIELDARM, &fieldarm24, &fieldarm23, &fieldarm22, &fieldarm21, &fieldarm20, &fieldarm19, &fieldarm18, &fieldarm17, &fieldarm16, &fieldarm15, &fieldarm14, &fieldarm13, &fieldarm12, &fieldarm11, &fieldarm10, &fieldarm9, &fieldarm8, &fieldarm7, &fieldarm6, &fieldarm5, &fieldarm4, &fieldarm3, &fieldarm2, &fieldarm1, 0);
        AbilityFind(&idDEFENSE, &defense24, &defense23, &defense22, &defense21, &defense20, &defense19, &defense18, &defense17, &defense16, &defense15, &defense14, &defense13, &defense12, &defense11, &defense10, &defense9, &defense8, &defense7, &defense6, &defense5, &defense4, &defense3, &defense2, &defense1, 0);
        AbilityFind(&idTHROATJAB, &throat3, &throat2, &throat1, 0);
        AbilityFind(&idKNEESTRIKE, &kneestrike, 0);
        AbilityFind(&idGUTPUNCH, &gutpunch, 0);
        AbilityFind(&idCALLCHALLENGE, &callchal, 0);
        AbilityFind(&idCOMMANDING, &commanding, 0);
        break;
    case Cleric: // CLR
        AbilityFind(&idYAULP, &yaulp, 0);
        break;
    case  Paladin: // PAL
        AbilityFind(&idLAYHAND, &layhand, 0);
        AbilityFind(&idCHALLENGEFOR, &honor30, &honor29, &honor28, &honor27, &honor26, &honor25, &honor24, &honor23, &honor22, &honor21, &honor20, &honor19, &honor18, &honor17, &honor16, &honor15, &honor14, &honor13, &honor12, &honor11, &honor10, &honor9, &honor8, &honor7, &honor6, &honor5, &honor4, &honor3, &honor2, &honor1, 0);        AbilityFind(&idPROVOKE[0], &stunaas3, &stunaas2, &stunaas1, 0);
        AbilityFind(&idPROVOKE[1], &stunpal52, &stunpal51, &stunpal50, &stunpal49, &stunpal48, &stunpal47, &stunpal46, &stunpal45, &stunpal44, &stunpal43, &stunpal42, &stunpal41, &stunpal40, &stunpal39, &stunpal38, &stunpal37, &stunpal36, &stunpal35, &stunpal34, &stunpal33, &stunpal32, &stunpal31, &stunpal30, &stunpal29, &stunpal28, &stunpal27, &stunpal26, &stunpal25, &stunpal24, &stunpal23, &stunpal22, &stunpal21, &stunpal20, &stunpal19, &stunpal18, &stunpal17, &stunpal16, &stunpal15, &stunpal14, &stunpal13, &stunpal12, &stunpal11, &stunpal10, &stunpal9, &stunpal8, &stunpal7, &stunpal6, &stunpal5, &stunpal4, &stunpal3, &stunpal2, &stunpal1, 0);
        AbilityFind(&idSTEELY, &steely24, &steely23, &steely22, &steely21, &steely20, &steely19, &steely18, &steely17, &steely16, &steely15, &steely14, &steely13, &steely12, &steely11, &steely10, &steely9, &steely8, &steely7, &steely6, &steely5, &steely4, &steely3, &steely2, &steely1, 0);
        AbilityFind(&idSTUN[0], &stunaas3, &stunaas2, &stunaas1, 0);
        AbilityFind(&idSTUN[1], &stunpal55, &stunpal54, &stunpal53, &stunpal52, &stunpal51, &stunpal50, &stunpal49, &stunpal48, &stunpal47, &stunpal46, &stunpal45, &stunpal44, &stunpal43, &stunpal42, &stunpal41, &stunpal40, &stunpal39, &stunpal38, &stunpal37, &stunpal36, &stunpal35, &stunpal34, &stunpal33, &stunpal32, &stunpal31, &stunpal30, &stunpal29, &stunpal28, &stunpal27, &stunpal26, &stunpal25, &stunpal24, &stunpal23, &stunpal22, &stunpal21, &stunpal20, &stunpal19, &stunpal18, &stunpal17, &stunpal16, &stunpal15, &stunpal14, &stunpal13, &stunpal12, &stunpal11, &stunpal10, &stunpal9, &stunpal8, &stunpal7, &stunpal6, &stunpal5, &stunpal4, &stunpal3, &stunpal2, &stunpal1, 0);
        AbilityFind(&idWITHSTAND, &withstand24, &withstand23, &withstand22, &withstand21, &withstand20, &withstand19, &withstand18, &withstand17, &withstand16, &withstand15, &withstand14, &withstand13, &withstand12, &withstand11, &withstand10, &withstand9, &withstand8, &withstand7, &withstand6, &withstand5, &withstand4, &withstand3, &withstand2, &withstand1, 0);
        AbilityFind(&idRIGHTIND, &rightidg18, &rightidg17, &rightidg16, &rightidg15, &rightidg14, &rightidg13, &rightidg12, &rightidg11, &rightidg10, &rightidg9, &rightidg8, &rightidg7, &rightidg6, &rightidg5, &rightidg4, &rightidg3, &rightidg2, &rightidg1, 0);
        break;
    case  Ranger: // RNG
        AbilityFind(&idJOLT, &joltrng2, &joltrng1, 0);
        AbilityFind(&idJLTKICKS, &jltkicks30, &jltkicks29, &jltkicks28, &jltkicks27, &jltkicks26, &jltkicks25, &jltkicks24, &jltkicks23, &jltkicks22, &jltkicks21, &jltkicks20, &jltkicks19, &jltkicks18, &jltkicks17, &jltkicks16, &jltkicks15, &jltkicks14, &jltkicks13, &jltkicks12, &jltkicks11, &jltkicks10, &jltkicks9, &jltkicks8, &jltkicks7, &jltkicks6, &jltkicks5, &jltkicks4, &jltkicks3, &jltkicks2, &jltkicks1, 0);
        AbilityFind(&idENRAGINGKICK, &enragingkick18, &enragingkick17, &enragingkick16, &enragingkick15, &enragingkick14, &enragingkick13, &enragingkick12, &enragingkick11, &enragingkick10, &enragingkick9, &enragingkick8, &enragingkick7, &enragingkick6, &enragingkick5, &enragingkick4, &enragingkick3, &enragingkick2, &enragingkick1, 0);
        AbilityFind(&idSTORMBLADES, &bladesrng27, &bladesrng26, &bladesrng25, &bladesrng21, &bladesrng20, &bladesrng19, &bladesrng18, &bladesrng17, &bladesrng16, &bladesrng15, &bladesrng14, &bladesrng13, &bladesrng12, &bladesrng11, &bladesrng10, &bladesrng9, &bladesrng8, &bladesrng7, &bladesrng6, &bladesrng5, &bladesrng4, &bladesrng3, &bladesrng2, &bladesrng1, 0);
        //AbilityFind(&idFEROCIOUSKICK, &ferociouskick, 0);
        break;
    case  Shadowknight: // SHD
        AbilityFind(&idFEIGN[0], &feigns24, &feigns23, &feigns22, &feigns21, &feigns20, &feigns19, &feigns18, &feigns17, &feigns16, &feigns15, &feigns14, &feigns13, &feigns12, &feigns11, &feigns10, &feigns9, &feigns8, &feigns7, &feigns6, &feigns5, &feigns4, &feigns3, &feigns2, &feigns1, 0);
        AbilityFind(&idFEIGN[1], &feigndp, 0);
        AbilityFind(&idHARMTOUCH, &harmtouch, 0);
        AbilityFind(&idGBLADE, &gblade18, &gblade17, &gblade16, &gblade15, &gblade14, &gblade13, &gblade12, &gblade11, &gblade10, &gblade9, &gblade8, &gblade7, &gblade6, &gblade5, &gblade4, &gblade3, &gblade2, &gblade1, 0);
        AbilityFind(&idPROVOKE[1], &terror36, &terror35, &terror34, &terror33, &terror32, &terror31, &terror30, &terror29, &terror28, &terror27, &terror26, &terror25, &terror24, &terror23, &terror22, &terror21, &terror20, &terror19, &terror18, &terror17, &terror16, &terror15, &terror14, &terror13, &terror12, &terror11, &terror10, &terror9, &terror8, &terror7, &terror6, &terror5, &terror4, &terror3, &terror2, &terror1, 0);
        AbilityFind(&idCHALLENGEFOR, &power30, &power29, &power28, &power27, &power26, &power25, &power24, &power23, &power22, &power21, &power20, &power19, &power18, &power17, &power16, &power15, &power14, &power13, &power12, &power11, &power10, &power9, &power8, &power7, &power6, &power5, &power4, &power3, &power2, &power1, 0);
        AbilityFind(&idSTEELY, &steely24, &steely23, &steely22, &steely21, &steely20, &steely19, &steely18, &steely17, &steely16, &steely15, &steely14, &steely13, &steely12, &steely11, &steely10, &steely9, &steely8, &steely7, &steely6, &steely5, &steely4, &steely3, &steely2, &steely1, 0);
        AbilityFind(&idWITHSTAND, &withstand24, &withstand23, &withstand22, &withstand21, &withstand20, &withstand19, &withstand18, &withstand17, &withstand16, &withstand15, &withstand14, &withstand13, &withstand12, &withstand11, &withstand10, &withstand9, &withstand8, &withstand7, &withstand6, &withstand5, &withstand4, &withstand3, &withstand2, &withstand1, 0);
        break;
    case  Monk: // MNK
        MonkClass = true;
        AbilityFind(&idFISTSOFWU, &fistswu, 0);
        AbilityFind(&idDRAGONPUNCH, &sdrpunch, 0);
        AbilityFind(&idEAGLESTRIKE, &sestrike, 0);
        AbilityFind(&idLEOPARDCLAW, &leop32, &leop31, &leop30, &leop29, &leop28, &leop27, &leop26, &leop25, &leop24, &leop23, &leop22, &leop21, &leop20, &leop19, &leop18, &leop17, &leop16, &leop15, &leop14, &leop13, &leop12, &leop11, &leop10, &leop9, &leop8, &leop7, &leop6, &leop5, &leop4, &leop3, &leop2, &leop1, 0);
        AbilityFind(&idTIGERCLAW, &stigclaw, 0);
        AbilityFind(&idROUNDKICK, &srndkick, 0);
        AbilityFind(&idFLYINGKICK, &sflykick, 0);
        AbilityFind(&idFEIGN[0], &sfeign, 0);
        AbilityFind(&idFEIGN[1], &feignid, 0);
        AbilityFind(&idPROVOKE[0], &stunmnk2, &stunmnk1, 0);
        AbilityFind(&idSTUN[0], &stunmnk2, &stunmnk1, 0);
        AbilityFind(&idSYNERGY, &synergy24, &synergy23, &synergy22, &synergy21, &synergy20, &synergy19, &synergy18, &synergy17, &synergy16, &synergy15, &synergy14, &synergy13, &synergy12, &synergy11, &synergy10, &synergy9, &synergy8, &synergy7, &synergy6, &synergy5, &synergy4, &synergy3, &synergy2, &synergy1, 0);
        AbilityFind(&idVIGSHURIKEN, &vigmnk3, &vigmnk2, &vigmnk1, 0);
        AbilityFind(&idCLOUD, &cloud12, &cloud11, &cloud10, &cloud9, &cloud8, &cloud7, &cloud6, &cloud5, &cloud5, &cloud4, &cloud3, &cloud2, &cloud1, 0);
        AbilityFind(&idMONKEY, &monkey3, &monkey2, &monkey1, 0);
        break;
    case  Bard: // BRD
        BardClass = true;
        AbilityFind(&idSELOS, &selos, 0);
        AbilityFind(&idBOASTFUL, &boastful, 0);
        break;
    case  Rogue: // ROG
        AbilityFind(&idTHIEFEYE, &thiefeye4, &thiefeye3, &thiefeye2, &thiefeye1, 0);
        AbilityFind(&idSTRIKE, &strike31, &strike30, &strike29, &strike28, &strike27, &strike26, &strike25, &strike24, &strike23, &strike22, &strike21, &strike20, &strike19, &strike18, &strike17, &strike16, &strike15, &strike14, &strike13, &strike12, &strike11, &strike10, &strike9, &strike8, &strike7, &strike6, &strike5, &strike4, &strike3, &strike2, &strike1, 0);
        AbilityFind(&idKNIFEPLAY, &knifeplay3, &knifeplay2, &knifeplay1, 0);
        AbilityFind(&idBLEED, &bleed24, &bleed23, &bleed22, &bleed21, &bleed20, &bleed19, &bleed18, &bleed17, &bleed16, &bleed15, &bleed14, &bleed13, &bleed12, &bleed11, &bleed10, &bleed9, &bleed8, &bleed7, &bleed6, &bleed5, &bleed4, &bleed3, &bleed2, &bleed1, 0);
        AbilityFind(&idVIGDAGGER, &vigrog24, &vigrog23, &vigrog22, &vigrog21, &vigrog20, &vigrog19, &vigrog18, &vigrog17, &vigrog16, &vigrog15, &vigrog14, &vigrog13, &vigrog12, &vigrog11, &vigrog10, &vigrog9, &vigrog8, &vigrog7, &vigrog6, &vigrog5, &vigrog4, &vigrog3, &vigrog2, &vigrog1, 0);
        AbilityFind(&idASSAULT, &assault24, &assault23, &assault22, &assault21, &assault20, &assault19, &assault18, &assault17, &assault16, &assault15, &assault14, &assault13, &assault12, &assault11, &assault10, &assault9, &assault8, &assault7, &assault6, &assault5, &assault4, &assault3, &assault2, &assault1, 0);
        AbilityFind(&idPINPOINT, &pinpoint24, &pinpoint23, &pinpoint22, &pinpoint21, &pinpoint20, &pinpoint19, &pinpoint18, &pinpoint17, &pinpoint16, &pinpoint15, &pinpoint14, &pinpoint13, &pinpoint12, &pinpoint11, &pinpoint10, &pinpoint9, &pinpoint8, &pinpoint7, &pinpoint6, &pinpoint5, &pinpoint4, &pinpoint3, &pinpoint2, &pinpoint1, 0);
        AbilityFind(&idJUGULAR, &jugular27, &jugular26, &jugular25, &jugular24, &jugular23, &jugular22, &jugular21, &jugular20, &jugular19, &jugular18, &jugular17, &jugular16, &jugular15, &jugular14, &jugular13, &jugular12, &jugular11, &jugular10, &jugular9, &jugular8, &jugular7, &jugular6, &jugular5, &jugular4, &jugular3, &jugular2, &jugular1, 0);
        AbilityFind(&idESCAPE, &escape, 0);
        AbilityFind(&idTWISTEDSHANK, &twisted, 0);
        AbilityFind(&idBACKSTAB, &sbkstab, 0);
        AbilityFind(&idPICKPOCKET, &sppocket, 0);
        // Seized Opportunity position fix 05/01/2011 (htw)
        SOValue = AAPoint(GetAAIndexByName("Seized Opportunity"));
        if (SOValue >= 74)
            doSTAB = 512;
        else if (SOValue >= 60)
            doSTAB = 256;
        else if (SOValue >= 39)
            doSTAB = 192;
        else if (SOValue >= 18)
            doSTAB = 128;
        else
            doSTAB = 64;
        break;
    case Necromancer: // NEC
        AbilityFind(&idFEIGN[0], &feign_n9, &feign_n8, &feign_n7, &feign_n6, &feign_n5, &feign_n4, &feign_n3, &feign_n2, &feign_n1, &feigns3, &feigns2, &feigns1, 0);
        AbilityFind(&idFEIGN[1], &feigndp, 0);
        AbilityFind(&idPETMEND, &mendpet2, &mendpet1, 0);
        break;
    case Mage: // MAG
        AbilityFind(&idPETMEND, &mendpet2, &mendpet1, 0);
        break;
    case Beastlord: // BST
        AbilityFind(&idRAKE, &rake28, &rake27, &rake26, &rake25, &rake24, &rake23, &rake22, &rake21, &rake20, &rake19, &rake18, &rake17, &rake16, &rake15, &rake14, &rake13, &rake12, &rake11, &rake10, &rake9, &rake8, &rake7, &rake6, &rake5, &rake4, &rake3, &rake2, &rake1, 0);
        AbilityFind(&idFERALSWIPE, &feral1, 0);
        AbilityFind(&idPETMEND, &mendpet1, &mendpet2, 0);
        AbilityFind(&idJOLT, &joltbst1, 0);
        AbilityFind(&idFEIGN[0], &feign_bst, 0);
        AbilityFind(&idASP, &asp, 0);
        AbilityFind(&idCSTRIKE, &cstrike, 0);
        AbilityFind(&idRAVENS, &ravens, 0);
        AbilityFind(&idFCLAW, &fclaw24, &fclaw23, &fclaw22, &fclaw21, &fclaw20, &fclaw19, &fclaw18, &fclaw17, &fclaw16, &fclaw15, &fclaw14, &fclaw13, &fclaw12, &fclaw11, &fclaw10, &fclaw9, &fclaw8, &fclaw7, &fclaw6, &fclaw5, &fclaw4, &fclaw3, &fclaw2, &fclaw1, 0);
        AbilityFind(&idBVIVI, &bvivi15, &bvivi14, &bvivi13, &bvivi12, &bvivi11, &bvivi10, &bvivi9, &bvivi8, &bvivi7, &bvivi6, &bvivi5, &bvivi4, &bvivi3, &bvivi2, &bvivi1, 0);
        AbilityFind(&idGORILLASMASH, &gorillasmash, 0);
        break;
    case Berserker: // BER
        BerserkerClass = true;
        AbilityFind(&idBLOODLUST, &bloodlust24, &bloodlust23, &bloodlust22, &bloodlust21, &bloodlust20, &bloodlust19, &bloodlust18, &bloodlust17, &bloodlust16, &bloodlust15, &bloodlust14, &bloodlust13, &bloodlust12, &bloodlust11, &bloodlust10, &bloodlust9, &bloodlust8, &bloodlust7, &bloodlust6, &bloodlust5, &bloodlust4, &bloodlust3, &bloodlust2, &bloodlust1, 0);
        AbilityFind(&idCRIPPLE, &cripple34, &cripple33, &cripple32, &cripple31, &cripple30, &cripple29, &cripple28, &cripple27, &cripple26, &cripple25, &cripple24, &cripple23, &cripple22, &cripple21, &cripple20, &cripple19, &cripple18, &cripple17, &cripple16, &cripple15, &cripple14, &cripple13, &cripple12, &cripple11, &cripple10, &cripple9, &cripple8, &cripple7, &cripple6, &cripple5, &cripple4, &cripple3, &cripple2, &cripple1, 0);
        AbilityFind(&idCRYHAVOC, &cryhavoc4, &cryhavoc3, &cryhavoc2, &cryhavoc1, 0);
        AbilityFind(&idJOLT, &joltber34, &joltber33, &joltber32, &joltber31, &joltber30, &joltber29, &joltber28, &joltber27, &joltber26, &joltber25, &joltber24, &joltber23, &joltber22, &joltber21, &joltber20, &joltber19, &joltber18, &joltber17, &joltber16, &joltber15, &joltber14, &joltber13, &joltber12, &joltber11, &joltber10, &joltber9, &joltber8, &joltber7, &joltber6, &joltber5, &joltber4, &joltber3, &joltber2, &joltber1, 0);
        AbilityFind(&idOPFRENZY, &opfrenzy24, &opfrenzy23, &opfrenzy22, &opfrenzy21, &opfrenzy20, &opfrenzy19, &opfrenzy18, &opfrenzy17, &opfrenzy16, &opfrenzy15, &opfrenzy14, &opfrenzy13, &opfrenzy12, &opfrenzy11, &opfrenzy10, &opfrenzy9, &opfrenzy8, &opfrenzy7, &opfrenzy6, &opfrenzy5, &opfrenzy4, &opfrenzy3, &opfrenzy2, &opfrenzy1, 0);
        AbilityFind(&idRALLOS, &rallos30, &rallos29, &rallos28, &rallos27, &rallos26, &rallos25, &rallos24, &rallos23, &rallos22, &rallos21, &rallos20, &rallos19, &rallos18, &rallos17, &rallos16, &rallos15, &rallos14, &rallos13, &rallos12, &rallos11, &rallos10, &rallos9, &rallos8, &rallos7, &rallos6, &rallos5, &rallos4, &rallos3, &rallos2, &rallos1, 0);
        AbilityFind(&idSLAPFACE, &slapface21, &slapface20, &slapface19, &slapface18, &slapface17, &slapface16, &slapface15, &slapface14, &slapface13, &slapface12, &slapface11, &slapface10, &slapface9, &slapface8, &slapface7, &slapface6, &slapface5, &slapface4, &slapface3, &slapface2, &slapface1, 0);
        AbilityFind(&idSTUN[1], &stunber31, &stunber30, &stunber29, &stunber28, &stunber27, &stunber26, &stunber25, &stunber24, &stunber23, &stunber22, &stunber21, &stunber20, &stunber19, &stunber18, &stunber17, &stunber16, &stunber15, &stunber14, &stunber13, &stunber12, &stunber11, &stunber10, &stunber9, &stunber8, &stunber7, &stunber6, &stunber5, &stunber4, &stunber3, &stunber2, &stunber1, 0);
        AbilityFind(&idVIGAXE, &vigber24, &vigber23, &vigber22, &vigber21, &vigber20, &vigber19, &vigber18, &vigber17, &vigber16, &vigber15, &vigber14, &vigber13, &vigber12, &vigber11, &vigber10, &vigber9, &vigber8, &vigber7, &vigber6, &vigber5, &vigber4, &vigber3, &vigber2, &vigber1, 0);
        AbilityFind(&idRAGEVOLLEY, &volley35, &volley34, &volley33, &volley32, &volley31, &volley30, &volley29, &volley28, &volley27, &volley26, &volley25, &volley24, &volley23, &volley22, &volley21, &volley20, &volley19, &volley18, &volley17, &volley16, &volley15, &volley14, &volley13, &volley12, &volley11, &volley10, &volley9, &volley8, &volley7, &volley6, &volley5, &volley4, &volley3, &volley2, &volley1, 0);
        AbilityFind(&idPROVOKE[1], &stunber34, &stunber33, &stunber32, &stunber31, &stunber30, &stunber29, &stunber28, &stunber27, &stunber26, &stunber25, &stunber24, &stunber23, &stunber22, &stunber21, &stunber20, &stunber19, &stunber18, &stunber17, &stunber16, &stunber15, &stunber14, &stunber13, &stunber12, &stunber11, &stunber10, &stunber9, &stunber8, &stunber7, &stunber6, &stunber5, &stunber4, &stunber3, &stunber2, &stunber1, 0);
        AbilityFind(&idFRENZY, &sfrenzy, 0);
        break;
    }

    if (GetPrivateProfileString(section, NULL, "", keys, sizeof(keys), INIFileName))
    {
        char* pKeys = keys;
        CHAR szTempKey[MAX_STRING] = { 0 };
        while (pKeys[0])
        {
            strcpy_s(szTempKey, pKeys);
            if (GetPrivateProfileString(section, szTempKey, "", temp, sizeof(temp), INIFileName))
            {
                _strlwr_s(szTempKey);
                if (ec != (c = CmdListe.find(szTempKey)))
                    (*c).second.Setup(temp);
                else if (ei != (i = IniListe.find(szTempKey)))
                    (*i).second.Setup(temp);
            }
            pKeys += strlen(pKeys) + 1;
        }
    }

    Loaded = true;
}

void Exporting() {
    char output[MAX_STRING];
    char defval[MAX_STRING];
    Liste::iterator c, e;
    WritePrivateProfileString(section, NULL, NULL, INIFileName);
    e = CmdListe.end();
    for (c = CmdListe.begin(); c != e; c++) {
        output[0] = 0;
        if ((*c).second.OptionCondition)
            if (std::string *OptionShow = (std::string*)(*c).second.Value())
                strcpy_s(output, OptionShow->c_str());
        if ((*c).second.OptionAbility || (*c).second.OptionValue)
            if (long *OptionValue = (long*)(*c).second.Value())
                _itoa_s(*OptionValue, output, 10);
        if (output[0]) {
            strcpy_s(defval, (*c).second.OptionDefault);
            if (defval[0])
                ParseMacroData(defval, sizeof(defval));
            if (strcmp(output, "0") || strcmp(output, defval))
                WritePrivateProfileString(section, (*c).second.OptionKey, output, INIFileName);
        }
    }
    e = IniListe.end();
    for (c = IniListe.begin(); c != e; c++) {
        output[0] = 0;
        if ((*c).second.OptionCondition)
            if (std::string *OptionShow = (std::string*)(*c).second.Value())
                strcpy_s(output, OptionShow->c_str());
        if ((*c).second.OptionAbility || (*c).second.OptionValue)
            if (long *OptionValue = (long*)(*c).second.Value())
                _itoa_s(*OptionValue, output, 10);
        if (output[0]) {
            strcpy_s(defval, sizeof(defval), (*c).second.OptionDefault);
            if (defval[0])
                ParseMacroData(defval, sizeof(defval));
            if (strcmp(output, "0") || strcmp(output, defval))
                WritePrivateProfileString(section, (*c).second.OptionKey, output, INIFileName);
        }
    }
    sprintf_s(output, "%1.3f", MQ2Version); WritePrivateProfileString(section, "version", output, INIFileName);
}

void MapInsert(Liste* MyList, Option&& MyOption)
{
    MyList->emplace(MyOption.OptionKey, std::move(MyOption));
}

void MeleeHelp()
{
    WriteChatf("%s::-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", PLUGIN_NAME);
    WriteChatf("%s::Version [\ag%1.3f\ax] Loaded!", PLUGIN_NAME, MQ2Version);
    WriteChatf("%s::-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", PLUGIN_NAME);
    for (Liste::iterator i = CmdListe.begin(); i != CmdListe.end(); i++) (*i).second.Write();
    WriteChatf("%s::-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", PLUGIN_NAME);
    if (NULL == GetPluginProc("mq2cast", "CastCommand"))
    {
        WriteChatf("%s::Required Latest [\arMQ2Cast\ax] for AA/SPELL/ITEM casting.", PLUGIN_NAME);
    }
    if (NULL == GetPluginProc("mq2moveutils", "StickCommand"))
    {
        WriteChatf("%s::Required Latest [\arMQ2MoveUtils\ax] for MOVEMENT.", PLUGIN_NAME);
    }
}

void PetSEEN()
{
    PetOnAttk = (unsigned long)clock() + 12000;
    PetOnHold = false;
}

void PetBACK()
{
    PetOnHold = true;
    if (doPETASSIST && pPetInfoWnd && PetButtonEnabled(UI_PetBack))
    {
        Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet back off");
        EzCommand("/pet back off");
    }
}

void PetATTK()
{
    if (MeleeTarg)
    {
        PSPAWNINFO MTarg = GetSpawnByID(MeleeTarg);
        Announce(SHOW_CONTROL, "%s::Checking PetEngageHPs [\ay%d<%d\ax].", PLUGIN_NAME, MTarg->HPCurrent, doPETENGAGEHPS);
        if (doPETASSIST && !(MTarg->HPCurrent<doPETENGAGEHPS)) return;
    }
    PetSEEN();
    if (doPETASSIST && pPetInfoWnd && !(onEVENT & 0x0003) && MeleeTarg && PetButtonEnabled(UI_PetAttk) && SpawnType(GetSpawnByID(MeleeTarg), NPC_TYPE))
    {
        Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet attack");
        if (!TargetID(MeleeTarg))
        {
            PSPAWNINFO Current = pTarget;
            pTarget = GetSpawnByID(MeleeTarg);
            EzCommand("/pet attack");
            if (Current) Command("/squelch /target id %d", Current->SpawnID);
        }
        else
        {
            EzCommand("/pet attack");
        }
        PetTarget = MeleeTarg;
        if (!doMELEE) MeleeReset();
    }
}

void StickReset(void)
{
    TimerStik = 0;
    if (Sticking) Stick("");
    StickArg[0] = 0;
    onSTICK = (doMELEE && !(onEVENT & 0x8000) && (doSTICKRANGE || doSTICKNORANGE) && bMULoaded); // x && x && !ranged && moveutils_loaded
}

void RangeReset()
{
    if (!doRANGE && AutoFire) EzCommand("/autofire");
    if (!doRANGE && (onEVENT & 0x8000)) onEVENT &= 0x7FFF;
}

void OtherReset()
{
    MeleeTarg  = 0;
    MeleeType  = 0;
    MeleeLife  = 0;
    MeleeCast  = 0;
    MeleeFlee  = 0;
    TimerBack  = 0;
    TimerLife  = 0;
    TimerFace  = 0;
    TimerStun  = 0;
    PetTarget  = 0;
    PetOnWait  = 0;
    SwingHits  = 0;
    TakenHits  = 0;
    onEVENT    = 0;
    doDOWN     = 0;
    doHOLY     = 0;
    StrikeFail = false;
}

void MeleeReset()
{
    if (!doMELEE && pEverQuestInfo->bAutoAttack)
    {
        MeleeTarg = 0;
        if (onEVENT & 0x0008) onEVENT |= 0x0008;
        AttackOFF();
        StickReset();
    }
}

void AggroReset()
{
    int onAGGRO = (doAGGRO && IsGrouped());
    onCHALLENGEFOR = (onAGGRO && idCHALLENGEFOR.Found());
    onENRAGINGKICK = (onAGGRO && idENRAGINGKICK.Found());
    onBELOW = (onAGGRO && doPROVOKEMAX) ? doPROVOKEMAX : 0;

    if (doMELEE)
    {
        if (long PW = (IsGrouped() && doAGGRO) ? elAGGROPRI : elMELEEPRI) Equip(PW, InvSlot_Primary);
        if (long SW = (IsGrouped() && doAGGRO) ? elAGGROSEC : elMELEESEC) Equip(SW, InvSlot_Secondary);
    }
}

void SneakOFF() {
    if (IsSneaking() && idSNEAK.Found()) idSNEAK.Press();
}

void HideOFF() {
    if (IsInvisible() && idHIDE.Found()) idHIDE.Press();
}

int StabCheck()
{
    if (elPOKER && OkayToEquip(Giant))
    {
        char szTempItem[25] = { 0 };
        sprintf_s(szTempItem, "%d", elPOKER);
        CItemLocation cFindItem;
        if (ItemFind(&cFindItem, szTempItem))
        {
            if (PokerType(cFindItem.pBagSlot)) return true;
        }
    }
    return PokerType(ContPrimary());
}

void StabPress() {
    long saveid = 0;
    CItemLocation cMoveItem;
    char szTempItem[25] = { 0 };
    sprintf_s(szTempItem, "%d", elPOKER);
    if (elPOKER && OkayToEquip(Giant) && ItemFind(&cMoveItem, szTempItem))
    {
        if (CONTENTS* pri = ContPrimary())
            if (GetItemFromContents(pri)->ItemNumber != elPOKER) {
                saveid = GetItemFromContents(pri)->ItemNumber;
                Equip(elPOKER, InvSlot_Primary);
            }
    }
    if (PokerType(ContPrimary())) {
        idBACKSTAB.Press();
        SwingHits++;
    }
    if (saveid) Equip(saveid, InvSlot_Primary);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API void ThrowIT(PlayerClient* pChar, const char* Cmd) {
    if (pEverQuestInfo->PrimaryAttackReady && pTarget && TargetType(NPC_TYPE) &&
        !InRange(SpawnMe(), pTarget, 35) &&
        fabs(AngularHeading(SpawnMe(), pTarget)) < 50 &&
        LineOfSight(SpawnMe(), pTarget)) {

        unsigned long ulSlot = 0;
        char szTempItem[25] = { 0 };
        CItemLocation cMoveItem;
        char szItemName[MAX_STRING] = { 0 };
        // test if we could do ranged with current ammo/range configuration
        long crT = 99; long crI = 0; long caT = 99; long caI = 0; long caQ = 0;
        if (CONTENTS* r = ContRange())
        {
            crI = GetItemFromContents(r)->ItemNumber;
            crT = GetItemFromContents(r)->ItemType;
            strcpy_s(szItemName, GetItemFromContents(r)->Name);
        }
        if (CONTENTS* a = ContAmmo()) {
            caI = GetItemFromContents(a)->ItemNumber;
            caT = GetItemFromContents(a)->ItemType;
            if (caT == 7 || caT == 19 || caT == 27) caQ = CountItemByID(caI);
        }
        if (!(caI && ((caT == 27 && crT == 5) || (caI == crI && (caT == 7 || caT == 19)))))
        {
            if (!OkayToEquip(Giant)) return;

            // grab information about user defined range/ammunition
            long erT = 99; long eaT = 99; long eaQ = 0;

            CONTENTS* r = NULL;
            CONTENTS* a = NULL;

            //if(CONTENTS* r=ItemLocate(elRANGED)) erT=GetItemFromContents(r)->ItemType;
            sprintf_s(szTempItem, "%d", elRANGED);
            if (ItemFind(&cMoveItem, szTempItem))
            {
                r = cMoveItem.pBagSlot;
                erT = GetItemFromContents(r)->ItemType;
            }

            //if(CONTENTS* a=ItemLocate(elARROWS)) {
            //    eaT=a->Item->ItemType;
            //    if(eaT == 7 || eaT == 19 || eaT == 27) eaQ=ItemCounts(elARROWS);
            //}
            sprintf_s(szTempItem, "%d", elARROWS);
            if (ItemFind(&cMoveItem, szTempItem))
            {
                a = cMoveItem.pBagSlot;
                eaT = GetItemFromContents(a)->ItemType;
                if (eaT == 7 || eaT == 19 || eaT == 27) eaQ = CountItemByID(elARROWS);
            }


            // find equipping scenario (bow+arrow) or (throw/throw).
            long EquipRangeID = 0; long EquipArrowID = 0;
            if ((crT == 5 || erT == 5) && (caT == 27 || eaT == 27)) {
                EquipRangeID = (crT == 5) ? crI : elRANGED;
                EquipArrowID = (caT == 27) ? caI : elARROWS;
            }
            else if ((caQ > 2 && (caT == 7 || caT == 19)) || (eaQ > 2 && (eaT == 7 || eaT == 19)))
            {
                EquipRangeID = (caQ > 2 && (caT == 7 || caT == 19)) ? caI : elARROWS;
                EquipArrowID = (caQ > 2 && (caT == 7 || caT == 19)) ? caI : elARROWS;
            }
            else return;

            // load equipping scenario found!
            if (EquipRangeID && EquipArrowID) {
                if (crI != EquipRangeID) Equip(EquipRangeID, InvSlot_Range);
                if (caI != EquipArrowID) Equip(EquipArrowID, InvSlot_Ammo);
                if (EquipArrowID == EquipRangeID && !ContRange()) { // Reloads thrown weapons
                    EzCommand("/ctrlkey /itemnotify ammo leftmouseup");
                    EzCommand("/shiftkey /itemnotify ranged leftmouseup");
                }
            }
        }

        // we can't call do_ranged directly because it will hit before the item swaps
        EzCommand("/ranged");

        // preserve ranged slot
        if (elRANGED != crI) {
            char szCommand[MAX_STRING] = { 0 };
            sprintf_s(szCommand, "/shiftkey /itemnotify \"%s\" leftmouseup", szItemName);
            EzCommand(szCommand);
            EzCommand("/shiftkey /itemnotify ranged leftmouseup");
            EzCommand("/autoinventory");
        }
    }
}

PLUGIN_API void Override(PlayerClient*, const char* Cmd)
{
    Announce(SHOW_OVERRIDE, Cmd, PLUGIN_NAME);
    MeleeTime = (unsigned long)clock() + delay;
    AttackOFF();
    if (onEVENT & 0x0008) onEVENT |= 0x0008;
    if (AutoFire)
    {
        AutoFire = false;
        EzCommand("/autofire");
    }
    if (doPETASSIST) PetBACK();
    StickReset();
    OtherReset();
}

PLUGIN_API void Melee(PlayerClient* pChar, const char* Cmd)
{
    char Tmp[MAX_STRING]; char Var[MAX_STRING]; char Set[MAX_STRING]; BYTE Parm = 1; bool Help = true;
    Liste::iterator c; Liste::iterator ec = CmdListe.end();
    do {
        GetArg(Tmp, Cmd, Parm++);
        _strlwr_s(Tmp);
        GetArg(Var, Tmp, 1, FALSE, FALSE, FALSE, '=');
        GetArg(Set, Tmp, 2, FALSE, FALSE, FALSE, '=');
        if (Var[0]) {
            c = CmdListe.find(Var);
            if (ec != c) {
                (*c).second.Setup(Set);
                (*c).second.Write();
                Help = false;
            }
            else if (!Set[0] && (!_stricmp(Var, "on") || !_stricmp(Var, "off"))) {
                if (ec != (c = CmdListe.find("plugin"))) (*c).second.Setup(Var);
                Help = false;
            }
            else if (!Set[0] && (!_stricmp(Var, "reload") || !_stricmp(Var, "load"))) {
                WriteChatf("%s::Loading...", PLUGIN_NAME);
                Configure();
                Help = false;
            }
            else if (!Set[0] && !_stricmp(Var, "save")) {
                WriteChatf("%s::Saving...", PLUGIN_NAME);
                Exporting();
                Help = false;
            }
            else if (!Set[0] && !_stricmp(Var, "key")) {
                char buffer[MAX_STRING]; KeyCombo combo;
                DescribeKeyCombo(pKeypressHandler->NormalKey[FindMappableCommand("AUTOPRIM")], buffer, sizeof(buffer));
                WriteChatf("%s::\ayATTACK\ax binded to [\ay%s\ax]", PLUGIN_NAME, buffer);
                GetMQ2KeyBind("MELEE", false, combo);
                DescribeKeyCombo(combo, buffer, sizeof(buffer));
                WriteChatf("%s::\ayMELEE\ax  binded to [\ay%s\ax]", PLUGIN_NAME, buffer);
                GetMQ2KeyBind("RANGE", false, combo);
                DescribeKeyCombo(combo, buffer, sizeof(buffer));
                WriteChatf("%s::\ayRANGE\ax  binded to [\ay%s\ax]", PLUGIN_NAME, buffer);
                Help = false;
            }
            else if (!Set[0] && !_stricmp(Var, "reset")) {
                Override(NULL, "%s::Resetting...");
                Help = false;
            }
            else {
                WriteChatf("%s::Unsupported Argument <\ar%s\ax>", PLUGIN_NAME, Var);
                break;
            }
        }
    } while (strlen(Tmp));
    if (Help) MeleeHelp();
}

PLUGIN_API void KillThis(PlayerClient* pChar, const char* Cmd)
{
    if ((doMELEE || doPETASSIST)  && doSKILL && pTarget && !TargetID(MeleeTarg) && TargetType(NPC_TYPE) && InGame())
    {
        if (IsFeigning()) EzCommand("/stand");
        StickReset();
        OtherReset();
        AggroReset();
        MeleeTarg = pTarget->SpawnID;
        strcpy_s(MeleeName, pTarget->DisplayedName);
        MeleeSize = (long)strlen(MeleeName) + 1;
        MeleeType = SpawnMask(pTarget);
        if (doMELEE) {
            onEVENT |= 0x0008;
            Announce(SHOW_ATTACKING, "%s::Attacking [\ay%s\ax].", PLUGIN_NAME, MeleeName, MeleeTarg);
        }
    }
}

PLUGIN_API void EnrageON(PlayerClient* pChar, const char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doENRAGE && MeleeTarg && InGame()) {
        PSPAWNINFO KillTarg = GetSpawnByID(MeleeTarg);
        if (!(onEVENT & 0x0001)) {
            if (!(onEVENT & 0x0002) && doPETASSIST) PetBACK();
            onEVENT |= 0x0001;
            Announce(SHOW_ENRAGING, "MQ2Melee::\arENRAGE\ax detected, taking action!");
        }
        if (pEverQuestInfo->bAutoAttack && onEVENT & 0x0003 && SpawnType(KillTarg, NPC_TYPE)) {
            double Back = fabs(AngularDistance(KillTarg->Heading, SpawnMe()->Heading));
            double View = fabs(AngularHeading(SpawnMe(), KillTarg));
            if (Back > 92 || View > 60 || onEVENT & 0x0002) {
                onEVENT |= 0x0008;
                AttackOFF();
            }
        }
    }
}

PLUGIN_API void EnrageOFF(PlayerClient* pChar, const char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doENRAGE && MeleeTarg && onEVENT & 0x0001 && InGame()) {
        onEVENT &= 0xFFFE;
        if (TargetID(PetTarget)) PetATTK();
        Announce(SHOW_ENRAGING, "MQ2Melee::\agENRAGE\ax ended, taking action!");
    }
}

PLUGIN_API void InfuriateON(PlayerClient* pChar, const char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doINFURIATE && MeleeTarg && InGame()) {
        if (!(onEVENT & 0x0002)) {
            if (!(onEVENT & 0x0001) && doPETASSIST) PetBACK();
            onEVENT |= 0x0002;
            Announce(SHOW_ENRAGING, "MQ2Melee::\arINFURIATE\ax detected, taking action!");
        }
        if (pEverQuestInfo->bAutoAttack) {
            AttackOFF();
            onEVENT |= 0x0008;
        }
    }
}

PLUGIN_API void InfuriateOFF(PlayerClient* pChar, const char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doINFURIATE && MeleeTarg && onEVENT & 0x0002 && InGame()) {
        onEVENT &= 0xFFFD;
        if (TargetID(PetTarget)) PetATTK();
        Announce(SHOW_ENRAGING, "MQ2Melee:\agINFURIATE\ax ended, taking action!");
    }
}

bool ParseMacroLine(char* szOriginal, size_t BufferSize, std::vector<std::string>& out)
{
    // find each {}
    char* pBrace = strstr(szOriginal, "${");
    if (!pBrace)
        return false;

    size_t NewLength = 0;
    bool Changed = false;
    char szCurrent[MAX_STRING] = { 0 };

    do
    {
        // find this brace's end
        char* pEnd = &pBrace[1];
        bool Quote = false;
        bool BeginParam = false;
        int nBrace = 1;

        while (nBrace)
        {
            ++pEnd;
            if (BeginParam)
            {
                BeginParam = false;
                if (*pEnd == '\"')
                {
                    Quote = true;
                }
                continue;
            }

            if (*pEnd == 0)
            {
                // unmatched brace or quote
                continue;
            }

            if (Quote)
            {
                if (*pEnd == '\"')
                {
                    if (pEnd[1] == ']' || pEnd[1] == ',')
                    {
                        Quote = false;
                    }
                }
            }
            else
            {
                if (*pEnd == '}')
                {
                    nBrace--;
                }
                else if (*pEnd == '{')
                {
                    nBrace++;
                }
                else if (*pEnd == '[' || *pEnd == ',')
                    BeginParam = true;
            }
        }

        *pEnd = 0;
        strcpy_s(szCurrent, &pBrace[2]);
        if (szCurrent[0] == 0)
        {
            continue;
        }

        if (ParseMacroLine(szCurrent, sizeof(szCurrent), out))
        {
            size_t NewLength = strlen(szCurrent);
            memmove(&pBrace[NewLength + 1], &pEnd[1], strlen(&pEnd[1]) + 1);

            int addrlen = (int)(pBrace - szOriginal);
            memcpy_s(pBrace, BufferSize - addrlen, szCurrent, NewLength);

            pEnd = &pBrace[NewLength];
            *pEnd = 0;
        }

        if (!strchr(szCurrent, '[') && !strchr(szCurrent, '.'))
            out.emplace_back(szCurrent);

        NewLength = strlen(szCurrent);
        memmove(&pBrace[NewLength], &pEnd[1], strlen(&pEnd[1]) + 1);

        int addrlen = (int)(pBrace - szOriginal);
        memcpy_s(pBrace, BufferSize - addrlen, szCurrent, NewLength);

        if (!bAllowCommandParse)
        {
            bAllowCommandParse = true;
            Changed = false;
            break;
        }
        else
        {
            Changed = true;
        }
    } while (pBrace = strstr(&pBrace[1], "${"));

    if (Changed)
    {
        while (ParseMacroLine(szOriginal, BufferSize, out))
            ;
    }

    return Changed;
}

bool OKtoParseShit(std::string &str)
{
    CHAR szBuffer[MAX_STRING] = { 0 };
    strcpy_s(szBuffer, str.c_str());
    std::vector<std::string> out;
    ParseMacroLine(szBuffer, MAX_STRING, out);
    bool bOkToCheck = true;
    if (out.size()) {
        for (const std::string& line : out) {
            bOkToCheck = false;
            if (FindMQ2Data(line.c_str())) {
                bOkToCheck = true;
                continue;
            }
            if (!bOkToCheck)
            {
                //ok fine we didnt find it in the tlo map...
                //lets check variables
                if (FindMQ2DataVariable(line.c_str())) {
                    bOkToCheck = true;
                    continue;
                }
            }
            if (!bOkToCheck)
            {
                //still not found...
                break;
            }
        }
    }
    return bOkToCheck;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void DowntimeHandle()
{
    if (doSENSETRAP && idSENSETRAP.Ready(ifSENSETRAP)) idSENSETRAP.Press();
    if (!MeleeTarg && (unsigned long)clock() > TimerAttk && Immobile) {
        if (DOWNSHITIF.length() < 5 || Evaluate((char*)("${If[" + DOWNSHITIF.substr(5, DOWNSHITIF.length() - 6) + ",1,0]}").c_str())) {
            if (DOWNSHIT[doDOWN].size()) {
                switch (doDOWNFLAG[doDOWN])
                {
                    case 1://not a macro dependant flag
                    {
                        EzCommand(DOWNSHIT[doDOWN].c_str());
                        break;
                    }
                    case 2://a macro must be running and the variable MUST exist
                    {
                        if (gRunning) {
                            if (OKtoParseShit(DOWNSHIT[doDOWN])) {
                                EzCommand(DOWNSHIT[doDOWN].c_str());
                            }
                        }
                        break;
                    }
                    case 3: //only parsed when no macro running
                    {
                        if (!gRunning) EzCommand(DOWNSHIT[doDOWN].c_str());
                        break;
                    }
                    default:
                        break;
                }
            }
            for (long x = 1; x < 91; x++) {
                long n = (x + doDOWN) % 91;
                if (doDOWNFLAG[n]) {
                    doDOWN = n;
                    break;
                }
            }
        }
        if (doFORAGE && idFORAGE.Ready(ifFORAGE))
        {
            idFORAGE.Press();
        }
        else if (doSNEAK && !IsSneaking() && idSNEAK.Ready(ifSNEAK))
        {
            idSNEAK.Press();
        }
        else if (doHIDE && !IsInvisible() && idHIDE.Ready(ifHIDE))
        {
            idHIDE.Press();
        }
    }
}

void MeleeHandle()
{
    // check opened windows that wont let us perform any melee actions.
    if (!BardClass  && WinState((CXWnd*)pCastingWnd))  return;
    if (WinState((CXWnd*)pSpellBookWnd))               return;
    if (WinState((CXWnd*)pLootWnd))                    return;
    if (WinState((CXWnd*)pBankWnd))                    return;
    if (WinState((CXWnd*)pMerchantWnd))                return;
    if (WinState((CXWnd*)pTradeWnd))                   return;
    if (WinState((CXWnd*)pGiveWnd))                    return;
    Silenced = false;

    // check detrimental buff that wont let ya perform melee actions.
    for (int b = 0; b < BuffMax; b++) {
        long SpellID = GetPcProfile()->GetEffect(b).SpellID;
        if (SpellID < 1) continue;
        if (PSPELL spell = GetSpellByID(SpellID)) {
            for (int a = 0; a < GetSpellNumEffects(spell); a++) {
                switch (GetSpellAttrib(spell, a)) {
                case 22: return;          // charmed
                case 23: return;          // feared
                case 31: return;          // mesmerized
                case 40: return;          // invulnerable
                case 96: Silenced = true; // silenced
                }
            }
        }
    }

    // check detrimental song that wont let ya perform melee actions.
    for (int s = 0; s < SongMax; s++) {
        long SpellID = GetPcProfile()->GetTempEffect(s).SpellID;
        if (SpellID < 1) continue;
        if (PSPELL spell = GetSpellByID(SpellID)) {
            for (int a = 0; a < GetSpellNumEffects(spell); a++) {
                switch (GetSpellAttrib(spell, a))
                {
                case 22: return;          // charmed
                case 23: return;          // feared
                case 31: return;          // mesmerized
                case 40: return;          // invulnerable
                case 96: Silenced = true; // silenced
                }
            }
        }
    }

    // yaulp for mana, if your a cleric
    if (doYAULP && idYAULP.Ready(ifYAULP)) idYAULP.Press();

    // check our health and perform some healing action if we can
    if ((Health = GetCurHPS() * 100 / GetMaxHPS()) < 100) {
        Ability *UseThis = NULL;
        if (doMEND && Health <= doMEND && idMEND.Ready(ifMEND))                  UseThis = &idMEND;
        else if (doLAYHAND && Health <= doLAYHAND && idLAYHAND.Ready(ifLAYHAND)) UseThis = &idLAYHAND;
        else if (doPOTHEALFAST && Health <= doPOTHEALFAST && idPOTHEALFAST.Ready(ifPOTHEALFAST)) UseThis = &idPOTHEALFAST;
        else if (doPOTHEALOVER && Health <= doPOTHEALOVER && idPOTHEALOVER.Ready(ifPOTHEALOVER)) UseThis = &idPOTHEALOVER;
        if (UseThis) {
            if (UseThis->ID == idLAYHAND.ID) {
                PSPAWNINFO TargetSave = pTarget;
                pTarget = SpawnMe();
                idLAYHAND.Press();
                pTarget = TargetSave;
            }
            else UseThis->Press();
        }
    }

    // check if we are stunned, if so we can't perform any melee actions
    if (IsStunned()) return;

    // check if we still have a killing target or we acquiring a new one
    if ((pTarget && TargetType(NPC_TYPE)) && (pEverQuestInfo->bAutoAttack || onEVENT & 0x8000))
    {
        if (!MeleeTarg)
        {
            KillThis(NULL, "");
        }
        else if (!TargetID(MeleeTarg))
        {
            Override(NULL, "%s::\arTARGET SWITCH\ax taking actions.");
            return;
        }
        TimerAttk = (unsigned long)clock() + delay * 12;
    }
    if (MeleeTarg)
    {
        if (PSPAWNINFO Tar = GetSpawnByID(MeleeTarg))
        {
            if (!MeleeLife || Tar->HPCurrent < MeleeLife) TimerLife = (unsigned long)clock() + 1500;
            MeleeLife = Tar->HPCurrent;
        }
    }

    // check if we should standup from an interrupted feign death
    if (BrokenFD && (unsigned long)clock()>BrokenFD) {
        BrokenFD = false;
        if (IsFeigning()) {
            Announce(SHOW_FEIGN, "%s::\arFAILED FEIGN DEATH\ax taking action!", PLUGIN_NAME);
            EzCommand("/stand");
            return;
        }
    }

    // check it's a good time to perform some downtime actions?
    //if (!IsCasting()) DowntimeHandle();
    if (!IsCasting() || BardClass) DowntimeHandle();

    // check it's a good time to drop combat?
    Ability *FeignDeath = NULL;
    if (idFEIGN[0].Ready(""))      FeignDeath = &idFEIGN[0];
    else if (idFEIGN[1].Ready("")) FeignDeath = &idFEIGN[1];
    Health = GetCurHPS() * 100 / GetMaxHPS();
    if (!doAGGRO && !(onEVENT & 0x0FF0) && !IsFeigning() && !IsInvisible()) {
        bool fTime = (doFEIGNDEATH && Health <= doFEIGNDEATH && FeignDeath);
        bool eTime = (doESCAPE     && Health <= doESCAPE     && idESCAPE.Ready(""));
        bool bTime = (doBACKOFF    && Health <= doBACKOFF);

        if (fTime || eTime || bTime) {
            if (pEverQuestInfo->bAutoAttack) {
                onEVENT |= 0x0008;
                AttackOFF();
            }
            else {
                if (fTime)      onEVENT |= 0x0040;
                else if (eTime) onEVENT |= 0x0020;
                else if (bTime) onEVENT |= 0x0010;
                if (onEVENT & 0x0020)      idESCAPE.Press();
                // else if (onEVENT & 0x0040) FeignDeath->Press();
                else if (onEVENT & 0x0040 && FeignDeath) FeignDeath->Press();
            }
            return;
        }
    }

    // check it's a good time to resume combat?
    if ((IsFeigning() || IsInvisible() || onEVENT & 0x0FF0) && !(onEVENT & 0x4000))
    {
        if (!TimerBack) TimerBack = (unsigned long)clock() + delay;
        else if ((unsigned long)clock() > TimerBack && (doAGGRO || (onEVENT & 0x0FF0 && Health > doRESUME)))
        {
            if ((IsInvisible() || onEVENT & 0x0220) && (!IsInvisible() || !(onEVENT & 0x0020))) onEVENT &= 0xFDDF;
            if ((IsFeigning() || onEVENT & 0x0440) && (!IsFeigning() || (IsFeigning() && doSTAND)))
            {
                onEVENT &= 0xFBBF;
                EzCommand("/stand");
                StickReset();
                return;
            }
            onEVENT &= 0xF99F;
            TimerBack = false;
        }
    }
    // end resume

    // time to handle dummy pet, check mending, check we have target in range, etc...
    if (doPETASSIST && MeleeTarg)
    {
        PSPAWNINFO Pet = SpawnPet();
        PSPAWNINFO Tar = GetSpawnByID(MeleeTarg);
        if (Pet && Tar)
        {
            if (doPETMEND && Pet->HPCurrent <= doPETMEND && idPETMEND.Ready("")) idPETMEND.Press();
            PetInDist = (!doPETRANGE || InRange(Pet, Tar, (FLOAT)doPETRANGE));
            if (!PetOnWait && PetInDist) PetOnWait = (unsigned long)clock() + doPETDELAY * 1000;
            if (PetOnWait && (unsigned long)clock() > PetOnWait && PetInDist)
            {
                if ((unsigned long)clock() > TimerLife || !IdlingArray[Tar->Animation & 0xFF])
                {
                    if ((unsigned long)clock() > PetOnAttk) PetATTK();
                }
            }
        }
    }
    // end pet

    // hold on?
    if (!MeleeTarg || !TargetID(MeleeTarg) || !IsStanding() || onEVENT & 0x0FF2) // this event is never referenced elsewhere
    {
        if (pEverQuestInfo->bAutoAttack) AttackOFF();
        if (onEVENT & 0x0008) onEVENT |= 0x0008;
        return;
    }

    // target is in range? could we engage and kill it?
    // ***** hard-coded override if we are more than 250 away  ****
    if ((MeleeDist = DistanceToSpawn(SpawnMe(), pTarget)) > 250)
    {
        Override(NULL, "");
        return;
    }
    // end 250 range enforce

    MeleeSpeed = fabs(FindSpeed(pTarget));
    MeleeBack = fabs(AngularDistance(pTarget->Heading, SpawnMe()->Heading));
    MeleeView = fabs(AngularHeading(SpawnMe(), pTarget));
    MeleeFlee = (MeleeFlee || (MeleeLife <= 85 && MeleeSpeed > 25.0f && IsMobFleeing(SpawnMe(), pTarget)));
    MeleeKill = pTarget->AvatarHeight + 12.0f;
    //Sticking=Evaluate("$If[$Stick.Active},1,0]}");
    if (bMULoaded && bMUPointers) Sticking = *pbStickOn;

    // are we discing? if so time to promote some actions?
    long disc = Discipline();
    if (disc && !(onEVENT & 0x7007)) switch (disc)
    {
    case d_assassin1:
    case d_assassin2:
    case d_assassin3:
    case d_assassin4:
    case d_assassin5:
    case d_assassin6:
    case d_assassin7:
        if (doMELEE && MeleeDist < MeleeKill && MeleeView<60 && MeleeBack<doSTAB && idBACKSTAB.Ready("") && StabCheck()) StabPress(); break;
    }

    // scripted rogue sequence striking/assassination codes
    if (doASSASSINATE && doBACKSTAB && doMELEE && onSTICK>0 && !SwingHits && !TakenHits && MeleeSpeed<2.0f && !pEverQuestInfo->bAutoAttack && StabCheck())
    {
        if (!Moving && Immobile)
        {
            if (!IsSneaking() && idSNEAK.Ready("")) idSNEAK.Press();
            if (!IsInvisible() && idHIDE.Ready("")) idHIDE.Press();
            if (IsSneaking() && TimeSince(SilentTimer)>1000 && IsInvisible() && TimeSince(HiddenTimer)>1000)
            {
                if (doSTRIKEMODE)    /// strikemode = 1
                {
                    strcpy_s(Reserved, StrikeCMD.c_str()); // copy our strike ini command to Reserved
                    ParseMacroData(Reserved, sizeof(Reserved)); // parse out TLO evaluations
                }
                else   // strikemode not set
                {
                    if (doSTAB > 191) sprintf_s(Reserved, "%2.2f id %d snapdist %2.2f snaproll", MeleeKill - 3.0f, MeleeTarg, MeleeKill - 3.0f); //Strike stick code to adjust for Rogue AAs
                //  else           sprintf_s(Reserved, "%2.2f id %d behindonce", MeleeKill - 3.0f, MeleeTarg); //Old default strike stick code
                    else           sprintf_s(Reserved, "%2.2f id %d snapdist %2.2f snaproll", MeleeKill - 3.0f, MeleeTarg, MeleeKill - 3.0f); //New default strike stick code htw 2/20/2011
                }
                if (!Sticking && strcmp(Reserved, StickArg))
                {
                    Stick(Reserved);
                    return;
                }
                if (MeleeDist > MeleeKill || MeleeView > 60 || MeleeBack > doSTAB)
                {
                    SwingHits++;
                }
                else if (idBACKSTAB.Ready("") && TimeSince(HiddenTimer) > 3000)
                //Original less verbose strike code
                {
                    if (Sticking) Stick("");
                    if (doSTRIKE && idSTRIKE.Ready(ifSTRIKE) && !StrikeFail) idSTRIKE.Press(); //Rogue StrikeFail Fix htw 2/20/2011
                    else StabPress();
                }
            }
        }
        if (Sticking && (SwingHits || TakenHits || !IsSneaking() || !IsInvisible())) Stick("");
        return;
    }
    // end rogue striking/assassination

    // jolting times!
    if ((doJLTKICKS || doJOLT || doSTORMBLADES) && !doAGGRO && SwingHits > doJOLT)
    {
        long MyEndu = GetPcProfile()->Endurance * 100 / GetMaxEndurance();
        if (idJOLT.Ready(ifJOLT))
        {
            idJOLT.Press();
            SwingHits = 1;
        }
        if (idJLTKICKS.Ready(ifJLTKICKS) && (MyEndu > doJLTKICKS))
        {
            idJLTKICKS.Press();
            SwingHits = 1;
        }
        if (idSTORMBLADES.Ready(ifSTORMBLADES) && (MyEndu > doSTORMBLADES))
        {
            idSTORMBLADES.Press();
            SwingHits = 1;
        }

    }
    //end jolt

    // handle melee
    if (doMELEE && !(onEVENT & 0x8000))
    {
        if (onEVENT & 0x0008 && !(onEVENT & 0xF007) && !pEverQuestInfo->bAutoAttack) AttackON();

        // start stick processing
        if (onSTICK)      // would have value if StickReset called, or set by itself
        {
            if (onSTICK > 0) // value would be positive from StickReset
            {
                // if no timer (set here) -- if (no range set or distance to target less than stick range, set timer
                if (!TimerStik) if (!doSTICKRANGE || doSTICKNORANGE || MeleeDist < doSTICKRANGE) TimerStik = (unsigned long)clock() + doSTICKDELAY * 1000;
                // if not moving and not sticking and (no delay or timer is up) and no range set or distance to target less than stick range
                // set onstick negative so below if is processed
                // *** if a range is set and we are greater than this range, this may cause those "open space" problems
                if (Immobile && !Sticking && (!doSTICKDELAY || (unsigned long)clock()>TimerStik) && (!doSTICKRANGE || doSTICKNORANGE || MeleeDist < doSTICKRANGE)) onSTICK = -1;
            }
            if (onSTICK < 0)        // set negative by above in cases where we should stick
            {
                if (bMULoaded && bMUPointers)
                {
                    switch (doSTICKMODE)
                    {
                        case 0: // stick from mq2melee
                        {
                            long type = Aggroed(MeleeTarg); // set to 1 if cases acceptable to attack (on hott, within range)
                            bool swim = (SpawnMe()->UnderWater == 5); // use "uw" arg if underwater
                            bool stab = (type < 1 && doBACKSTAB && doSTAB < 192); // use "behind" if not aggro and strike option enabled, else !front
                            bool tank = (type > 0 || (!IsInvisible() && (doAGGRO || !pLocalPC->pGroupInfo))); // use "moveback" if tank and aggro, else process stab
                            double dist = MeleeKill - 3.0f - (MeleeFlee * 3.0f); // stick numeric parameter for distance
                            sprintf_s(Reserved, "%2.2f id %d%s%s", dist, MeleeTarg, MeleeFlee ? "" : (tank ? " moveback" : (!stab ? " !front" : " behind")), swim ? " uw" : "");
                            if (strcmp(Reserved, StickArg) || (!doSTICKBREAK && !*pbStickOn)) Stick(Reserved);
                            break;
                        }
                        case 1: // stick from ini
                        {
                            strcpy_s(Reserved, StickCMD.c_str());           // copy our ini command to Reserved
                            ParseMacroData(Reserved, sizeof(Reserved));     // parse out TLO evaluations
                            if (strcmp(Reserved, StickArg) || (!doSTICKBREAK && !*pbStickOn)) Stick(Reserved);
                            break;
                        }
                        default: // do not stick
                            break;
                    }
                }
            }
        }
        // end stick processing

        // not behind enraged/infuriated target?
        if (onEVENT & 0x0003 && pEverQuestInfo->bAutoAttack)
        {
            long haveAggro = Aggroed(MeleeTarg); // set to 1 if cases acceptable to attack (on hott, within range)
            if ((MeleeBack > 92 || onEVENT & 0x0002) && (haveAggro < 1))
            {
                if (MeleeBack > 92) Announce(SHOW_ENRAGING, "%s::\arNOT BEHIND\ax enraged target, taking action!", PLUGIN_NAME);
                onEVENT |= 0x0008;
                AttackOFF();
                return;
            }
        }

        // check target is in melee range?
        if (MeleeDist < MeleeKill)
        {
            // attack is off, good time for stealing/begging or evading?
            if (!pEverQuestInfo->bAutoAttack)
            {
#if !IS_CLIENT_DATE(20140402)
                if (doEVADE && !doAGGRO && !IsInvisible() && idHIDE.Ready(ifEVADE)) idHIDE.Press();
#endif                
                onEVENT &= 0xBFFF;
                if (!MeleeFlee)
                {
                    if (MeleeDist < 10 && doPICKPOCKET && idPICKPOCKET.Ready(ifPICKPOCKET))
                    {
                        if (!CursorEmpty()) EzCommand("/autoinventory");
                        idPICKPOCKET.Press();
                        onEVENT |= 0x1008;
                    }
                    else if (doBEGGING && !IsInvisible() && idBEGGING.Ready(ifBEGGING))
                    {
                        idBEGGING.Press();
                        onEVENT |= 0x2008;
                    }
                    //if (doEVADE && !doAGGRO && Immobile && !IsInvisible() && idHIDE.Ready(ifEVADE))
                    //{
                    //    idHIDE.Press();
                    //    onEVENT |= 0x0208;
                    //}
                    //else if (doFALLS && !doAGGRO && FeignDeath->Ready(ifFALLS))
                    else if (doFALLS && !doAGGRO && FeignDeath && FeignDeath->Ready(ifFALLS))
                    {
                        FeignDeath->Press();
                        onEVENT |= 0x0408;
                    }
                }
                if (onEVENT & 0x0001 && !(onEVENT & 0xFFF6) && MeleeBack < 92)
                {
                    Announce(SHOW_ENRAGING, "%s::\agBEHIND\ax TARGET kicking attack ON!!!", PLUGIN_NAME);
                    onEVENT &= 0xFFF6; AttackON(); onEVENT |= 0x0009;
                }
            }
            else   // attack is on so lets do some dps?
            {
                if (doBACKSTAB && MeleeBack < doSTAB && MeleeView < 60 && idBACKSTAB.Ready(ifBACKSTAB) && StabCheck()) StabPress();
                if (doBASH && idBASH.Ready(ifBASH) && BashCheck()) BashPress();
                if (doSLAM && idSLAM.Ready(ifSLAM)) idSLAM.Press();
                if (doFRENZY && idFRENZY.Ready(ifFRENZY)) idFRENZY.Press();
                if (doKICK && idKICK.Ready(ifKICK)) idKICK.Press();
                if (!disc && !IsInvisible() && !MeleeFlee && onEVENT & 0x4000 && pEverQuestInfo->bAutoAttack)  AttackOFF();
                if (!disc && Immobile && !IsInvisible() && !MeleeFlee)
                {
                    if (MeleeDist < 10 && doPICKPOCKET   && idPICKPOCKET.Ready(ifPICKPOCKET))                    onEVENT |= 0x4008;
                    if (doBEGGING      && idBEGGING.Ready(ifBEGGING))                          onEVENT |= 0x4008;
                    // if (doFALLS        && !doAGGRO && FeignDeath->Ready(ifFALLS))              onEVENT |= 0x4008;
                    if (doFALLS        && !doAGGRO && FeignDeath && FeignDeath->Ready(ifFALLS))              onEVENT |= 0x4008;
                    // Hide/evade no longer requires attack be turned off
                    // if (doEVADE        && !(doAGGRO || !IsGrouped()) && idHIDE.Ready(ifEVADE)) onEVENT |= 0x4008;
                    // if (onEVENT & 0x4000 && pEverQuestInfo->bAutoAttack) AttackOFF();
                }

                if (doEVADE && !doAGGRO && !IsInvisible() && idHIDE.Ready(ifEVADE)) {
#if !IS_CLIENT_DATE(20140402)
                    AttackOFF();
#else
                    idHIDE.Press(); // On clients newer than 2014, attack doesn't have to be turned off to evade
#endif
                }
                if (MeleeDist < 15)
                {
                    if (doDISARM && idDISARM.Ready(ifDISARM)) idDISARM.Press();
                }
            }
            if (doINTIMIDATION && idINTIMIDATION.Ready(ifINTIMIDATION)) idINTIMIDATION.Press();
            if (doTAUNT && doAGGRO && idTAUNT.Ready(ifTAUNT)) idTAUNT.Press();
        }
        //end target is in range

        if (MeleeFlee) SneakOFF();
        if (onEVENT & 0x3000) onEVENT &= 0xCFFF;
    }
    // end handle melee

    // handle ranged?
    if (doRANGE || onEVENT & 0x8000)
    {
        // should we face target? not moving, stopped >2sec, facing>2sec and not sticking?
        if (doFACING && Immobile && (unsigned long)clock() > TimerFace && !Sticking)
        {
            if (MeleeView > 30) Face(SpawnMe(), "nolook");
            TimerFace = (unsigned long)clock() + delay * 8;
        }
        // are we in good ranged for ranged?
        if (MeleeDist<(doRANGE ? doRANGE : 250) && MeleeDist>35 && MeleeDist > MeleeKill + 20)
        {
            if (!AutoFire && pEverQuestInfo->PrimaryAttackReady) ThrowIT(NULL, "");
            if (pEverQuestInfo->bAutoAttack)
            {
                if (Sticking) Stick("");
                onEVENT |= 0x8000;
                AttackOFF();
                Announce(SHOW_SWITCHING, "%s::Switching [\ayRange\ax].", PLUGIN_NAME);
            }
        }
        else if (!pEverQuestInfo->bAutoAttack) // target too close? or too far?
        {
            if (AutoFire)
            {
                EzCommand("/autofire");
                AutoFire = false;
            }
            if (Immobile && onEVENT & 0x8000)
            {
                onEVENT &= 0x7FF7;
                if (doMELEE)
                {
                    StickReset();
                    onEVENT |= 0x0008;
                    AttackON();
                    Announce(SHOW_SWITCHING, "%s::Switching [\ayMelee\ax].", PLUGIN_NAME);
                }
            }
        }
    }

    // time to handle spell casting?
    if (!TargetID(MeleeTarg) || MeleeDist > 200) return;
    long MyEndu = GetPcProfile()->Endurance * 100 / GetMaxEndurance();

    // slap in the face needs to happen before combat starts
    if (doSLAPFACE && MyEndu > doSLAPFACE && idSLAPFACE.Ready(ifSLAPFACE)) idSLAPFACE.Press();

    // should we stun that target?
    if (doSTUNNING && MeleeLife <= doSTUNNING)
    {
        Ability *UseThis = NULL;
        if (idSTUN[0].Ready(ifSTUNNING))      UseThis = &idSTUN[0];
        else if (idSTUN[1].Ready(ifSTUNNING)) UseThis = &idSTUN[1];
        if (UseThis)
        {
            Announce(SHOW_STUNNING, "%s::Stunning [\ay%s\ax].", PLUGIN_NAME, MeleeName);
            UseThis->Press();
        }
    }

    // are we grouped?
    if (IsGrouped())
    {
        // Time to build and maintain aggro?
        if (doAGGRO)
        {
            long HaveAggro = Aggroed(MeleeTarg);

            // should we challenge for to maintain aggro over time?
            if (doCHALLENGEFOR && onCHALLENGEFOR && HaveAggro == 1 && idCHALLENGEFOR.Ready(ifCHALLENGEFOR)) {
                Announce(SHOW_PROVOKING, "%s::Challenging [\ay%s\ax].", PLUGIN_NAME, MeleeName);
                idCHALLENGEFOR.Press();
                onCHALLENGEFOR--;
            }

              // should we challenge for to maintain aggro over time?
            if (doENRAGINGKICK && onENRAGINGKICK && HaveAggro == 1 && idENRAGINGKICK.Ready(ifENRAGINGKICK)) {
                Announce(SHOW_PROVOKING, "%s::Enraging Kick [\ay%s\ax].", PLUGIN_NAME, MeleeName);
                idENRAGINGKICK.Press();
                onENRAGINGKICK--;
            }

            // should we provoke at least once and/or when aggro is lost?
            if (onBELOW && MeleeLife > doPROVOKEEND && MeleeDist < 100 && (HaveAggro < 1 || (doPROVOKEONCE && onBELOW == doPROVOKEMAX)))
            {
                Ability *UseThis = NULL;
                if (idPROVOKE[0].Ready(ifPROVOKE))      UseThis = &idPROVOKE[0];
                else if (idPROVOKE[1].Ready(ifPROVOKE)) UseThis = &idPROVOKE[1];
                if (UseThis)
                {
                    Announce(SHOW_PROVOKING, "%s::Provoking [\ay%s\ax].", PLUGIN_NAME, MeleeName);
                    UseThis->Press();
                    onBELOW--;
                }
            }
        }

        // should we use short duration melee buff?
        if (GetPcProfile()->Endurance > 200)
        {
            if (doBLOODLUST  && MyEndu > doBLOODLUST && idBLOODLUST.Ready(ifBLOODLUST) && MeleeDist < 50) idBLOODLUST.Press();
            if (doCOMMANDING && MyEndu > doCOMMANDING && idCOMMANDING.Ready(ifCOMMANDING)) idCOMMANDING.Press();
            if (doFISTSOFWU  && MyEndu > doFISTSOFWU  && idFISTSOFWU.Ready(ifFISTSOFWU))   idFISTSOFWU.Press();
            if (doCRYHAVOC   && MyEndu > doCRYHAVOC   && idCRYHAVOC.Ready(ifCRYHAVOC) && disc != d_cleaverage1 && disc != d_cleaverage2 && disc != d_cleaverage4 && disc != d_cleaverage4 && disc != d_cleaverage5) idCRYHAVOC.Press();
            if (doTHIEFEYE   && MyEndu > doTHIEFEYE   && idTHIEFEYE.Ready(ifTHIEFEYE))     idTHIEFEYE.Press();
        }
    }
    // If berserker do discs
    if (BerserkerClass && PressDelay < MQGetTickCount64())
    {
        Ability *UseThis = NULL;
        PressDelay = 0;
        if (doCRIPPLE && MyEndu > doCRIPPLE && idCRIPPLE.Ready(ifCRIPPLE) && MeleeDist < 175) UseThis = &idCRIPPLE;
        else if (doRAGEVOLLEY && MyEndu > doRAGEVOLLEY && idRAGEVOLLEY.Ready(ifRAGEVOLLEY) && MeleeDist < 175)  UseThis = &idRAGEVOLLEY;
        else if (doVIGAXE && MyEndu > doVIGAXE && idVIGAXE.Ready(ifVIGAXE)) UseThis = &idVIGAXE;
        else if (doRALLOS && MyEndu > doRALLOS && idRALLOS.Ready(ifRALLOS) && MeleeDist < 50) UseThis = &idRALLOS;
        else if (doOPFRENZY && MyEndu > doOPFRENZY && idOPFRENZY.Ready(ifOPFRENZY)) UseThis = &idOPFRENZY;
        if (UseThis)
        {
            UseThis->Press();
            PressDelay = MQGetTickCount64() + 1000;
        }
    }
    // If Monk do discs and attacks
    if (MonkClass && PressDelay < MQGetTickCount64())
    {
        Ability *UseThis = NULL;
        PressDelay = 0;
        if (!disc && doMONKEY && MyEndu > doMONKEY && idMONKEY.Ready(ifMONKEY) && MeleeDist < 50) UseThis = &idMONKEY;
        else if (doLEOPARDCLAW && MyEndu > doLEOPARDCLAW && idLEOPARDCLAW.Ready(ifLEOPARDCLAW)) UseThis = &idLEOPARDCLAW;
        else if (doVIGSHURIKEN && MyEndu > doVIGSHURIKEN && idVIGSHURIKEN.Ready(ifVIGSHURIKEN)) UseThis = &idVIGSHURIKEN;
        else if (doDRAGONPUNCH && idDRAGONPUNCH.Ready(ifDRAGONPUNCH)) UseThis = &idDRAGONPUNCH;
        else if (doEAGLESTRIKE && idEAGLESTRIKE.Ready(ifEAGLESTRIKE)) UseThis = &idEAGLESTRIKE;
        else if (doTIGERCLAW && idTIGERCLAW.Ready(ifTIGERCLAW)) UseThis = &idTIGERCLAW;
        else if (doROUNDKICK && idROUNDKICK.Ready(ifROUNDKICK)) UseThis = &idROUNDKICK;
        else if (doSYNERGY && MyEndu > doSYNERGY && idSYNERGY.Ready(ifSYNERGY)) UseThis = &idSYNERGY;
        else if (doFLYINGKICK && idFLYINGKICK.Ready(ifFLYINGKICK)) UseThis = &idFLYINGKICK;
        else if (doCLOUD && MyEndu > doCLOUD && idCLOUD.Ready(ifCLOUD))  UseThis = &idCLOUD;
        if (UseThis)
        {
            UseThis->Press();
            PressDelay = MQGetTickCount64() + 500;
        }
    }
    // is target close enough for those?
    if (MeleeDist < 50)
    {
        // ON/OFF switch
        if (doBATTLELEAP             && idBATTLELEAP.Ready(ifBATTLELEAP))                    idBATTLELEAP.Press();
        if (doASP                    && idASP.Ready(ifASP))                                  idASP.Press();
        if (doBOASTFUL               && idBOASTFUL.Ready(ifBOASTFUL))                        idBOASTFUL.Press();
        if (doHARMTOUCH              && idHARMTOUCH.Ready(ifHARMTOUCH))                      idHARMTOUCH.Press();
        if (doTHROATJAB              && idTHROATJAB.Ready(ifTHROATJAB))                      idTHROATJAB.Press();
        if (doCALLCHALLENGE          && idCALLCHALLENGE.Ready(ifCALLCHALLENGE))              idCALLCHALLENGE.Press();
        if (doCSTRIKE                && idCSTRIKE.Ready(ifCSTRIKE))                          idCSTRIKE.Press();
        if (doSELOS                  && idSELOS.Ready(ifSELOS))                              idSELOS.Press();
        if (doEYEGOUGE               && idEYEGOUGE.Ready(ifEYEGOUGE))                        idEYEGOUGE.Press();
        if (doFERALSWIPE             && idFERALSWIPE.Ready(ifFERALSWIPE))                    idFERALSWIPE.Press();
        if (doGORILLASMASH           && idGORILLASMASH.Ready(ifGORILLASMASH))                idGORILLASMASH.Press();
        if (doGUTPUNCH               && idGUTPUNCH.Ready(ifGUTPUNCH))                        idGUTPUNCH.Press();
        if (doJOLT                   && idJOLT.Ready(ifJOLT))                                idJOLT.Press();
        if (doKNEESTRIKE             && idKNEESTRIKE.Ready(ifKNEESTRIKE))                    idKNEESTRIKE.Press();
        if (doRAVENS                 && idRAVENS.Ready(ifRAVENS))                            idRAVENS.Press();
        if (doSTEELY                 && idSTEELY.Ready(ifSTEELY))                            idSTEELY.Press();

        // Endurance above disc switch
        //Rogue begin
        if (doPINPOINT && MyEndu > doPINPOINT && idPINPOINT.Ready(ifPINPOINT)) idPINPOINT.Press();
        if (doKNIFEPLAY && idKNIFEPLAY.Ready(ifKNIFEPLAY)) idKNIFEPLAY.Press();
        if (doVIGDAGGER && MyEndu > doVIGDAGGER && idVIGDAGGER.Ready(ifVIGDAGGER)) idVIGDAGGER.Press();
        if (doTWISTEDSHANK && idTWISTEDSHANK.Ready(ifTWISTEDSHANK)) idTWISTEDSHANK.Press();
        if (doJUGULAR && MyEndu > doJUGULAR && idJUGULAR.Ready(ifJUGULAR)) idJUGULAR.Press();
        if (doASSAULT && MyEndu > doASSAULT && idASSAULT.Ready(ifASSAULT)) idASSAULT.Press();
        if (doBLEED && MyEndu > doBLEED && idBLEED.Ready(ifBLEED)) idBLEED.Press();
        // end Rogue

        // Endurance above disc switch
        //Ranger begin
        if (doJLTKICKS && MyEndu > doJLTKICKS && idJLTKICKS.Ready(ifJLTKICKS)) idJLTKICKS.Press();
        if (doSTORMBLADES && MyEndu > doSTORMBLADES && idSTORMBLADES.Ready(ifSTORMBLADES)) idSTORMBLADES.Press();
        //if (doFEROCIOUSKICK && idFEROCIOUSKICK.Ready(ifFEROCIOUSKICK)) idFEROCIOUSKICK.Press();
        // end Ranger

        if (doBVIVI       && MyEndu > doBVIVI          && idBVIVI.Ready(ifBVIVI))              idBVIVI.Press();
        if (doDEFENSE     && MyEndu > doDEFENSE        && idDEFENSE.Ready(ifDEFENSE))          idDEFENSE.Press();
        if (doFCLAW       && MyEndu > doFCLAW          && idFCLAW.Ready(ifFCLAW))              idFCLAW.Press();
        if (doFIELDARM    && MyEndu > doFIELDARM       && idFIELDARM.Ready(ifFIELDARM))        idFIELDARM.Press();
        if (doGBLADE       && MyEndu> doGBLADE         && idGBLADE.Ready(ifGBLADE))            idGBLADE.Press();
        if (doRAKE        && MyEndu > doRAKE           && idRAKE.Ready(ifRAKE))                idRAKE.Press();
        if (doRIGHTIND    && MyEndu > doRIGHTIND       && idRIGHTIND.Ready(ifRIGHTIND))        idRIGHTIND.Press();
        if (doTHROWSTONE  && MyEndu > doTHROWSTONE     && idTHROWSTONE.Ready(ifTHROWSTONE))    idTHROWSTONE.Press();
        if (doWITHSTAND   && MyEndu > doWITHSTAND      && idWITHSTAND.Ready(ifWITHSTAND))      idWITHSTAND.Press();
        // Opportunistic Strike check if mob health below 20%
        if (doOPPORTUNISTICSTRIKE && MyEndu > doOPPORTUNISTICSTRIKE && MeleeLife <= 20 && idOPPORTUNISTICSTRIKE.Ready(ifOPPORTUNISTICSTRIKE)) idOPPORTUNISTICSTRIKE.Press();
    }

    // time to handle holy shit?
    if (HOLYSHITIF.length()<5 || Evaluate((char*)("${If[" + HOLYSHITIF.substr(5, HOLYSHITIF.length() - 6) + ",1,0]}").c_str())) {

        if (HOLYSHIT[doHOLY].size()) {
            switch (doHOLYFLAG[doHOLY])
            {
            case 1://not a macro dependant flag
            {
                EzCommand((char*)HOLYSHIT[doHOLY].c_str());
                break;
            }
            case 2://a macro must be running and the variable MUST exist
            {
                if (gRunning) {
                    if (OKtoParseShit(HOLYSHIT[doHOLY])) {
                        EzCommand((char*)HOLYSHIT[doHOLY].c_str());
                    }
                }
                break;
            }
            case 3: //only parsed when no macro running
            {
                if (!gRunning) EzCommand((char*)HOLYSHIT[doHOLY].c_str());
                break;
            }
            default:
                break;
            }
        }
        for (long x = 1; x < 91; x++)
        {
            long n = (x + doHOLY) % 91;
            if (doHOLYFLAG[n])
            {
                doHOLY = n;
                break;
            }
        }
    }
}

void KeyMelee(const char* NAME, bool Down)
{
    if (Down && pTarget)
    {
        if (!doSKILL) EzCommand("/keypress AUTOPRIM");
        else if (!MeleeTarg) KillThis(NULL, "");
        else Override(NULL, "%s::\arOVERRIDE\ax taking actions!");
    }
}

void KeyRange(const char* NAME, bool Down)
{
    if (Down && pTarget)
    {
        if (!doSKILL) EzCommand("/keypress RANGED");
        else ThrowIT(NULL, "");
    }
}

void Bindding(bool BindMode)
{
    if (BindMode)
    {
        if (!Binded)
        {
            KeyCombo  MeleeCombo, RangeCombo;
            RemoveMQ2KeyBind("MELEE"); AddMQ2KeyBind("MELEE", KeyMelee); ParseKeyCombo(MeleeKey, MeleeCombo); SetMQ2KeyBind("MELEE", false, MeleeCombo);
            RemoveMQ2KeyBind("RANGE"); AddMQ2KeyBind("RANGE", KeyRange); ParseKeyCombo(RangeKey, RangeCombo); SetMQ2KeyBind("RANGE", false, RangeCombo);
            Binded = true;
        }
    }
    else if (Binded)
    {
        RemoveMQ2KeyBind("MELEE");
        RemoveMQ2KeyBind("RANGE");
        Binded = false;
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void __stdcall AUTOFIREOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    AutoFire = false;
    if (AutoFire) KeyMelee("", true);
}

void __stdcall AUTOFIREON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (pTarget && TargetType(NPC_TYPE) && InGame()) AutoFire = true;
}

void __stdcall CASTING(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (doSKILL && MeleeTarg && ci_equals(pValues->Value, MeleeName, MeleeSize)) MeleeCast = (unsigned long)clock();
}

void __stdcall ENRAGEON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && ci_equals(pValues->Value, MeleeName, MeleeSize)) EnrageON(NULL, "");
}

void __stdcall ENRAGEOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && ci_equals(pValues->Value, MeleeName, MeleeSize)) EnrageOFF(NULL, "");
}

void __stdcall INFURIATEON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && ci_equals(pValues->Value, MeleeName, MeleeSize)) InfuriateON(NULL, "");
}

void __stdcall INFURIATEOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && ci_equals(pValues->Value, MeleeName, MeleeSize)) InfuriateOFF(NULL, "");
}

void __stdcall PETATTK(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    PetSEEN();
}

void __stdcall PETBACK(unsigned int ID, void *pData, PBLECHVALUE pValues) {
   if (HaveGHold && doPETASSIST)
    {
        Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet ghold on");
        EzCommand("/pet ghold on");
        if (PetOnAttk) PetOnAttk = (unsigned long)clock() + 1000;
        PetOnHold = true;
    }
    else
    {
        if (HaveHold && doPETASSIST)
        {
            Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet hold on");
            EzCommand("/pet hold on");
            if (PetOnAttk) PetOnAttk = (unsigned long)clock() + 1000;
            PetOnHold = true;
        }
    }
}
void __stdcall PETHOLD(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    PetOnHold = true;
}

void __stdcall FALLEN(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (!doSKILL || ((long)(uintptr_t)pData && !ci_equals(pValues->Value, pLocalPC->Name, strlen(pLocalPC->Name) + 1))) return;
    Announce(SHOW_FEIGN, "%s::\arFAILED FEIGN DEATH\ax taking action!", PLUGIN_NAME);
    EzCommand("/stand");
}

void __stdcall BROKEN(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (doSKILL) BrokenFD = (unsigned long)clock() + 1;
}

void __stdcall RESUME(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (doSKILL) BrokenFD = 0;
}

void __stdcall SNEAKON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    SilentTimer = (unsigned long)clock();
}

void __stdcall SNEAKOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    SilentTimer = 0;
}

void __stdcall HIDEON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    HiddenTimer = (unsigned long)clock();
}

void __stdcall HIDEOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    HiddenTimer = 0;
}
//Rogue Strike Fix htw 2/20/2011
void __stdcall STRIKERESET(unsigned int ID, void *pData, PBLECHVALUE pValues)
{
    StrikeFail = true;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API void InitializePlugin()
{
    NPC_TYPE = GetPrivateProfileInt("Settings", "SpawnType", 0x000A, INIFileName);
    GetPrivateProfileString("Settings", "MeleeKeys", "z", MeleeKey, sizeof(MeleeKey), INIFileName);
    GetPrivateProfileString("Settings", "RangeKeys", "x", RangeKey, sizeof(RangeKey), INIFileName);
    SETBINDINGS = GetPrivateProfileInt("Settings", "Bindings", 1, INIFileName);

    CmdListe.clear();
    REGISTER_ABILITY_OPTION(pDEBUG, NULL, &doDEBUG);
    REGISTER_ABILITY_OPTION(pAGGRO, AggroReset, &doAGGRO);
    REGISTER_ABILITY_OPTION(pAGGRP, NULL, &elAGGROPRI);
    REGISTER_ABILITY_OPTION(pAGGRS, NULL, &elAGGROSEC);
    REGISTER_ABILITY_OPTION(pARROW, NULL, &elARROWS);
    REGISTER_ABILITY_OPTION(pASSLT, NULL, &doASSAULT);
    REGISTER_ABILITY_OPTION(pASSAS, NULL, &doASSASSINATE);
    REGISTER_ABILITY_OPTION(pBASHS, NULL, &doBASH);
    REGISTER_ABILITY_OPTION(pBBLOW, NULL, &doBOASTFUL);
    REGISTER_ABILITY_OPTION(pBGING, NULL, &doBEGGING);
    REGISTER_ABILITY_OPTION(pBKOFF, NULL, &doBACKOFF);
    REGISTER_ABILITY_OPTION(pBLEED, NULL, &doBLEED);
    REGISTER_ABILITY_OPTION(pBLUST, NULL, &doBLOODLUST);
    REGISTER_ABILITY_OPTION(pBOWID, NULL, &elRANGED);
    REGISTER_ABILITY_OPTION(pBSTAB, NULL, &doBACKSTAB);
    REGISTER_ABILITY_OPTION(pBTASP, NULL, &doASP);
    REGISTER_ABILITY_OPTION(pBTLLP, NULL, &doBATTLELEAP);
    REGISTER_ABILITY_OPTION(pBVIVI, NULL, &doBVIVI);
    REGISTER_ABILITY_OPTION(pCALLC, NULL, &doCALLCHALLENGE);
    REGISTER_ABILITY_OPTION(pCFIST, NULL, &doCLOUD);
    REGISTER_ABILITY_OPTION(pCHAMS, NULL, &doCSTRIKE);
    REGISTER_ABILITY_OPTION(pCHFOR, NULL, &doCHALLENGEFOR);
    REGISTER_ABILITY_OPTION(pCOMMG, NULL, &doCOMMANDING);
    REGISTER_ABILITY_OPTION(pCRIPS, NULL, &doCRIPPLE);
    REGISTER_ABILITY_OPTION(pCRYHC, NULL, &doCRYHAVOC);
    REGISTER_ABILITY_OPTION(pDEFEN, NULL, &doDEFENSE);
    REGISTER_ABILITY_OPTION(pDISRM, NULL, &doDISARM);
    REGISTER_ABILITY_OPTION(pDMONK, NULL, &doMONKEY);
    REGISTER_ABILITY_OPTION(pDRPNC, NULL, &doDRAGONPUNCH);
    REGISTER_ABILITY_OPTION(pDWNF0, NULL, &doDOWNFLAG[0]);
    REGISTER_ABILITY_OPTION(pDWNF1, NULL, &doDOWNFLAG[1]);
    REGISTER_ABILITY_OPTION(pDWNF2, NULL, &doDOWNFLAG[2]);
    REGISTER_ABILITY_OPTION(pDWNF3, NULL, &doDOWNFLAG[3]);
    REGISTER_ABILITY_OPTION(pDWNF4, NULL, &doDOWNFLAG[4]);
    REGISTER_ABILITY_OPTION(pDWNF5, NULL, &doDOWNFLAG[5]);
    REGISTER_ABILITY_OPTION(pDWNF6, NULL, &doDOWNFLAG[6]);
    REGISTER_ABILITY_OPTION(pDWNF7, NULL, &doDOWNFLAG[7]);
    REGISTER_ABILITY_OPTION(pDWNF8, NULL, &doDOWNFLAG[8]);
    REGISTER_ABILITY_OPTION(pDWNF9, NULL, &doDOWNFLAG[9]);
    REGISTER_ABILITY_OPTION(pDWNF10, NULL, &doDOWNFLAG[10]);
    REGISTER_ABILITY_OPTION(pDWNF11, NULL, &doDOWNFLAG[11]);
    REGISTER_ABILITY_OPTION(pDWNF12, NULL, &doDOWNFLAG[12]);
    REGISTER_ABILITY_OPTION(pDWNF13, NULL, &doDOWNFLAG[13]);
    REGISTER_ABILITY_OPTION(pDWNF14, NULL, &doDOWNFLAG[14]);
    REGISTER_ABILITY_OPTION(pDWNF15, NULL, &doDOWNFLAG[15]);
    REGISTER_ABILITY_OPTION(pDWNF16, NULL, &doDOWNFLAG[16]);
    REGISTER_ABILITY_OPTION(pDWNF17, NULL, &doDOWNFLAG[17]);
    REGISTER_ABILITY_OPTION(pDWNF18, NULL, &doDOWNFLAG[18]);
    REGISTER_ABILITY_OPTION(pDWNF19, NULL, &doDOWNFLAG[19]);
    REGISTER_ABILITY_OPTION(pDWNF20, NULL, &doDOWNFLAG[20]);
    REGISTER_ABILITY_OPTION(pDWNF21, NULL, &doDOWNFLAG[21]);
    REGISTER_ABILITY_OPTION(pDWNF22, NULL, &doDOWNFLAG[22]);
    REGISTER_ABILITY_OPTION(pDWNF23, NULL, &doDOWNFLAG[23]);
    REGISTER_ABILITY_OPTION(pDWNF24, NULL, &doDOWNFLAG[24]);
    REGISTER_ABILITY_OPTION(pDWNF25, NULL, &doDOWNFLAG[25]);
    REGISTER_ABILITY_OPTION(pDWNF26, NULL, &doDOWNFLAG[26]);
    REGISTER_ABILITY_OPTION(pDWNF27, NULL, &doDOWNFLAG[27]);
    REGISTER_ABILITY_OPTION(pDWNF28, NULL, &doDOWNFLAG[28]);
    REGISTER_ABILITY_OPTION(pDWNF29, NULL, &doDOWNFLAG[29]);
    REGISTER_ABILITY_OPTION(pDWNF30, NULL, &doDOWNFLAG[30]);
    REGISTER_ABILITY_OPTION(pDWNF31, NULL, &doDOWNFLAG[31]);
    REGISTER_ABILITY_OPTION(pDWNF32, NULL, &doDOWNFLAG[32]);
    REGISTER_ABILITY_OPTION(pDWNF33, NULL, &doDOWNFLAG[33]);
    REGISTER_ABILITY_OPTION(pDWNF34, NULL, &doDOWNFLAG[34]);
    REGISTER_ABILITY_OPTION(pDWNF35, NULL, &doDOWNFLAG[35]);
    REGISTER_ABILITY_OPTION(pDWNF36, NULL, &doDOWNFLAG[36]);
    REGISTER_ABILITY_OPTION(pDWNF37, NULL, &doDOWNFLAG[37]);
    REGISTER_ABILITY_OPTION(pDWNF38, NULL, &doDOWNFLAG[38]);
    REGISTER_ABILITY_OPTION(pDWNF39, NULL, &doDOWNFLAG[39]);
    REGISTER_ABILITY_OPTION(pDWNF40, NULL, &doDOWNFLAG[40]);
    REGISTER_ABILITY_OPTION(pDWNF41, NULL, &doDOWNFLAG[41]);
    REGISTER_ABILITY_OPTION(pDWNF42, NULL, &doDOWNFLAG[42]);
    REGISTER_ABILITY_OPTION(pDWNF43, NULL, &doDOWNFLAG[43]);
    REGISTER_ABILITY_OPTION(pDWNF44, NULL, &doDOWNFLAG[44]);
    REGISTER_ABILITY_OPTION(pDWNF45, NULL, &doDOWNFLAG[45]);
    REGISTER_ABILITY_OPTION(pDWNF46, NULL, &doDOWNFLAG[46]);
    REGISTER_ABILITY_OPTION(pDWNF47, NULL, &doDOWNFLAG[47]);
    REGISTER_ABILITY_OPTION(pDWNF48, NULL, &doDOWNFLAG[48]);
    REGISTER_ABILITY_OPTION(pDWNF49, NULL, &doDOWNFLAG[49]);
    REGISTER_ABILITY_OPTION(pDWNF50, NULL, &doDOWNFLAG[50]);
    REGISTER_ABILITY_OPTION(pDWNF51, NULL, &doDOWNFLAG[51]);
    REGISTER_ABILITY_OPTION(pDWNF52, NULL, &doDOWNFLAG[52]);
    REGISTER_ABILITY_OPTION(pDWNF53, NULL, &doDOWNFLAG[53]);
    REGISTER_ABILITY_OPTION(pDWNF54, NULL, &doDOWNFLAG[54]);
    REGISTER_ABILITY_OPTION(pDWNF55, NULL, &doDOWNFLAG[55]);
    REGISTER_ABILITY_OPTION(pDWNF56, NULL, &doDOWNFLAG[56]);
    REGISTER_ABILITY_OPTION(pDWNF57, NULL, &doDOWNFLAG[57]);
    REGISTER_ABILITY_OPTION(pDWNF58, NULL, &doDOWNFLAG[58]);
    REGISTER_ABILITY_OPTION(pDWNF59, NULL, &doDOWNFLAG[59]);
    REGISTER_ABILITY_OPTION(pDWNF60, NULL, &doDOWNFLAG[60]);
    REGISTER_ABILITY_OPTION(pDWNF61, NULL, &doDOWNFLAG[61]);
    REGISTER_ABILITY_OPTION(pDWNF62, NULL, &doDOWNFLAG[62]);
    REGISTER_ABILITY_OPTION(pDWNF63, NULL, &doDOWNFLAG[63]);
    REGISTER_ABILITY_OPTION(pDWNF64, NULL, &doDOWNFLAG[64]);
    REGISTER_ABILITY_OPTION(pDWNF65, NULL, &doDOWNFLAG[65]);
    REGISTER_ABILITY_OPTION(pDWNF66, NULL, &doDOWNFLAG[66]);
    REGISTER_ABILITY_OPTION(pDWNF67, NULL, &doDOWNFLAG[67]);
    REGISTER_ABILITY_OPTION(pDWNF68, NULL, &doDOWNFLAG[68]);
    REGISTER_ABILITY_OPTION(pDWNF69, NULL, &doDOWNFLAG[69]);
    REGISTER_ABILITY_OPTION(pDWNF70, NULL, &doDOWNFLAG[70]);
    REGISTER_ABILITY_OPTION(pDWNF71, NULL, &doDOWNFLAG[71]);
    REGISTER_ABILITY_OPTION(pDWNF72, NULL, &doDOWNFLAG[72]);
    REGISTER_ABILITY_OPTION(pDWNF73, NULL, &doDOWNFLAG[73]);
    REGISTER_ABILITY_OPTION(pDWNF74, NULL, &doDOWNFLAG[74]);
    REGISTER_ABILITY_OPTION(pDWNF75, NULL, &doDOWNFLAG[75]);
    REGISTER_ABILITY_OPTION(pDWNF76, NULL, &doDOWNFLAG[76]);
    REGISTER_ABILITY_OPTION(pDWNF77, NULL, &doDOWNFLAG[77]);
    REGISTER_ABILITY_OPTION(pDWNF78, NULL, &doDOWNFLAG[78]);
    REGISTER_ABILITY_OPTION(pDWNF79, NULL, &doDOWNFLAG[79]);
    REGISTER_ABILITY_OPTION(pDWNF80, NULL, &doDOWNFLAG[80]);
    REGISTER_ABILITY_OPTION(pDWNF81, NULL, &doDOWNFLAG[81]);
    REGISTER_ABILITY_OPTION(pDWNF82, NULL, &doDOWNFLAG[82]);
    REGISTER_ABILITY_OPTION(pDWNF83, NULL, &doDOWNFLAG[83]);
    REGISTER_ABILITY_OPTION(pDWNF84, NULL, &doDOWNFLAG[84]);
    REGISTER_ABILITY_OPTION(pDWNF85, NULL, &doDOWNFLAG[85]);
    REGISTER_ABILITY_OPTION(pDWNF86, NULL, &doDOWNFLAG[86]);
    REGISTER_ABILITY_OPTION(pDWNF87, NULL, &doDOWNFLAG[87]);
    REGISTER_ABILITY_OPTION(pDWNF88, NULL, &doDOWNFLAG[88]);
    REGISTER_ABILITY_OPTION(pDWNF89, NULL, &doDOWNFLAG[89]);
    REGISTER_ABILITY_OPTION(pDWNF90, NULL, &doDOWNFLAG[90]);
    REGISTER_ABILITY_OPTION(pEAGLE, NULL, &doEAGLESTRIKE);
    REGISTER_ABILITY_OPTION(pERAGE, NULL, &doENRAGE);
    REGISTER_ABILITY_OPTION(pERKCK, NULL, &doENRAGINGKICK);
    REGISTER_ABILITY_OPTION(pESCAP, NULL, &doESCAPE);
    REGISTER_ABILITY_OPTION(pEVADE, NULL, &doEVADE);
    REGISTER_ABILITY_OPTION(pFACES, NULL, &doFACING);
    REGISTER_ABILITY_OPTION(pFALLS, NULL, &doFALLS);
    REGISTER_ABILITY_OPTION(pFCLAW, NULL, &doFCLAW);
    REGISTER_ABILITY_OPTION(pFEIGN, NULL, &doFEIGNDEATH);
    REGISTER_ABILITY_OPTION(pFERAL, NULL, &doFERALSWIPE);
    REGISTER_ABILITY_OPTION(pFIELD, NULL, &doFIELDARM);
    REGISTER_ABILITY_OPTION(pFISTS, NULL, &doFISTSOFWU);
    //REGISTER_ABILITY_OPTION(pFKICK, NULL, &doFEROCIOUSKICK);
    REGISTER_ABILITY_OPTION(pFLYKC, NULL, &doFLYINGKICK);
    REGISTER_ABILITY_OPTION(pFORAG, NULL, &doFORAGE);
    REGISTER_ABILITY_OPTION(pFRENZ, NULL, &doFRENZY);
    REGISTER_ABILITY_OPTION(pGBLDE, NULL, &doGBLADE);
    REGISTER_ABILITY_OPTION(pGORSM, NULL, &doGORILLASMASH);
    REGISTER_ABILITY_OPTION(pGTPUN, NULL, &doGUTPUNCH);
    REGISTER_ABILITY_OPTION(pHARMT, NULL, &doHARMTOUCH);
    REGISTER_ABILITY_OPTION(pHFAST, NULL, &doPOTHEALFAST);
    REGISTER_ABILITY_OPTION(pHIDES, NULL, &doHIDE);
    REGISTER_ABILITY_OPTION(pHOLF0, NULL, &doHOLYFLAG[0]);
    REGISTER_ABILITY_OPTION(pHOLF1, NULL, &doHOLYFLAG[1]);
    REGISTER_ABILITY_OPTION(pHOLF2, NULL, &doHOLYFLAG[2]);
    REGISTER_ABILITY_OPTION(pHOLF3, NULL, &doHOLYFLAG[3]);
    REGISTER_ABILITY_OPTION(pHOLF4, NULL, &doHOLYFLAG[4]);
    REGISTER_ABILITY_OPTION(pHOLF5, NULL, &doHOLYFLAG[5]);
    REGISTER_ABILITY_OPTION(pHOLF6, NULL, &doHOLYFLAG[6]);
    REGISTER_ABILITY_OPTION(pHOLF7, NULL, &doHOLYFLAG[7]);
    REGISTER_ABILITY_OPTION(pHOLF8, NULL, &doHOLYFLAG[8]);
    REGISTER_ABILITY_OPTION(pHOLF9, NULL, &doHOLYFLAG[9]);
    REGISTER_ABILITY_OPTION(pHOLF10, NULL, &doHOLYFLAG[10]);
    REGISTER_ABILITY_OPTION(pHOLF11, NULL, &doHOLYFLAG[11]);
    REGISTER_ABILITY_OPTION(pHOLF12, NULL, &doHOLYFLAG[12]);
    REGISTER_ABILITY_OPTION(pHOLF13, NULL, &doHOLYFLAG[13]);
    REGISTER_ABILITY_OPTION(pHOLF14, NULL, &doHOLYFLAG[14]);
    REGISTER_ABILITY_OPTION(pHOLF15, NULL, &doHOLYFLAG[15]);
    REGISTER_ABILITY_OPTION(pHOLF16, NULL, &doHOLYFLAG[16]);
    REGISTER_ABILITY_OPTION(pHOLF17, NULL, &doHOLYFLAG[17]);
    REGISTER_ABILITY_OPTION(pHOLF18, NULL, &doHOLYFLAG[18]);
    REGISTER_ABILITY_OPTION(pHOLF19, NULL, &doHOLYFLAG[19]);
    REGISTER_ABILITY_OPTION(pHOLF20, NULL, &doHOLYFLAG[20]);
    REGISTER_ABILITY_OPTION(pHOLF21, NULL, &doHOLYFLAG[21]);
    REGISTER_ABILITY_OPTION(pHOLF22, NULL, &doHOLYFLAG[22]);
    REGISTER_ABILITY_OPTION(pHOLF23, NULL, &doHOLYFLAG[23]);
    REGISTER_ABILITY_OPTION(pHOLF24, NULL, &doHOLYFLAG[24]);
    REGISTER_ABILITY_OPTION(pHOLF25, NULL, &doHOLYFLAG[25]);
    REGISTER_ABILITY_OPTION(pHOLF26, NULL, &doHOLYFLAG[26]);
    REGISTER_ABILITY_OPTION(pHOLF27, NULL, &doHOLYFLAG[27]);
    REGISTER_ABILITY_OPTION(pHOLF28, NULL, &doHOLYFLAG[28]);
    REGISTER_ABILITY_OPTION(pHOLF29, NULL, &doHOLYFLAG[29]);
    REGISTER_ABILITY_OPTION(pHOLF30, NULL, &doHOLYFLAG[30]);
    REGISTER_ABILITY_OPTION(pHOLF31, NULL, &doHOLYFLAG[31]);
    REGISTER_ABILITY_OPTION(pHOLF32, NULL, &doHOLYFLAG[32]);
    REGISTER_ABILITY_OPTION(pHOLF33, NULL, &doHOLYFLAG[33]);
    REGISTER_ABILITY_OPTION(pHOLF34, NULL, &doHOLYFLAG[34]);
    REGISTER_ABILITY_OPTION(pHOLF35, NULL, &doHOLYFLAG[35]);
    REGISTER_ABILITY_OPTION(pHOLF36, NULL, &doHOLYFLAG[36]);
    REGISTER_ABILITY_OPTION(pHOLF37, NULL, &doHOLYFLAG[37]);
    REGISTER_ABILITY_OPTION(pHOLF38, NULL, &doHOLYFLAG[38]);
    REGISTER_ABILITY_OPTION(pHOLF39, NULL, &doHOLYFLAG[39]);
    REGISTER_ABILITY_OPTION(pHOLF40, NULL, &doHOLYFLAG[40]);
    REGISTER_ABILITY_OPTION(pHOLF41, NULL, &doHOLYFLAG[41]);
    REGISTER_ABILITY_OPTION(pHOLF42, NULL, &doHOLYFLAG[42]);
    REGISTER_ABILITY_OPTION(pHOLF43, NULL, &doHOLYFLAG[43]);
    REGISTER_ABILITY_OPTION(pHOLF44, NULL, &doHOLYFLAG[44]);
    REGISTER_ABILITY_OPTION(pHOLF45, NULL, &doHOLYFLAG[45]);
    REGISTER_ABILITY_OPTION(pHOLF46, NULL, &doHOLYFLAG[46]);
    REGISTER_ABILITY_OPTION(pHOLF47, NULL, &doHOLYFLAG[47]);
    REGISTER_ABILITY_OPTION(pHOLF48, NULL, &doHOLYFLAG[48]);
    REGISTER_ABILITY_OPTION(pHOLF49, NULL, &doHOLYFLAG[49]);
    REGISTER_ABILITY_OPTION(pHOLF50, NULL, &doHOLYFLAG[50]);
    REGISTER_ABILITY_OPTION(pHOLF51, NULL, &doHOLYFLAG[51]);
    REGISTER_ABILITY_OPTION(pHOLF52, NULL, &doHOLYFLAG[52]);
    REGISTER_ABILITY_OPTION(pHOLF53, NULL, &doHOLYFLAG[53]);
    REGISTER_ABILITY_OPTION(pHOLF54, NULL, &doHOLYFLAG[54]);
    REGISTER_ABILITY_OPTION(pHOLF55, NULL, &doHOLYFLAG[55]);
    REGISTER_ABILITY_OPTION(pHOLF56, NULL, &doHOLYFLAG[56]);
    REGISTER_ABILITY_OPTION(pHOLF57, NULL, &doHOLYFLAG[57]);
    REGISTER_ABILITY_OPTION(pHOLF58, NULL, &doHOLYFLAG[58]);
    REGISTER_ABILITY_OPTION(pHOLF59, NULL, &doHOLYFLAG[59]);
    REGISTER_ABILITY_OPTION(pHOLF60, NULL, &doHOLYFLAG[60]);
    REGISTER_ABILITY_OPTION(pHOLF60, NULL, &doHOLYFLAG[60]);
    REGISTER_ABILITY_OPTION(pHOLF61, NULL, &doHOLYFLAG[61]);
    REGISTER_ABILITY_OPTION(pHOLF62, NULL, &doHOLYFLAG[62]);
    REGISTER_ABILITY_OPTION(pHOLF63, NULL, &doHOLYFLAG[63]);
    REGISTER_ABILITY_OPTION(pHOLF64, NULL, &doHOLYFLAG[64]);
    REGISTER_ABILITY_OPTION(pHOLF65, NULL, &doHOLYFLAG[65]);
    REGISTER_ABILITY_OPTION(pHOLF66, NULL, &doHOLYFLAG[66]);
    REGISTER_ABILITY_OPTION(pHOLF67, NULL, &doHOLYFLAG[67]);
    REGISTER_ABILITY_OPTION(pHOLF68, NULL, &doHOLYFLAG[68]);
    REGISTER_ABILITY_OPTION(pHOLF69, NULL, &doHOLYFLAG[69]);
    REGISTER_ABILITY_OPTION(pHOLF70, NULL, &doHOLYFLAG[70]);
    REGISTER_ABILITY_OPTION(pHOLF71, NULL, &doHOLYFLAG[71]);
    REGISTER_ABILITY_OPTION(pHOLF72, NULL, &doHOLYFLAG[72]);
    REGISTER_ABILITY_OPTION(pHOLF73, NULL, &doHOLYFLAG[73]);
    REGISTER_ABILITY_OPTION(pHOLF74, NULL, &doHOLYFLAG[74]);
    REGISTER_ABILITY_OPTION(pHOLF75, NULL, &doHOLYFLAG[75]);
    REGISTER_ABILITY_OPTION(pHOLF76, NULL, &doHOLYFLAG[76]);
    REGISTER_ABILITY_OPTION(pHOLF77, NULL, &doHOLYFLAG[77]);
    REGISTER_ABILITY_OPTION(pHOLF78, NULL, &doHOLYFLAG[78]);
    REGISTER_ABILITY_OPTION(pHOLF79, NULL, &doHOLYFLAG[79]);
    REGISTER_ABILITY_OPTION(pHOLF80, NULL, &doHOLYFLAG[80]);
    REGISTER_ABILITY_OPTION(pHOLF81, NULL, &doHOLYFLAG[81]);
    REGISTER_ABILITY_OPTION(pHOLF82, NULL, &doHOLYFLAG[82]);
    REGISTER_ABILITY_OPTION(pHOLF83, NULL, &doHOLYFLAG[83]);
    REGISTER_ABILITY_OPTION(pHOLF84, NULL, &doHOLYFLAG[84]);
    REGISTER_ABILITY_OPTION(pHOLF85, NULL, &doHOLYFLAG[85]);
    REGISTER_ABILITY_OPTION(pHOLF86, NULL, &doHOLYFLAG[86]);
    REGISTER_ABILITY_OPTION(pHOLF87, NULL, &doHOLYFLAG[87]);
    REGISTER_ABILITY_OPTION(pHOLF88, NULL, &doHOLYFLAG[88]);
    REGISTER_ABILITY_OPTION(pHOLF89, NULL, &doHOLYFLAG[89]);
    REGISTER_ABILITY_OPTION(pHOLF90, NULL, &doHOLYFLAG[90]);
    REGISTER_ABILITY_OPTION(pHOVER, NULL, &doPOTHEALOVER);
    REGISTER_ABILITY_OPTION(pINFUR, NULL, &doINFURIATE);
    REGISTER_ABILITY_OPTION(pINTIM, NULL, &doINTIMIDATION);
    REGISTER_ABILITY_OPTION(pJKICK, NULL, &doJLTKICKS);
    REGISTER_ABILITY_OPTION(pJOLTS, NULL, &doJOLT);
    REGISTER_ABILITY_OPTION(pJUGUL, NULL, &doJUGULAR);
    REGISTER_ABILITY_OPTION(pKICKS, NULL, &doKICK);
    REGISTER_ABILITY_OPTION(pKNEES, NULL, &doKNEESTRIKE);
    REGISTER_ABILITY_OPTION(pKNFPL, NULL, &doKNIFEPLAY);
    REGISTER_ABILITY_OPTION(pLCLAW, NULL, &doLEOPARDCLAW);
    REGISTER_ABILITY_OPTION(pLHAND, NULL, &doLAYHAND);
    REGISTER_ABILITY_OPTION(pMELEE, NULL, &doMELEE);
    REGISTER_ABILITY_OPTION(pMELEP, NULL, &elMELEEPRI);
    REGISTER_ABILITY_OPTION(pMELES, NULL, &elMELEESEC);
    REGISTER_ABILITY_OPTION(pMENDS, NULL, &doMEND);
    REGISTER_ABILITY_OPTION(pOFREN, NULL, &doOPFRENZY);
    REGISTER_ABILITY_OPTION(pOSTRK, NULL, &doOPPORTUNISTICSTRIKE);
    REGISTER_ABILITY_OPTION(pPETAS, NULL, &doPETASSIST);
    REGISTER_ABILITY_OPTION(pPETDE, NULL, &doPETDELAY);
    REGISTER_ABILITY_OPTION(pPETENG, NULL, &doPETENGAGEHPS);
    REGISTER_ABILITY_OPTION(pPETMN, NULL, &doPETMEND);
    REGISTER_ABILITY_OPTION(pPETRN, NULL, &doPETRANGE);
    REGISTER_ABILITY_OPTION(pPICKP, NULL, &doPICKPOCKET);
    REGISTER_ABILITY_OPTION(pPINPT, NULL, &doPINPOINT);
    REGISTER_ABILITY_OPTION(pPLUGS, NULL, &doSKILL);
    REGISTER_ABILITY_OPTION(pPOKER, NULL, &elPOKER);
    REGISTER_ABILITY_OPTION(pPRVK0, NULL, &idPROVOKE[0]);
    REGISTER_ABILITY_OPTION(pPRVK1, NULL, &idPROVOKE[1]);
    REGISTER_ABILITY_OPTION(pPRVKE, NULL, &doPROVOKEEND);
    REGISTER_ABILITY_OPTION(pPRVKM, AggroReset, &doPROVOKEMAX);
    REGISTER_ABILITY_OPTION(pPRVKO, NULL, &doPROVOKEONCE);
    REGISTER_ABILITY_OPTION(pRAKES, NULL, &doRAKE);
    REGISTER_ABILITY_OPTION(pRALLO, NULL, &doRALLOS);
    REGISTER_ABILITY_OPTION(pRANGE, RangeReset, &doRANGE);
    REGISTER_ABILITY_OPTION(pRAVEN, NULL, &doRAVENS);
    REGISTER_ABILITY_OPTION(pRAVOL, NULL, &doRAGEVOLLEY);
    REGISTER_ABILITY_OPTION(pRESUM, NULL, &doRESUME);
    REGISTER_ABILITY_OPTION(pRGHTI, NULL, &doRIGHTIND);
    REGISTER_ABILITY_OPTION(pRKICK, NULL, &doROUNDKICK);
    REGISTER_ABILITY_OPTION(pSBLADES, NULL, &doSTORMBLADES);
    REGISTER_ABILITY_OPTION(pSELOK, NULL, &doSELOS);
    REGISTER_ABILITY_OPTION(pSENSE, NULL, &doSENSETRAP);
    REGISTER_ABILITY_OPTION(pSHIEL, NULL, &elSHIELD);
    REGISTER_ABILITY_OPTION(pSLAMS, NULL, &doSLAM);
    REGISTER_ABILITY_OPTION(pSLAPF, NULL, &doSLAPFACE);
    REGISTER_ABILITY_OPTION(pSNEAK, NULL, &doSNEAK);
    REGISTER_ABILITY_OPTION(pSTAND, NULL, &doSTAND);
    REGISTER_ABILITY_OPTION(pSTEEL, NULL, &doSTEELY);
    REGISTER_ABILITY_OPTION(pSTIKD, StickReset, &doSTICKDELAY);
    REGISTER_ABILITY_OPTION(pSTIKKB, StickReset, &doSTICKBREAK);
    REGISTER_ABILITY_OPTION(pSTIKM, NULL, &doSTICKMODE);
    REGISTER_ABILITY_OPTION(pSTIKNR, StickReset, &doSTICKNORANGE);
    REGISTER_ABILITY_OPTION(pSTIKR, StickReset, &doSTICKRANGE);
    REGISTER_ABILITY_OPTION(pSTRIK, NULL, &doSTRIKE);
    REGISTER_ABILITY_OPTION(pSTRIKM, NULL, &doSTRIKEMODE);
    REGISTER_ABILITY_OPTION(pSTUN0, NULL, &idSTUN[0]);
    REGISTER_ABILITY_OPTION(pSTUN1, NULL, &idSTUN[1]);
    REGISTER_ABILITY_OPTION(pSTUNS, NULL, &doSTUNNING);
    REGISTER_ABILITY_OPTION(pSYNGY, NULL, &doSYNERGY);
    REGISTER_ABILITY_OPTION(pTAUNT, NULL, &doTAUNT);
    REGISTER_ABILITY_OPTION(pTHIEF, NULL, &doTHIEFEYE);
    REGISTER_ABILITY_OPTION(pTHROW, NULL, &doTHROWSTONE);
    REGISTER_ABILITY_OPTION(pTIGER, NULL, &doTIGERCLAW);
    REGISTER_ABILITY_OPTION(pTTJAB, NULL, &doTHROATJAB);
    REGISTER_ABILITY_OPTION(pTWIST, NULL, &doTWISTEDSHANK);
    REGISTER_ABILITY_OPTION(pVIGAX, NULL, &doVIGAXE);
    REGISTER_ABILITY_OPTION(pVIGDR, NULL, &doVIGDAGGER);
    REGISTER_ABILITY_OPTION(pVIGSN, NULL, &doVIGSHURIKEN);
    REGISTER_ABILITY_OPTION(pWITHS, NULL, &doWITHSTAND);
    REGISTER_ABILITY_OPTION(pYAULP, NULL, &doYAULP);

    IniListe.clear();
    MapInsert(&IniListe, Option("assaultif", "", "", "", NULL, &ifASSAULT));
    MapInsert(&IniListe, Option("backstabif", "", "", "", NULL, &ifBACKSTAB));
    MapInsert(&IniListe, Option("bashif", "", "", "", NULL, &ifBASH));
    MapInsert(&IniListe, Option("beggingif", "", "", "", NULL, &ifBEGGING));
    MapInsert(&IniListe, Option("aspif", "", "", "", NULL, &ifASP));
    MapInsert(&IniListe, Option("bviviif", "", "", "", NULL, &ifBVIVI));
    MapInsert(&IniListe, Option("cloudif", "", "", "", NULL, &ifCLOUD));
    MapInsert(&IniListe, Option("boastfulif", "", "", "", NULL, &ifBOASTFUL));
    MapInsert(&IniListe, Option("bleedif", "", "", "", NULL, &ifBLEED));
    MapInsert(&IniListe, Option("bloodlustif", "", "", "", NULL, &ifBLOODLUST));
    MapInsert(&IniListe, Option("callchallengeif", "", "", "", NULL, &ifCALLCHALLENGE));
    MapInsert(&IniListe, Option("cstrikeif", "", "", "", NULL, &ifCSTRIKE));
    MapInsert(&IniListe, Option("crippleif", "", "", "", NULL, &ifCRIPPLE));
    MapInsert(&IniListe, Option("gutpunchif", "", "", "", NULL, &ifGUTPUNCH));
    MapInsert(&IniListe, Option("opstrikeif", "", "", "", NULL, &ifOPPORTUNISTICSTRIKE));
    MapInsert(&IniListe, Option("throatjabif", "", "", "", NULL, &ifTHROATJAB));
    MapInsert(&IniListe, Option("battleleapif", "", "", "", NULL, &ifBATTLELEAP));
    MapInsert(&IniListe, Option("challengeforif", "", "", "", NULL, &ifCHALLENGEFOR));
    MapInsert(&IniListe, Option("commandingif", "", "", "", NULL, &ifCOMMANDING));
    MapInsert(&IniListe, Option("cryhavocif", "", "", "", NULL, &ifCRYHAVOC));
    MapInsert(&IniListe, Option("defenseif", "", "", "", NULL, &ifDEFENSE));
    MapInsert(&IniListe, Option("disarmif", "", "", "", NULL, &ifDISARM));
    MapInsert(&IniListe, Option("dragonpunchif", "", "", "", NULL, &ifDRAGONPUNCH));
    MapInsert(&IniListe, Option("eaglestrikeif", "", "", "", NULL, &ifEAGLESTRIKE));
    MapInsert(&IniListe, Option("enragingkickif", "", "", "", NULL, &ifENRAGINGKICK));
    MapInsert(&IniListe, Option("eyegougeif", "", "", "", NULL, &ifEYEGOUGE));
    MapInsert(&IniListe, Option("evadeif", "", "", "", NULL, &ifEVADE));
    MapInsert(&IniListe, Option("fallsif", "", "", "", NULL, &ifFALLS));
    MapInsert(&IniListe, Option("feralswipeif", "", "", "", NULL, &ifFERALSWIPE));
    //MapInsert(&IniListe, Option("ferociouskickif", "", "", "", NULL, &ifFEROCIOUSKICK));
    MapInsert(&IniListe, Option("fieldarmif", "", "", "", NULL, &ifFIELDARM));
    MapInsert(&IniListe, Option("fistofwuif", "", "", "", NULL, &ifFISTSOFWU));
    MapInsert(&IniListe, Option("fclaw", "", "", "", NULL, &ifFCLAW));
    MapInsert(&IniListe, Option("flyingkickif", "", "", "", NULL, &ifFLYINGKICK));
    MapInsert(&IniListe, Option("forageif", "", "", "", NULL, &ifFORAGE));
    MapInsert(&IniListe, Option("frenzyif", "", "", "", NULL, &ifFRENZY));
    MapInsert(&IniListe, Option("gbladeif", "", "", "", NULL, &ifGBLADE));
    MapInsert(&IniListe, Option("gorillasmashif", "", "", "", NULL, &ifGORILLASMASH));
    MapInsert(&IniListe, Option("harmtouchif", "", "", "", NULL, &ifHARMTOUCH));
    MapInsert(&IniListe, Option("pothealfastif", "", "", "", NULL, &ifPOTHEALFAST));
    MapInsert(&IniListe, Option("pothealoverif", "", "", "", NULL, &ifPOTHEALOVER));
    MapInsert(&IniListe, Option("hideif", "", "", "", NULL, &ifHIDE));
    MapInsert(&IniListe, Option("intimidationif", "", "", "", NULL, &ifINTIMIDATION));
    MapInsert(&IniListe, Option("joltif", "", "", "", NULL, &ifJOLT));
    MapInsert(&IniListe, Option("jltkicksif", "", "", "", NULL, &ifJLTKICKS));
    MapInsert(&IniListe, Option("jugularif", "", "", "", NULL, &ifJUGULAR));
    MapInsert(&IniListe, Option("kickif", "", "", "", NULL, &ifKICK));
    MapInsert(&IniListe, Option("kneestrikeif", "", "", "", NULL, &ifKNEESTRIKE));
    MapInsert(&IniListe, Option("knifeplayif", "", "", "", NULL, &ifKNIFEPLAY));
    MapInsert(&IniListe, Option("layhandif", "", "", "", NULL, &ifLAYHAND));
    MapInsert(&IniListe, Option("leopardclawif", "", "", "", NULL, &ifLEOPARDCLAW));
    MapInsert(&IniListe, Option("mendif", "", "", "", NULL, &ifMEND));
    MapInsert(&IniListe, Option("monkeyif", "", "", "", NULL, &ifMONKEY));
    MapInsert(&IniListe, Option("opfrenzyif", "", "", "", NULL, &ifOPFRENZY));
    MapInsert(&IniListe, Option("pickpocketif", "", "", "", NULL, &ifPICKPOCKET));
    MapInsert(&IniListe, Option("pinpointif", "", "", "", NULL, &ifPINPOINT));
    MapInsert(&IniListe, Option("provokeif", "", "", "", NULL, &ifPROVOKE));
    MapInsert(&IniListe, Option("ragevolleyif", "", "", "", NULL, &ifRAGEVOLLEY));
    MapInsert(&IniListe, Option("rakeif", "", "", "", NULL, &ifRAKE));
    MapInsert(&IniListe, Option("rallosif", "", "", "", NULL, &ifRALLOS));
    MapInsert(&IniListe, Option("ravensif", "", "", "", NULL, &ifRAVENS));
    MapInsert(&IniListe, Option("rightindif", "", "", "", NULL, &ifRIGHTIND));
    MapInsert(&IniListe, Option("roundkickif", "", "", "", NULL, &ifROUNDKICK));
    MapInsert(&IniListe, Option("sensetrapif", "", "", "", NULL, &ifSENSETRAP));
    MapInsert(&IniListe, Option("selosif", "", "", "", NULL, &ifSELOS));
    MapInsert(&IniListe, Option("slamif", "", "", "", NULL, &ifSLAM));
    MapInsert(&IniListe, Option("slapfaceif", "", "", "", NULL, &ifSLAPFACE));
    MapInsert(&IniListe, Option("sneakif", "", "", "", NULL, &ifSNEAK));
    MapInsert(&IniListe, Option("steelyif", "", "", "", NULL, &ifSTEELY));
    MapInsert(&IniListe, Option("stormbladesif", "", "", "", NULL, &ifSTORMBLADES));
    MapInsert(&IniListe, Option("strikeif", "", "", "", NULL, &ifSTRIKE));
    MapInsert(&IniListe, Option("stunningif", "", "", "", NULL, &ifSTUNNING));
    MapInsert(&IniListe, Option("synergyif", "", "", "", NULL, &ifSYNERGY));
    MapInsert(&IniListe, Option("tauntif", "", "", "", NULL, &ifTAUNT));
    MapInsert(&IniListe, Option("thiefeyeif", "", "", "", NULL, &ifTHIEFEYE));
    MapInsert(&IniListe, Option("throwstoneif", "", "", "", NULL, &ifTHROWSTONE));
    MapInsert(&IniListe, Option("tigerclawif", "", "", "", NULL, &ifTIGERCLAW));
    MapInsert(&IniListe, Option("twistedshankif", "", "", "", NULL, &ifTWISTEDSHANK));
    MapInsert(&IniListe, Option("vigaxeif", "", "", "", NULL, &ifVIGAXE));
    MapInsert(&IniListe, Option("vigdaggerif", "", "", "", NULL, &ifVIGDAGGER));
    MapInsert(&IniListe, Option("vigshurikenif", "", "", "", NULL, &ifVIGSHURIKEN));
    MapInsert(&IniListe, Option("withstandif", "", "", "", NULL, &ifWITHSTAND));
    MapInsert(&IniListe, Option("yaulp", "", "", "", NULL, &ifYAULP));
    MapInsert(&IniListe, Option("stickcmd", "", "", "", NULL, &StickCMD));
    MapInsert(&IniListe, Option("strikecmd", "", "", "", NULL, &StrikeCMD));
    MapInsert(&IniListe, Option("downshit0", "", "", "", NULL, &DOWNSHIT[0]));
    MapInsert(&IniListe, Option("downshit1", "", "", "", NULL, &DOWNSHIT[1]));
    MapInsert(&IniListe, Option("downshit2", "", "", "", NULL, &DOWNSHIT[2]));
    MapInsert(&IniListe, Option("downshit3", "", "", "", NULL, &DOWNSHIT[3]));
    MapInsert(&IniListe, Option("downshit4", "", "", "", NULL, &DOWNSHIT[4]));
    MapInsert(&IniListe, Option("downshit5", "", "", "", NULL, &DOWNSHIT[5]));
    MapInsert(&IniListe, Option("downshit6", "", "", "", NULL, &DOWNSHIT[6]));
    MapInsert(&IniListe, Option("downshit7", "", "", "", NULL, &DOWNSHIT[7]));
    MapInsert(&IniListe, Option("downshit8", "", "", "", NULL, &DOWNSHIT[8]));
    MapInsert(&IniListe, Option("downshit9", "", "", "", NULL, &DOWNSHIT[9]));
    MapInsert(&IniListe, Option("downshit10", "", "", "", NULL, &DOWNSHIT[10]));
    MapInsert(&IniListe, Option("downshit11", "", "", "", NULL, &DOWNSHIT[11]));
    MapInsert(&IniListe, Option("downshit12", "", "", "", NULL, &DOWNSHIT[12]));
    MapInsert(&IniListe, Option("downshit13", "", "", "", NULL, &DOWNSHIT[13]));
    MapInsert(&IniListe, Option("downshit14", "", "", "", NULL, &DOWNSHIT[14]));
    MapInsert(&IniListe, Option("downshit15", "", "", "", NULL, &DOWNSHIT[15]));
    MapInsert(&IniListe, Option("downshit16", "", "", "", NULL, &DOWNSHIT[16]));
    MapInsert(&IniListe, Option("downshit17", "", "", "", NULL, &DOWNSHIT[17]));
    MapInsert(&IniListe, Option("downshit18", "", "", "", NULL, &DOWNSHIT[18]));
    MapInsert(&IniListe, Option("downshit19", "", "", "", NULL, &DOWNSHIT[19]));
    MapInsert(&IniListe, Option("downshit20", "", "", "", NULL, &DOWNSHIT[20]));
    MapInsert(&IniListe, Option("downshit21", "", "", "", NULL, &DOWNSHIT[21]));
    MapInsert(&IniListe, Option("downshit22", "", "", "", NULL, &DOWNSHIT[22]));
    MapInsert(&IniListe, Option("downshit23", "", "", "", NULL, &DOWNSHIT[23]));
    MapInsert(&IniListe, Option("downshit24", "", "", "", NULL, &DOWNSHIT[24]));
    MapInsert(&IniListe, Option("downshit25", "", "", "", NULL, &DOWNSHIT[25]));
    MapInsert(&IniListe, Option("downshit26", "", "", "", NULL, &DOWNSHIT[26]));
    MapInsert(&IniListe, Option("downshit27", "", "", "", NULL, &DOWNSHIT[27]));
    MapInsert(&IniListe, Option("downshit28", "", "", "", NULL, &DOWNSHIT[28]));
    MapInsert(&IniListe, Option("downshit29", "", "", "", NULL, &DOWNSHIT[29]));
    MapInsert(&IniListe, Option("downshit30", "", "", "", NULL, &DOWNSHIT[30]));
    MapInsert(&IniListe, Option("downshit31", "", "", "", NULL, &DOWNSHIT[31]));
    MapInsert(&IniListe, Option("downshit32", "", "", "", NULL, &DOWNSHIT[32]));
    MapInsert(&IniListe, Option("downshit33", "", "", "", NULL, &DOWNSHIT[33]));
    MapInsert(&IniListe, Option("downshit34", "", "", "", NULL, &DOWNSHIT[34]));
    MapInsert(&IniListe, Option("downshit35", "", "", "", NULL, &DOWNSHIT[35]));
    MapInsert(&IniListe, Option("downshit36", "", "", "", NULL, &DOWNSHIT[36]));
    MapInsert(&IniListe, Option("downshit37", "", "", "", NULL, &DOWNSHIT[37]));
    MapInsert(&IniListe, Option("downshit38", "", "", "", NULL, &DOWNSHIT[38]));
    MapInsert(&IniListe, Option("downshit39", "", "", "", NULL, &DOWNSHIT[39]));
    MapInsert(&IniListe, Option("downshit40", "", "", "", NULL, &DOWNSHIT[40]));
    MapInsert(&IniListe, Option("downshit41", "", "", "", NULL, &DOWNSHIT[41]));
    MapInsert(&IniListe, Option("downshit42", "", "", "", NULL, &DOWNSHIT[42]));
    MapInsert(&IniListe, Option("downshit43", "", "", "", NULL, &DOWNSHIT[43]));
    MapInsert(&IniListe, Option("downshit44", "", "", "", NULL, &DOWNSHIT[44]));
    MapInsert(&IniListe, Option("downshit45", "", "", "", NULL, &DOWNSHIT[45]));
    MapInsert(&IniListe, Option("downshit46", "", "", "", NULL, &DOWNSHIT[46]));
    MapInsert(&IniListe, Option("downshit47", "", "", "", NULL, &DOWNSHIT[47]));
    MapInsert(&IniListe, Option("downshit48", "", "", "", NULL, &DOWNSHIT[48]));
    MapInsert(&IniListe, Option("downshit49", "", "", "", NULL, &DOWNSHIT[49]));
    MapInsert(&IniListe, Option("downshit50", "", "", "", NULL, &DOWNSHIT[50]));
    MapInsert(&IniListe, Option("downshit51", "", "", "", NULL, &DOWNSHIT[51]));
    MapInsert(&IniListe, Option("downshit52", "", "", "", NULL, &DOWNSHIT[52]));
    MapInsert(&IniListe, Option("downshit53", "", "", "", NULL, &DOWNSHIT[53]));
    MapInsert(&IniListe, Option("downshit54", "", "", "", NULL, &DOWNSHIT[54]));
    MapInsert(&IniListe, Option("downshit55", "", "", "", NULL, &DOWNSHIT[55]));
    MapInsert(&IniListe, Option("downshit56", "", "", "", NULL, &DOWNSHIT[56]));
    MapInsert(&IniListe, Option("downshit57", "", "", "", NULL, &DOWNSHIT[57]));
    MapInsert(&IniListe, Option("downshit58", "", "", "", NULL, &DOWNSHIT[58]));
    MapInsert(&IniListe, Option("downshit59", "", "", "", NULL, &DOWNSHIT[59]));
    MapInsert(&IniListe, Option("downshit60", "", "", "", NULL, &DOWNSHIT[60]));
    MapInsert(&IniListe, Option("downshit61", "", "", "", NULL, &DOWNSHIT[61]));
    MapInsert(&IniListe, Option("downshit62", "", "", "", NULL, &DOWNSHIT[62]));
    MapInsert(&IniListe, Option("downshit63", "", "", "", NULL, &DOWNSHIT[63]));
    MapInsert(&IniListe, Option("downshit64", "", "", "", NULL, &DOWNSHIT[64]));
    MapInsert(&IniListe, Option("downshit65", "", "", "", NULL, &DOWNSHIT[65]));
    MapInsert(&IniListe, Option("downshit66", "", "", "", NULL, &DOWNSHIT[66]));
    MapInsert(&IniListe, Option("downshit67", "", "", "", NULL, &DOWNSHIT[67]));
    MapInsert(&IniListe, Option("downshit68", "", "", "", NULL, &DOWNSHIT[68]));
    MapInsert(&IniListe, Option("downshit69", "", "", "", NULL, &DOWNSHIT[69]));
    MapInsert(&IniListe, Option("downshit70", "", "", "", NULL, &DOWNSHIT[70]));
    MapInsert(&IniListe, Option("downshit71", "", "", "", NULL, &DOWNSHIT[71]));
    MapInsert(&IniListe, Option("downshit72", "", "", "", NULL, &DOWNSHIT[72]));
    MapInsert(&IniListe, Option("downshit73", "", "", "", NULL, &DOWNSHIT[73]));
    MapInsert(&IniListe, Option("downshit74", "", "", "", NULL, &DOWNSHIT[74]));
    MapInsert(&IniListe, Option("downshit75", "", "", "", NULL, &DOWNSHIT[75]));
    MapInsert(&IniListe, Option("downshit76", "", "", "", NULL, &DOWNSHIT[76]));
    MapInsert(&IniListe, Option("downshit77", "", "", "", NULL, &DOWNSHIT[77]));
    MapInsert(&IniListe, Option("downshit78", "", "", "", NULL, &DOWNSHIT[78]));
    MapInsert(&IniListe, Option("downshit79", "", "", "", NULL, &DOWNSHIT[79]));
    MapInsert(&IniListe, Option("downshit80", "", "", "", NULL, &DOWNSHIT[80]));
    MapInsert(&IniListe, Option("downshit81", "", "", "", NULL, &DOWNSHIT[81]));
    MapInsert(&IniListe, Option("downshit82", "", "", "", NULL, &DOWNSHIT[82]));
    MapInsert(&IniListe, Option("downshit83", "", "", "", NULL, &DOWNSHIT[83]));
    MapInsert(&IniListe, Option("downshit84", "", "", "", NULL, &DOWNSHIT[84]));
    MapInsert(&IniListe, Option("downshit85", "", "", "", NULL, &DOWNSHIT[85]));
    MapInsert(&IniListe, Option("downshit86", "", "", "", NULL, &DOWNSHIT[86]));
    MapInsert(&IniListe, Option("downshit87", "", "", "", NULL, &DOWNSHIT[87]));
    MapInsert(&IniListe, Option("downshit88", "", "", "", NULL, &DOWNSHIT[88]));
    MapInsert(&IniListe, Option("downshit89", "", "", "", NULL, &DOWNSHIT[89]));
    MapInsert(&IniListe, Option("downshit90", "", "", "", NULL, &DOWNSHIT[90]));
    MapInsert(&IniListe, Option("downshitif", "", "", "", NULL, &DOWNSHITIF));
    MapInsert(&IniListe, Option("holyshit0", "", "", "", NULL, &HOLYSHIT[0]));
    MapInsert(&IniListe, Option("holyshit1", "", "", "", NULL, &HOLYSHIT[1]));
    MapInsert(&IniListe, Option("holyshit2", "", "", "", NULL, &HOLYSHIT[2]));
    MapInsert(&IniListe, Option("holyshit3", "", "", "", NULL, &HOLYSHIT[3]));
    MapInsert(&IniListe, Option("holyshit4", "", "", "", NULL, &HOLYSHIT[4]));
    MapInsert(&IniListe, Option("holyshit5", "", "", "", NULL, &HOLYSHIT[5]));
    MapInsert(&IniListe, Option("holyshit6", "", "", "", NULL, &HOLYSHIT[6]));
    MapInsert(&IniListe, Option("holyshit7", "", "", "", NULL, &HOLYSHIT[7]));
    MapInsert(&IniListe, Option("holyshit8", "", "", "", NULL, &HOLYSHIT[8]));
    MapInsert(&IniListe, Option("holyshit9", "", "", "", NULL, &HOLYSHIT[9]));
    MapInsert(&IniListe, Option("holyshit10", "", "", "", NULL, &HOLYSHIT[10]));
    MapInsert(&IniListe, Option("holyshit11", "", "", "", NULL, &HOLYSHIT[11]));
    MapInsert(&IniListe, Option("holyshit12", "", "", "", NULL, &HOLYSHIT[12]));
    MapInsert(&IniListe, Option("holyshit13", "", "", "", NULL, &HOLYSHIT[13]));
    MapInsert(&IniListe, Option("holyshit14", "", "", "", NULL, &HOLYSHIT[14]));
    MapInsert(&IniListe, Option("holyshit15", "", "", "", NULL, &HOLYSHIT[15]));
    MapInsert(&IniListe, Option("holyshit16", "", "", "", NULL, &HOLYSHIT[16]));
    MapInsert(&IniListe, Option("holyshit17", "", "", "", NULL, &HOLYSHIT[17]));
    MapInsert(&IniListe, Option("holyshit18", "", "", "", NULL, &HOLYSHIT[18]));
    MapInsert(&IniListe, Option("holyshit19", "", "", "", NULL, &HOLYSHIT[19]));
    MapInsert(&IniListe, Option("holyshit20", "", "", "", NULL, &HOLYSHIT[20]));
    MapInsert(&IniListe, Option("holyshit21", "", "", "", NULL, &HOLYSHIT[21]));
    MapInsert(&IniListe, Option("holyshit22", "", "", "", NULL, &HOLYSHIT[22]));
    MapInsert(&IniListe, Option("holyshit23", "", "", "", NULL, &HOLYSHIT[23]));
    MapInsert(&IniListe, Option("holyshit24", "", "", "", NULL, &HOLYSHIT[24]));
    MapInsert(&IniListe, Option("holyshit25", "", "", "", NULL, &HOLYSHIT[25]));
    MapInsert(&IniListe, Option("holyshit26", "", "", "", NULL, &HOLYSHIT[26]));
    MapInsert(&IniListe, Option("holyshit27", "", "", "", NULL, &HOLYSHIT[27]));
    MapInsert(&IniListe, Option("holyshit28", "", "", "", NULL, &HOLYSHIT[28]));
    MapInsert(&IniListe, Option("holyshit29", "", "", "", NULL, &HOLYSHIT[29]));
    MapInsert(&IniListe, Option("holyshit30", "", "", "", NULL, &HOLYSHIT[30]));
    MapInsert(&IniListe, Option("holyshit31", "", "", "", NULL, &HOLYSHIT[31]));
    MapInsert(&IniListe, Option("holyshit32", "", "", "", NULL, &HOLYSHIT[32]));
    MapInsert(&IniListe, Option("holyshit33", "", "", "", NULL, &HOLYSHIT[33]));
    MapInsert(&IniListe, Option("holyshit34", "", "", "", NULL, &HOLYSHIT[34]));
    MapInsert(&IniListe, Option("holyshit35", "", "", "", NULL, &HOLYSHIT[35]));
    MapInsert(&IniListe, Option("holyshit36", "", "", "", NULL, &HOLYSHIT[36]));
    MapInsert(&IniListe, Option("holyshit37", "", "", "", NULL, &HOLYSHIT[37]));
    MapInsert(&IniListe, Option("holyshit38", "", "", "", NULL, &HOLYSHIT[38]));
    MapInsert(&IniListe, Option("holyshit39", "", "", "", NULL, &HOLYSHIT[39]));
    MapInsert(&IniListe, Option("holyshit40", "", "", "", NULL, &HOLYSHIT[40]));
    MapInsert(&IniListe, Option("holyshit41", "", "", "", NULL, &HOLYSHIT[41]));
    MapInsert(&IniListe, Option("holyshit42", "", "", "", NULL, &HOLYSHIT[42]));
    MapInsert(&IniListe, Option("holyshit43", "", "", "", NULL, &HOLYSHIT[43]));
    MapInsert(&IniListe, Option("holyshit44", "", "", "", NULL, &HOLYSHIT[44]));
    MapInsert(&IniListe, Option("holyshit45", "", "", "", NULL, &HOLYSHIT[45]));
    MapInsert(&IniListe, Option("holyshit46", "", "", "", NULL, &HOLYSHIT[46]));
    MapInsert(&IniListe, Option("holyshit47", "", "", "", NULL, &HOLYSHIT[47]));
    MapInsert(&IniListe, Option("holyshit48", "", "", "", NULL, &HOLYSHIT[48]));
    MapInsert(&IniListe, Option("holyshit49", "", "", "", NULL, &HOLYSHIT[49]));
    MapInsert(&IniListe, Option("holyshit50", "", "", "", NULL, &HOLYSHIT[50]));
    MapInsert(&IniListe, Option("holyshit51", "", "", "", NULL, &HOLYSHIT[51]));
    MapInsert(&IniListe, Option("holyshit52", "", "", "", NULL, &HOLYSHIT[52]));
    MapInsert(&IniListe, Option("holyshit53", "", "", "", NULL, &HOLYSHIT[53]));
    MapInsert(&IniListe, Option("holyshit54", "", "", "", NULL, &HOLYSHIT[54]));
    MapInsert(&IniListe, Option("holyshit55", "", "", "", NULL, &HOLYSHIT[55]));
    MapInsert(&IniListe, Option("holyshit56", "", "", "", NULL, &HOLYSHIT[56]));
    MapInsert(&IniListe, Option("holyshit57", "", "", "", NULL, &HOLYSHIT[57]));
    MapInsert(&IniListe, Option("holyshit58", "", "", "", NULL, &HOLYSHIT[58]));
    MapInsert(&IniListe, Option("holyshit59", "", "", "", NULL, &HOLYSHIT[59]));
    MapInsert(&IniListe, Option("holyshit60", "", "", "", NULL, &HOLYSHIT[60]));
    MapInsert(&IniListe, Option("holyshit61", "", "", "", NULL, &HOLYSHIT[61]));
    MapInsert(&IniListe, Option("holyshit62", "", "", "", NULL, &HOLYSHIT[62]));
    MapInsert(&IniListe, Option("holyshit63", "", "", "", NULL, &HOLYSHIT[63]));
    MapInsert(&IniListe, Option("holyshit64", "", "", "", NULL, &HOLYSHIT[64]));
    MapInsert(&IniListe, Option("holyshit65", "", "", "", NULL, &HOLYSHIT[65]));
    MapInsert(&IniListe, Option("holyshit66", "", "", "", NULL, &HOLYSHIT[66]));
    MapInsert(&IniListe, Option("holyshit67", "", "", "", NULL, &HOLYSHIT[67]));
    MapInsert(&IniListe, Option("holyshit68", "", "", "", NULL, &HOLYSHIT[68]));
    MapInsert(&IniListe, Option("holyshit69", "", "", "", NULL, &HOLYSHIT[69]));
    MapInsert(&IniListe, Option("holyshit70", "", "", "", NULL, &HOLYSHIT[70]));
    MapInsert(&IniListe, Option("holyshit71", "", "", "", NULL, &HOLYSHIT[71]));
    MapInsert(&IniListe, Option("holyshit72", "", "", "", NULL, &HOLYSHIT[72]));
    MapInsert(&IniListe, Option("holyshit73", "", "", "", NULL, &HOLYSHIT[73]));
    MapInsert(&IniListe, Option("holyshit74", "", "", "", NULL, &HOLYSHIT[74]));
    MapInsert(&IniListe, Option("holyshit75", "", "", "", NULL, &HOLYSHIT[75]));
    MapInsert(&IniListe, Option("holyshit76", "", "", "", NULL, &HOLYSHIT[76]));
    MapInsert(&IniListe, Option("holyshit77", "", "", "", NULL, &HOLYSHIT[77]));
    MapInsert(&IniListe, Option("holyshit78", "", "", "", NULL, &HOLYSHIT[78]));
    MapInsert(&IniListe, Option("holyshit79", "", "", "", NULL, &HOLYSHIT[79]));
    MapInsert(&IniListe, Option("holyshit80", "", "", "", NULL, &HOLYSHIT[80]));
    MapInsert(&IniListe, Option("holyshit81", "", "", "", NULL, &HOLYSHIT[81]));
    MapInsert(&IniListe, Option("holyshit82", "", "", "", NULL, &HOLYSHIT[82]));
    MapInsert(&IniListe, Option("holyshit83", "", "", "", NULL, &HOLYSHIT[83]));
    MapInsert(&IniListe, Option("holyshit84", "", "", "", NULL, &HOLYSHIT[84]));
    MapInsert(&IniListe, Option("holyshit85", "", "", "", NULL, &HOLYSHIT[85]));
    MapInsert(&IniListe, Option("holyshit86", "", "", "", NULL, &HOLYSHIT[86]));
    MapInsert(&IniListe, Option("holyshit87", "", "", "", NULL, &HOLYSHIT[87]));
    MapInsert(&IniListe, Option("holyshit88", "", "", "", NULL, &HOLYSHIT[88]));
    MapInsert(&IniListe, Option("holyshit89", "", "", "", NULL, &HOLYSHIT[89]));
    MapInsert(&IniListe, Option("holyshit90", "", "", "", NULL, &HOLYSHIT[90]));
    MapInsert(&IniListe, Option("holyshitif", "", "", "", NULL, &HOLYSHITIF));

    VarListe.clear();
    MapInsert(&VarListe, Option("idassault", "", "", "", NULL, &idASSAULT));
    MapInsert(&VarListe, Option("idbackstab", "", "", "", NULL, &idBACKSTAB));
    MapInsert(&VarListe, Option("idbash", "", "", "", NULL, &idBASH));
    MapInsert(&VarListe, Option("idbattleleap", "", "", "", NULL, &idBATTLELEAP));
    MapInsert(&VarListe, Option("idbvivi", "", "", "", NULL, &idBVIVI));
    MapInsert(&VarListe, Option("idcloud", "", "", "", NULL, &idCLOUD));
    MapInsert(&VarListe, Option("idbleed", "", "", "", NULL, &idBLEED));
    MapInsert(&VarListe, Option("idbloodlust", "", "", "", NULL, &idBLOODLUST));
    MapInsert(&VarListe, Option("idboastful", "", "", "", NULL, &idBOASTFUL));
    MapInsert(&VarListe, Option("idbegging", "", "", "", NULL, &idBEGGING));
    MapInsert(&VarListe, Option("idASP", "", "", "", NULL, &idASP));
    MapInsert(&VarListe, Option("idcallchallenge", "", "", "", NULL, &idCALLCHALLENGE));
    MapInsert(&VarListe, Option("idcstrike", "", "", "", NULL, &idCSTRIKE));
    MapInsert(&VarListe, Option("idcommanding", "", "", "", NULL, &idCOMMANDING));
    MapInsert(&VarListe, Option("idCRIPPLE", "", "", "", NULL, &idCRIPPLE));
    MapInsert(&VarListe, Option("idcryhavoc", "", "", "", NULL, &idCRYHAVOC));
    MapInsert(&VarListe, Option("iddefense", "", "", "", NULL, &idDEFENSE));
    MapInsert(&VarListe, Option("iddisarm", "", "", "", NULL, &idDISARM));
    MapInsert(&VarListe, Option("iddragonpunch", "", "", "", NULL, &idDRAGONPUNCH));
    MapInsert(&VarListe, Option("ideaglestrike", "", "", "", NULL, &idEAGLESTRIKE));
    MapInsert(&VarListe, Option("idenragingkick", "", "", "", NULL, &idENRAGINGKICK));
    MapInsert(&VarListe, Option("ideyegouge", "", "", "", NULL, &idEYEGOUGE));
    MapInsert(&VarListe, Option("idescape", "", "", "", NULL, &idESCAPE));
    MapInsert(&VarListe, Option("idferalswipe", "", "", "", NULL, &idFERALSWIPE));
    //MapInsert(&VarListe, Option("idferociouskick", "", "", "", NULL, &idFEROCIOUSKICK));
    MapInsert(&VarListe, Option("idfieldarm", "", "", "", NULL, &idFIELDARM));
    MapInsert(&VarListe, Option("idfistsofwu", "", "", "", NULL, &idFISTSOFWU));
    MapInsert(&VarListe, Option("idfclaw", "", "", "", NULL, &idFCLAW));
    MapInsert(&VarListe, Option("idflyingkick", "", "", "", NULL, &idFLYINGKICK));
    MapInsert(&VarListe, Option("idforage", "", "", "", NULL, &idFORAGE));
    MapInsert(&VarListe, Option("idfrenzy", "", "", "", NULL, &idFRENZY));
    MapInsert(&VarListe, Option("idgblade", "", "", "", NULL, &idGBLADE));
    MapInsert(&VarListe, Option("idgorillasmash", "", "", "", NULL, &idGORILLASMASH));
    MapInsert(&VarListe, Option("idgutpunch", "", "", "", NULL, &idGUTPUNCH));
    MapInsert(&VarListe, Option("idharmtouch", "", "", "", NULL, &idHARMTOUCH));
    MapInsert(&VarListe, Option("idhide", "", "", "", NULL, &idHIDE));
    MapInsert(&VarListe, Option("idintimidation", "", "", "", NULL, &idINTIMIDATION));
    MapInsert(&VarListe, Option("idjltkicks", "", "", "", NULL, &idJLTKICKS));
    MapInsert(&VarListe, Option("idjolt", "", "", "", NULL, &idJOLT));
    MapInsert(&VarListe, Option("idjugular", "", "", "", NULL, &idJUGULAR));
    MapInsert(&VarListe, Option("idkick", "", "", "", NULL, &idKICK));
    MapInsert(&VarListe, Option("idkneestrike", "", "", "", NULL, &idKNEESTRIKE));
    MapInsert(&VarListe, Option("idknifeplay", "", "", "", NULL, &idKNIFEPLAY));
    MapInsert(&VarListe, Option("idlayhand", "", "", "", NULL, &idLAYHAND));
    MapInsert(&VarListe, Option("idleopardclaw", "", "", "", NULL, &idLEOPARDCLAW));
    MapInsert(&VarListe, Option("idmend", "", "", "", NULL, &idMEND));
    MapInsert(&VarListe, Option("idmonkey", "", "", "", NULL, &idMONKEY));
    MapInsert(&VarListe, Option("idopfrenzy", "", "", "", NULL, &idOPFRENZY));
    MapInsert(&VarListe, Option("idpickpocket", "", "", "", NULL, &idPICKPOCKET));
    MapInsert(&VarListe, Option("idpinpoint", "", "", "", NULL, &idPINPOINT));
    MapInsert(&VarListe, Option("idoppstrike", "", "", "", NULL, &idOPPORTUNISTICSTRIKE));
    MapInsert(&VarListe, Option("idragevolley", "", "", "", NULL, &idRAGEVOLLEY));
    MapInsert(&VarListe, Option("idrake", "", "", "", NULL, &idRAKE));
    MapInsert(&VarListe, Option("idrallos", "", "", "", NULL, &idRALLOS));
    MapInsert(&VarListe, Option("idravens", "", "", "", NULL, &idRAVENS));
    MapInsert(&VarListe, Option("idroundkick", "", "", "", NULL, &idROUNDKICK));
    MapInsert(&VarListe, Option("idrightidg", "", "", "", NULL, &idRIGHTIND));
    MapInsert(&VarListe, Option("idselos", "", "", "", NULL, &idSELOS));
    MapInsert(&VarListe, Option("idsensetrap", "", "", "", NULL, &idSENSETRAP));
    MapInsert(&VarListe, Option("idslam", "", "", "", NULL, &idSLAM));
    MapInsert(&VarListe, Option("idslapface", "", "", "", NULL, &idSLAPFACE));
    MapInsert(&VarListe, Option("idsneak", "", "", "", NULL, &idSNEAK));
    MapInsert(&VarListe, Option("idsteely", "", "", "", NULL, &idSTEELY));
    MapInsert(&VarListe, Option("idstrike", "", "", "", NULL, &idSTRIKE));
    MapInsert(&VarListe, Option("idsynergy", "", "", "", NULL, &idSYNERGY));
    MapInsert(&VarListe, Option("idtaunt", "", "", "", NULL, &idTAUNT));
    MapInsert(&VarListe, Option("idthroatjab", "", "", "", NULL, &idTHROATJAB));
    MapInsert(&VarListe, Option("idthiefeye", "", "", "", NULL, &idTHIEFEYE));
    MapInsert(&VarListe, Option("idthrowstone", "", "", "", NULL, &idTHROWSTONE));
    MapInsert(&VarListe, Option("idtigerclaw", "", "", "", NULL, &idTIGERCLAW));
    MapInsert(&VarListe, Option("idvigaxe", "", "", "", NULL, &idVIGAXE));
    MapInsert(&VarListe, Option("idvigdagger", "", "", "", NULL, &idVIGDAGGER));
    MapInsert(&VarListe, Option("idvigshuriken", "", "", "", NULL, &idVIGSHURIKEN));
    MapInsert(&VarListe, Option("idwithstand", "", "", "", NULL, &idWITHSTAND));
    MapInsert(&VarListe, Option("idyaulp", "", "", "", NULL, &idYAULP));
    MapInsert(&VarListe, Option("idfeign0", "", "", "", NULL, &idFEIGN[0]));
    MapInsert(&VarListe, Option("idfeign1", "", "", "", NULL, &idFEIGN[1]));
    MapInsert(&VarListe, Option("idpetassist", "", "", "", NULL, &idPETASSIST));
    MapInsert(&VarListe, Option("idpetdelay", "", "", "", NULL, &idPETDELAY));
    MapInsert(&VarListe, Option("idpetrange", "", "", "", NULL, &idPETRANGE));
    MapInsert(&VarListe, Option("idpetmend", "", "", "", NULL, &idPETMEND));
    MapInsert(&VarListe, Option("idpothealfast", "", "", "", NULL, &idPOTHEALFAST));
    MapInsert(&VarListe, Option("idpothealover", "", "", "", NULL, &idPOTHEALOVER));
    MapInsert(&VarListe, Option("idprovoke0", "", "", "", NULL, &idPROVOKE[0]));
    MapInsert(&VarListe, Option("idprovoke1", "", "", "", NULL, &idPROVOKE[1]));
    MapInsert(&VarListe, Option("idstun0", "", "", "", NULL, &idSTUN[0]));
    MapInsert(&VarListe, Option("idstun1", "", "", "", NULL, &idSTUN[1]));

    ZeroMemory(AttackArray, sizeof(AttackArray));
    AttackArray[0]   = true;
    AttackArray[1]   = true;
    AttackArray[2]   = true;
    AttackArray[3]   = true;
    AttackArray[4]   = true;
    AttackArray[5]   = true; // hit with main hand
    AttackArray[6]   = true;
    AttackArray[7]   = true;
    AttackArray[8]   = true; // punch
    AttackArray[9]   = true;
    AttackArray[10]  = true;
    AttackArray[11]  = true;
    AttackArray[12]  = true; // hit with offhand
    AttackArray[42]  = true; // casting type 1
    AttackArray[43]  = true; // casting type 2
    AttackArray[44]  = true; // casting type 3
    AttackArray[80]  = true; // attacking
    AttackArray[106] = true; // attacking
    AttackArray[129] = true; // attacking
    AttackArray[133] = true; // attacking
    AttackArray[143] = true; // attacking
    AttackArray[144] = true; // attacking

    ZeroMemory(IdlingArray, sizeof(IdlingArray));
    IdlingArray[26]  = true; // mezzed maybe?
    IdlingArray[32]  = true; // sitting
    IdlingArray[71]  = true; // standing after sitting
    IdlingArray[104] = true; // standing still
    IdlingArray[107] = true; // standing still
    IdlingArray[110] = true; // standing still

    SaveIndx    = 0;
    pMeleeEvent = new Blech('#');

    ZeroMemory(SaveList, sizeof(SaveList));
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("Auto fire off#*#"                                                     , AUTOFIREOFF  , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("Auto fire on#*#"                                                      , AUTOFIREON   , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Attacking #*# Master.#*#"                                          , PETATTK      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Sorry#*#calming down.#*#"                                          , PETBACK      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Waiting for your order to attack, Master.#*#"                      , PETHOLD      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Now Holding#*#I will not attack until ordered.#*#"                 , PETHOLD      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# begins casting a spell#*#"                                        , CASTING      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# begins to cast#*#"                                                , CASTING      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# has become ENRAGED#*#"                                            , ENRAGEON     , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# is no longer enraged#*#"                                          , ENRAGEOFF    , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# is infuriated#*#"                                                 , INFURIATEON  , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# no longer infuriated#*#"                                          , INFURIATEOFF , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# has fallen to the ground#*#"                                      , FALLEN       , (void *)1);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You are no longer feigning death#*#"                               , BROKEN       , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Your body aches from a wave of pain#*#"                            , BROKEN       , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#The strength of your will allows you to resume feigning death#*#"  , RESUME       , (void *)0);

    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You failed to hide yourself#*#"                                    , HIDEOFF      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You stop hiding#*#"                                                , HIDEOFF      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You have moved and are no longer hidden#*#"                        , HIDEOFF      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You have hidden yourself from view#*#"                             , HIDEON       , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You are as quiet as a herd of running elephants#*#"                , SNEAKOFF     , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You stop sneaking#*#"                                              , SNEAKOFF     , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You are as quiet as a cat stalking its prey#*#"                    , SNEAKON      , (void *)0);
    //Rogue Strike Fix htw 2/20/2011
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You can not use this ability because you have not been hidden for long enough.#*#", STRIKERESET, (void *)0);

    pMeleeTypes = new MQ2MeleeType;
    AddMQ2Data("Melee",    DataMelee);
    AddMQ2Data("meleemvb", datameleemvb);
    AddMQ2Data("meleemvi", datameleemvi);
    AddMQ2Data("meleemvs", datameleemvs);

    AddCommand("/enrageon",     EnrageON);
    AddCommand("/enrageoff",    EnrageOFF);
    AddCommand("/infuriateon",  InfuriateON);
    AddCommand("/infuriateoff", InfuriateOFF);
    AddCommand("/killthis",     KillThis);
    AddCommand("/melee",        Melee);
    AddCommand("/throwit",      ThrowIT);

    bMULoaded = HandleMoveUtils();
}

PLUGIN_API void SetGameState(unsigned long GameState)
{
    if (SETBINDINGS)
        Bindding(GameState == GAMESTATE_INGAME);

    if (GameState == GAMESTATE_INGAME)
    {
        if (!Loaded)
        {
            Configure();
            DebugSpew("MQ2Melee SetGameState GAMESTATE_INGAME & NOT LOADED, calling Configure()");
        }
        else
        {
            DebugSpew("MQ2Melee SetGameState GAMESTATE_INGAME & LOADED=true, not calling anything");
        }
    }
    else if (GameState != GAMESTATE_LOGGINGIN)
    {
        if (Loaded)
        {
            DebugSpew("MQ2Melee SetGameState not INGAME or LOGGINGIN & LOADED=true, setting LOADED false");
            Loaded = false;
        }
        else
        {
            DebugSpew("MQ2Melee SetGameState not INGAME and *is* LOGGINGIN, not changing loaded");
        }
    }
    DebugSpew("MQ2Melee SetGameState Loaded == %s", Loaded ? "true" : "false");
}

PLUGIN_API unsigned long OnIncomingChat(char* Line, unsigned long Color)
{
    CHAR szLine[MAX_STRING] = { 0 };
    strcpy_s(szLine, Line);
    if (doSKILL && gbInZone && pMeleeEvent)
    {
        switch (Color)
        {
        case 265:
        case 267:
            SwingHits++;
            break;
        case 266:
        case 268:
            TakenHits++;
            break;
        case 13:
        case 270:
        case 273:
        case 288:
        case 289:
        case 306:
        case 328:
        case 337:
            pMeleeEvent->Feed(szLine);
        }
    }
    return 0;
}

float CalcDist = 0.0f;  // Calculate Distance Moved Since Last Pulse
float SaveX = 0.0f;     // Current Position X
float SaveY = 0.0f;     // Current Position Y
float SaveZ = 0.0f;     // Current Position Z

PLUGIN_API void OnPulse()
{
    if (doSKILL && Loaded && gbInZone && SpawnMe())
    {
        if (PcProfile* Me = GetPcProfile())
        {
            if (GetPcProfile()->Shrouded != Shrouded)
            {
                SetGameState(GAMESTATE_UNLOADING);
                SetGameState(GAMESTATE_INGAME);
            }
            if (!HiddenTimer && IsInvisible()) HiddenTimer = (unsigned long)clock();
            if (!SilentTimer && IsSneaking())  SilentTimer = (unsigned long)clock();

            Travel = 5.0f; // assume we moving like a jet :P
            if (PSPAWNINFO MySpawn = SpawnMe())
            {
                CalcDist = fabs(GetDistance3D(SaveX, SaveY, SaveZ, MySpawn->X, MySpawn->Y, MySpawn->Z));
                SaveX    = MySpawn->X;
                SaveY    = MySpawn->Y;
                SaveZ    = MySpawn->Z;
                Travel   = SpeedRun(MySpawn);
            }
            if (Moving = (Travel > 0.05 || Travel < -0.05 || CalcDist > 12.0f)) TimerMove = (unsigned long)clock() + delay * 7;
            Immobile = (!(gbMoving) && (!TimerMove || (unsigned long)clock() > TimerMove));

            if (doPETASSIST)
            {
                if (PSPAWNINFO Pet = SpawnPet())
                {
                    if (Pet->WhoFollowing) PetSEEN();
                    //else if (!PetOnHold && (unsigned long)clock() > PetOnAttk && !Pet->WhoFollowing) PetBACK();
                }
            }
            if (MeleeTarg && (!pTarget || MeleeType != SpawnMask(GetSpawnByID(MeleeTarg)))) Override(NULL, "");
            if ((unsigned long)clock() > MeleeTime)
            {
                MeleeTime = (unsigned long)clock() + delay;
                MeleeHandle();
            }
        }
    }
    if ((bMULoaded && !bMUPointers) || (bMUPointers && !bMULoaded))
    {
        bMULoaded = HandleMoveUtils();
    }
}

PLUGIN_API void ShutdownPlugin()
{
    SetGameState(GAMESTATE_UNLOADING);

    RemoveCommand("/enrageon");
    RemoveCommand("/enrageoff");
    RemoveCommand("/infuriateon");
    RemoveCommand("/infuriateoff");
    RemoveCommand("/killthis");
    RemoveCommand("/melee");
    RemoveCommand("/throwit");

    RemoveMQ2Data("Melee");
    RemoveMQ2Data("meleemvs");
    RemoveMQ2Data("meleemvi");
    RemoveMQ2Data("meleemvb");

    pMeleeEvent->Reset();
    delete pMeleeEvent;
    delete pMeleeTypes;

    if (SETBINDINGS)
        Bindding(false);
}
