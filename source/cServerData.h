
#ifndef __CSERVERDATA__
#define __CSERVERDATA__
#include <map>
#include <string>
#include <cstdint>
#include "StringUtility.hpp"

//=======================================================================================
enum ClientFeatures {
	CF_BIT_CHAT = 0,		// 0x01
	CF_BIT_UOR,			// 0x02
	CF_BIT_TD,			// 0x04
	CF_BIT_LBR,			// 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
	CF_BIT_AOS,			// 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works for 4.0+
	CF_BIT_SIXCHARS,		// 0x20
	CF_BIT_SE,			// 0x40
	CF_BIT_ML,			// 0x80
	CF_BIT_EIGHTAGE,		// 0x100
	CF_BIT_NINTHAGE,		// 0x200
	CF_BIT_TENTHAGE,		// 0x400
	CF_BIT_UNKNOWN1,		// 0x800 - Increased housing/bank storage?
	CF_BIT_SEVENCHARS,	// 0x1000
	//CF_BIT_KRFACES,		// 0x2000
	//CF_BIT_TRIAL,		// 0x4000
	CF_BIT_EXPANSION = 15,	// 0x8000
	CF_BIT_SA,			// 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles - clients 6.0.14.2+
	CF_BIT_HS,			// 0x20000 - Enable HS features: boats, new movementtype? ++
	CF_BIT_GOTHHOUSE,		// 0x40000
	CF_BIT_RUSTHOUSE,		// 0x80000
	CF_BIT_JUNGLEHOUSE,	// 0x100000 - Enable Jungle housing tiles
	CF_BIT_SHADOWHOUSE,	// 0x200000 - Enable Shadowguard housing tiles
	CF_BIT_TOLHOUSE,		// 0x400000 - Enable Time of Legends features
	CF_BIT_ENDLESSHOUSE,	// 0x800000 - Enable Endless Journey account
	CF_BIT_COUNT
};


enum ServerFeatures {
	SF_BIT_UNKNOWN1 = 0,	// 0x01
	SF_BIT_IGR,			// 0x02
	SF_BIT_ONECHAR,		// 0x04 - One char only, Siege-server style
	SF_BIT_CONTEXTMENUS,	// 0x08
	SF_BIT_LIMITCHAR,		// 0x10 - Limit amount of chars, combine with OneChar
	SF_BIT_AOS,			// 0x20 - Enable Tooltips, fight system book - but not monsters/map/skills/necro/pala classes
	SF_BIT_SIXCHARS,		// 0x40 - Use 6 character slots instead of 5
	SF_BIT_SE,			// 0x80 - Samurai and Ninja classes, bushido, ninjitsu
	SF_BIT_ML,			// 0x100 - Elven race, spellweaving
	SF_BIT_UNKNOWN2,		// 0x200 - added with UO:KR launch
	SF_BIT_SEND3DTYPE,	// 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
	SF_BIT_UNKNOWN4,		// 0x800 - added sometime between UO:KR and UO:SA
	SF_BIT_SEVENCHARS,	// 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
	SF_BIT_UNKNOWN5,		// 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing?
	SF_BIT_NEWMOVE,		// 0x4000 - new movement system
	SF_BIT_FACTIONAREAS,	// 0x8000 - Unlock new Felucca faction-areas
	SF_BIT_COUNT
};

enum AssistantFeatures : std::uint64_t {
	AF_NONE = 0,

	// Razor/RE/AssistUO
	AF_FILTERWEATHER = 1 << 0,		// Weather Filter
	AF_FILTERLIGHT = 1 << 1,		// Light Filter
	AF_SMARTTARGET = 1 << 2,		// Smart Last Target
	AF_RANGEDTARGET = 1 << 3,		// Range Check Last Target
	AF_AUTOOPENDOORS = 1 << 4,		// Automatically Open Doors
	AF_DEQUIPONCAST = 1 << 5,		// Unequip Weapon on spell cast
	AF_AUTOPOTIONEQUIP = 1 << 6,		// Un/Re-equip weapon on potion use
	AF_POISONEDCHECKS = 1 << 7,		// Block heal If poisoned/Macro IIf Poisoned condition/Heal or Cure self
	AF_LOOPEDMACROS = 1 << 8,		// Disallow Looping macros, For loops, and macros that call other macros
	AF_USEONCEAGENT = 1 << 9,		// The use once agent
	AF_RESTOCKAGENT = 1 << 10,		// The restock agent
	AF_SELLAGENT = 1 << 11,			// The sell agent
	AF_BUYAGENT = 1 << 12,			// The buy agent
	AF_POTIONHOTKEYS = 1 << 13,		// All potion hotkeys
	AF_RANDOMTARGETS = 1 << 14,		// All random target hotkeys (Not target next, last target, target self)
	AF_CLOSESTTARGETS = 1 << 15,		// All closest target hotkeys
	AF_OVERHEADHEALTH = 1 << 16,		// Health and Mana/Stam messages shown over player's heads
	
	// AssistUO only?
	AF_AUTOLOOTAGENT = 1 << 17,		// The autoloot agent
	AF_BONECUTTERAGENT = 1 << 18,		// The bone cutter agent
	AF_JSCRIPTMACROS = 1 << 19,		// Javascript macro engine
	AF_AUTOREMOUNT = 1 << 20,		// Auto remount after dismount

	// UOSteam
	AF_AUTOBANDAGE = 1 << 21,		// Automatically apply bandages when health is low
	AF_ENEMYTARGETSHARE = 1 << 22,	// Share target with party/guild/alliance
	AF_FILTERSEASON = 1 << 23,		// Season filter that forces a specific season
	AF_SPELLTARGETSHARE = 1 << 24,	// Share spell target with party/guild/alliance
	AF_HUMANOIDHEALTHCHECKS = 1 << 25,	// 
	AF_SPEECHJOURNALCHECKS = 1 << 26,	// ???
	AF_ALL = 0xFFFFFFFFFFFFFFFF		// Every feature possible
};

enum cSD_TID {
	tSERVER_ERROR = -1,
	tSERVER_CORPSEDECAY = 0,	// Amount of time for a corpse to decay.
	tSERVER_WEATHER,			// Amount of time between changing light levels (day cycles).
	tSERVER_SHOPSPAWN,		// Amount of time between shopkeeper restocks.
	tSERVER_DECAY,			// Amount of time a decayable item will remain on the ground before dissapearing.
	tSERVER_DECAYINHOUSE,		// Amount of time a decayable item will remain on the floor of a house before dissapearing, if not locked down.
	tSERVER_INVISIBILITY,		// Duration of the invisibility spell.
	tSERVER_OBJECTUSAGE,		// Amount of time a player must wait between using objects.
	tSERVER_GATE,			// Duration of a summoned moongate.
	tSERVER_POISON,			// Duration of the poison effect on a character.
	tSERVER_LOGINTIMEOUT,		// Amount of time for an idle connection to time out.
	tSERVER_HITPOINTREGEN,		// Amount of time required to regenerate a single point of health.
	tSERVER_STAMINAREGEN,		// Amount of time required to regenerate a single point of stamina.
	tSERVER_MANAREGEN,		// Amount of time required to regenerate a single point of mana.
	tSERVER_FISHINGBASE,		// Base time for fishing skill to complete.
	tSERVER_FISHINGRANDOM,		// Max random time for fishing skill to complete.
	tSERVER_SPIRITSPEAK,		// Duration of the spirit speak skill.
	tSERVER_HUNGERRATE,		// Amount of time a player has before his hunger level decreases.
	tSERVER_THIRSTRATE,		// Amount of time a player has before his thirst level decreases.
	tSERVER_POLYMORPH,		// Duration of the polymorph spell.
	tSERVER_ESCORTWAIT,		// Amount of time until an escort NPC will dissapear while waiting for a player to start his quest.
	tSERVER_ESCORTACTIVE,		// Amount of time until an escort NPC will dissapear while a player is escorting him.
	tSERVER_ESCORTDONE,		// Amount of time until an escort NPC will dissapear when his quest is finished.
	tSERVER_MURDERDECAY,		// Amount of time before a permanent murder count will decay.
	tSERVER_CRIMINAL,			// Amount of time a character remains criminal after committing a criminal act.
	tSERVER_POTION,			// Delay between using potions
	tSERVER_PETOFFLINECHECK,	// Delay between checks for the PetOfflineTimeout
	tSERVER_NPCFLAGUPDATETIMER, 	// Delay in seconds between each time NPC flags are updated
	tSERVER_BLOODDECAY,		// Delay in seconds before blood spatter spawned from combat decays
	tSERVER_BLOODDECAYCORPSE,	// Delay in seconds before blood spawned along with corpses decays
	tSERVER_NPCCORPSEDECAY,		// Delay in seconds before NPC corpses decay
	tSERVER_LOYALTYRATE,		// Amount of time between each time loyalty decreases for a pet
	tSERVER_COUNT
};

enum CSDDirectoryPaths {
	CSDDP_ROOT = 0,
	CSDDP_DATA,
	CSDDP_DEFS,
	CSDDP_ACCESS,
	CSDDP_ACCOUNTS,
	CSDDP_SCRIPTS,
	CSDDP_BACKUP,
	CSDDP_MSGBOARD,
	CSDDP_SHARED,
	CSDDP_HTML,
	CSDDP_LOGS,
	CSDDP_DICTIONARIES,
	CSDDP_BOOKS,
	CSDDP_SCRIPTDATA,
	CSDDP_COUNT
};
struct __STARTLOCATIONDATA__
{
	__STARTLOCATIONDATA__()
	{
		x=0;
		y=0;
		z=0;
		worldNum = 0 ;
		instanceID = 0;
		clilocDesc = 0 ;
	}
	std::string oldTown ;
	std::string oldDescription;
	std::string newTown ;
	std::string newDescription ;
	SI16	x;
	SI16	y;
	SI16	z;
	SI16	worldNum;
	UI16	instanceID;
	UI32	clilocDesc;
	
} ;
//============================================================================
// physicalServer
//==========================================================================
//==========================================================================
class physicalServer {
public:
	auto setName(const std::string& newName) ->void;
	auto setDomain(const std::string& newDomain) ->void;
	auto setIP(const std::string& newIP) ->void;
	auto setPort(UI16 newPort) ->void;
	auto getName() const ->const std::string&;
	auto getDomain() const ->const std::string&;
	auto getIP() const ->const std::string&;
	auto getPort() const ->std::uint16_t;

private:
	std::string name;
	std::string domain;
	std::string ip;
	UI16 port;
};

class CServerData {
private:

	std::bitset< CF_BIT_COUNT > clientFeatures;
	std::bitset< SF_BIT_COUNT > serverFeatures;
	// Once over 62, bitsets are costly.  std::vector<bool> has a special exception in the c++ specificaiton, to minimize wasted space for bools
	// These should be updated
	std::bitset< 77 >	boolVals;				// Many values stored this way, rather than using bools.
	std::bitset< 64 >	spawnRegionsFacets;		// Used to determine which facets to enable spawn regions for, set in UOX>INI

	// ServerSystems
	std::string sServerName;				// 04/03/2004 - Need a place to store the name of the server (Added to support the UOG Info Request)
	UI16		port;						//	Port number that the server listens on, for connections
	std::string externalIP;
	std::vector< physicalServer > serverList;		//	Series of server entries for shard list
	UI16		serverLanguage;				//	Default language used on server
	UI08		consolelogenabled;			//	Various levels of legging 0 == none, 1 == normal, 2 == normal + all speech
	char		commandprefix;				//	Character that acts as the command prefix
	SI16		backupRatio;				//	Number of saves before a backup occurs
	UI32		serversavestimer;				//	Number of seconds between world saves
	UI32		netRcvTimeout;				// 04/03/2004 - Used to be hardcoded as 2 seconds (2 * 1000ms) for some raly laggy nets this would drop alot of packets, and disconnect people.
	UI32		netSndTimeout;				// 04/03/2004 - Not used at this time.
	UI32		netRetryCount;				// 04/03/2004 - Used to set the number of times a network recieve will be attempted before it throws an error
	bool		uogEnabled;					// 04/03/2004 - Added to support the UOG Info Request Service
	bool		randomStartingLocation;			// Enable or disable randomizing starting location for new players based on starting location entries
	UI16		jsEngineSize;				// gcMaxBytes limit in MB per JS runtime
	UI32		maxBytesIn;					// Max bytes that can be received from a client in a 10-second window before client is warned/kicked for excessive data use
	UI32		maxBytesOut;				// Max bytes that can be sent to a client in a 10-second window before client is warned/kicked for excessive data use
	UI32		trafficTimeban;				// Duration in minutes that player will be banned for if they exceed their network traffic budget
	bool		useUnicodeMessages;			// Enable or disable sending messages originating on server in Unicode format
	UI16		sysMsgColour;				// Default text colour for system messages displayed in bottom left corner of screen

	// Client Support
	bool		Clients4000Enabled;			// Allow client connections from 4.0.0 to 4.0.11f
	bool		Clients5000Enabled;			// Allow client connections from 5.0.0.0 to 5.0.8.2 (Mondain's Legacy)
	bool		Clients6000Enabled;			// Allow client connections from 6.0.0.0 to 6.0.4.0
	bool		Clients6050Enabled;			// Allow client connections from 6.0.5.0 to 6.0.14.2
	bool		Clients7000Enabled;			// Allow client connections from 7.0.0.0 to 7.0.8.2
	bool		Clients7090Enabled;			// Allow client connections from 7.0.9.0 to 7.0.15.1 (High Seas)
	bool		Clients70160Enabled;			// Allow client connections from 7.0.16.0 to 7.0.23.1
	bool		Clients70240Enabled;			// Allow client connections from 7.0.24.0+
	bool		Clients70300Enabled;			// Allow client connections from 7.0.30.0+
	bool		Clients70331Enabled;			// Allow client connections from 7.0.33.1+
	bool		Clients704565Enabled;			// Allow client connections from 7.0.45.65+ (Time of Legends)
	bool		Clients70610Enabled;			// Allow client connections from 7.0.61.0+ (Endless Journey)

	// facet block
	bool		useFacetSaves;
	std::vector< std::string > facetNameList;
	std::vector< UI32 > facetAccessFlags;

	// Skills & Stats
	UI16		skilltotalcap;				//	A cap on the total of all a PC's skills
	UI16		skillcap;					//	A cap on each individual PC skill
	UI08		skilldelay;					//	Number of seconds after a skill is used before another skill can be used
	UI16		statcap;					//	A cap on the total of a PC's stats
	SI16		maxstealthmovement;			//	Max number of steps allowed with stealth skill at 100.0
	SI16		maxstaminamovement;			//	Max number of steps allowed while running before stamina is reduced

	// ServerTimers
	UI16		serverTimers[tSERVER_COUNT];
	// Directories
	std::string serverDirectories[CSDDP_COUNT];

	// Settings
	SI16		ambientsounds;				//	Ambient sounds - values from 1->10 - higher values indicate sounds occur less often
	SI16		htmlstatusenabled;			//	If > 0 then it's enabled - only used at PC char creation - use elsewhere? (was # of seconds between updates)
	SI16		sellmaxitems;				//	Maximum number of items that can be sold to a vendor
	R32		weightPerSTR;				//	How much weight per point of STR a character can hold.
	UI16		petOfflineTimeout;			//	Offline time after a player looses all pets
	bool		paperdollGuildButton;			//	Enable Guild-button on paperdoll to access guild-menus without going through guildstone
	SI16		fishingstaminaloss;			//	The amount of stamina lost with each use of fishing skill
	UI16		maxPlayerPackItems;			//	The max amount of items a player's backpack can contain
	UI16		maxPlayerBankItems;			//	The max amount of items a player's bankbox can contain
	UI08		maxControlSlots;				//	The default max amount of pet/follower control slots for each player
	UI08		maxFollowers;				//	The default max amount of followers a player can have active
	UI08		maxPetOwners;				//	The default max amount of different owners a pet may have in its lifetime
	UI16		petLoyaltyGainOnSuccess;		//	The default amount of pet loyalty gained on successful use of a pet command
	UI16		petLoyaltyLossOnFailure;		//	The default amount of pet loyalty lost on a failed attempt to use a pet command
	UI08		maxSafeTeleports;				//	The default max amount of free teleports to safety players get via the help menu per day

	// SpeedUp
	R64			checkitems;				//	How often (in seconds) items are checked for decay and other things
	R64			checkboats;				//	How often (in seconds) boats are checked for motion and so forth
	R64			checknpcai;				//	How often (in seconds) NPCs can execute an AI routine
	R64			checkspawnregions;		//	How often (in seconds) spawn regions are checked for new spawns
	R32			npcWalkingSpeed;			//	Speed at which walking NPCs move
	R32			npcRunningSpeed;			//	Speed at which running NPCs move
	R32			npcFleeingSpeed;			//	Speed at which fleeing NPCs move
	R32			npcMountedWalkingSpeed;		//	Speed at which (mounted) walking NPCs move
	R32			npcMountedRunningSpeed;		//	Speed at which (mounted) running NPCs move
	R32			npcMountedFleeingSpeed;		//	Speed at which (mounted) fleeing NPCs move
	R64			flushTime;				//	How often (in minutes) online accounts are checked to see if they really ARE online
	R32			archeryShootDelay;		//  Attack delay for archers; after coming to a full stop, they need to wait this amount of time before they can fire an arrow. Defaults to 1.0 seconds
	R32			globalattackspeed;		//  Global attack speed that can be tweaked to quickly increase or decrease overall combat speed. Defaults to 1.0
	R32			npcspellcastspeed;		//  For adjusting the overall speed of (or delay between) NPC spell casts. Defaults to 1.0

	// MessageBoards
	UI08		msgpostinglevel;				//	If not 0, then players can post
	UI08		msgremovallevel;				//	If not 0, then players can remove posts

	// WorldLight
	LIGHTLEVEL	dungeonlightlevel;			//	Default light level for a dungeon, if not subject to a weather system
	LIGHTLEVEL	currentlightlevel;			//	Default current light level if not subject to a weather system
	LIGHTLEVEL	brightnesslightlevel;			//	Default brightest light level if not subject to a weather system
	LIGHTLEVEL	darknesslightlevel;			//	Default darkest light level if not subject to a weather system

	// WorldTimer						//	days/hours/minutes/seconds to it's own file?
	UI16		secondsperuominute;			//	Number of seconds for a UOX minute.
	SI16		moon[2];					//	Moon current state
	SI16		days;						//	Number of days the world has been running (persistent)
	UI08		hours;					//	Number of hours the world has been running (persistent)
	UI08		minutes;					//	Number of minutes the world has been running (persistent)
	UI08		seconds;					//	Number of seconds the world has been running (persistent)
	bool		ampm;						//	Whether our current time is in the morning or afternoon

	// Tracking
	UI16		trackingbaserange;			//	Minimum range even a novice can track from
	UI16		trackingbasetimer;			//	Tracking timer - maximum time for a GM to track at
	UI08		trackingmaxtargets;			//	Maximum number of targets that can be tracked
	UI16		trackingmsgredisplaytimer;		//	How often (in seconds) the tracking message is redisplayed

	// Reputation
	UI16		maxmurdersallowed;			//	Maximum number of kills before turning red

	// Resources
	UI16		resourceAreaSize;				//	Size of each resource area to split each world into
	UI08		minecheck;					//	Type of mining check performed - 0 anywhere 1 only mountains/floors 2 region based (not working)
	SI16		oreperarea;					//	TIMERVAL? Maximum number of ores in a resource area
	UI16		orerespawntimer;				//	Time at which ore is respawned (only 1 ore, not all)
	SI16		logsperarea;				//	Maximum number of logs in a resource area
	UI16		logsrespawntimer;				//	TIMERVAL? Time at which logs are respawned (only 1 log, not all)
	SI16		fishperarea;				//	Maximum number of fish in a resource area
	UI16		fishrespawntimer;				//	TIMERVAL? Time at which fish are respawned (only 1 fish, not all)

	// Hunger & Food
	SI16		hungerdamage;				//	Amount of damage applied if hungry and below threshold
	// Thirst
	SI16		thirstdrain;				//	Amount of stamina drained if thirsty and below threshold

	// Combat
	SI16		combatmaxrange;				//	RANGE?  Range at which combat can actually occur
	SI16		combatmaxspellrange;			//	RANGE?  Range at which spells can be cast
	UI08		combatanimalattackchance;		//	Chance of animals being attacked (0-100)
	SI16		combatnpcdamagerate;			//	NPC Damage divisor - PCs sustain less than NPCs.  If a PC, damage is 1/value
	SI16		combatnpcbasefleeat;			//	% of HP where an NPC will flee, if it's not defined for them
	SI16		combatnpcbasereattackat;		//	% of HP where an NPC will resume attacking
	SI16		combatattackstamina;			//	Amount of stamina lost when hitting an opponent
	SI08		combatArcheryHitBonus;			//  Bonus to hit chance for Archery skill in combat, applied after regular hit chance calculation
	UI08		combatweapondamagechance;		//  Chance of weapons being damaged when attacking in combat (0-100)
	UI08		combatweapondamagemin;			//  Minimum amount of hitpoints a weapon can lose when being damaged in combat
	UI08		combatweapondamagemax;			//  Maximum amount of hitpoints a weapon can lose when being damaged in combat
	UI08		combatarmordamagechance;		//  Chance of armor being damaged when defending in combat (0-100)
	UI08		combatarmordamagemin;			//  Minimum amount of hitpoints an armor can lose when being damaged in combat
	UI08		combatarmordamagemax;			//  Maximum amount of hitpoints an armor can lose when being damaged in combat
	UI08		combatparrydamagechance;		//  Chance of shield being damaged when parrying in combat (0-100)
	UI08		combatparrydamagemin;			//  Minimum amount of hitpoints a shield/weapon can lose when successfully parrying in combat
	UI08		combatparrydamagemax;			//  Maximum amount of hitpoints a shield/weapon can lose when successfully parrying in combat
	UI08		combatbloodeffectchance;		//  Chance of blood splatter effects spawning during combat
	UI08		alchemyDamageBonusModifier;		//  Modifier used to calculate bonus damage for explosion potions based on alchemy skill

	// Start & Location Settings
	std::vector< __STARTLOCATIONDATA__ >	startlocations;
	SI16		startgold;					//	Amount of gold created when a PC is made
	UI16		startprivs;					//	Starting privileges of characters

	// Anything under this comment is left here for symantics
	UI08		skilllevel;					//	Some skill value associated with the rank system
	SI16		buyThreshold;				//	Value above which money will be sourced from the bank rather than the player

	// Gump stuff
	UI16		titleColour;				//	Default text colour for titles in gumps
	UI16		leftTextColour;				//	Default text colour for left text in gumps (2 column ones)
	UI16		rightTextColour;				//	Default text colour for right text in gumps
	UI16		buttonCancel;				//	Default Button ID for cancel button in gumps
	UI16		buttonLeft;					//	Default Button ID for left button in gumps
	UI16		buttonRight;				//	Default Button ID for right button in gumps
	UI16		backgroundPic;				//	Default Gump ID for background gump

	// Houses
	UI16		maxHousesOwnable;				//	Max amount of houses that a player can own
	UI16		maxHousesCoOwnable;			//	Max amount of houses that a player can co-own

	// Townstone stuff
	UI32		numSecsPollOpen;				//	Time (in seconds) for which a town voting poll is open
	UI32		numSecsAsMayor;				//	Time (in seconds) that a PC would be a mayor
	UI32		taxPeriod;					//	Time (in seconds) between periods of taxes for PCs
	UI32		guardPayment;				//	Time (in seconds) between payments for guards

	void	PostLoadDefaults();

	static const std::map<std::string,std::int32_t> uox3inicasevalue;
public:
	UI64		DisabledAssistantFeatures;
	
	auto 		lookupINIValue(const std::string& tag) ->std::int32_t ;

	auto		SetServerFeature( ServerFeatures, bool ) ->void;
	auto		SetServerFeatures( size_t ) ->void;
	auto		GetServerFeature( ServerFeatures ) const ->bool;
	auto		GetServerFeatures() const ->size_t;

	auto		SetClientFeature( ClientFeatures, bool ) ->void;
	auto		SetClientFeatures( UI32 ) ->void;
	auto		GetClientFeature( ClientFeatures ) const ->bool;
	UI32		GetClientFeatures() const;

	auto		SetDisabledAssistantFeature( AssistantFeatures, bool ) ->void;
	auto		SetDisabledAssistantFeatures( UI64 ) ->void;
	auto		GetDisabledAssistantFeature( AssistantFeatures ) const ->bool;
	UI64		GetDisabledAssistantFeatures() const;

	auto		SetAssistantNegotiation( bool value ) ->void;
	auto		GetAssistantNegotiation() const ->bool;

	auto		GetSpawnRegionsFacetStatus( UI32 value ) const ->bool;
	auto		SetSpawnRegionsFacetStatus( UI32 value, bool status ) ->void;
	UI32		GetSpawnRegionsFacetStatus() const;
	auto		SetSpawnRegionsFacetStatus( UI32 value ) ->void;

	auto		SetClassicUOMapTracker( bool value ) ->void;
	auto		GetClassicUOMapTracker() const ->bool;

	auto		UseUnicodeMessages( bool value ) ->void;
	auto		UseUnicodeMessages() const ->bool;

	SI16		ServerMoon( SI16 slot ) const;
	LIGHTLEVEL	WorldLightDarkLevel() const;
	LIGHTLEVEL	WorldLightBrightLevel() const;
	LIGHTLEVEL	WorldLightCurrentLevel() const;
	LIGHTLEVEL	DungeonLightLevel() const;
	auto		ServerStartPrivs() const ->std::uint16_t;
	SI16		ServerStartGold() const;

	auto		ServerMoon( SI16 slot, SI16 value ) ->void;
	auto		WorldLightDarkLevel( LIGHTLEVEL value ) ->void;
	auto		WorldLightBrightLevel( LIGHTLEVEL value ) ->void;
	auto		WorldLightCurrentLevel( LIGHTLEVEL value ) ->void;
	auto		DungeonLightLevel( LIGHTLEVEL value ) ->void;
	auto		ServerStartPrivs( UI16 value ) ->void;
	auto		ServerStartGold( SI16 value ) ->void;
	auto		ParseINI( const std::string& filename ) ->bool;
	auto		HandleLine( const std::string& tag, const std::string& value ) ->bool;

	auto		Load() ->void;
	auto		save() ->bool;
	auto		save( const std::string &filename ) ->bool;

	auto ResetDefaults() ->void ;
	auto startup() ->void ;
	//void		RefreshIPs( void );

	CServerData();
	auto		ServerName( const std::string &setname ) ->void;
	auto		ServerDomain( const std::string &setdomain ) ->void;
	auto		ServerIP( const std::string &setip ) ->void;
	auto		ServerName() const ->const std::string &;
	auto		ServerDomain() const ->const std::string &;
	auto		ServerIP() const ->const std::string &;
	auto		ExternalIP() const ->const std::string &;
	auto 		ExternalIP( const std::string &ip ) ->void;
	auto		ServerPort( UI16 setport ) ->void;
	auto		ServerPort() const ->std::uint16_t;
	auto		ServerConsoleLog( bool setting ) ->void;
	auto		ServerConsoleLog() const ->bool;
	auto		ServerNetworkLog(bool setting) ->void;
	auto		ServerNetworkLog() const ->bool;
	auto		ServerSpeechLog( bool setting ) ->void;
	auto		ServerSpeechLog() const ->bool;
	auto		ServerCommandPrefix( char cmdvalue ) ->void;
	char		ServerCommandPrefix() const;
	auto		ServerAnnounceSaves( bool setting ) ->void;
	auto		ServerAnnounceSavesStatus() const ->bool;
	auto		ServerJoinPartAnnouncements( bool setting ) ->void;
	auto		ServerJoinPartAnnouncementsStatus() const ->bool;
	auto		ServerMulCaching( bool setting ) ->void;
	auto		ServerMulCachingStatus() const ->bool;
	auto		ServerBackups( bool setting ) ->void;
	auto		ServerBackupStatus() const ->bool;
	auto		ServerContextMenus( bool setting ) ->void;
	auto		ServerContextMenus() const ->bool;
	auto		ServerSavesTimer( UI32 timer ) ->void;
	UI32		ServerSavesTimerStatus() const;
	auto		ServerMainThreadTimer( UI32 threadtimer ) ->void;
	UI32		ServerMainThreadTimerStatus() const;
	auto		ServerSkillTotalCap( UI16 cap ) ->void;
	auto		ServerSkillTotalCapStatus() const ->std::uint16_t;
	auto		ServerSkillCap( UI16 cap ) ->void;
	auto		ServerSkillCapStatus() const ->std::uint16_t;
	auto		ServerSkillDelay( UI08 skdelay ) ->void;
	UI08		ServerSkillDelayStatus() const;
	auto		ServerStatCap( UI16 cap ) ->void;
	auto		ServerStatCapStatus() const ->std::uint16_t;
	auto		MaxStealthMovement( SI16 value ) ->void;
	SI16		MaxStealthMovement() const;
	auto		MaxStaminaMovement( SI16 value ) ->void;
	SI16		MaxStaminaMovement() const;
	auto		SystemTimer( cSD_TID timerid, UI16 value ) ->void;
	auto		SystemTimer( cSD_TID timerid ) const ->std::uint16_t;
	TIMERVAL	BuildSystemTimeValue( cSD_TID timerID ) const;
	auto		SysMsgColour( UI16 value ) ->void;
	auto		SysMsgColour() const ->std::uint16_t;

	auto		ServerUOGEnabled() const ->bool { return uogEnabled; }
	auto		ServerUOGEnabled(bool uogValue) ->void {	uogEnabled = uogValue; }
	auto		ConnectUOServerPoll( bool value ) ->void;
	auto		ConnectUOServerPoll() const ->bool;
	auto		ServerRandomStartingLocation() const ->bool { return randomStartingLocation; }
	auto		ServerRandomStartingLocation( bool rndStartLocValue ) ->void { randomStartingLocation = rndStartLocValue; }
	UI32		ServerNetRetryCount() const { return netRetryCount; }
	auto		ServerNetRetryCount(UI32 retryValue)  ->void{ netRetryCount = retryValue; }
	UI32		ServerNetRcvTimeout() const { return netRcvTimeout; }
	auto		ServerNetRcvTimeout(UI32 timeoutValue) ->void { netRcvTimeout = timeoutValue; }
	UI32		ServerNetSndTimeout() const { return netSndTimeout; }
	auto		ServerNetSndTimeout(UI32 timeoutValue) ->void { netSndTimeout = timeoutValue; }

	// ClientSupport used to determine login-restrictions
	auto		ClientSupport4000() const ->bool { return Clients4000Enabled; }
	auto		ClientSupport4000(bool cliSuppValue) ->void { Clients4000Enabled = cliSuppValue; }
	auto		ClientSupport5000() const ->bool { return Clients5000Enabled; }
	auto		ClientSupport5000(bool cliSuppValue) ->void { Clients5000Enabled = cliSuppValue; }
	auto		ClientSupport6000() const  ->bool{ return Clients6000Enabled; }
	auto		ClientSupport6000(bool cliSuppValue) ->void { Clients6000Enabled = cliSuppValue; }
	auto		ClientSupport6050() const  ->bool{ return Clients6050Enabled; }
	auto		ClientSupport6050(bool cliSuppValue) ->void { Clients6050Enabled = cliSuppValue; }
	auto		ClientSupport7000() const  ->bool{ return Clients7000Enabled; }
	auto		ClientSupport7000(bool cliSuppValue) ->void { Clients7000Enabled = cliSuppValue; }
	auto		ClientSupport7090() const ->bool { return Clients7090Enabled; }
	auto		ClientSupport7090(bool cliSuppValue) ->void { Clients7090Enabled = cliSuppValue; }
	auto		ClientSupport70160() const ->bool { return Clients70160Enabled; }
	auto		ClientSupport70160(bool cliSuppValue) ->void { Clients70160Enabled = cliSuppValue; }
	auto		ClientSupport70240() const ->bool { return Clients70240Enabled; }
	auto		ClientSupport70240(bool cliSuppValue) ->void { Clients70240Enabled = cliSuppValue; }
	auto		ClientSupport70300() const  ->bool{ return Clients70300Enabled; }
	auto		ClientSupport70300( bool cliSuppValue ) ->void { Clients70300Enabled = cliSuppValue; }
	auto		ClientSupport70331() const ->bool { return Clients70331Enabled; }
	auto		ClientSupport70331( bool cliSuppValue ) ->void { Clients70331Enabled = cliSuppValue; }
	auto		ClientSupport704565() const ->bool { return Clients704565Enabled; }
	auto		ClientSupport704565( bool cliSuppValue ) ->void { Clients704565Enabled = cliSuppValue; }
	auto		ClientSupport70610() const  ->bool{ return Clients70610Enabled; }
	auto		ClientSupport70610( bool cliSuppValue ) ->void { Clients70610Enabled = cliSuppValue; }

	auto		StatsAffectSkillChecks( bool setting ) ->void;
	auto		StatsAffectSkillChecks() const ->bool;

	// Enable/disable higher total for starting stats, and/or fourth starting skill in clients 7.0.16+
	auto		ExtendedStartingStats( bool setting ) ->void;
	auto		ExtendedStartingStats() const ->bool;
	auto		ExtendedStartingSkills( bool setting ) ->void;
	auto		ExtendedStartingSkills() const ->bool;

	// Define all Path Get/Set's here please
	auto		Directory( CSDDirectoryPaths dp, std::string value ) ->void;
	std::string Directory( CSDDirectoryPaths dp );


	auto		CorpseLootDecay( bool value ) ->void;
	auto		CorpseLootDecay() const ->bool;

	auto		GuardStatus( bool value ) ->void;
	auto		GuardsStatus() const ->bool;

	auto		DeathAnimationStatus( bool value ) ->void;
	auto		DeathAnimationStatus() const ->bool;

	auto		WorldAmbientSounds( SI16 value ) ->void;
	SI16		WorldAmbientSounds() const;

	auto		AmbientFootsteps( bool value ) ->void;
	auto		AmbientFootsteps() const ->bool;

	auto		InternalAccountStatus( bool value ) ->void;
	auto		InternalAccountStatus() const ->bool;

	auto		ShowOfflinePCs( bool value ) ->void;
	auto		ShowOfflinePCs() const ->bool;

	auto		RogueStatus( bool value ) ->void;
	auto		RogueStatus() const ->bool;

	auto		SnoopIsCrime( bool value ) ->void;
	auto		SnoopIsCrime() const ->bool;

	auto		PlayerPersecutionStatus( bool value ) ->void;
	auto		PlayerPersecutionStatus() const ->bool;

	auto		HtmlStatsStatus( SI16 value ) ->void;
	SI16		HtmlStatsStatus() const;

	auto		SellByNameStatus( bool value ) ->void;
	auto		SellByNameStatus() const ->bool;

	auto		SellMaxItemsStatus( SI16 value ) ->void;
	SI16		SellMaxItemsStatus() const;

	auto		TradeSystemStatus( bool value ) ->void;
	auto		TradeSystemStatus() const ->bool;

	auto		RankSystemStatus( bool value ) ->void;
	auto		RankSystemStatus() const ->bool;

	auto		CutScrollRequirementStatus( bool value ) ->void;
	auto		CutScrollRequirementStatus() const ->bool;

	auto		CheckPetControlDifficulty( bool value ) ->void;
	auto		CheckPetControlDifficulty() const ->bool;

	auto		NPCTrainingStatus( bool setting ) ->void;
	auto		NPCTrainingStatus() const ->bool;

	auto		CheckItemsSpeed( R64 value ) ->void;
	R64		CheckItemsSpeed() const;

	auto		CheckBoatSpeed( R64 value ) ->void;
	R64		CheckBoatSpeed() const;

	auto		CheckNpcAISpeed( R64 value ) ->void;
	R64		CheckNpcAISpeed() const;

	auto		CheckSpawnRegionSpeed( R64 value ) ->void;
	R64		CheckSpawnRegionSpeed() const;

	auto		GlobalAttackSpeed( R32 value ) ->void;
	R32		GlobalAttackSpeed() const;

	auto		NPCSpellCastSpeed( R32 value ) ->void;
	R32		NPCSpellCastSpeed() const;

	auto		MsgBoardPostingLevel( UI08 value ) ->void;
	UI08		MsgBoardPostingLevel() const;

	auto		MsgBoardPostRemovalLevel( UI08 value ) ->void;
	UI08		MsgBoardPostRemovalLevel() const;

	auto		MineCheck( UI08 value ) ->void;
	UI08		MineCheck() const;

	auto		AlchemyDamageBonusEnabled( bool value ) ->void;
	auto		AlchemyDamageBonusEnabled() const ->bool;

	auto		AlchemyDamageBonusModifier( UI08 value ) ->void;
	UI08		AlchemyDamageBonusModifier() const;

	auto		ItemsInterruptCasting( bool value ) ->void;
	auto		ItemsInterruptCasting() const ->bool;

	auto		CombatArmorClassDamageBonus( bool value ) ->void;
	auto		CombatArmorClassDamageBonus() const ->bool;

	auto		CombatDisplayHitMessage( bool value ) ->void;
	auto		CombatDisplayHitMessage() const ->bool;

	auto		CombatDisplayDamageNumbers( bool value ) ->void;
	auto		CombatDisplayDamageNumbers() const ->bool;

	auto		CombatAttackSpeedFromStamina( bool value ) ->void;
	auto		CombatAttackSpeedFromStamina() const ->bool;

	auto		FishingStaminaLoss( SI16 value ) ->void;
	SI16		FishingStaminaLoss() const;

	auto		CombatAttackStamina( SI16 value ) ->void;
	SI16		CombatAttackStamina() const;

	auto		CombatNPCDamageRate( SI16 value ) ->void;
	SI16		CombatNPCDamageRate() const;

	UI08		SkillLevel() const;
	auto		SkillLevel( UI08 value ) ->void;

	auto		EscortsEnabled( bool value ) ->void;
	auto		EscortsEnabled() const ->bool;

	auto		ItemsDetectSpeech( bool value ) ->void;
	auto		ItemsDetectSpeech() const ->bool;

	auto		ForceNewAnimationPacket( bool value ) ->void;
	auto		ForceNewAnimationPacket() const ->bool;

	auto		MapDiffsEnabled( bool value ) ->void;
	auto		MapDiffsEnabled() const ->bool;

	auto		MaxPlayerPackItems( UI16 value ) ->void;
	auto		MaxPlayerPackItems() const ->std::uint16_t;

	auto		MaxPlayerBankItems( UI16 value ) ->void;
	auto		MaxPlayerBankItems() const ->std::uint16_t;

	auto		BasicTooltipsOnly( bool value ) ->void;
	auto		BasicTooltipsOnly() const ->bool;

	auto		ShowNpcTitlesInTooltips( bool value ) ->void;
	auto		ShowNpcTitlesInTooltips() const ->bool;

	auto		GlobalItemDecay( bool value ) ->void;
	auto		GlobalItemDecay() const ->bool;

	auto		ScriptItemsDecayable( bool value ) ->void;
	auto		ScriptItemsDecayable() const ->bool;

	auto		BaseItemsDecayable( bool value ) ->void;
	auto		BaseItemsDecayable() const ->bool;

	auto		ItemDecayInHouses( bool value ) ->void;
	auto		ItemDecayInHouses() const ->bool;

	auto		ProtectPrivateHouses( bool value ) ->void;
	auto		ProtectPrivateHouses() const ->bool;

	auto		TrackHousesPerAccount( bool value ) ->void;
	auto		TrackHousesPerAccount() const ->bool;

	auto		CanOwnAndCoOwnHouses( bool value ) ->void;
	auto		CanOwnAndCoOwnHouses() const ->bool;

	auto		CoOwnHousesOnSameAccount( bool value ) ->void;
	auto		CoOwnHousesOnSameAccount() const ->bool;

	auto		MaxHousesOwnable( UI16 value ) ->void;
	auto		MaxHousesOwnable() const ->std::uint16_t;

	auto		MaxHousesCoOwnable( UI16 value ) ->void;
	auto		MaxHousesCoOwnable() const ->std::uint16_t;

	auto		PaperdollGuildButton( bool value ) ->void;
	auto		PaperdollGuildButton() const ->bool;

	auto		CombatMonstersVsAnimals( bool value ) ->void;
	auto		CombatMonstersVsAnimals() const ->bool;

	auto		CombatAnimalsAttackChance( UI16 value ) ->void;
	auto		CombatAnimalsAttackChance() const ->std::uint16_t;

	auto		CombatArcheryShootDelay( R32 value ) ->void;
	R32		CombatArcheryShootDelay() const;

	auto		CombatArcheryHitBonus( SI08 value ) ->void;
	SI08		CombatArcheryHitBonus() const;

	auto		CombatWeaponDamageChance( UI08 value ) ->void;
	UI08		CombatWeaponDamageChance() const;

	auto		CombatWeaponDamageMin( UI08 value ) ->void;
	UI08		CombatWeaponDamageMin( void ) const;

	auto		CombatWeaponDamageMax( UI08 value ) ->void;
	UI08		CombatWeaponDamageMax() const;

	auto		CombatArmorDamageChance( UI08 value ) ->void;
	UI08		CombatArmorDamageChance() const;

	auto		CombatArmorDamageMin( UI08 value ) ->void;
	UI08		CombatArmorDamageMin() const;

	auto		CombatArmorDamageMax( UI08 value ) ->void;
	UI08		CombatArmorDamageMax() const;

	auto		CombatParryDamageChance( UI08 value ) ->void;
	UI08		CombatParryDamageChance() const;

	auto		CombatParryDamageMin( UI08 value ) ->void;
	UI08		CombatParryDamageMin() const;

	auto		CombatParryDamageMax( UI08 value ) ->void;
	UI08		CombatParryDamageMax() const;

	auto		CombatBloodEffectChance( UI08 value ) ->void;
	UI08		CombatBloodEffectChance() const;

	auto		TravelSpellsFromBoatKeys( bool value ) ->void;
	auto		TravelSpellsFromBoatKeys() const ->bool;

	auto		TravelSpellsWhileOverweight( bool value ) ->void;
	auto		TravelSpellsWhileOverweight() const ->bool;

	auto		MarkRunesInMultis( bool value ) ->void;
	auto		MarkRunesInMultis() const ->bool;

	auto		TravelSpellsBetweenWorlds( bool value ) ->void;
	auto		TravelSpellsBetweenWorlds() const ->bool;

	auto		TravelSpellsWhileAggressor( bool value ) ->void;
	auto		TravelSpellsWhileAggressor() const ->bool;

	auto		ToolUseLimit( bool value ) ->void;
	auto		ToolUseLimit() const ->bool;

	auto		ToolUseBreak( bool value ) ->void;
	auto		ToolUseBreak() const ->bool;

	auto		ItemRepairDurabilityLoss( bool value ) ->void;
	auto		ItemRepairDurabilityLoss() const ->bool;

	auto		HideStatsForUnknownMagicItems( bool value ) ->void;
	auto		HideStatsForUnknownMagicItems() const ->bool;

	auto		CraftColouredWeapons( bool value ) ->void;
	auto		CraftColouredWeapons() const ->bool;

	auto		MaxControlSlots( UI08 value ) ->void;
	UI08		MaxControlSlots() const;

	auto		MaxFollowers( UI08 value ) ->void;
	UI08		MaxFollowers() const;

	auto		MaxPetOwners( UI08 value ) ->void;
	UI08		MaxPetOwners() const;

	auto		SetPetLoyaltyGainOnSuccess( UI16 value ) ->void;
	auto		GetPetLoyaltyGainOnSuccess() const ->std::uint16_t;

	auto		SetPetLoyaltyLossOnFailure( UI16 value ) ->void;
	auto		GetPetLoyaltyLossOnFailure() const ->std::uint16_t;

	auto		MaxSafeTeleportsPerDay( UI08 value ) ->void;
	UI08		MaxSafeTeleportsPerDay() const;

	auto		TeleportToNearestSafeLocation( bool value ) ->void;
	auto		TeleportToNearestSafeLocation() const ->bool;

	auto		AllowAwakeNPCs( bool value ) ->void;
	auto		AllowAwakeNPCs() const ->bool;

	auto		HungerSystemEnabled( bool value ) ->void;
	auto		HungerSystemEnabled() const ->bool;

	auto		ThirstSystemEnabled( bool value ) ->void;
	auto		ThirstSystemEnabled() const ->bool;

	auto		HungerDamage( SI16 value ) ->void;
	SI16		HungerDamage() const;

	auto		ThirstDrain( SI16 value ) ->void;
	SI16		ThirstDrain() const;

	auto		PetHungerOffline( bool value ) ->void;
	auto		PetHungerOffline() const ->bool;

	auto		PetThirstOffline( bool value ) ->void;
	auto		PetThirstOffline() const ->bool;

	auto		PetOfflineTimeout( UI16 value ) ->void;
	auto		PetOfflineTimeout() const ->std::uint16_t;

	auto		BuyThreshold( SI16 value ) ->void;
	SI16		BuyThreshold() const;

	auto		BackupRatio( SI16 value ) ->void;
	SI16		BackupRatio() const;

	auto		CombatMaxRange( SI16 value ) ->void;
	SI16		CombatMaxRange() const;

	auto		CombatMaxSpellRange( SI16 value ) ->void;
	SI16		CombatMaxSpellRange() const;

	auto		CombatAnimalsGuarded( bool value ) ->void;
	auto		CombatAnimalsGuarded() const ->bool;

	auto		CombatNPCBaseFleeAt( SI16 value ) ->void;
	SI16		CombatNPCBaseFleeAt() const;

	auto		CombatNPCBaseReattackAt( SI16 value ) ->void;
	SI16		CombatNPCBaseReattackAt() const;

	auto		ShootOnAnimalBack( bool setting ) ->void;
	auto		ShootOnAnimalBack() const ->bool;

	auto		NPCWalkingSpeed( R32 value ) ->void;
	R32		NPCWalkingSpeed() const;

	auto		NPCRunningSpeed( R32 value ) ->void;
	R32		NPCRunningSpeed() const;

	auto		NPCFleeingSpeed( R32 value ) ->void;
	R32		NPCFleeingSpeed() const;

	auto		NPCMountedWalkingSpeed( R32 value ) ->void;
	R32		NPCMountedWalkingSpeed() const;

	auto		NPCMountedRunningSpeed( R32 value ) ->void;
	R32		NPCMountedRunningSpeed() const;

	auto		NPCMountedFleeingSpeed( R32 value ) ->void;
	R32		NPCMountedFleeingSpeed() const;

	auto		TitleColour( UI16 value ) ->void;
	auto		TitleColour() const ->std::uint16_t;

	auto		LeftTextColour( UI16 value ) ->void;
	auto		LeftTextColour() const ->std::uint16_t;

	auto		RightTextColour( UI16 value ) ->void;
	auto		RightTextColour() const ->std::uint16_t;

	auto		ButtonCancel( UI16 value ) ->void;
	auto		ButtonCancel() const ->std::uint16_t;

	auto		ButtonLeft( UI16 value ) ->void;
	auto		ButtonLeft() const ->std::uint16_t;

	auto		ButtonRight( UI16 value ) ->void;
	auto		ButtonRight() const ->std::uint16_t;

	auto		BackgroundPic( UI16 value ) ->void;
	auto		BackgroundPic() const ->std::uint16_t;

	auto		TownNumSecsPollOpen( UI32 value ) ->void;
	UI32		TownNumSecsPollOpen() const;

	auto		TownNumSecsAsMayor( UI32 value ) ->void;
	UI32		TownNumSecsAsMayor() const;

	auto		TownTaxPeriod( UI32 value ) ->void;
	UI32		TownTaxPeriod() const;

	auto		TownGuardPayment( UI32 value ) ->void;
	UI32		TownGuardPayment() const;

	auto		RepMaxKills( UI16 value ) ->void;
	auto		RepMaxKills() const ->std::uint16_t;

	auto		ResLogs( SI16 value ) ->void;
	SI16		ResLogs() const;

	auto		ResLogTime( UI16 value ) ->void;
	auto		ResLogTime() const ->std::uint16_t;

	auto		ResOre( SI16 value ) ->void;
	SI16		ResOre() const;

	auto		ResOreTime( UI16 value ) ->void;
	auto		ResOreTime() const ->std::uint16_t;

	auto		ResourceAreaSize( UI16 value ) ->void;
	auto		ResourceAreaSize() const ->std::uint16_t;

	auto		ResFish( SI16 value ) ->void;
	SI16		ResFish() const;

	auto		ResFishTime( UI16 value ) ->void;
	auto		ResFishTime() const ->std::uint16_t;

	auto		AccountFlushTimer( R64 value ) ->void;
	R64		AccountFlushTimer() const;

	auto		TrackingBaseRange( UI16 value ) ->void;
	auto		TrackingBaseRange() const ->std::uint16_t;

	auto		TrackingMaxTargets( UI08 value ) ->void;
	UI08		TrackingMaxTargets() const;

	auto		TrackingBaseTimer( UI16 value ) ->void;
	auto		TrackingBaseTimer() const ->std::uint16_t;

	auto		TrackingRedisplayTime( UI16 value ) ->void;
	auto		TrackingRedisplayTime() const ->std::uint16_t;

	// Sept 22, 2002 - Support for HideWhileMounted fix.
	auto		CharHideWhileMounted( bool value ) ->void;
	auto		CharHideWhileMounted() const ->bool;

	auto		WeightPerStr( R32 newVal ) ->void;
	R32		WeightPerStr() const;

	auto		ServerOverloadPackets( bool newVal ) ->void;
	auto		ServerOverloadPackets() const ->bool;

	auto		ArmorAffectManaRegen( bool newVal ) ->void;
	auto		ArmorAffectManaRegen() const ->bool;

	auto		AdvancedPathfinding( bool value ) ->void;
	auto		AdvancedPathfinding() const ->bool;

	auto		LootingIsCrime( bool value ) ->void;
	auto		LootingIsCrime() const ->bool;

	auto		KickOnAssistantSilence( bool value ) ->void;
	auto		KickOnAssistantSilence() const ->bool;

	auto		dumpPaths() ->void;

	auto		ServerLocation( std::string toSet ) ->void;
	auto 		ServerLocation( size_t locNum ) ->__STARTLOCATIONDATA__ *;

	auto		NumServerLocations() const ->size_t ;

	auto		ServerSecondsPerUOMinute() const ->std::uint16_t;
	auto		ServerSecondsPerUOMinute( UI16 newVal ) ->void;

	auto		ServerLanguage() const ->std::uint16_t;
	auto		ServerLanguage( UI16 newVal ) ->void;

	UI32		MaxClientBytesIn() const;
	auto		MaxClientBytesIn( UI32 newVal ) ->void;

	UI32		MaxClientBytesOut() const;
	auto		MaxClientBytesOut( UI32 newVal ) ->void;

	UI32		NetTrafficTimeban() const;
	auto		NetTrafficTimeban( UI32 newVal ) ->void;

	auto		GetJSEngineSize() const ->std::uint16_t;
	auto		SetJSEngineSize( UI16 newVal ) ->void;

	SI16		ServerTimeDay() const;
	UI08		ServerTimeHours() const;
	UI08		ServerTimeMinutes() const;
	UI08		ServerTimeSeconds() const;
	auto		ServerTimeAMPM() const ->bool;

	auto		ServerTimeDay( SI16 nValue ) ->void;
	auto		ServerTimeHours( UI08 nValue ) ->void;
	auto		ServerTimeMinutes( UI08 nValue ) ->void;
	auto		ServerTimeSeconds( UI08 nValue ) ->void;
	auto		ServerTimeAMPM( bool nValue ) ->void;

	auto		SaveTime() ->void;
	auto		LoadTime() ->void;
	auto		LoadTimeTags( std::ifstream &input ) ->void;

	// These functions return true if it's a new day
	auto		incSecond() ->bool;
	auto		incMinute() ->bool;
	auto		incHour() ->bool;
	auto		incDay() ->bool;

	auto		incMoon( SI32 mNumber ) ->void;

	physicalServer *ServerEntry( UI16 entryNum );
	auto		ServerCount() const ->std::uint16_t ;

private:
	bool		resettingDefaults;

};

#endif

