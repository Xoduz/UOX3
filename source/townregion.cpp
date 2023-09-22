#include "townregion.h"

#include "cchar.h"
#include "cgump.h"
#include "cjsengine.h"
#include "cjsmapping.h"
#include "classes.h"
#include "subsystem/console.hpp"
#include "cpacketsend.h"
#include "craces.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "cspawnregion.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "jail.h"
#include "mapstuff.h"
#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "skills.h"
#include "speech.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"
#include "other/uoxglobal.hpp"

// Implementation of town regions

const std::int8_t MAYOR = 0x01;
// const std::int8_t ENEMY = 0x02;
// const std::int8_t JOINER = 0x03;

const std::uint32_t BIT_GUARDED = 0;
const std::uint32_t BIT_MARK = 1;
const std::uint32_t BIT_GATE = 2;
const std::uint32_t BIT_RECALL = 3;
const std::uint32_t BIT_AGGRESSIVE = 6;
const std::uint32_t BIT_DUNGEON = 7;
const std::uint32_t BIT_SAFEZONE = 8;
const std::uint32_t BIT_TELEPORT = 9;
const std::uint32_t BIT_HOUSING = 10;
const std::uint32_t BIT_SUBREGION = 11;

const raceid_t DEFTOWN_RACE = 0;
const weathid_t DEFTOWN_WEATHER = 255;
const std::int32_t DEFTOWN_MUSICLIST = 0;
const serial_t DEFTOWN_MAYOR = INVALIDSERIAL;
const std::uint16_t DEFTOWN_TAXEDRESOURCE = 0x0EED;
const std::uint16_t DEFTOWN_TAXEDAMOUNT = 0;
const std::int32_t DEFTOWN_GOLDRESERVED = 0;
const std::int16_t DEFTOWN_GUARDSPURCHASED = 0;
const std::uint32_t DEFTOWN_RESOURCECOLLECTED = 0;
const worldtype_t DEFTOWN_VISUALAPPEARANCE = WRLD_SPRING;
const std::int16_t DEFTOWN_HEALTH = 30000;
const std::uint32_t DEFTOWN_ELECTIONCLOSE = 0;
const std::uint32_t DEFTOWN_NEXTPOLL = 0;
const std::uint32_t DEFTOWN_GUARDSPAID = 0;
const std::uint32_t DEFTOWN_TAXEDMEMBERS = 0;
const std::uint8_t DEFTOWN_WORLDNUMBER = 0;
const std::uint16_t DEFTOWN_INSTANCEID = 0;
const std::uint16_t DEFTOWN_JSSCRIPT = 0xFFFF;
const std::uint8_t DEFTOWN_FINDBIGORE = 0;
const std::uint16_t DEFTOWN_NUMGUARDS = 10;
const std::uint16_t DEFTOWN_PARENTREGION = 0;

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion( std::uint16_t region )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor for CTownRegion class
// o------------------------------------------------------------------------------------------------o
CTownRegion::CTownRegion(std::uint16_t region) : regionNum(region), numGuards(DEFTOWN_NUMGUARDS), parentRegion(DEFTOWN_PARENTREGION), musicList(DEFTOWN_MUSICLIST), worldNumber(DEFTOWN_WORLDNUMBER), instanceId(DEFTOWN_INSTANCEID), mayorSerial(DEFTOWN_MAYOR), race(DEFTOWN_RACE), weather(DEFTOWN_WEATHER), goldReserved(DEFTOWN_GOLDRESERVED), visualAppearance(DEFTOWN_VISUALAPPEARANCE), timeSinceGuardsPaid(DEFTOWN_GUARDSPAID), timeSinceTaxedMembers(DEFTOWN_TAXEDMEMBERS), timeToElectionClose(DEFTOWN_ELECTIONCLOSE), timeToNextPoll(DEFTOWN_NEXTPOLL), guardsPurchased(DEFTOWN_GUARDSPURCHASED), resourceCollected(DEFTOWN_RESOURCECOLLECTED), taxedResource(DEFTOWN_TAXEDRESOURCE), taxedAmount(DEFTOWN_TAXEDAMOUNT), health(DEFTOWN_HEALTH), chanceFindBigOre(DEFTOWN_FINDBIGORE), jsScript(DEFTOWN_JSSCRIPT) {
    
    priv.reset();
    townMember.resize(0);
    alliedTowns.resize(0);
    orePreferences.resize(0);
    locations.resize(0);
    name = Dictionary->GetEntry(1117);
    guardowner = Dictionary->GetEntry(1118);
    guardList = "guard";
    goodList.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	~CTownRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deconstructor for CTownRegion class
// o------------------------------------------------------------------------------------------------o
CTownRegion::~CTownRegion() {
    JSEngine->ReleaseObject(IUE_REGION, this);
    
    townMember.resize(0);
    alliedTowns.resize(0);
    orePreferences.resize(0);
    locations.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads town regions from region world files
//|	Notes		-	ss is a script section containing all the data!
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::load(Script *ss) {
    size_t location = 0xFFFFFFFF;
    std::string tag;
    std::string data;
    std::string UTag;
    std::string sect = std::string("TOWNREGION ") + util::ntos(regionNum);
    if (!ss->IsInSection(sect)) // doesn't exist
        return false;
    
    auto target = ss->FindEntry(sect);
    if (target == nullptr)
        return false;
    
    for (const auto &sec : target->collection()) {
        tag = sec->tag;
        data = sec->data;
        UTag = util::upper(tag);
        std::uint32_t duint;
        try {
            duint = static_cast<std::uint32_t>(std::stoul(data, nullptr, 0));
        }
        catch (...) {
            duint = 0;
        }
        switch ((UTag.data()[0])) {
            case 'A':
                if (UTag == "ALLYTOWN") {
                    alliedTowns.push_back(static_cast<std::uint8_t>(duint));
                }
                break;
            case 'E':
                if (UTag == "ELECTIONTIME") {
                    timeToElectionClose = static_cast<std::int32_t>(duint);
                }
                break;
            case 'G':
                if (UTag == "GUARDOWNER") {
                    guardowner = data;
                }
                else if (UTag == "GUARD") // load our purchased guard
                {
                    ++numGuards;
                }
                else if (UTag == "GUARDSBOUGHT") // num guards bought
                {
                    guardsPurchased = static_cast<std::int16_t>(duint);
                }
                break;
            case 'H':
                if (UTag == "HEALTH") {
                    health = static_cast<std::int16_t>(duint);
                }
                break;
            case 'M':
                if (UTag == "MEMBER") {
                    location = townMember.size();
                    townMember.resize(location + 1);
                    townMember[location].targVote = INVALIDSERIAL;
                    townMember[location].townMember = duint;
                }
                else if (UTag == "MAYOR") {
                    mayorSerial = duint;
                }
                break;
            case 'N':
                if (UTag == "NAME") {
                    name = data.substr(0, 49);
                }
                else if (UTag == "NUMGUARDS") {
                    numGuards = static_cast<std::uint16_t>(duint);
                }
                break;
            case 'P':
                if (UTag == "PARENTREGION") {
                    if (parentRegion > 0) {
                        parentRegion = static_cast<std::uint16_t>(duint);
                        IsSubRegion(true);
                    }
                }
                else if (UTag == "PRIV") {
                    // Overwrites privs loaded from regions.dfn, making it impossible to change privs
                    // after initial server startup Currently not used for anything, so we disable until
                    // we find a better solution.
                    // priv = std::bitset<10>( data.toUShort() );
                }
                else if (UTag == "POLLTIME") {
                    timeToNextPoll = static_cast<std::int32_t>(duint);
                }
                break;
            case 'R':
                if (UTag == "RACE") {
                    race = static_cast<std::uint16_t>(duint);
                }
                else if (UTag == "RESOURCEAMOUNT") {
                    goldReserved = static_cast<std::int32_t>(duint);
                }
                else if (UTag == "RESOURCECOLLECTED") {
                    resourceCollected = static_cast<std::int32_t>(duint);
                }
                break;
            case 'T':
                if (UTag == "TAXEDID") {
                    taxedResource = static_cast<std::uint16_t>(duint);
                }
                else if (UTag == "TAXEDAMOUNT") {
                    taxedAmount = static_cast<std::uint16_t>(duint);
                }
                else if (UTag == "TIMET") {
                    timeSinceTaxedMembers = static_cast<std::int32_t>(duint);
                }
                else if (UTag == "TIMEG") {
                    timeSinceGuardsPaid = static_cast<std::int32_t>(duint);
                }
                break;
            case 'V':
                if (UTag == "VOTE" && location != 0xFFFFFFFF) {
                    townMember[location].targVote = duint;
                }
                break;
        }
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::Save()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves town region data to worldfiles
//|	Notes		-	entry is the region #, fp is the file to save in
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::Save(std::ostream &outStream) {
    const char newLine = '\n';
    outStream << "[TOWNREGION " + std::to_string(regionNum) + "]\n{\n";
    outStream << "RACE=" + std::to_string(race) + newLine;
    outStream << "GUARDOWNER=" + guardowner + newLine;
    outStream << "MAYOR=0x" << std::hex << mayorSerial << std::dec << newLine;
    outStream << "PRIV=" + std::to_string(priv.to_ulong()) + newLine;
    outStream << "RESOURCEAMOUNT=" + std::to_string(goldReserved) + newLine;
    outStream << "TAXEDID=0x" << std::hex << taxedResource << std::dec << newLine;
    outStream << "TAXEDAMOUNT=" + std::to_string(taxedAmount) + newLine;
    outStream << "GUARDSPURCHASED=" + std::to_string(guardsPurchased) + newLine;
    outStream << "TIMEG=" + std::to_string(timeSinceGuardsPaid) + newLine;
    outStream << "TIMET=" + std::to_string(timeSinceTaxedMembers) + newLine;
    outStream << "RESOURCECOLLECTED=" + std::to_string(resourceCollected) + newLine;
    outStream << "HEALTH=" + std::to_string(health) + newLine;
    outStream << "ELECTIONTIME=" + std::to_string(timeToElectionClose) + newLine;
    outStream << "POLLTIME=" + std::to_string(timeToNextPoll) + newLine;
    outStream << "WORLD=" + std::to_string(worldNumber) + newLine;
    outStream << "INSTANCEID=" + std::to_string(instanceId) + newLine;
    outStream << "PARENTREGION=" + std::to_string(parentRegion) + newLine;
    outStream << "NUMGUARDS=" + std::to_string(numGuards) + newLine;
    
    std::vector<TownPers_st>::const_iterator mIter;
    for (mIter = townMember.begin(); mIter != townMember.end(); ++mIter) {
        outStream << "MEMBER=0x" << std::hex << (*mIter).townMember << std::dec << newLine;
        outStream << "VOTE=0x" << std::hex << (*mIter).targVote << std::dec << newLine;
    }
    std::vector<std::uint16_t>::const_iterator aIter;
    for (aIter = alliedTowns.begin(); aIter != alliedTowns.end(); ++aIter) {
        outStream << "ALLYTOWN=" << static_cast<std::uint16_t>((*aIter)) << newLine;
    }
    outStream << "}\n\n";
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CalcNewMayor()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculates new town mayor based on votes
//|	Notes		-	There has got to be a better way than this hideous O(n^2) algy
// o------------------------------------------------------------------------------------------------o
void CTownRegion::CalcNewMayor() {
    if (townMember.empty())
        return;
    
    // if there are no members, there are no new mayors
    std::vector<std::int32_t> votes;
    votes.resize(townMember.size());
    for (size_t counter = 0; counter < votes.size(); ++counter) {
        votes[counter] = 0; // init the count before adding
        for (size_t counter2 = 0; counter2 < votes.size(); ++counter2) {
            if (townMember[counter].townMember == townMember[counter2].targVote) {
                ++votes[counter];
            }
        }
    }
    size_t maxIndex = 0;
    for (size_t indexCounter = 1; indexCounter < votes.size(); ++indexCounter) {
        if (votes[indexCounter] > votes[maxIndex]) {
            maxIndex = indexCounter;
        }
    }
    // maxIndex is now our new mayor!
    CChar *oldMayor = GetMayor();
    if (ValidateObject(oldMayor) && mayorSerial == townMember[maxIndex].townMember) {
        CSocket *targSock = oldMayor->GetSocket();
        if (targSock != nullptr) {
            targSock->SysMessage(1119); // Welcome back for another term, Mr Mayor. // "Mr"?
        }
        // welcome the mayor back for another term
    }
    if (votes[maxIndex] > 0) {
        mayorSerial = townMember[maxIndex].townMember;
    }
    else {
        mayorSerial = 0xFFFFFFF;
    }
    
    CChar *newMayor = GetMayor();
    if (ValidateObject(oldMayor)) {
        oldMayor->SetTownpriv(1);
    }
    if (ValidateObject(newMayor)) {
        newMayor->SetTownpriv(2); // set mayor priv last
    }
    if (newMayor != oldMayor) {
        const bool oldMayorExists = ValidateObject(oldMayor);
        const bool newMayorExists = ValidateObject(newMayor);
        if (!oldMayorExists && newMayorExists) {
            TellMembers(1120, newMayor->GetName().c_str()); // Your new mayor is %s.
        }
        else if (oldMayorExists && !newMayorExists) {
            TellMembers(1121); // Your old mayor has retired and there is no new one.
        }
        else if (oldMayorExists && newMayorExists) {
            TellMembers(1122, oldMayor->GetName().c_str(),
                        newMayor->GetName()
                        .c_str()); // Your old mayor, %s, has retired and your new mayor is %s.
        }
        else {
            TellMembers(1123); // There's still no mayor for this town.
        }
    }
    else if (ValidateObject(newMayor)) {
        TellMembers(
                    1124,
                    oldMayor->GetName().c_str()); // Your old mayor, %s, has stayed on for another term.
    }
    else {
        TellMembers(1123); // There's still no mayor for this town.
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetMayor()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the character who is town mayor
// o------------------------------------------------------------------------------------------------o
CChar *CTownRegion::GetMayor() { return CalcCharObjFromSer(mayorSerial); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetMayorSerial()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the serial of the character who is town mayor
// o------------------------------------------------------------------------------------------------o
serial_t CTownRegion::GetMayorSerial() const { return mayorSerial; }
void CTownRegion::SetMayorSerial(serial_t newValue) { mayorSerial = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::AddAsTownMember()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds specified character to the town's member list
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::AddAsTownMember(CChar &toAdd) {
    if (Races->CompareByRace(toAdd.GetRace(), race) <= RACE_ENEMY) // if we're racial enemies
        return false; // we can't have a racial enemy in the town!
    
    for (std::uint32_t counter = 0; counter < townMember.size(); ++counter) { // exhaustive error checking
        if (townMember[counter].townMember == toAdd.GetSerial())
            return false; // already exists in our town!
    }
    townMember.resize(townMember.size() + 1);
    townMember[townMember.size() - 1].townMember = toAdd.GetSerial();
    townMember[townMember.size() - 1].targVote = INVALIDSERIAL;
    townMember[townMember.size() - 1].PlayerBank = nullptr;
    toAdd.SetTown(regionNum);
    toAdd.SetTownpriv(1); // set as resident
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::RemoveTownMember()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes specified character from the town's member list
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::RemoveTownMember(CChar &toAdd) {
    if (toAdd.GetTown() == 0 || toAdd.GetTown() != regionNum)
        return false; // not in our town
    
    for (size_t counter = 0; counter < townMember.size(); ++counter) {
        if (toAdd.GetSerial() == townMember[counter].townMember) {
            RemoveCharacter(counter);
            toAdd.SetTown(255); // don't know what default town is, now it's 255 :>
            toAdd.SetTownpriv(0);
            return true;
        }
    }
    return false; // we're not in our town
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::oreSkillComparator()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sorts list of ore preferences for the town region in descending
// order based on |					minimum skill required to mine said ore
// o------------------------------------------------------------------------------------------------o
bool oreSkillComparator(OrePref_st o1, OrePref_st o2) {
    if (o1.oreIndex == nullptr)
        return false;
    
    if (o2.oreIndex == nullptr)
        return true;
    
    return o1.oreIndex->minSkill > o2.oreIndex->minSkill;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::InitFromScript()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes town region from associated script section
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::InitFromScript(CScriptSection *toScan) {
    std::string tag;
    std::string data;
    std::string UTag;
    std::int32_t actgood = -1;
    bool orePrefLoaded = false;
    
    // Some default values
    numGuards = 10;
    chanceFindBigOre = 80;
    CanTeleport(true);
    CanPlaceHouse(false);
    
    RegLocs_st ourLoc;
    for (const auto &sec : toScan->collection()) {
        tag = sec->tag;
        data = sec->data;
        UTag = util::upper(tag);
        std::uint32_t duint;
        try {
            duint = static_cast<std::uint32_t>(std::stoul(data, nullptr, 0));
        }
        catch (...) {
            duint = 0;
        }
        switch ((UTag.data()[0])) {
            case 'A':
                if (UTag == "ABWEATH") {
                    weather = static_cast<std::uint8_t>(duint);
                }
                else if (UTag == "APPEARANCE") {
                    visualAppearance = WRLD_COUNT;
                    for (worldtype_t wt = WRLD_SPRING; wt < WRLD_COUNT;
                         wt = static_cast<worldtype_t>(wt + static_cast<worldtype_t>(1))) {
                        if (data == WorldTypeNames[wt]) {
                            visualAppearance = wt;
                            break;
                        }
                    }
                    if (visualAppearance == WRLD_COUNT) {
                        visualAppearance = WRLD_UNKNOWN;
                    }
                }
                break;
            case 'B':
                if (UTag == "BUYABLE") {
                    if (actgood > -1) {
                        goodList[actgood].buyVal = static_cast<std::int32_t>(duint);
                    }
                    else {
                        Console::shared().error("regions dfn -> You must write BUYABLE after GOOD <num>!");
                    }
                }
                break;
            case 'C':
                if (UTag == "CHANCEFORBIGORE") {
                    chanceFindBigOre = static_cast<std::uint8_t>(duint);
                }
                break;
            case 'D':
                if (UTag == "DUNGEON") {
                    IsDungeon((duint == 1));
                }
                break;
            case 'E':
                if (UTag == "ESCORTS") {
                    // Load the region number in the global array of valid escortable regions
                    if (duint == 1) {
                        worldMain.escortRegions.push_back(regionNum);
                    }
                }
                break;
            case 'G':
                if (UTag == "GUARDNUM") {
                    break;
                }
                else if (UTag == "GUARDLIST") {
                    guardList = data;
                }
                else if (UTag == "GUARDOWNER") {
                    guardowner = data;
                }
                else if (UTag == "GUARDED") {
                    IsGuarded((duint == 1));
                }
                else if (UTag == "GATE") {
                    CanGate((duint == 1));
                }
                else if (UTag == "GOOD") {
                    actgood = static_cast<int>(duint);
                }
                break;
            case 'H':
                if (UTag == "HOUSING") {
                    CanPlaceHouse((duint == 1));
                }
                break;
            case 'I':
                if (UTag == "INSTANCEID") {
                    instanceId = static_cast<std::uint16_t>(duint);
                }
                break;
            case 'M':
                if (UTag == "MUSICLIST" || UTag == "MIDILIST") {
                    musicList = static_cast<std::uint16_t>(duint);
                }
                else if (UTag == "MAGICDAMAGE") {
                    CanCastAggressive((duint == 1));
                }
                else if (UTag == "MARK") {
                    CanMark((duint == 1));
                }
                break;
            case 'N':
                if (UTag == "NAME") {
                    name = data;
                    actgood = -1;
                }
                break;
            case 'O':
                if (UTag == "OREPREF") {
                    std::string numPart;
                    std::string oreName;
                    OrePref_st toPush;
                    data = util::simplify(data);
                    oreName = oldstrutil::extractSection(data, ",", 0, 0);
                    toPush.oreIndex = Skills->FindOre(oreName);
                    if (toPush.oreIndex != nullptr) {
                        auto csecs = oldstrutil::sections(data, ",");
                        if (csecs.size() > 1) {
                            // Use chance specified in orepref
                            toPush.percentChance = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                        }
                        else if (toPush.oreIndex->oreChance > 0) {
                            // No chance specified in orepref, use default chance of ore type
                            toPush.percentChance = toPush.oreIndex->oreChance;
                        }
                        else {
                            // Fallback for older skills.dfn setups
                            toPush.percentChance = 1000 / static_cast<std::uint16_t>(Skills->GetNumberOfOres());
                        }
                        
                        orePreferences.push_back(toPush);
                        orePrefLoaded = true;
                    }
                    else {
                        Console::shared().error(util::format("Invalid ore preference in region %i as %s", regionNum, oreName.c_str()));
                    }
                }
                break;
            case 'P':
                if (UTag == "PARENTREGION") {
                    // Region has a parent region! Store reference to parent region...
                    parentRegion = static_cast<std::uint16_t>(duint);
                    
                    // ...and mark this region as a subregion
                    IsSubRegion(true);
                }
                break;
            case 'R':
                if (UTag == "RECALL") {
                    CanRecall((duint == 1));
                }
                else if (UTag == "RANDOMVALUE") {
                    if (actgood > -1) {
                        auto ssecs = oldstrutil::sections(data, " ");
                        if (ssecs.size() > 1) {
                            goodList[actgood].rand1 = std::stoi(util::trim(util::strip(ssecs[0], "//")), nullptr, 0);
                            goodList[actgood].rand2 = std::stoi(util::trim(util::strip(ssecs[1], "//")), nullptr, 0);
                        }
                        else {
                            goodList[actgood].rand1 = static_cast<std::int32_t>(duint);
                            goodList[actgood].rand2 = goodList[actgood].rand1;
                        }
                        if (goodList[actgood].rand2 < goodList[actgood].rand1) {
                            Console::shared().error(util::format(" regions dfn -> You must write RANDOMVALUE NUM2[%i] greater than NUM1[%i].", goodList[actgood].rand2, goodList[actgood].rand1));
                            goodList[actgood].rand2 = goodList[actgood].rand1 = 0;
                        }
                    }
                    else {
                        Console::shared().error(" regions dfn -> You must write RANDOMVALUE after GOOD <num>!");
                    }
                }
                else if (UTag == "RACE") {
                    race = static_cast<std::uint16_t>(duint);
                }
                break;
            case 'S':
                if (UTag == "SAFEZONE") {
                    IsSafeZone((duint == 1));
                }
                else if (UTag == "SELLABLE") {
                    if (actgood > -1) {
                        goodList[actgood].sellVal = static_cast<std::int32_t>(duint);
                    }
                    else {
                        Console::shared().error(" regions dfn -> You must write SELLABLE after GOOD <num>!");
                    }
                }
                else if (UTag == "SPAWN") {
                    std::string sect = "PREDEFINED_SPAWN " + data;
                    CScriptSection *predefSpawn = FileLookup->FindEntry(sect, spawn_def);
                    if (predefSpawn == nullptr) {
                        Console::shared().warning(util::format("Undefined region spawn %s, check your regions.dfn and spawn.dfn files", data.c_str()));
                    }
                    else {
                        for (std::uint16_t i = 0xFFFF; i > 0; --i) {
                            if (worldMain.spawnRegions.find(i) !=
                                worldMain.spawnRegions.end()) {
                                CSpawnRegion *spawnReg = new CSpawnRegion(i);
                                worldMain.spawnRegions[i] = spawnReg;
                                if (spawnReg != nullptr) {
                                    spawnReg->SetX1(locations[locations.size() - 1].x1);
                                    spawnReg->SetY1(locations[locations.size() - 1].y1);
                                    spawnReg->SetX2(locations[locations.size() - 1].x2);
                                    spawnReg->SetY2(locations[locations.size() - 1].y2);
                                    spawnReg->load(predefSpawn);
                                }
                                break;
                            }
                        }
                    }
                }
                else if (UTag == "SCRIPT") {
                    std::uint16_t scriptId = static_cast<std::uint16_t>(duint);
                    if (scriptId != 0) {
                        cScript *toExecute = JSMapping->GetScript(scriptId);
                        if (toExecute == nullptr) {
                            Console::shared().warning(util::format("SCRIPT tag found with invalid script ID (%s) when loading region data!", util::ntos(scriptId).c_str()));
                        }
                        else {
                            this->AddScriptTrigger(scriptId);
                        }
                    }
                }
                break;
            case 'T':
                if (UTag == "TELEPORT") {
                    CanTeleport((duint == 1));
                }
                break;
            case 'W':
                if (UTag == "WORLD") {
                    worldNumber = static_cast<std::uint8_t>(duint);
                }
                break;
            case 'X':
                if (UTag == "X1") {
                    ourLoc.x1 = static_cast<std::uint16_t>(duint);
                }
                else if (UTag == "X2") {
                    ourLoc.x2 = static_cast<std::uint16_t>(duint);
                }
                break;
            case 'Y':
                if (UTag == "Y1") {
                    ourLoc.y1 = static_cast<std::uint16_t>(duint);
                }
                else if (UTag == "Y2") {
                    ourLoc.y2 = static_cast<std::uint16_t>(duint);
                    locations.push_back(ourLoc);
                }
                break;
        }
    }
    
    // No ore preference tags found for this region, apply default chance to find ores
    if (!orePrefLoaded) {
        OrePref_st toLoad;
        size_t numOres = Skills->GetNumberOfOres();
        for (size_t myCounter = 0; myCounter < numOres; ++myCounter) {
            toLoad.oreIndex = Skills->GetOre(myCounter);
            toLoad.percentChance = toLoad.oreIndex->oreChance;
            orePreferences.push_back(toLoad);
        }
    }
    // sort orePreferences in order of descending minSkill
    std::sort(orePreferences.begin(), orePreferences.end(), oreSkillComparator);
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::IsGuarded()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion is protected by guards
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::IsGuarded() const { return priv.test(BIT_GUARDED); }
void CTownRegion::IsGuarded(bool value) { priv.set(BIT_GUARDED, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CanPlaceHouse()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion allows player houses
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::CanPlaceHouse() const { return priv.test(BIT_HOUSING); }
void CTownRegion::CanPlaceHouse(bool value) { priv.set(BIT_HOUSING, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CanMark()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast the Mark spell in the townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::CanMark() const { return priv.test(BIT_MARK); }
void CTownRegion::CanMark(bool value) { priv.set(BIT_MARK, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CanGate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast the Gate spell in the townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::CanGate() const { return priv.test(BIT_GATE); }
void CTownRegion::CanGate(bool value) { priv.set(BIT_GATE, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CanRecall()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast the Recall spell in the
// townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::CanRecall() const { return priv.test(BIT_RECALL); }
void CTownRegion::CanRecall(bool value) { priv.set(BIT_RECALL, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CanCastAggressive()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether players can cast Aggressive spells in the
// townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::CanCastAggressive() const { return priv.test(BIT_AGGRESSIVE); }
void CTownRegion::CanCastAggressive(bool value) { priv.set(BIT_AGGRESSIVE, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::IsSafeZone()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion is considered a safe zone or not
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::IsSafeZone() const { return priv.test(BIT_SAFEZONE); }
void CTownRegion::IsSafeZone(bool value) { priv.set(BIT_SAFEZONE, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::CanTeleport()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the townregion allows use of the Teleport spell or
// not
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::CanTeleport() const { return priv.test(BIT_TELEPORT); }
void CTownRegion::CanTeleport(bool value) { priv.set(BIT_TELEPORT, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetName()
//|					CTownRegion::SetName()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the name of the townregion
// o------------------------------------------------------------------------------------------------o
std::string CTownRegion::GetName() const { return name; }
void CTownRegion::SetName(std::string toSet) { name = toSet.substr(0, 49); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetOwner()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the name of the guard owner for the townregion
// o------------------------------------------------------------------------------------------------o
std::string CTownRegion::GetOwner() const { return guardowner; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetWeather()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the weather in the townregion
// o------------------------------------------------------------------------------------------------o
weathid_t CTownRegion::GetWeather() const { return weather; }
void CTownRegion::SetWeather(weathid_t newValue) { weather = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetGoodSell()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the trade system good sell-value at specified index for the
// townregion
// o------------------------------------------------------------------------------------------------o
std::int32_t CTownRegion::GetGoodSell(std::uint8_t index) const {
    std::int32_t rVal = 0;
    std::map<std::int32_t, GoodData_st>::const_iterator gIter = goodList.find(index);
    if (gIter != goodList.end()) {
        rVal = gIter->second.sellVal;
    }
    return rVal;
}
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetGoodBuy()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the trade system good buy-value at specified index for the
// townregion
// o------------------------------------------------------------------------------------------------o
std::int32_t CTownRegion::GetGoodBuy(std::uint8_t index) const {
    std::int32_t rVal = 0;
    std::map<std::int32_t, GoodData_st>::const_iterator gIter = goodList.find(index);
    if (gIter != goodList.end()) {
        rVal = gIter->second.buyVal;
    }
    return rVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetGoodRnd1()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets minimum random value for trade system good at specified index
// for the townregion
// o------------------------------------------------------------------------------------------------o
std::int32_t CTownRegion::GetGoodRnd1(std::uint8_t index) const {
    std::int32_t rVal = 0;
    std::map<std::int32_t, GoodData_st>::const_iterator gIter = goodList.find(index);
    if (gIter != goodList.end()) {
        rVal = gIter->second.rand1;
    }
    return rVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetGoodRnd2()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets maximum random value for trade system good at specified index
// for the townregion
// o------------------------------------------------------------------------------------------------o
std::int32_t CTownRegion::GetGoodRnd2(std::uint8_t index) const {
    std::int32_t rVal = 0;
    std::map<std::int32_t, GoodData_st>::const_iterator gIter = goodList.find(index);
    if (gIter != goodList.end()) {
        rVal = gIter->second.rand2;
    }
    return rVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetMusicList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets musicList for the townregion, as originally specified in region
// DFNs
// o------------------------------------------------------------------------------------------------o
std::uint16_t CTownRegion::GetMusicList() const { return musicList; }
void CTownRegion::SetMusicList(std::uint16_t newValue) { musicList = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetRandomGuard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns and returns a random NPC guard for the townregion
// o------------------------------------------------------------------------------------------------o
CChar *CTownRegion::GetRandomGuard() {
    CChar *ourGuard = Npcs->CreateRandomNPC(guardList);
    ourGuard->SetNPCAiType(AI_GUARD);
    return ourGuard;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::DisplayTownMenu()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Displays town menu for townstone used by the player
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::DisplayTownMenu(CItem *used, CSocket *sock, std::int8_t flag) {
    if (flag == MAYOR) {
        SendMayorGump(sock);
        return true;
    }
    CChar *tChar = sock->CurrcharObj();
    if (!IsMemberOfTown(tChar)) {
        if (Races->CompareByRace(tChar->GetRace(), race) <= RACE_ENEMY) { // if we're racial enemies
            if (tChar->GetTown() != 255) {
                SendEnemyGump(sock);
            }
            else {
                sock->SysMessage(1125); // ou can not join this town!
            }
        }
        else if (tChar->GetTown() != 255) { // another town person
            if (tChar->GetTown() == regionNum || tChar->GetTown() == 0) { // they think we're in this region!!!
                if (!AddAsTownMember((*tChar))) {
                    sock->SysMessage(1126); // An error occurred joining you to your rightful town,
                    // please call a GM!
                    return false;
                }
                else {
                    DisplayTownMenu(used, sock, flag);
                }
            }
            else {
                SendBasicInfo(sock);
            }
        }
        else {
            SendPotentialMember(sock);
        }
    }
    else {
        SendDefaultGump(sock);
    }
    
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::IsMemberOfTown()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether a specific player is member of the townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::IsMemberOfTown(CChar *player) const {
    if (!ValidateObject(player))
        return false;
    
    for (size_t counter = 0; counter < townMember.size(); ++counter) {
        if (townMember[counter].townMember == player->GetSerial())
            return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendEnemyGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client with a list of the townregion's enemies
// o------------------------------------------------------------------------------------------------o
void CTownRegion::SendEnemyGump(CSocket *sock) {
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(3);
    
    toSend.addCommand("page 0");
    toSend.addCommand(util::format("resizepic 0 0 %u 320 340", ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC] ));
    toSend.addCommand(util::format("button 280 10 %u %i 1 0 1",  ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL], ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1)); // OKAY
    toSend.addCommand(util::format("text 70 10 %u 0", ServerConfig::shared().ushortValues[UShortValue::TITLECOLOR])); // text <Spaces from Left> <Space from top>
    // <Length, Color?> <# in order>
    toSend.addCommand("page 1");
    
    toSend.addText("Enemy");
    toSend.addCommand("gumppic 25 50 1141"); // town name
    toSend.addCommand(util::format("text 35 51 %u 1", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // town name
    toSend.addCommand(util::format("text 25 71 %u 2", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // population
    toSend.addCommand(util::format("text 55 111 %u 3", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // Seize townstone
    toSend.addCommand(util::format("text 55 131 %u 4", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // Destroy townstone
    
    toSend.addCommand(util::format("button 25 111 %u %i 1 0 61", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // seize townstone
    toSend.addCommand(util::format("button 25 131 %u %i 1 0 62", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // destroy townstone
    toSend.addText(util::format("%s (%s)", name.c_str(), Races->Name(race).c_str()));
    toSend.addText(util::format("Population %i", GetPopulation()));
    toSend.addText("Seize Townstone");
    toSend.addText("Attack Townstone");
    
    toSend.Finalize();
    sock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendBasicInfo()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client with basic townstone gump menu
// o------------------------------------------------------------------------------------------------o
void CTownRegion::SendBasicInfo(CSocket *sock) {
    CGumpDisplay BasicGump(sock);
    BasicGump.setTitle("Basic Townstone gump");
    BasicGump.Send(4, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendPotentialMember()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client of a potential townregion member
// o------------------------------------------------------------------------------------------------o
void CTownRegion::SendPotentialMember(CSocket *sock) {
    unicodetypes_t sLang = sock->Language();
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(3);
    
    toSend.addCommand("page 0");
    toSend.addCommand(util::format("resizepic 0 0 %u 320 340", ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC]));
    toSend.addCommand(util::format("button 280 10 %u %i 1 0 1",ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL],ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1)); // OKAY
    toSend.addCommand(util::format("text 70 10 %u 0", ServerConfig::shared().ushortValues[UShortValue::TITLECOLOR])); // text <Spaces from Left> <Space from top>
    // <Length, Color?> <# in order>
    toSend.addCommand("page 1");
    
    toSend.addText("Outsider");              // our title
    toSend.addCommand("gumppic 25 50 1141"); // town name
    toSend.addCommand(util::format("text 35 51 %u 1", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // town name
    toSend.addCommand(util::format("text 25 71 %u 2", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // population
    toSend.addCommand(util::format("text 55 91 %u 3", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // join town
    toSend.addCommand(util::format("text 55 111 %u 4", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // view taxes (to help make decisions about joining?)
    
    toSend.addCommand(util::format("button 25 91 %u %i 1 0 41", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // leave town
    toSend.addCommand(util::format("button 25 111 %u %i 1 0 3", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // view taxes
    
    toSend.addText(util::format("%s (%s)", name.c_str(), Races->Name(race).c_str()));
    
    toSend.addText(util::format(Dictionary->GetEntry(1127, sLang), GetPopulation()));
    toSend.addText(Dictionary->GetEntry(1128, sLang));
    toSend.addText(Dictionary->GetEntry(1129, sLang));
    
    toSend.Finalize();
    sock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendMayorGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends Gump menu to client with townregion mayor info and functions
// o------------------------------------------------------------------------------------------------o
void CTownRegion::SendMayorGump(CSocket *sock) {
    unicodetypes_t sLang = sock->Language();
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(3);
    
    toSend.addCommand("page 0");
    toSend.addCommand(util::format("resizepic 0 0 %u 320 340", ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC]));
    toSend.addCommand(util::format("button 280 10 %u %i 1 0 1", ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL],ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1)); // OKAY
    toSend.addCommand(util::format("text 70 10 %u 0", ServerConfig::shared().ushortValues[UShortValue::TITLECOLOR])); // text <Spaces from Left> <Space from top>
    // <Length, Color?> <# in order>
    toSend.addCommand("page 1");
    
    toSend.addText("Mayor Controls"); // our title
    
    toSend.addCommand("gumppic 25 50 1141"); // town name
    toSend.addCommand("gumppic 25 260 1141");
    toSend.addCommand(util::format("text 35 51 %u 1", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // town name
    toSend.addCommand(util::format("text 25 71 %u 2", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // population
    toSend.addCommand(util::format("text 55 91 %u 3", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // set taxes
    toSend.addCommand(util::format("text 55 280 %u 4", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // return to main menu
    toSend.addCommand(util::format("text 55 111 %u 5", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // list town members
    toSend.addCommand(util::format("text 55 131 %u 6", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // force early election
    toSend.addCommand(util::format("text 55 151 %u 7", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // purchase more guards
    toSend.addCommand(util::format("text 55 171 %u 8", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // fire a guard
    toSend.addCommand(util::format("text 55 261 %u 9", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // treasury amount
    toSend.addCommand(util::format("text 55 191 %u 10", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // make ally
    
    toSend.addCommand(util::format("button 25 91 %u %i 1 0 21", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // set taxes
    toSend.addCommand(util::format("button 25 111 %u %i 1 0 22", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // list town members
    toSend.addCommand(util::format("button 25 131 %u %i 1 0 23", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // force early election
    toSend.addCommand(util::format("button 25 151 %u %i 1 0 24", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // purchase more guards
    toSend.addCommand(util::format("button 25 171 %u %i 1 0 25", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT],  ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // fire a guard
    toSend.addCommand(util::format("button 25 280 %u %i 1 0 40", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT],  ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // return to main menu
    toSend.addCommand(util::format("button 25 191 %u %i 1 0 26", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // make ally of other town
    
    toSend.addText(util::format("%s (%s)", name.c_str(), Races->Name(race).c_str()));
    toSend.addText(util::format(Dictionary->GetEntry(1130, sLang), GetPopulation()));
    toSend.addText(Dictionary->GetEntry(1131, sLang));
    toSend.addText(Dictionary->GetEntry(1132, sLang));
    toSend.addText(Dictionary->GetEntry(1133, sLang));
    toSend.addText(Dictionary->GetEntry(1134, sLang));
    toSend.addText(Dictionary->GetEntry(1135, sLang));
    toSend.addText(Dictionary->GetEntry(1136, sLang));
    toSend.addText(util::format(Dictionary->GetEntry(1137, sLang), goldReserved));
    toSend.addText(Dictionary->GetEntry(1138, sLang));
    
    toSend.Finalize();
    sock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendDefaultGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends default townregion gump menu to client
// o------------------------------------------------------------------------------------------------o
void CTownRegion::SendDefaultGump(CSocket *sock) {
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(3);
    
    toSend.addCommand("page 0");
    toSend.addCommand(util::format("resizepic 0 0 %u 320 340", ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC]));
    toSend.addCommand(util::format("button 280 10 %u %i 1 0 1", ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL],ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1)); // OKAY
    toSend.addCommand(util::format("text 70 10 %u 0",ServerConfig::shared().ushortValues[UShortValue::TITLECOLOR])); // text <Spaces from Left> <Space from top>
    // <Length, Color?> <# in order>
    toSend.addCommand("page 1");
    
    toSend.addText("Generic View");          // our title
    toSend.addCommand("gumppic 25 50 1141"); // town name
    toSend.addCommand(util::format("text 35 51 %u 1",ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // town name
    toSend.addCommand(util::format("text 25 71 %u 2", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // population
    toSend.addCommand(util::format("text 55 91 %u 3",ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // leave town
    toSend.addCommand(util::format("text 55 111 %u 4",ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // view taxes
    toSend.addCommand(util::format("text 55 131 %u 5",ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // toggle town title on/off
    toSend.addCommand(util::format("text 55 151 %u 6", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // vote for mayor
    toSend.addCommand(util::format("text 55 171 %u 7", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // donate resource
    toSend.addCommand(util::format("tilepic 205 171 %u", GetResourceId())); // picture of the resource
    toSend.addCommand(util::format("text 55 191 %u 8", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // view budget
    toSend.addCommand(util::format("text 55 211 %u 9", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // view allied towns
    toSend.addCommand(util::format("text 55 231 %u 10", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // view enemy towns
    
    toSend.addCommand(util::format("button 25 91 %u %i 1 0 2", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // leave town
    toSend.addCommand(util::format("button 25 111 %u %i 1 0 3", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT],  ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // view taxes
    toSend.addCommand(util::format("button 25 131 %u %i 1 0 4", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // toggle title
    toSend.addCommand(util::format("button 25 151 %u %i 1 0 5", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // vote for mayor
    toSend.addCommand(util::format("button 25 171 %u %i 1 0 6", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // donate gold
    toSend.addCommand(util::format("button 25 191 %u %i 1 0 7", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // view budget
    toSend.addCommand(util::format("button 25 211 %u %i 1 0 8", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // view allied towns
    toSend.addCommand(util::format("button 25 231 %u %i 1 0 9", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1)); // view enemy towns
    
    CChar *mChar = sock->CurrcharObj();
    unicodetypes_t sLang = sock->Language();
    toSend.addText(util::format("%s (%s)", name.c_str(), Races->Name(race).c_str()));
    toSend.addText(util::format(Dictionary->GetEntry(1139, sLang), GetPopulation()));
    toSend.addText(Dictionary->GetEntry(1140, sLang));
    toSend.addText(Dictionary->GetEntry(1141, sLang));
    toSend.addText(util::format(Dictionary->GetEntry(1142, sLang), mChar->GetTownTitle() ? "Off" : "On"));
    toSend.addText(Dictionary->GetEntry(1143, sLang));
    toSend.addText(Dictionary->GetEntry(1144, sLang));
    toSend.addText(Dictionary->GetEntry(1145, sLang));
    toSend.addText(Dictionary->GetEntry(1146, sLang));
    toSend.addText(Dictionary->GetEntry(1147, sLang));
    
    if (mChar->GetTownPriv() == 2 || mChar->IsGM()) { // if we've got a mayor (remove isGM check!)
        toSend.addCommand(util::format("button 25 281 %u %i 1 0 20", ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT], ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1));
        toSend.addCommand(util::format("text 55 281 %u 11", ServerConfig::shared().ushortValues[UShortValue::LEFTTEXTCOLOR]));
        toSend.addText(Dictionary->GetEntry(1148, sLang));
    }
    toSend.Finalize();
    sock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetPopulation()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets population of townregion
// o------------------------------------------------------------------------------------------------o
size_t CTownRegion::GetPopulation() const { return townMember.size(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::DisplayTownMembers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets population of townregion
// o------------------------------------------------------------------------------------------------o
void CTownRegion::DisplayTownMembers(CSocket *sock) {
    CGumpDisplay townListing(sock, 300, 300);
    townListing.setTitle(Dictionary->GetEntry(1149, sock->Language()));
    CChar *sChar = sock->CurrcharObj();
    for (size_t counter = 0; counter < townMember.size(); ++counter) {
        CChar *townChar = CalcCharObjFromSer(townMember[counter].townMember);
        if (ValidateObject(townChar)) {
            if (sChar->IsGM()) {
                townListing.AddData(townChar->GetName(), townChar->GetSerial(), 3);
            }
            else {
                townListing.AddData(townChar->GetName(), " "); // don't want them to know the serial
            }
        }
        else {
            RemoveCharacter(counter);
            --counter;
        }
    }
    townListing.Send(4, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetTownMemberSerials()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a comma-separated string of town member serials
// o------------------------------------------------------------------------------------------------o
std::string CTownRegion::GetTownMemberSerials() const {
    std::string townMemberSerials;
    for (size_t counter = 0; counter < townMember.size(); ++counter) {
        CChar const *townMemberChar = CalcCharObjFromSer(townMember[counter].townMember);
        if (ValidateObject(townMemberChar)) {
            if (townMemberSerials.empty()) {
                townMemberSerials += std::to_string(townMember[counter].townMember);
            }
            else {
                townMemberSerials +=
                std::string(",") + std::to_string(townMember[counter].townMember);
            }
        }
    }
    return townMemberSerials;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetTownMembers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a comma-separated string of town member serials
// o------------------------------------------------------------------------------------------------o
std::vector<CTownRegion::TownPers_st> CTownRegion::GetTownMembers() const { return townMember; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::VoteForMayor()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Submits player's vote for mayor
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::VoteForMayor(CSocket *sock) {
    serial_t serial = sock->GetDWord(7);
    CChar *srcChar = sock->CurrcharObj();
    CChar *target = CalcCharObjFromSer(serial);
    if (ValidateObject(target)) {
        if (!IsMemberOfTown(target)) { // he's not in our town!
            sock->SysMessage(1150); // The person you have voted for is not of your town.
            return false;
        }
        serial_t serialPos = FindPositionOf((*srcChar));
        if (serialPos == INVALIDSERIAL) {
            sock->SysMessage(1151); // An error occurred finding you in the town roster!
            return false;
        }
        if (townMember[serialPos].targVote != INVALIDSERIAL) {
            sock->SysMessage(1152); // You have already voted in this election!
            return false;
        }
        townMember[serialPos].targVote = target->GetSerial();
        sock->SysMessage(1153); // Thank you for your vote in this election!
        return true;
    }
    else {
        sock->SysMessage(1154); // That is not a valid person!
        return false;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::FindPositionOf()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find position of specified player in townregion's member list
// o------------------------------------------------------------------------------------------------o
serial_t CTownRegion::FindPositionOf(CChar &toAdd) {
    for (serial_t counter = 0; counter < townMember.size(); ++counter) {
        if (townMember[counter].townMember == toAdd.GetSerial())
            return counter;
    }
    return INVALIDSERIAL;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetResourceId()
//|					CTownRegion::SetResourceId()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ID of resource taxed in townregion
// o------------------------------------------------------------------------------------------------o
std::uint16_t CTownRegion::GetResourceId() const { return taxedResource; }
void CTownRegion::SetResourceId(std::uint16_t resId) { taxedResource = resId; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::DonateResource()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Register player's resource donation for townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::DonateResource(CSocket *s, std::int32_t amount) {
    goldReserved += amount;
    CChar *tChar = s->CurrcharObj();
    if (amount > 1000) {
        s->SysMessage(1155, tChar->GetName().c_str()); // Thank you most kindly %s for your most generous donation!
    }
    else if (amount > 750) {
        s->SysMessage(1156, tChar->GetName().c_str()); // Thank you %s for your significant contribution!
    }
    else if (amount > 500) {
        s->SysMessage(1157, tChar->GetName().c_str()); // Thank you %s for a generous donation!
    }
    else if (amount > 250) {
        s->SysMessage(1158, tChar->GetName().c_str()); // Thank you %s for your donation.
    }
    else {
        s->SysMessage(1159, tChar->GetName().c_str()); // =Thank you %s for your pitiful donation.
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::PurchaseGuard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Purchase a new town guard for the townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::PurchaseGuard(CSocket *sock, std::uint8_t number) {
    if (number * 10000 < goldReserved) { // if we don't have the cash
        sock->SysMessage(1160); // You cannot afford that many guards!
        return false;
    }
    
    goldReserved -= (number * 10000);
    for (std::uint8_t counter = 0; counter < number; ++counter) {
        ++numGuards;
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::ViewBudget()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	View townregion's budget for guards
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::ViewBudget(CSocket *sock) {
    unicodetypes_t sLang = sock->Language();
    CGumpDisplay Budget(sock, 300, 300);
    Budget.setTitle(Dictionary->GetEntry(1161, sLang));                 // Budget
    Budget.AddData(Dictionary->GetEntry(1162, sLang), guardsPurchased); // Guards Bought
    Budget.AddData(Dictionary->GetEntry(1163, sLang), numGuards);       // Guards Total
    Budget.AddData(Dictionary->GetEntry(1164, sLang), numGuards * 20);  // Guard Upkeep
    Budget.Send(4, false, INVALIDSERIAL);
    
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::PeriodicCheck()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Perform periodic check of townregion related functions
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::PeriodicCheck() {
    time_t now;
    time(&now);
    if (difftime(now, timeSinceTaxedMembers) >= ServerConfig::shared().uintValues[UIntValue::TAXPERIOD]) {
        for (size_t memberCounter = 0; memberCounter < townMember.size(); ++memberCounter) {
            CChar *townMem = CalcCharObjFromSer(townMember[memberCounter].townMember);
            if (ValidateObject(townMem)) {
                std::uint16_t resourceId = GetResourceId();
                std::uint32_t numResources = GetItemAmount(townMem, resourceId);
                
                if (taxedAmount > numResources) {
                    std::uint32_t bankAmount = GetBankCount(townMem, resourceId, 0, 0);
                    if (taxedAmount > (numResources + bankAmount)) {
                        JailSys->JailPlayer(townMem, 900);
                    }
                    else {
                        DeleteItemAmount(townMem, numResources, resourceId);
                        DeleteBankItem(townMem, bankAmount - numResources, resourceId);
                    }
                }
                else {
                    DeleteItemAmount(townMem, taxedAmount, resourceId);
                    resourceCollected += taxedAmount;
                }
            }
        }
        timeSinceTaxedMembers = static_cast<std::int32_t>(now);
    }
    if (difftime(now, timeSinceGuardsPaid) >= ServerConfig::shared().uintValues[UIntValue::GUARDPAID]) { // init to 1000 seconds
        if (goldReserved < static_cast<std::int32_t>((20 * numGuards))) {
            // insufficient resources
            bool enoughGuards = false;
            while (!enoughGuards) {
                --numGuards;
                enoughGuards = (goldReserved >= static_cast<std::int32_t>((20 * numGuards)));
            }
        }
        goldReserved -= 20 * numGuards;
        timeSinceGuardsPaid = static_cast<std::int32_t>(now);
    }
    
    if (now > timeToNextPoll && !townMember.empty()) {
        TellMembers(1165); // Time to head to the polling booths again folks, vote for your mayor today!
        for (size_t counter = 0; counter < townMember.size(); ++counter) {
            townMember[counter].targVote = INVALIDSERIAL;
        }
        timeToElectionClose =
        static_cast<std::int32_t>(now) + ServerConfig::shared().uintValues[UIntValue::POLLTIME]; // 2 days polls are open
        timeToNextPoll = timeToElectionClose + ServerConfig::shared().uintValues[UIntValue::MAYORTIME] ; // secs as mayor over the top of the end of the poll
        CChar *mayor = GetMayor();
        if (ValidateObject(mayor)) {
            mayor->SetTownpriv(1); // mayor becomes a basic member again, so he can't do anything
            // while the poll is occuring :>
        }
    }
    
    if (now > timeToElectionClose && !townMember.empty()) { // election finished
        TellMembers(1166); // The voting has finished, thank you for your participation!
        CalcNewMayor();
        timeToElectionClose = timeToNextPoll + 1000; // just so long as it's after the next election
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetAppearance()
//|					CTownRegion::SetAppearance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets visual appearance (season) of townregion
// o------------------------------------------------------------------------------------------------o
worldtype_t CTownRegion::GetAppearance() const { return visualAppearance; }
void CTownRegion::SetAppearance(worldtype_t worldType) { visualAppearance = worldType; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::ViewTaxes()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send Gump menu with information about townregion's taxes
// o------------------------------------------------------------------------------------------------o
void CTownRegion::ViewTaxes(CSocket *sock) {
    CPSendGumpMenu toSend;
    toSend.UserId(INVALIDSERIAL);
    toSend.GumpId(3);
    
    toSend.addCommand("page 0");
    toSend.addCommand(util::format("resizepic 0 0 %u 320 340", ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC]));
    toSend.addCommand(util::format("button 280 10 %u %i 1 0 1",ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL],ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1)); // OKAY
    toSend.addCommand(util::format("text 70 10 %u 0", ServerConfig::shared().ushortValues[UShortValue::TITLECOLOR])); // text <Spaces from Left> <Space from top>
    // <Length, Color?> <# in order>
    toSend.addCommand("page 1");
    
    toSend.addText("Taxes");                 // our title
    toSend.addCommand("gumppic 25 50 1141"); // town name
    toSend.addCommand(util::format("text 35 51 %u 1", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // town name
    toSend.addCommand(util::format("text 35 71 %u 2", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // population
    toSend.addCommand(util::format( "text 35 111 %u 3", ServerConfig::shared().ushortValues[UShortValue::RIGHTTEXTCOLOR])); // # of resources
    toSend.addCommand(util::format("tilepic 5 111 %u", GetResourceId())); // picture of the resource
    
    toSend.addText(util::format("%s (%s)", name.c_str(), Races->Name(race).c_str()));
    toSend.addText(util::format("Population %i", GetPopulation()));
    CTile &tile = Map->SeekTile(GetResourceId());
    toSend.addText(util::format("%i %ss", taxedAmount, tile.Name().c_str()));
    toSend.Finalize();
    sock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetHealth()
//|					vCTownRegion::SetHealth()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the health of townstone in this townregion
// o------------------------------------------------------------------------------------------------o
std::int16_t CTownRegion::GetHealth() const { return health; }
void CTownRegion::SetHealth(std::int16_t newValue) { health = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::DoDamage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deal damage to townregion's townstone and notify online townregion
// members
// o------------------------------------------------------------------------------------------------o
void CTownRegion::DoDamage(std::int16_t reduction) {
    health -= reduction;
    if (health < 0) {
        health = 0;
    }
    if (health == 0) {
        TellMembers(1167); // Your town has been taken over!
    }
    else if (health <= 1000) {
        TellMembers(1168); // Your town is nearing destruction!
    }
    else if (health <= 5000) {
        TellMembers(1169); // Your town is looking decidedly unhealthy!
    }
    else {
        TellMembers(1170); // Your town is under attack!
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::IsAlliedTown()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether another townregion is in the list of allies for the
// townregion
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::IsAlliedTown(std::uint16_t townToCheck) const {
    for (size_t counter = 0; counter < alliedTowns.size(); ++counter) {
        if (alliedTowns[counter] == townToCheck)
            return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::MakeAlliedTown()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds specified townregion to townregion's list of allied townregions
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::MakeAlliedTown(std::uint16_t townToMake) {
    if (regionNum == townToMake)
        return false;
    
    if (IsAlliedTown(townToMake)) { // already allied
        TellMembers(1171); // The mayor attempted to ally you with a town you already belong to.
        return false;
    }
    
    if (Races->CompareByRace(worldMain.townRegions[townToMake]->GetRace(), race) <= RACE_ENEMY) // if we're racial enemies
        return false;
    
    // let's ally ourselves
    alliedTowns.push_back(townToMake);
    TellMembers(1172, name.c_str(), worldMain.townRegions[townToMake]->GetName().c_str()); // Your town of %s has now allied itself with %s.
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::TellMembers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a client system message to all online members of townregion
// o------------------------------------------------------------------------------------------------o
void CTownRegion::TellMembers(std::int32_t dictEntry, ...) {
    for (size_t memberCounter = 0; memberCounter < townMember.size(); ++memberCounter) {
        CChar *targetChar = CalcCharObjFromSer(townMember[memberCounter].townMember);
        if (!ValidateObject(targetChar))
            continue;
        
        CSocket *targetSock = targetChar->GetSocket();
        if (targetSock != nullptr) {
            std::string txt = Dictionary->GetEntry(dictEntry, targetSock->Language());
            std::string msg = "TOWN: ";
            va_list argptr;
            va_start(argptr, dictEntry);
            msg += oldstrutil::format(txt, argptr);
            
            if (ServerConfig::shared().enabled(ServerSwitch::UNICODEMESSAGE)) {
                CPUnicodeMessage unicodeMessage;
                unicodeMessage.Message(msg);
                unicodeMessage.Font(FNT_NORMAL);
                unicodeMessage.Colour(0x000B);
                unicodeMessage.Type(SYSTEM);
                unicodeMessage.Language("ENG");
                unicodeMessage.Name("System");
                unicodeMessage.ID(INVALIDID);
                unicodeMessage.Serial(INVALIDSERIAL);
                targetSock->Send(&unicodeMessage);
            }
            else {
                CSpeechEntry &toAdd = SpeechSys->Add();
                toAdd.Speech(msg);
                toAdd.Font(FNT_NORMAL);
                toAdd.Speaker(INVALIDSERIAL);
                toAdd.SpokenTo(townMember[memberCounter].townMember);
                toAdd.Colour(0x000B);
                toAdd.Type(SYSTEM);
                toAdd.At(worldMain.GetUICurrentTime());
                toAdd.TargType(SPTRG_INDIVIDUAL);
            }
            va_end(argptr);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetRace()
//|					CTownRegion::SetRace()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race ID associated with townregion
// o------------------------------------------------------------------------------------------------o
raceid_t CTownRegion::GetRace() const { return race; }
void CTownRegion::SetRace(raceid_t newRace) { race = newRace; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendAlliedTowns()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends list of townregion's allied townregions to client
// o------------------------------------------------------------------------------------------------o
void CTownRegion::SendAlliedTowns(CSocket *sock) {
    CGumpDisplay Ally(sock, 300, 300);
    
    auto temp = util::format(Dictionary->GetEntry(1173, sock->Language()).c_str(), alliedTowns.size()); // Allied Towns (%i)
    Ally.setTitle(temp);
    for (size_t counter = 0; counter < alliedTowns.size(); ++counter) {
        Ally.AddData(worldMain.townRegions[alliedTowns[counter]]->GetName(), " ");
    }
    
    Ally.Send(4, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::ForceEarlyElection()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces early election for townregion
// o------------------------------------------------------------------------------------------------o
void CTownRegion::ForceEarlyElection() {
    time_t now;
    time(&now);
    CChar *mayor = GetMayor();
    timeToNextPoll = static_cast<std::int32_t>(now); // time to open poll
    TellMembers(1174); // An early election has been forced, please proceed to all polling booths.
    for (size_t counter = 0; counter < townMember.size(); ++counter) {
        townMember[counter].targVote = INVALIDSERIAL;
    }
    if (ValidateObject(mayor)) {
        mayor->SetTownpriv(1);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::SendEnemyTowns()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends list of townregion's enemy townregions to client
// o------------------------------------------------------------------------------------------------o
auto CTownRegion::SendEnemyTowns(CSocket *sock) -> void {
    auto Enemy = CGumpDisplay(sock, 300, 300);
    
    std::uint8_t enemyCount = 0;
    std::for_each(worldMain.townRegions.begin(), worldMain.townRegions.end(), [this, &enemyCount, &Enemy](const std::pair<std::uint16_t, CTownRegion *> &entry) {
        if ((entry.first != regionNum) && (Races->CompareByRace(race, entry.second->GetRace()) <= RACE_ENEMY)) {
            ++enemyCount;
            Enemy.AddData(entry.second->GetName(), Races->Name(entry.second->GetRace()));
        }
    });
    
    Enemy.setTitle(util::format("Enemy Towns (%u)", enemyCount));
    Enemy.Send(4, false, INVALIDSERIAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::Possess()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Another townregion takes possession of the townregion, and clears
// out old |					memberlist for the possessed townregion
// o------------------------------------------------------------------------------------------------o
void CTownRegion::Possess(CTownRegion *possessorTown) {
    possessorTown->SetRace(race);
    possessorTown->TellMembers(1175); // There has been an influx of money due to theft of another town's stone!
    possessorTown->SetReserves(possessorTown->GetReserves() + GetReserves() / 3 * 2);
    possessorTown->SetTaxesLeft(possessorTown->GetTaxes() + GetTaxes() / 3 * 2);
    TellMembers(1176); // The town has been destroyed, beware the guards!
    MakeAlliedTown(possessorTown->GetRegionNum());
    possessorTown->MakeAlliedTown(regionNum);
    
    // remove the old members, preparing the way for the new ones
    CChar *targChar;
    for (size_t counter = townMember.size(); counter-- > 0 && counter < townMember.size();) {
        targChar = CalcCharObjFromSer(townMember[counter].townMember);
        RemoveCharacter(counter);
        if (ValidateObject(targChar)) {
            targChar->SetTown(255);
            targChar->SetTownpriv(0);
        }
    }
    townMember.resize(0);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetReserves()
//|					CTownRegion::SetReserves()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets collected resource reserves for townregion
// o------------------------------------------------------------------------------------------------o
std::uint32_t CTownRegion::GetReserves() const { return resourceCollected; }
void CTownRegion::SetReserves(std::uint32_t newValue) { resourceCollected = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetTaxes()
//|					CTownRegion::SetTaxesLeft()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets collected gold reserves for townregion
// o------------------------------------------------------------------------------------------------o
std::uint32_t CTownRegion::GetTaxes() const { return goldReserved; }
void CTownRegion::SetTaxesLeft(std::uint32_t newValue) { goldReserved = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetChanceBigOre()
//|					CTownRegion::SetChanceBigOre()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets chance to find big ore in the townregion
// o------------------------------------------------------------------------------------------------o
std::uint8_t CTownRegion::GetChanceBigOre() const { return chanceFindBigOre; }
void CTownRegion::SetChanceBigOre(std::uint8_t newValue) { chanceFindBigOre = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::RemoveCharacter()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove character from townregion's member list
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::RemoveCharacter(size_t position) {
    townMember.erase(townMember.begin() + position);
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetNumOrePreferences()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the number of ore preferences present for the townregion
// o------------------------------------------------------------------------------------------------o
size_t CTownRegion::GetNumOrePreferences() const { return orePreferences.size(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetOrePreference()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the townregion's preference for a specified ore
// o------------------------------------------------------------------------------------------------o
const OrePref_st *CTownRegion::GetOrePreference(size_t targValue) const {
    if (targValue >= orePreferences.size())
        return nullptr;
    
    return &orePreferences[targValue];
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetOreChance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the chance to find ore when mining in the townregion
// o------------------------------------------------------------------------------------------------o
std::int32_t CTownRegion::GetOreChance() const {
    std::int32_t sumReturn = 0;
    std::vector<OrePref_st>::const_iterator oIter;
    for (oIter = orePreferences.begin(); oIter != orePreferences.end(); ++oIter) {
        sumReturn += (*oIter).percentChance;
    }
    return sumReturn;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::IsDungeon()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether townregion is a dungeon
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::IsDungeon() const { return priv.test(BIT_DUNGEON); }
void CTownRegion::IsDungeon(bool value) { priv.set(BIT_DUNGEON, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::IsSubRegion()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether townregion is subregion of another region
// o------------------------------------------------------------------------------------------------o
bool CTownRegion::IsSubRegion() const { return priv.test(BIT_SUBREGION); }
void CTownRegion::IsSubRegion(bool value) { priv.set(BIT_SUBREGION, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::NumGuards()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the number of guards in the townregion
// o------------------------------------------------------------------------------------------------o
std::uint16_t CTownRegion::NumGuards() const { return numGuards; }
void CTownRegion::SetNumGuards(std::uint16_t newValue) { numGuards = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::WorldNumber()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the worldnumber of the townregion
// o------------------------------------------------------------------------------------------------o
std::uint8_t CTownRegion::WorldNumber() const { return worldNumber; }
void CTownRegion::WorldNumber(std::uint8_t newValue) { worldNumber = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetInstanceId()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the worldnumber of the townregion
// o------------------------------------------------------------------------------------------------o
std::uint16_t CTownRegion::GetInstanceId() const { return instanceId; }
void CTownRegion::SetInstanceId(std::uint16_t newValue) { instanceId = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::TaxedAmount()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the amount of gold taxed from townregion members
// o------------------------------------------------------------------------------------------------o
void CTownRegion::TaxedAmount(std::uint16_t amount) { taxedAmount = amount; }
std::uint16_t CTownRegion::TaxedAmount() const { return taxedAmount; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetScriptTriggers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of script triggers on object
// o------------------------------------------------------------------------------------------------o
std::vector<std::uint16_t> CTownRegion::GetScriptTriggers() { return scriptTriggers; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::AddScriptTrigger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a script trigger to object's list of script triggers
// o------------------------------------------------------------------------------------------------o
void CTownRegion::AddScriptTrigger(std::uint16_t newValue) {
    if (std::find(scriptTriggers.begin(), scriptTriggers.end(), newValue) == scriptTriggers.end()) {
        // Add scriptId to scriptTriggers if not already present
        scriptTriggers.push_back(newValue);
    }
    
    // Sort vector in ascending order, so order in which scripts are evaluated is predictable
    std::sort(scriptTriggers.begin(), scriptTriggers.end());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::RemoveScriptTrigger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a specific script trigger to object's list of script
// triggers
// o------------------------------------------------------------------------------------------------o
void CTownRegion::RemoveScriptTrigger(std::uint16_t newValue) {
    // Remove all elements containing specified script trigger from vector
    scriptTriggers.erase(std::remove(scriptTriggers.begin(), scriptTriggers.end(), newValue),scriptTriggers.end());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::ClearScriptTriggers()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears out all script triggers from object
// o------------------------------------------------------------------------------------------------o
void CTownRegion::ClearScriptTriggers() {
    scriptTriggers.clear();
    scriptTriggers.shrink_to_fit();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetRegionNum()
//|					CTownRegion::SetRegionNum()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets region number (from region DFNs) for the townregion
// o------------------------------------------------------------------------------------------------o
std::uint16_t CTownRegion::GetRegionNum() const { return regionNum; }
void CTownRegion::SetRegionNum(std::uint16_t newVal) { regionNum = newVal; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetNumLocations()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets number of locations in townregion as defined in region DFNs via
// X1/Y1, X2/Y2 pairs
// o------------------------------------------------------------------------------------------------o
size_t CTownRegion::GetNumLocations() const { return locations.size(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CTownRegion::GetLocation()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets specified location in townregion
// o------------------------------------------------------------------------------------------------o
const RegLocs_st *CTownRegion::GetLocation(size_t locNum) const {
    if (locNum >= locations.size())
        return nullptr;
    
    return &locations[locNum];
}
