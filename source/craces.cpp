
#include "craces.h"

#include <unordered_set>

#include "cchar.h"
#include "ceffects.h"
#include "citem.h"
#include "cjsengine.h"
#include "classes.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "subsystem/console.hpp"

#include "funcdecl.h"

#include "power.h"

#include "ssection.h"
#include "utility/strutil.hpp"

#include "worldmain.h"

cRaces *Races = nullptr;

const std::uint8_t MALE = 1;
const std::uint8_t FEMALE = 2;

// const RACEREL MAX_ENEMY = -100;
// const RACEREL MAX_ALLY  = 100;

const std::uint32_t BIT_REQBEARD = 0;
const std::uint32_t BIT_NOBEARD = 1;
const std::uint32_t BIT_PLAYERRACE = 2;
const std::uint32_t BIT_NOHAIR = 3;

bool cRaces::InvalidRace(raceid_t x) const { return (x >= races.size()); }

const std::string cRaces::Name(raceid_t race) const {
    return races[race]->Name();
}

CRace *cRaces::Race(raceid_t x) {
    if (InvalidRace(x))
        return nullptr;
    
    return races[x];
}

cRaces::cRaces() : initialized(false) {}
cRaces::~cRaces() {
    if (/* DISABLES CODE */ (false)) {
        // if (initialized){
        JSEngine->ReleaseObject(IUE_RACE, this);
        
        for (size_t i = 0; i < races.size(); ++i) {
            delete races[i];
            races[i] = nullptr;
        }
        races.clear();
    }
}

void cRaces::DefaultInitCombat() {
    combat.resize(4);
    combat[0].value = 0;
    combat[1].value = 50;
    combat[2].value = 100;
    combat[3].value = 200;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load race details from races.dfn
// o------------------------------------------------------------------------------------------------o
void cRaces::load() {
    initialized = true;
    std::uint32_t i = 0;
    std::uint32_t raceCount = 0;
    bool done = false;
    
    std::string sect;
    std::string tag;
    std::string data;
    
    while (!done) {
        sect = std::string("RACE ") + util::ntos(raceCount);
        CScriptSection *tempSect = FileLookup->FindEntry(sect, race_def);
        if (tempSect == nullptr) {
            done = true;
        }
        else {
            ++raceCount;
        }
    }
    
    for (i = 0; i < raceCount; ++i) {
        races.push_back(new CRace(raceCount));
    }
    
    CScriptSection *CombatMods = FileLookup->FindEntry(std::string("COMBAT MODS"), race_def);
    if (CombatMods != nullptr) {
        tag = CombatMods->First();
        if (tag.empty()) // location didn't exist!!!
        {
            DefaultInitCombat();
        }
        else {
            if (util::upper(tag) != "MODCOUNT") {
                Console::shared().error("MODCOUNT must come before any entries!");
                DefaultInitCombat();
            }
            else {
                std::uint32_t modifierCount =
                static_cast<std::uint32_t>(std::stoul(CombatMods->GrabData(), nullptr, 0));
                if (modifierCount < 4) {
                    Console::shared().warning(
                                              "MODCOUNT must be more >= 4, or it uses the defaults!");
                    DefaultInitCombat();
                }
                else {
                    combat.resize(modifierCount);
                    for (i = 0; i < modifierCount; ++i) {
                        tag = CombatMods->Next();
                        data = CombatMods->GrabData();
                        if (!data.empty()) {
                            combat[i].value = static_cast<std::uint8_t>(std::stoul(data, nullptr, 0));
                        }
                        else {
                            combat[i].value = 100;
                        }
                    }
                }
            }
        }
    }
    else {
        DefaultInitCombat();
    }
    for (size_t er = 0; er < raceCount; ++er) {
        races[er]->load(er, static_cast<std::int32_t>(combat.size()));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Compare()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compare and return the relation between the races of two characters
//|	Notes		-	Returns 0 if no enemy or ally, -1 if enemy or 1 if ally
// o------------------------------------------------------------------------------------------------o
RaceRelate cRaces::Compare(CChar *player1, CChar *player2) const {
    if (!ValidateObject(player1) || !ValidateObject(player2))
        return RACE_NEUTRAL;
    
    raceid_t r1 = player1->GetRace();
    raceid_t r2 = player2->GetRace();
    if (r1 >= races.size() || r2 >= races.size())
        return RACE_NEUTRAL;
    
    return races[r1]->RaceRelation(r2);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::CompareByRace()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Compare and return the relation between two races
//|	Notes		-	Returns 0 if no enemy or ally, -1 if enemy or 1 if ally
// o------------------------------------------------------------------------------------------------o
RaceRelate cRaces::CompareByRace(raceid_t race1, raceid_t race2) const {
    if (race1 >= races.size()) { // invalid race?
        return RACE_NEUTRAL;
    }
    else if (race2 >= races.size()) { // invalid race?
        return RACE_NEUTRAL;
    }
    else {
        return races[race1]->RaceRelation(race2); // enemy race
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::ApplyRace()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply race (and all the consequences of that) to a character
//|	Notes		-	Primarily used by race gates
// o------------------------------------------------------------------------------------------------o
void cRaces::ApplyRace(CChar *s, raceid_t x, bool always) {
    if (!ValidateObject(s))
        return;
    
    CItem *hairobject = nullptr, *beardobject = nullptr;
    
    CRace *pRace = Race(x);
    if (pRace == nullptr)
        return;
    
    CSocket *mSock = s->GetSocket();
    if (mSock && !pRace->IsPlayerRace()) {
        mSock->SysMessage(369); // This race is not for players!
        return;
    }
    if (s->GetRaceGate() == 65535 || always) {
        std::uint16_t hairColor = 0;
        std::map<std::uint8_t, std::string> lossMap;
        
        lossMap[STRENGTH] = "strength";
        lossMap[DEXTERITY] = "speed";
        lossMap[INTELLECT] = "wisdom";
        
        beardobject = s->GetItemAtLayer(IL_FACIALHAIR);
        hairobject = s->GetItemAtLayer(IL_HAIR);
        if (pRace->GenderRestriction() != 0) {
            if (pRace->GenderRestriction() != FEMALE &&
                (s->GetId() == 0x0191 || s->GetId() == 0x025E || s->GetId() == 0x029B || s->GetId() == 0x02EF || s->GetId() == 0x00B8 || s->GetId() == 0x00BA)) {
                if (mSock) {
                    mSock->SysMessage(370); // You are not of the right gender!
                }
                return;
            }
            if (pRace->GenderRestriction() != MALE && (s->GetId() == 0x0190 || s->GetId() == 0x025D || s->GetId() == 0x029A || s->GetId() == 0x02EE || s->GetId() == 0x00B7 || s->GetId() == 0x00B9)) {
                if (mSock) {
                    mSock->SysMessage(370); // You are not of the right gender!
                }
                return;
            }
        }
        s->SetRaceGate(x);
        s->SetRace(x);
        
        std::int16_t stats[3];
        stats[0] = s->ActualStrength();
        stats[1] = s->ActualDexterity();
        stats[2] = s->ActualIntelligence();
        
        for (std::uint8_t counter = STRENGTH; counter <= INTELLECT; ++counter) {
            if (stats[counter - STRENGTH] > pRace->Skill(counter)) {
                if (mSock) {
                    mSock->SysMessage(371, lossMap[counter].c_str());
                }
                stats[counter - STRENGTH] = pRace->Skill(counter);
            }
            else {
                stats[counter - STRENGTH] = 0;
            }
        }
        if (stats[0] != 0) {
            s->SetStrength(stats[0]);
        }
        if (stats[1] != 0) {
            s->SetDexterity(stats[1]);
        }
        if (stats[2] != 0) {
            s->SetIntelligence(stats[2]);
        }
        
        if (ValidateObject(hairobject)) {
            if (pRace->IsHairRestricted()) {
                hairColor = (hairobject->GetColour());
                if (pRace->IsValidHair(hairColor)) {
                    hairColor = RandomHair(x);
                    hairobject->SetColour(hairColor);
                }
            }
            if (pRace->NoHair()) {
                hairobject->Delete();
                hairobject = nullptr;
            }
        }
        if (pRace->RequiresBeard() && (s->GetId() == 0x0190 || s->GetId() == 0x025D) && !ValidateObject(beardobject)) {
            if (pRace->IsBeardRestricted()) {
                hairColor = RandomBeard(x);
            }
            else {
                hairColor = 0x0480;
            }
            CItem *n = Items->CreateItem(nullptr, s, 0x204C, 1, hairColor, CBaseObject::OT_ITEM);
            if (n != nullptr) {
                n->SetDecayable(false);
                n->SetLayer(IL_FACIALHAIR);
                if (n->SetCont(s)) {
                    beardobject = n;
                }
            }
        }
        if (ValidateObject(beardobject)) {
            if (pRace->IsBeardRestricted()) {
                hairColor = beardobject->GetColour();
                if (pRace->IsValidBeard(hairColor)) {
                    hairColor = RandomBeard(x);
                    beardobject->SetColour(hairColor);
                }
            }
            if (pRace->NoBeard()) {
                beardobject->Delete();
                beardobject = nullptr;
            }
        }
        if (pRace->IsSkinRestricted()) { // do we have a limited skin colour range?
            hairColor = s->GetSkin();
            if (pRace->IsValidSkin(hairColor)) { // if not in range
                hairColor = RandomSkin(x); // get random skin in range
                s->SetSkin(hairColor);
                s->SetOrgSkin(hairColor);
            }
        }
        s->Teleport();
        Effects->PlayStaticAnimation(s, 0x373A, 0, 15);
        Effects->PlaySound(s, 0x01E9);
    }
    else {
        if (mSock) {
            mSock->SysMessage(372); // You have already used a race gate with this character.
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::BeardInRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified colour is a valid beard colour for specified
// race
// o------------------------------------------------------------------------------------------------o
bool cRaces::BeardInRange(colour_t color, raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->IsValidBeard(color);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::SkinInRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified colour is a valid skin colour for specified race
// o------------------------------------------------------------------------------------------------o
bool cRaces::SkinInRange(colour_t color, raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->IsValidSkin(color);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::HairInRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified colour is a valid hair colour for specified race
// o------------------------------------------------------------------------------------------------o
bool cRaces::HairInRange(colour_t color, raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->IsValidHair(color);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Skill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets skill bonus associated with race
// o------------------------------------------------------------------------------------------------o
skillval_t cRaces::Skill(std::int32_t skill, raceid_t race) const {
    if (InvalidRace(race) || skill >= ALLSKILLS)
        return 0;
    
    return races[race]->Skill(skill);
}
void cRaces::Skill(std::int32_t skill, std::int32_t value, raceid_t race) {
    if (InvalidRace(race))
        return;
    
    races[race]->Skill(value, skill);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GenderRestrict()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's gender restriction
//|	Purpose		-	0 = none, 1 = male, 2 = female
// o------------------------------------------------------------------------------------------------o
gender_t cRaces::GenderRestrict(raceid_t race) const {
    if (InvalidRace(race))
        return MALE;
    
    return races[race]->GenderRestriction();
}
void cRaces::GenderRestrict(gender_t gender, raceid_t race) {
    if (InvalidRace(race))
        return;
    
    races[race]->GenderRestriction(gender);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RequireBeard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race members must have a beard
// o------------------------------------------------------------------------------------------------o
bool cRaces::RequireBeard(raceid_t race) const {
    if (InvalidRace(race))
        return false;
    
    return races[race]->RequiresBeard();
}
void cRaces::RequireBeard(bool value, raceid_t race) {
    if (InvalidRace(race))
        return;
    
    races[race]->RequiresBeard(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::ArmorRestrict()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets armor class of race
// o------------------------------------------------------------------------------------------------o
void cRaces::ArmorRestrict(raceid_t race, armorclass_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->ArmourClassRestriction(value);
}
armorclass_t cRaces::ArmorRestrict(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->ArmourClassRestriction();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RandomSkin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random, valid skin colour for specified race
// o------------------------------------------------------------------------------------------------o
colour_t cRaces::RandomSkin(raceid_t x) const {
    if (InvalidRace(x))
        return 0000;
    
    return races[x]->RandomSkin();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RandomHair()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random, valid hair colour for specified race
// o------------------------------------------------------------------------------------------------o
colour_t cRaces::RandomHair(raceid_t x) const {
    if (InvalidRace(x))
        return 0000;
    
    return races[x]->RandomHair();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RandomBeard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a random, valid beard colour for specified race
// o------------------------------------------------------------------------------------------------o
colour_t cRaces::RandomBeard(raceid_t x) const {
    if (InvalidRace(x))
        return 0;
    
    return races[x]->RandomBeard();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::BloodColour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets blood colour for members of specified race
// o------------------------------------------------------------------------------------------------o
colour_t cRaces::BloodColour(raceid_t x) const {
    if (InvalidRace(x))
        return 0000;
    
    return races[x]->BloodColour();
}
void cRaces::BloodColour(raceid_t x, colour_t newValue) {
    if (InvalidRace(x))
        return;
    
    races[x]->BloodColour(newValue);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::BeardRestricted()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if race's beard color is restricted
// o------------------------------------------------------------------------------------------------o
bool cRaces::BeardRestricted(raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->IsBeardRestricted();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::HairRestricted()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if race's hair color is restricted
// o------------------------------------------------------------------------------------------------o
bool cRaces::HairRestricted(raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->IsHairRestricted();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::SkinRestricted()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if race's skin color is restricted
// o------------------------------------------------------------------------------------------------o
bool cRaces::SkinRestricted(raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->IsSkinRestricted();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DamageFromSkill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns bonus percentage modifier for damage based on skill and race
// o------------------------------------------------------------------------------------------------o
std::int32_t cRaces::DamageFromSkill(std::int32_t skill, raceid_t x) const {
    if (InvalidRace(x))
        return 0;
    
    if (skill >= ALLSKILLS)
        return 0;
    
    auto modifier = races[x]->Skill(skill);
    if (modifier >= static_cast<skillval_t>(combat.size())) {
        return -(combat[modifier].value);
    }
    else {
        return (combat[modifier].value);
    }
    return 0;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::FightPercent()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns positive/negative fight damage bonus for race x with skill
// skill
// o------------------------------------------------------------------------------------------------o
std::int32_t cRaces::FightPercent(std::int32_t skill, raceid_t x) const {
    if (InvalidRace(x))
        return 100;
    
    auto modifier = races[x]->Skill(skill);
    std::int32_t divValue = combat[modifier].value / 10;
    divValue = divValue / 10;
    if (divValue == 0)
        return 100;
    
    if (modifier >= static_cast<std::int32_t>(combat.size())) {
        return -static_cast<std::int32_t>(100 / static_cast<float>(divValue));
    }
    else {
        return static_cast<std::int32_t>(100 / static_cast<float>(divValue));
    }
    return 100;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialInfo()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set the relation between two races to a specified value
// o------------------------------------------------------------------------------------------------o
void cRaces::RacialInfo(raceid_t race, raceid_t toSet, RaceRelate value) {
    if (InvalidRace(race))
        return;
    
    races[race]->RaceRelation(value, toSet);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialEnemy()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets up two races as racial enemies
// o------------------------------------------------------------------------------------------------o
void cRaces::RacialEnemy(raceid_t race, raceid_t enemy) {
    if (InvalidRace(race))
        return;
    
    RacialInfo(race, enemy, RACE_ENEMY);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialAlly()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets up two races as racial allies
// o------------------------------------------------------------------------------------------------o
void cRaces::RacialAlly(raceid_t race, raceid_t ally) {
    if (InvalidRace(race))
        return;
    
    RacialInfo(race, ally, RACE_ALLY);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::RacialNeutral()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets up two races as neutral to each other
// o------------------------------------------------------------------------------------------------o
void cRaces::RacialNeutral(raceid_t race, raceid_t neutral) {
    if (InvalidRace(race))
        return;
    
    RacialInfo(race, neutral, RACE_NEUTRAL);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::LanguageMin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum (spirit speak?) skill required to understand
// race's language
// o------------------------------------------------------------------------------------------------o
skillval_t cRaces::LanguageMin(raceid_t x) const { return races[x]->LanguageMin(); }
void cRaces::LanguageMin(skillval_t toSetTo, raceid_t race) {
    if (InvalidRace(race))
        return;
    
    races[race]->LanguageMin(toSetTo);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::LightLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the light level the race burns (takes damage) at
// o------------------------------------------------------------------------------------------------o
void cRaces::LightLevel(raceid_t race, lightlevel_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->LightLevel(value);
}
lightlevel_t cRaces::LightLevel(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->LightLevel();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::ColdLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the cold level the race burns (takes damage) at
// o------------------------------------------------------------------------------------------------o
void cRaces::ColdLevel(raceid_t race, coldlevel_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->ColdLevel(value);
}
coldlevel_t cRaces::ColdLevel(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->ColdLevel();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::HeatLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set the heat level the race burns (takes damage) at
// o------------------------------------------------------------------------------------------------o
void cRaces::HeatLevel(raceid_t race, heatlevel_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->HeatLevel(value);
}
heatlevel_t cRaces::HeatLevel(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->HeatLevel();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DoesHunger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether members of race will grow hungry
// o------------------------------------------------------------------------------------------------o
void cRaces::DoesHunger(raceid_t race, bool value) {
    if (InvalidRace(race))
        return;
    
    races[race]->DoesHunger(value);
}
bool cRaces::DoesHunger(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->DoesHunger();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DoesThirst()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether members of race will grow thirsty
// o------------------------------------------------------------------------------------------------o
void cRaces::DoesThirst(raceid_t race, bool value) {
    if (InvalidRace(race))
        return;
    
    races[race]->DoesThirst(value);
}
bool cRaces::DoesThirst(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->DoesThirst();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetHungerRate()
//|					cRaces::SetHungerRate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set rate at which members of race will grow hungry
// o------------------------------------------------------------------------------------------------o
std::uint16_t cRaces::GetHungerRate(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->GetHungerRate();
}
void cRaces::SetHungerRate(raceid_t race, std::uint16_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->SetHungerRate(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetThirstRate()
//|					cRaces::SetThirstRate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set rate at which members of race will grow thirsty
// o------------------------------------------------------------------------------------------------o
std::uint16_t cRaces::GetThirstRate(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->GetThirstRate();
}
void cRaces::SetThirstRate(raceid_t race, std::uint16_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->SetThirstRate(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetHungerDamage()
//|					cRaces::SetHungerDamage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set damage that members of race will take when hungry
// o------------------------------------------------------------------------------------------------o
std::int16_t cRaces::GetHungerDamage(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->GetHungerDamage();
}
void cRaces::SetHungerDamage(raceid_t race, std::int16_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->SetHungerDamage(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::GetThirstDrain()
//|					cRaces::SetThirstDrain()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set stamina drain that members of race will suffer when thirsty
// o------------------------------------------------------------------------------------------------o
std::int16_t cRaces::GetThirstDrain(raceid_t race) const {
    if (InvalidRace(race))
        return 0;
    
    return races[race]->GetThirstDrain();
}
void cRaces::SetThirstDrain(raceid_t race, std::int16_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->SetThirstDrain(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Affect()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether race is affected by a particular element
// o------------------------------------------------------------------------------------------------o
bool cRaces::Affect(raceid_t race, Weather::type_t element) const {
    bool rValue = false;
    if (!InvalidRace(race)) {
        rValue = races[race]->AffectedBy(element);
    }
    return rValue;
}
void cRaces::Affect(raceid_t race, Weather::type_t element, bool value) {
    if (!InvalidRace(race)) {
        races[race]->AffectedBy(value, element);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Secs()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set number of seconds between burns for race from element
// o------------------------------------------------------------------------------------------------o
seconds_t cRaces::Secs(raceid_t race, Weather::type_t element) const {
    if (InvalidRace(race))
        return 1;
    
    return races[race]->WeatherSeconds(element);
}

void cRaces::Secs(raceid_t race, Weather::type_t element, seconds_t value) {
    if (InvalidRace(race))
        return;
    
    races[race]->WeatherSeconds(value, element);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::Damage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set damage incurred by race from element when they burn
// o------------------------------------------------------------------------------------------------o
std::int8_t cRaces::Damage(raceid_t race, Weather::type_t element) const {
    if (InvalidRace(race))
        return 1;
    
    return races[race]->WeatherDamage(element);
}
void cRaces::Damage(raceid_t race, Weather::type_t element, std::int8_t damage) {
    if (InvalidRace(race))
        return;
    
    races[race]->WeatherDamage(damage, element);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::VisLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set light level bonus for race
// o------------------------------------------------------------------------------------------------o
lightlevel_t cRaces::VisLevel(raceid_t x) const {
    if (InvalidRace(x))
        return 0;
    
    return races[x]->NightVision();
}
void cRaces::VisLevel(raceid_t x, lightlevel_t bonus) {
    if (InvalidRace(x))
        return;
    
    races[x]->NightVision(bonus);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::VisRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set distance that race can see objects at
// o------------------------------------------------------------------------------------------------o
range_t cRaces::VisRange(raceid_t x) const {
    if (InvalidRace(x))
        return 0;
    
    return races[x]->VisibilityRange();
}
void cRaces::VisRange(raceid_t x, range_t range) {
    if (InvalidRace(x))
        return;
    
    races[x]->VisibilityRange(range);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::NoBeard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether race requires members to not have beards
// o------------------------------------------------------------------------------------------------o
bool cRaces::NoBeard(raceid_t x) const {
    if (InvalidRace(x))
        return false;
    
    return races[x]->NoBeard();
}
void cRaces::NoBeard(bool value, raceid_t race) {
    if (InvalidRace(race))
        return;
    
    races[race]->NoBeard(value);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DebugPrint()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump info about a particular race to Console
// o------------------------------------------------------------------------------------------------o
void cRaces::DebugPrint(raceid_t x) {
    if (InvalidRace(x))
        return;
    
    Console::shared() << "Race ID: " << x << myendl;
    Console::shared() << "Race: " << races[x]->Name() << myendl;
    if (races[x]->RequiresBeard()) {
        Console::shared() << "Req Beard: Yes" << myendl;
    }
    else {
        Console::shared() << "Req Beard: No" << myendl;
    }
    if (races[x]->NoBeard()) {
        Console::shared() << "No Beard: Yes" << myendl;
    }
    else {
        Console::shared() << "No Beard: No" << myendl;
    }
    if (races[x]->IsPlayerRace()) {
        Console::shared() << "Player Race: Yes" << myendl;
    }
    else {
        Console::shared() << "Player Race: No" << myendl;
    }
    Console::shared() << "Restrict Gender: " << races[x]->GenderRestriction() << myendl;
    Console::shared() << "LightLevel: " << races[x]->LightLevel() << myendl;
    Console::shared() << "NightVistion: " << races[x]->NightVision() << myendl;
    Console::shared() << "ArmorRestrict: " << races[x]->ArmourClassRestriction() << myendl;
    Console::shared() << "LangMin: " << races[x]->LanguageMin() << myendl;
    Console::shared() << "Vis Distance: " << races[x]->VisibilityRange() << myendl << myendl;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::DebugPrintAll()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump info about all races to Console
// o------------------------------------------------------------------------------------------------o
void cRaces::DebugPrintAll() {
    for (raceid_t x = 0; x < races.size(); ++x) {
        DebugPrint(x);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::IsPlayerRace()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get/Set whether race is a player race or not
// o------------------------------------------------------------------------------------------------o
bool cRaces::IsPlayerRace(raceid_t race) const {
    if (InvalidRace(race))
        return false;
    
    return races[race]->IsPlayerRace();
}
void cRaces::IsPlayerRace(raceid_t x, bool value) {
    if (InvalidRace(x))
        return;
    
    races[x]->IsPlayerRace(value);
}

// o------------------------------------------------------------------------------------------------o
//|	cRace
// o------------------------------------------------------------------------------------------------o

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::Skill()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets skill bonus associated with race (?)
// o------------------------------------------------------------------------------------------------o
skillval_t CRace::Skill(std::int32_t skillNum) const { return iSkills[skillNum]; }
void CRace::Skill(skillval_t newValue, std::int32_t iNum) { iSkills[iNum] = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::Name()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets name of race
// o------------------------------------------------------------------------------------------------o
const std::string CRace::Name() const { return raceName; }
void CRace::Name(const std::string &newName) { raceName = newName; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::RequiresBeard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race requires a beard of its members
// o------------------------------------------------------------------------------------------------o
bool CRace::RequiresBeard() const { return bools.test(BIT_REQBEARD); }
void CRace::RequiresBeard(bool newValue) { bools.set(BIT_REQBEARD, newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::NoBeard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race requires its members to NOT have beards
// o------------------------------------------------------------------------------------------------o
bool CRace::NoBeard() const { return bools.test(BIT_NOBEARD); }
void CRace::NoBeard(bool newValue) { bools.set(BIT_NOBEARD, newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::IsPlayerRace()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race is counted as a player race
// o------------------------------------------------------------------------------------------------o
bool CRace::IsPlayerRace() const { return bools.test(BIT_PLAYERRACE); }
void CRace::IsPlayerRace(bool newValue) { bools.set(BIT_PLAYERRACE, newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::NoHair()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether requires its members to NOT have hair
// o------------------------------------------------------------------------------------------------o
bool CRace::NoHair() const { return bools.test(BIT_NOHAIR); }
void CRace::NoHair(bool newValue) { bools.set(BIT_NOHAIR, newValue); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GenderRestriction()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether race is gender-restricted
// o------------------------------------------------------------------------------------------------o
gender_t CRace::GenderRestriction() const { return restrictGender; }
void CRace::GenderRestriction(gender_t newValue) { restrictGender = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::LightLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the light level at which members of race will burn
// o------------------------------------------------------------------------------------------------o
lightlevel_t CRace::LightLevel() const { return lightLevel; }
void CRace::LightLevel(lightlevel_t newValue) { lightLevel = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::ColdLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the cold level at which members of race will burn
// o------------------------------------------------------------------------------------------------o
coldlevel_t CRace::ColdLevel() const { return coldLevel; }
void CRace::ColdLevel(coldlevel_t newValue) { coldLevel = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::HeatLevel()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the heat level at which members of race will burn
// o------------------------------------------------------------------------------------------------o
heatlevel_t CRace::HeatLevel() const { return heatLevel; }
void CRace::HeatLevel(heatlevel_t newValue) { heatLevel = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::NightVision()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets bonus the race gets to lightlevels
// o------------------------------------------------------------------------------------------------o
lightlevel_t CRace::NightVision() const { return nightVision; }
void CRace::NightVision(lightlevel_t newValue) { nightVision = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::ArmourClassRestriction()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets which armour classes race is restricted from wearing
// o------------------------------------------------------------------------------------------------o
armorclass_t CRace::ArmourClassRestriction() const { return armourRestrict; }
void CRace::ArmourClassRestriction(armorclass_t newValue) { armourRestrict = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::WeatherSeconds()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets interval at which members of a race burn from a given
// weather type
// o------------------------------------------------------------------------------------------------o
seconds_t CRace::WeatherSeconds(Weather::type_t iNum) const { return weathSecs[iNum]; }
void CRace::WeatherSeconds(seconds_t newValue, Weather::type_t iNum) { weathSecs[iNum] = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::WeatherDamage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets how much damage members of race take from a given weather
// type when they burn
// o------------------------------------------------------------------------------------------------o
std::int8_t CRace::WeatherDamage(Weather::type_t iNum) const { return weathDamage[iNum]; }
void CRace::WeatherDamage(std::int8_t newValue, Weather::type_t iNum) { weathDamage[iNum] = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::LanguageMin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets minimum language skill (spirit speak?) required to
// understand race's language
// o------------------------------------------------------------------------------------------------o
skillval_t CRace::LanguageMin() const { return languageMin; }
void CRace::LanguageMin(skillval_t newValue) { languageMin = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::VisibilityRange()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ???
// o------------------------------------------------------------------------------------------------o
range_t CRace::VisibilityRange() const { return visDistance; }
void CRace::VisibilityRange(range_t newValue) { visDistance = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::AffectedBy()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether members of race are affected by a given weather
// type
// o------------------------------------------------------------------------------------------------o
bool CRace::AffectedBy(Weather::type_t iNum) const { return weatherAffected.test(iNum); }
void CRace::AffectedBy(bool value, Weather::type_t iNum) { weatherAffected.set(iNum, value); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetHungerRate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets rate at which members of race grow more hungry
// o------------------------------------------------------------------------------------------------o
std::uint16_t CRace::GetHungerRate() const { return hungerRate; }
void CRace::SetHungerRate(std::uint16_t newValue) { hungerRate = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetThirstRate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets rate at which members of race grow more thirsty
// o------------------------------------------------------------------------------------------------o
std::uint16_t CRace::GetThirstRate() const { return thirstRate; }
void CRace::SetThirstRate(std::uint16_t newValue) { thirstRate = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetHungerDamage()
//|					cRace::SetHungerDamage
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of damage members of race take when hungry
// o------------------------------------------------------------------------------------------------o
std::int16_t CRace::GetHungerDamage() const { return hungerDamage; }
void CRace::SetHungerDamage(std::int16_t newValue) { hungerDamage = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::GetThirstDrain()
//|					cRace::SetThirstDrain
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets amount of stamina drain members of race suffer when
// thirsty
// o------------------------------------------------------------------------------------------------o
std::int16_t CRace::GetThirstDrain() const { return thirstDrain; }
void CRace::SetThirstDrain(std::int16_t newValue) { thirstDrain = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::DoesHunger()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether members of race become hungry
// o------------------------------------------------------------------------------------------------o
bool CRace::DoesHunger() const { return doesHunger; }
void CRace::DoesHunger(bool newValue) { doesHunger = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::DoesThirst()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether members of race become thirsty
// o------------------------------------------------------------------------------------------------o
bool CRace::DoesThirst() const { return doesThirst; }
void CRace::DoesThirst(bool newValue) { doesThirst = newValue; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRaces::CanEquipItem()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether a given race can equip a given item, based on lists
// of allowed |					or banned equipment from dfndata/race/races.dfn
// o------------------------------------------------------------------------------------------------o
bool CRace::CanEquipItem(std::uint16_t itemId) const {
    if (allowedEquipment.size() > 0) {
        // Race has a list of allowed equipment. If it's not in the list, disallow usage
        // return ( std::find( allowedEquipment.begin(), allowedEquipment.end(), itemId ) !=
        // allowedEquipment.end() );
        return (allowedEquipment.find(itemId) != allowedEquipment.end());
    }
    
    if (bannedEquipment.size() > 0) {
        // Race has a list of banned equipment. If item is in the list, disallow usage
        // return !( std::find( bannedEquipment.begin(), bannedEquipment.end(), itemId ) !=
        // bannedEquipment.end() );
        return !(bannedEquipment.find(itemId) != bannedEquipment.end());
    }
    return true;
}

CRace::CRace() : bools(4), visDistance(0), nightVision(0), armourRestrict(0), lightLevel(1), restrictGender(0), languageMin(0), poisonResistance(0.0f), magicResistance(0.0f), bloodColour(0) {
    
    iSkills.fill(0);
    weathDamage.fill(0);
    weathSecs.fill(60);
    // memset( &iSkills[0], 0, sizeof( skillval_t ) * SKILLS );
    // memset( &weathDamage[0], 0, sizeof( std::int8_t ) * Weather::numberweather );
    // memset( &weathSecs[0], 60, sizeof( SECONDS ) * Weather::numberweather );
    
    Skill(100, STRENGTH);
    Skill(100, DEXTERITY);
    Skill(100, INTELLECT);
    HPModifier(0);
    ManaModifier(0);
    StamModifier(0);
    DoesHunger(false);
    SetHungerRate(0);
    SetHungerDamage(0);
    DoesThirst(false);
    SetThirstRate(0);
    SetThirstDrain(0);
}

CRace::CRace(std::int32_t numRaces) : bools(4), visDistance(0), nightVision(0), armourRestrict(0), lightLevel(1), restrictGender(0), languageMin(0), poisonResistance(0.0f), magicResistance(0.0f), bloodColour(0) {
    
    NumEnemyRaces(numRaces);
    
    iSkills.fill(0);
    weathDamage.fill(0);
    weathSecs.fill(60);
    
    Skill(100, STRENGTH);
    Skill(100, DEXTERITY);
    Skill(100, INTELLECT);
    HPModifier(0);
    ManaModifier(0);
    StamModifier(0);
    DoesHunger(false);
    SetHungerRate(0);
    SetHungerDamage(0);
    DoesThirst(false);
    SetThirstRate(0);
    SetThirstDrain(0);
    weatherAffected.reset();
}
void CRace::NumEnemyRaces(std::int32_t iNum) { racialEnemies.resize(iNum); }
RaceRelate CRace::RaceRelation(raceid_t race) const { return racialEnemies[race]; }

colour_t CRace::RandomSkin() const {
    if (!IsSkinRestricted())
        return 0;
    
    size_t sNum = RandomNum(static_cast<size_t>(0), skinColours.size() - 1);
    return static_cast<colour_t>(RandomNum(skinColours[sNum].cMin, skinColours[sNum].cMax));
}
colour_t CRace::RandomHair() const {
    if (!IsHairRestricted())
        return 0;
    
    size_t sNum = RandomNum(static_cast<size_t>(0), hairColours.size() - 1);
    return static_cast<colour_t>(RandomNum(hairColours[sNum].cMin, hairColours[sNum].cMax));
}
colour_t CRace::RandomBeard() const {
    if (!IsBeardRestricted())
        return 0;
    
    size_t sNum = RandomNum(static_cast<size_t>(0), beardColours.size() - 1);
    return static_cast<colour_t>(RandomNum(beardColours[sNum].cMin, beardColours[sNum].cMax));
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cRace::BloodColour()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the blood colour of members of race
// o------------------------------------------------------------------------------------------------o
colour_t CRace::BloodColour() const { return bloodColour; }
void CRace::BloodColour(colour_t newValue) { bloodColour = newValue; }

bool CRace::IsSkinRestricted() const { return (!skinColours.empty()); }
bool CRace::IsHairRestricted() const { return (!hairColours.empty()); }
bool CRace::IsBeardRestricted() const { return (!beardColours.empty()); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::IsValidSkin()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specific colour is valid for skins for this race
// o------------------------------------------------------------------------------------------------o
bool CRace::IsValidSkin(colour_t val) const {
    if (!IsSkinRestricted())
        return true;
    
    for (size_t i = 0; i < skinColours.size(); ++i) {
        if (val >= skinColours[i].cMin && val <= skinColours[i].cMax)
            return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::IsValidHair()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specific colour is valid for hairs for this race
// o------------------------------------------------------------------------------------------------o
bool CRace::IsValidHair(colour_t val) const {
    if (!IsHairRestricted())
        return true;
    
    for (size_t i = 0; i < hairColours.size(); ++i) {
        if (val >= hairColours[i].cMin && val <= hairColours[i].cMax)
            return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::IsValidBeard()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether specific colour is valid for beards for this race
// o------------------------------------------------------------------------------------------------o
bool CRace::IsValidBeard(colour_t val) const {
    if (!IsBeardRestricted())
        return true;
    
    for (size_t i = 0; i < beardColours.size(); ++i) {
        if (val >= beardColours[i].cMin && val <= beardColours[i].cMax)
            return true;
    }
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::RaceRelation()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets race's ractial relations with another race
// o------------------------------------------------------------------------------------------------o
void CRace::RaceRelation(RaceRelate value, raceid_t race) { racialEnemies[race] = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::HPModifier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's hp modifier
// o------------------------------------------------------------------------------------------------o
std::int16_t CRace::HPModifier() const { return HPMod; }
void CRace::HPModifier(std::int16_t value) {
    if (value > -100) {
        HPMod = value;
    }
    else {
        HPMod = -99;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::ManaModifier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's mana modifier
// o------------------------------------------------------------------------------------------------o
std::int16_t CRace::ManaModifier() const { return ManaMod; }
void CRace::ManaModifier(std::int16_t value) {
    if (value > -100) {
        ManaMod = value;
    }
    else {
        ManaMod = -99;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::StamModifier()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's stam modifier
// o------------------------------------------------------------------------------------------------o
std::int16_t CRace::StamModifier() const { return StamMod; }
void CRace::StamModifier(std::int16_t value) {
    if (value > -100) {
        StamMod = value;
    }
    else {
        StamMod = -99;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load details of race from races.dfn
// o------------------------------------------------------------------------------------------------o
void CRace::load(size_t sectNum, std::int32_t modCount) {
    std::string tag;
    std::string data;
    std::string UTag;
    std::int32_t raceDiff = 0;
    std::string sect = std::string("RACE ") + util::ntos(sectNum);
    CScriptSection *RacialPart = FileLookup->FindEntry(sect, race_def);
    if (RacialPart == nullptr)
        return;
    
    colour_t beardMin = 0, skinMin = 0, hairMin = 0;
    
    for (tag = RacialPart->First(); !RacialPart->AtEnd(); tag = RacialPart->Next()) {
        UTag = util::upper(tag);
        data = RacialPart->GrabData();
        
        switch (tag[0]) {
            case 'a':
            case 'A': {
                if (UTag == "ALLOWEQUIPLIST") {
                    // Allowed equipment from [EQUIPMENT #] sections in races.dfn
                    std::string subTag;
                    std::string subUTag;
                    std::string subData;
                    std::string subSect = std::string("EQUIPLIST ") +
                    util::ntos(static_cast<std::uint8_t>(std::stoul(data, nullptr, 0)));
                    CScriptSection *RacialEquipment = FileLookup->FindEntry(subSect, race_def);
                    if (RacialEquipment == nullptr)
                        break;
                    
                    for (subTag = RacialEquipment->First(); !RacialEquipment->AtEnd();
                         subTag = RacialEquipment->Next()) {
                        subUTag = util::upper(subTag);
                        subData = RacialEquipment->GrabData();
                        switch (subTag[0]) {
                            case 'i':
                            case 'I':
                                if (subUTag == "ITEMS") {
                                    auto csecs = oldstrutil::sections(subData, ",");
                                    for (size_t i = 0; i < csecs.size() - 1; i++) {
                                        std::uint16_t temp = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[i], "//")), nullptr, 0));
                                        // allowedEquipment.push_back( temp );
                                        allowedEquipment.insert(temp);
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                else if (UTag == "ARMORREST") { // 8 classes, value 0 is all, else it's a bit comparison
                    ArmourClassRestriction(static_cast<std::uint8_t>(std::stoul(data, nullptr, 0)));
                }
                break;
            }
            case 'b':
            case 'B':
                if (UTag == "BANEQUIPLIST") {
                    // Banned equipment from [EQUIPMENT #] sections in races.dfn
                    std::string subTag;
                    std::string subUTag;
                    std::string subData;
                    std::string subSect = std::string("EQUIPLIST ") +
                    util::ntos(static_cast<std::uint8_t>(std::stoul(data, nullptr, 0)));
                    CScriptSection *RacialEquipment = FileLookup->FindEntry(subSect, race_def);
                    if (RacialEquipment == nullptr)
                        break;
                    
                    for (subTag = RacialEquipment->First(); !RacialEquipment->AtEnd();
                         subTag = RacialEquipment->Next()) {
                        subUTag = util::upper(subTag);
                        subData = RacialEquipment->GrabData();
                        switch (subTag[0]) {
                            case 'i':
                            case 'I':
                                if (subUTag == "ITEMS") {
                                    auto csecs = oldstrutil::sections(subData, ",");
                                    for (size_t i = 0; i < csecs.size() - 1; i++) {
                                        std::uint16_t temp = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[i], "//")), nullptr, 0));
                                        // bannedEquipment.push_back( temp );
                                        bannedEquipment.insert(temp);
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                else if (UTag == "BEARDMIN") {
                    beardMin = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                }
                else if (UTag == "BEARDMAX") {
                    beardColours.push_back(ColourPair(beardMin, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0))));
                }
                else if (UTag == "BLOODCOLOUR") {
                    bloodColour = static_cast<colour_t>(std::stoul(data, nullptr, 0));
                }
                break;
                
            case 'c':
            case 'C':
                if (UTag == "COLDAFFECT"){ // are we affected by cold?
                    AffectedBy(true, Weather::COLD);
                }
                else if (UTag == "COLDLEVEL") { // cold level at which to take damage
                    ColdLevel(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                }
                else if (UTag == "COLDDAMAGE") { // how much damage to take from cold
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::COLD);
                }
                else if (UTag == "COLDSECS"){ // how often cold affects in secs
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::COLD);
                }
                break;
                
            case 'd':
            case 'D':
                if (UTag == "DEXCAP") {
                    Skill(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), DEXTERITY);
                }
                break;
                
            case 'g':
            case 'G':
                if (UTag == "GENDER") {
                    auto udata = util::upper(data);
                    if (udata == "MALE") {
                        GenderRestriction(MALE);
                    }
                    else if (udata == "FEMALE") {
                        GenderRestriction(FEMALE);
                    }
                    else {
                        GenderRestriction(MALE);
                    }
                }
                break;
                
            case 'h':
            case 'H':
                if (UTag == "HAIRMIN") {
                    hairMin = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                }
                else if (UTag == "HAIRMAX") {
                    hairColours.push_back(ColourPair(hairMin, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0))));
                }
                else if (UTag == "HEATAFFECT") {
                    // are we affected by light?
                    AffectedBy(true, Weather::HEAT);
                }
                else if (UTag == "HEATDAMAGE") {
                    // how much damage to take from light
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::HEAT);
                }
                else if (UTag == "HEATLEVEL") {
                    // heat level at which to take damage
                    HeatLevel(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                }
                else if (UTag == "HEATSECS") { // how often light affects in secs
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::HEAT);
                }
                else if (UTag == "HPMOD") {
                    // how high percentage of strength is added as bonus hitpoints
                    HPModifier(static_cast<std::int16_t>(std::stoi(data, nullptr, 0)));
                }
                else if (UTag == "HUNGER") {
                    // does race suffer from hunger
                    auto csecs = oldstrutil::sections(data, ",");
                    if (csecs.size() > 1) {
                        SetHungerRate(static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                        SetHungerDamage(static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                    }
                    else {
                        SetHungerRate(0);
                        SetHungerDamage(0);
                    }
                    if (GetHungerRate() > 0) {
                        DoesHunger(true);
                    }
                    else {
                        DoesHunger(false);
                    }
                }
                break;
                
            case 'i':
            case 'I':
                if (UTag == "INTCAP") {
                    Skill(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), INTELLECT);
                }
                break;
                
            case 'l':
            case 'L':
                if (UTag == "LIGHTAFFECT") { // are we affected by light?
                    AffectedBy(true, Weather::LIGHT);
                }
                else if (UTag == "LIGHTDAMAGE") { // how much damage to take from light
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::LIGHT);
                }
                else if (UTag == "LIGHTLEVEL") { // light level at which to take damage
                    LightLevel(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                }
                else if (UTag == "LIGHTSECS"){ // how often light affects in secs
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::LIGHT);
                }
                
                else if (UTag == "LIGHTNINGAFFECT") { // are we affected by light?
                    AffectedBy(true, Weather::LIGHTNING);
                }
                else if (UTag == "LIGHTNINGDAMAGE") { // how much damage to take from light
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::LIGHTNING);
                }
                else if (UTag == "LIGHTNINGCHANCE") { // how big is the chance to get hit by a lightning
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::LIGHTNING);
                }
                else if (UTag == "LANGUAGEMIN") { // set language min
                    LanguageMin(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                }
                break;
                
            case 'm':
            case 'M':
                if (UTag == "MAGICRESISTANCE") { // magic resistance?
                    MagicResistance(std::stof(data));
                }
                else if (UTag == "MANAMOD") { // how high percentage of int to add as bonus mana
                    ManaModifier(static_cast<std::int16_t>(std::stoi(data, nullptr, 0)));
                }
                break;
                
            case 'n':
            case 'N':
                if (UTag == "NAME") {
                    Name(data);
                }
                else if (UTag == "NOBEARD") {
                    NoBeard(true);
                }
                else if (UTag == "NOHAIR") {
                    NoHair(true);
                }
                else if (UTag == "NIGHTVIS") // night vision level... light bonus
                {
                    NightVision(static_cast<std::uint8_t>(std::stoul(data, nullptr, 0)));
                }
                break;
                
            case 'p':
            case 'P':
                if (UTag == "PLAYERRACE") { // is it a player race?
                    IsPlayerRace((static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)) != 0));
                }
                else if (UTag == "POISONRESISTANCE") { // poison resistance?
                    PoisonResistance(std::stof(data));
                }
                else if (UTag == "PARENTRACE") {
                    CRace *pRace = Races->Race(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                    if (pRace != nullptr) {
                        (*this) = (*pRace);
                    }
                }
                break;
                
            case 'r':
            case 'R':
                if (UTag == "REQUIREBEARD") {
                    RequiresBeard(true);
                }
                else if (UTag == "RAINAFFECT") { // are we affected by light?
                    AffectedBy(true, Weather::RAIN);
                }
                else if (UTag == "RAINDAMAGE"){ // how much damage to take from light
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::RAIN);
                }
                else if (UTag == "RAINSECS"){ // how often light affects in secs
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::RAIN);
                }
                else if (UTag == "RACERELATION") {
                    auto ssecs = oldstrutil::sections(data, " ");
                    if (ssecs.size() > 1) {
                        RaceRelation(static_cast<RaceRelate>(std::stoi(util::trim(util::strip(ssecs[1], "//")), nullptr, 0)), static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[0], "//")), nullptr, 0)));
                    }
                }
                else if (UTag == "RACIALENEMY") {
                    raceDiff = std::stoi(data, nullptr, 0);
                    if (raceDiff > static_cast<std::int32_t>(racialEnemies.size())) {
                        Console::shared() << "Error in race " << static_cast<std::uint32_t>(sectNum) << ", invalid enemy race " << raceDiff << myendl;
                    }
                    else {
                        RaceRelation(RACE_ENEMY, static_cast<raceid_t>(raceDiff));
                    }
                }
                else if (UTag == "RACIALAID") {
                    raceDiff = std::stoi(data, nullptr, 0);
                    if (raceDiff > static_cast<std::int32_t>(racialEnemies.size())) {
                        Console::shared() << "Error in race " << static_cast<std::uint32_t>(sectNum) << ", invalid ally race " << raceDiff << myendl;
                    }
                    else {
                        RaceRelation(RACE_ALLY, static_cast<raceid_t>(raceDiff));
                    }
                }
                break;
                
            case 's':
            case 'S':
                if (UTag == "STRCAP") {
                    Skill(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), STRENGTH);
                }
                else if (UTag == "SKINMIN") {
                    skinMin = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                }
                else if (UTag == "SKINMAX") {
                    skinColours.push_back(ColourPair(skinMin, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0))));
                }
                else if (UTag == "SNOWAFFECT") { // are we affected by light?
                    AffectedBy(true, Weather::SNOW);
                }
                else if (UTag == "SNOWDAMAGE"){ // how much damage to take from light
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::SNOW);
                }
                else if (UTag == "SNOWSECS"){ // how often light affects in secs
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::SNOW);
                }
                else if (UTag == "STORMAFFECT"){ // are we affected by storm?
                    AffectedBy(true, Weather::STORM);
                }
                else if (UTag == "STORMDAMAGE"){ // how much damage to take from storm
                    WeatherDamage(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::STORM);
                }
                else if (UTag == "STORMSECS"){ // how often storm affects in secs
                    WeatherSeconds(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), Weather::STORM);
                }
                else if (UTag == "STAMMOD"){ // how high percentage of dex is added as bonus stamina
                    StamModifier(static_cast<std::int16_t>(std::stoi(data, nullptr, 0)));
                }
                break;
            case 't':
            case 'T':
                if (UTag == "THIRST") {
                    // does race suffer from thirst
                    auto csecs = oldstrutil::sections(data, ",");
                    if (csecs.size() > 1) {
                        SetThirstRate(static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0)));
                        SetThirstDrain(static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0)));
                    }
                    else {
                        SetThirstRate(0);
                        SetThirstDrain(0);
                    }
                    if (GetThirstRate() > 0) {
                        DoesThirst(true);
                    }
                    else {
                        DoesThirst(false);
                    }
                }
                break;
                
            case 'v':
            case 'V':
                if (UTag == "VISRANGE") { // set visibility range ... defaults to 18
                    VisibilityRange(static_cast<char>(std::stoi(data, nullptr, 0)));
                }
                break;
        }
        
        for (std::int32_t iCountA = 0; iCountA < ALLSKILLS; ++iCountA) {
            std::string skillthing = cwmWorldState->skill[iCountA].name;
            skillthing += "G";
            if (skillthing == tag) {
                Skill(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), iCountA);
            }
            else {
                skillthing = cwmWorldState->skill[iCountA].name;
                skillthing += "L";
                if (skillthing == tag) {
                    Skill(modCount + static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)), iCountA);
                }
            }
        }
    }
}

CRace::~CRace() {}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::MagicResistance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's magic resistance
// o------------------------------------------------------------------------------------------------o
float CRace::MagicResistance() const { return magicResistance; }
void CRace::MagicResistance(float value) { magicResistance = value; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CRace::PoisonResistance()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets race's posion resistance
// o------------------------------------------------------------------------------------------------o
float CRace::PoisonResistance() const { return poisonResistance; }
void CRace::PoisonResistance(float value) { poisonResistance = value; }

CRace &CRace::operator=(CRace &trgRace) {
    iSkills = trgRace.iSkills;
    // memcpy( iSkills, trgRace.iSkills, sizeof( skillval_t ) * SKILLS );
    raceName = trgRace.raceName;
    
    beardColours.resize(trgRace.beardColours.size());
    for (size_t bCtr = 0; bCtr < beardColours.size(); ++bCtr) {
        beardColours[bCtr].cMax = trgRace.beardColours[bCtr].cMax;
        beardColours[bCtr].cMin = trgRace.beardColours[bCtr].cMin;
    }
    
    hairColours.resize(trgRace.hairColours.size());
    for (size_t hCtr = 0; hCtr < hairColours.size(); ++hCtr) {
        hairColours[hCtr].cMax = trgRace.hairColours[hCtr].cMax;
        hairColours[hCtr].cMin = trgRace.hairColours[hCtr].cMin;
    }
    
    skinColours.resize(trgRace.skinColours.size());
    for (size_t sCtr = 0; sCtr < skinColours.size(); ++sCtr) {
        skinColours[sCtr].cMax = trgRace.skinColours[sCtr].cMax;
        skinColours[sCtr].cMin = trgRace.skinColours[sCtr].cMin;
    }
    
    bools = trgRace.bools;
    restrictGender = trgRace.restrictGender;
    
    racialEnemies.resize(trgRace.racialEnemies.size());
    for (size_t rCtr = 0; rCtr < racialEnemies.size(); ++rCtr) {
        racialEnemies[rCtr] = trgRace.racialEnemies[rCtr];
    }
    
    lightLevel = trgRace.lightLevel;
    nightVision = trgRace.nightVision;
    armourRestrict = trgRace.armourRestrict;
    weathSecs = trgRace.weathSecs;
    weathDamage = trgRace.weathDamage;
    // memcpy( weathSecs, trgRace.weatSec, sizeof( SECONDS ) * Weather::numberweather );
    // memcpy( weathSecs, trgRace.weathDamage, sizeof( std::int8_t ) * Weather::numberweather );
    
    languageMin = trgRace.languageMin;
    visDistance = trgRace.visDistance;
    poisonResistance = trgRace.poisonResistance;
    magicResistance = trgRace.magicResistance;
    bloodColour = trgRace.bloodColour;
    
    return (*this);
}

size_t cRaces::Count() const { return races.size(); }
