#include "speech.h"

#include <fstream>

#include "cchar.h"
#include "ceffects.h"
#include "citem.h"
#include "cjsmapping.h"
#include "commands.h"
#include "cpacketsend.h"
#include "craces.h"
#include "cresponse.h"
#include "cscript.h"
#include "csocket.h"
#include "dictionary.h"
#include "funcdecl.h"
#include "movement.h"

#include "other/uoxversion.hpp"
#include "skills.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ClilocMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a cliloc message to the client, which is displayed as a system
// message
// o------------------------------------------------------------------------------------------------o
void ClilocMessage(CSocket *mSock, SpeechType speechType, std::uint16_t hue, std::uint16_t font, std::uint32_t messageNum,
                   const char *types = "", ...) {
    bool multipleArgs = false;
    std::string argList = "";
    std::string stringVal = "";
    const char *typesPtr = types;

    va_list marker;
    va_start(marker, types);
    while (*typesPtr != '\0') {
        if (*typesPtr == 'i') {
            stringVal = util::ntos(va_arg(marker, std::int32_t));
        }
        else if (*typesPtr == 's') {
            stringVal = va_arg(marker, char *);
        }

        if (!stringVal.empty()) {
            if (!multipleArgs) {
                multipleArgs = true;
                argList = stringVal;
            }
            else {
                argList += util::format("\t%s", stringVal.c_str());
            }
        }
        ++typesPtr;
    }
    va_end(marker);

    CPClilocMessage toSend;
    toSend.Type(speechType);
    toSend.Hue(hue);
    toSend.Font(font);
    toSend.Message(messageNum);
    toSend.ArgumentString(argList);

    mSock->Send(&toSend);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	ClilocMessage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a clilocmessage to the client, which will be displayed as if
// said by srcObj
// o------------------------------------------------------------------------------------------------o
void ClilocMessage(CSocket *mSock, CBaseObject *srcObj, SpeechType speechType, std::uint16_t hue, std::uint16_t font,
                   std::uint32_t messageNum, bool sendAll, const char *types = "", ...) {
    bool multipleArgs = false;
    std::string argList = "";
    std::string stringVal = "";
    const char *typesPtr = types;

    va_list marker;
    va_start(marker, types);
    while (*typesPtr != '\0') {
        if (*typesPtr == 'i') {
            stringVal = util::ntos(va_arg(marker, std::int32_t));
        }
        else if (*typesPtr == 's') {
            stringVal = va_arg(marker, char *);
        }

        if (!stringVal.empty()) {
            if (!multipleArgs) {
                multipleArgs = true;
                argList = stringVal;
            }
            else {
                argList += util::format("\t%s", stringVal.c_str());
            }
        }
        ++typesPtr;
    }
    va_end(marker);

    CPClilocMessage toSend((*srcObj));
    toSend.Type(speechType);
    toSend.Hue(hue);
    toSend.Font(font);
    toSend.Message(messageNum);
    toSend.ArgumentString(argList);

    bool sendSock = (mSock != nullptr);
    if (sendAll) {
        std::uint16_t searchDistance = DIST_SAMESCREEN;
        if (speechType == WHISPER || speechType == ASCIIWHISPER) {
            searchDistance = DIST_SAMETILE;
        }
        else if (speechType == YELL || speechType == ASCIIYELL) {
            searchDistance = DIST_SAMESCREEN * 1.5;
        }
        else if (speechType == EMOTE || speechType == ASCIIEMOTE) {
            searchDistance = DIST_INRANGE;
        }

        for (auto &tmSock : FindNearbyPlayers(srcObj, searchDistance)) {
            if (sendSock && (tmSock == mSock)) {
                sendSock = false;
            }
            tmSock->Send(&toSend);
        }
    }

    if (sendSock) {
        mSock->Send(&toSend);
    }
}

CSpeechQueue *SpeechSys;

std::map<std::string, UnicodeTypes> codeLookup;

void InitializeLookup() {
    for (std::int32_t i = static_cast<std::int32_t>(ZERO); i < static_cast<std::int32_t>(TOTAL_LANGUAGES); ++i) {
        codeLookup[LanguageCodes[static_cast<UnicodeTypes>(i)]] = static_cast<UnicodeTypes>(i);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	FindLanguage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Attempt to find language used by client
// o------------------------------------------------------------------------------------------------o
UnicodeTypes FindLanguage(CSocket *s, std::uint16_t offset) {
    if (s == nullptr)
        return ZERO;

    char langCode[4];
    langCode[0] = s->GetByte(offset);
    langCode[1] = s->GetByte(++offset);
    langCode[2] = s->GetByte(++offset);
    langCode[3] = 0;

    std::string ulangCode = langCode;
    ulangCode = util::upper(ulangCode);

    UnicodeTypes cLang = s->Language();
    if (LanguageCodes[cLang] != ulangCode.c_str()) {
        std::map<std::string, UnicodeTypes>::const_iterator p = codeLookup.find(ulangCode);
        if (p != codeLookup.end()) {
            return p->second;
        }
        else {
            Console::shared().Error(
                util::format("Unknown language type \"%s\". PLEASE report this in the Bugs section "
                             "of the forums at https://www.uox3.org!",
                             ulangCode.c_str()));
        }
    }
    return cLang;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	SysBroadcast()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	This function was adapted to be used with the new code
//|						in the console thread that allows text to be entered
// on |						the console and it be shipped out to all logged in
// players.
//|
//|	Changes		-	  (February 27, 2000)
// o------------------------------------------------------------------------------------------------o
void SysBroadcast(const std::string &txt) {
    if (!txt.empty()) {
        /*if( cwmWorldState->ServerData()->UseUnicodeMessages() )
        {
                Network->pushConn();
                for( CSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock =
        Network->NextSocket() )
                {
                        CChar *mChar = mSock->CurrcharObj();
                        if( ValidateObject( mChar ))
                        {
                                        CPUnicodeMessage unicodeMessage;
                                        unicodeMessage.Message( txt );
                                        unicodeMessage.Font( 0xFFFF );
                                        unicodeMessage.Colour( 0xFFFF );
                                        unicodeMessage.Type( 1 );
                                        unicodeMessage.Language( "ENG" );
                                        unicodeMessage.Name( "System" );
                                        unicodeMessage.ID( 0 );
                                        unicodeMessage.Serial( 0 );
                                        mSock->Send( &unicodeMessage );
                        }
                }
                Network->popConn();
        }
        else
        {*/
        CSpeechEntry &toAdd = SpeechSys->Add();
        toAdd.Speech(txt);
        toAdd.Font(FNT_NORMAL);
        toAdd.Speaker(INVALIDSERIAL);
        toAdd.SpokenTo(INVALIDSERIAL);
        toAdd.Type(SYSTEM);
        toAdd.At(cwmWorldState->GetUICurrentTime());
        toAdd.TargType(SPTRG_BROADCASTPC);
        toAdd.Colour(0x084D);
        toAdd.Font(FNT_BOLD);
        toAdd.SpeakerName("System: ");
        //}
    }
}

bool WhichResponse(CSocket *mSock, CChar *mChar, std::string text, CChar *tChar = nullptr);
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPITalkRequest::Handle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles speech requests from client
//|
//|	Notes		-	Unicode speech format
//|					byte=char, short=char[2], int=char[4], wchar=char[2]=unicode
// character
//|
//|					Message Sent By Client:
//|					0xAD - Unicode Speech Request
//|					std::uint8_t cmd (0xAD)
//|					short msgsize 1,2
//|					byte type (0=say, 2=emote, 8=whisper, 9=yell) 3
//|					short color 4,5
//|					short font 6,7
//|					std::uint8_t[4] lang (null terminated, "enu " for US english.)
// 8,9,10,11 |					wchar[?] text (null terminated, ?=(msgsize-12)/2) 13
//|
//|					Message Sent By Server:
//|					Unicode Speech message(Variable # of bytes)
//|					�	BYTE cmd
// 0 |					�	BYTE[2] blockSize
// 1-2 |					�	BYTE[4] ID
// 3-6 |					�	BYTE[2] Model
// 7-8 |					�	BYTE Type
// 9 |					�	BYTE[2] Color
// 10-11 |					�	BYTE[2] Font
// 12-13 |					�	BYTE[4] Language
// 14-17 (3 byte unicode language, mirrored from client) |					�
// BYTE[30] Name										18-47
// | �	BYTE[?][2] Msg - Null Terminated (blockSize - 48)	48+?
// o------------------------------------------------------------------------------------------------o
bool CPITalkRequest::Handle(void) {
    if (HandleCommon())
        return true;

    CChar *mChar = tSock->CurrcharObj();
    if (!ValidateObject(mChar))
        return true;

    char *asciiText = Text();
    if (strlen(asciiText) == 0)
        return true;

    std::vector<std::uint16_t> scriptTriggers = mChar->GetScriptTriggers();
    for (auto scriptTrig : scriptTriggers) {
        cScript *toExecute = JSMapping->GetScript(scriptTrig);
        if (toExecute != nullptr) {
            // If script returned false/0, prevent hard-code (and other scripts with event) from
            // running
            if (toExecute->OnTalk(mChar, asciiText) == 0) {
                return true;
            }
        }
    }

    if ((asciiText[0] == cwmWorldState->ServerData()->ServerCommandPrefix()) ||
        ((asciiText[0] == '.') && (asciiText[1] != '.'))) {
        Commands->Command(tSock, mChar, &asciiText[1]);
    }
    else {
        if (mChar->IsDead()) {
            if (!mChar->IsAtWar()) {
                mChar->SetWar(true);
                CPWarMode wMode(1);
                tSock->Send(&wMode);
            }
        }

        if ((Type() != WHISPER && Type() != ASCIIWHISPER) &&
            (mChar->GetVisible() == VT_TEMPHIDDEN || mChar->GetVisible() == VT_INVISIBLE)) {
            mChar->ExposeToView();
        }

        if ((Type() == YELL || Type() == ASCIIYELL) && mChar->CanBroadcast()) {
            CSpeechEntry &toAdd = SpeechSys->Add();
            toAdd.Speech(asciiText);
            toAdd.Font(static_cast<FontType>(mChar->GetFontType()));
            toAdd.Speaker(mChar->GetSerial());
            toAdd.SpokenTo(INVALIDSERIAL);
            toAdd.Type(PROMPT);
            toAdd.At(cwmWorldState->GetUICurrentTime());
            toAdd.TargType(SPTRG_BROADCASTPC);
            if (mChar->GetSayColour() == 0x1700) {
                toAdd.Colour(0x5A);
            }
            else if (mChar->GetSayColour() == 0x0) {
                toAdd.Colour(0x5A);
            }
            else {
                toAdd.Colour(mChar->GetSayColour());
            }
            toAdd.Font(FNT_BOLD);

            std::string mCharName = GetNpcDictName(mChar, nullptr, NRS_SPEECH);
            toAdd.SpeakerName(mCharName);
        }
        else {
            std::string text(asciiText);

            if (Type() == 0) {
                mChar->SetSayColour(TextColour());
            }
            if (Type() == 2) {
                mChar->SetEmoteColour(TextColour());
            }
            if (cwmWorldState->ServerData()->ServerSpeechLog() && !mChar->IsNpc()) // Logging
            {
                auto temp = util::format("%s.log", mChar->GetName().c_str());
                auto temp2 =
                    util::format("%s [%x %x %x %x] [%i]: %s\n", mChar->GetName().c_str(),
                                 mChar->GetSerial(1), mChar->GetSerial(2), mChar->GetSerial(3),
                                 mChar->GetSerial(4), mChar->GetAccount().accountNumber, asciiText);
                Console::shared().Log(temp, temp2);
            }

            std::string upperText = util::upper(text);
            if (upperText.find("DEVTEAM033070") != std::string::npos) {
                std::string temp3 = "RBuild: " + UOXVersion::realBuild +
                                    " PBuild: " + UOXVersion::build +
                                    " --> Version: " + UOXVersion::version;
                tSock->SysMessage(temp3.c_str());
            }

            if (!WhichResponse(tSock, mChar, text))
                return true;

            CPUOXBuffer *txtToSend = nullptr;
            CPUOXBuffer *ghostedText = nullptr;
            CPUnicodeSpeech *uniTxtToSend = nullptr;
            CPUnicodeSpeech *uniGhostedText = nullptr;
            CPacketSpeech *asciiTxtToSend = nullptr;
            CPacketSpeech *asciiGhostedText = nullptr;

            if (IsUnicode()) {
                uniTxtToSend = new CPUnicodeSpeech();
                uniTxtToSend->Object(*(static_cast<CPITalkRequestUnicode *>(this)));
                uniTxtToSend->Object(*mChar);
                txtToSend = uniTxtToSend;

                uniGhostedText = new CPUnicodeSpeech();
                (*uniGhostedText) = (*uniTxtToSend);
                uniGhostedText->GhostIt(0);

                ghostedText = uniGhostedText;
            }
            else {
                asciiTxtToSend = new CPacketSpeech(*(static_cast<CPITalkRequestAscii *>(this)));
                asciiTxtToSend->SpeakerSerial(mChar->GetSerial());
                asciiTxtToSend->SpeakerModel(mChar->GetId());

                std::string mCharName = GetNpcDictName(mChar, nullptr, NRS_SPEECH);
                asciiTxtToSend->SpeakerName(mCharName);
                txtToSend = asciiTxtToSend;

                asciiGhostedText = new CPacketSpeech();
                (*asciiGhostedText) = (*asciiTxtToSend);
                asciiGhostedText->GhostIt(0);

                ghostedText = asciiGhostedText;
            }
            tSock->Send(txtToSend);

            std::vector<CSocket *> nearbyChars;
            // Distance at which other players can hear the speech depends on speech-type
            if ((Type() == WHISPER || Type() == ASCIIWHISPER) && !mChar->IsGM() &&
                !mChar->IsCounselor()) {
                nearbyChars = FindNearbyPlayers(mChar, 1);
            }
            else if ((Type() == WHISPER || Type() == ASCIIWHISPER) &&
                     (mChar->IsGM() || mChar->IsCounselor())) {
                nearbyChars = FindNearbyPlayers(mChar, 3);
            }
            else if (Type() == YELL || Type() == ASCIIYELL) {
                nearbyChars = FindNearbyPlayers(mChar, (DIST_SAMESCREEN * 1.5));
            }
            else {
                nearbyChars = FindNearbyPlayers(mChar);
            }
            for (auto &tSock : nearbyChars) {
                CChar *tChar = tSock->CurrcharObj();
                if (mChar != tChar) {
                    // Line of Sight check!
                    if (!tChar->IsGM() &&
                        !LineOfSight(mChar->GetSocket(), tChar, mChar->GetX(), mChar->GetY(),
                                     mChar->GetZ() + 15,
                                     WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING, false, 0, false))
                        continue;

                    if (mChar->IsDead() && tChar->GetCommandLevel() < CL_CNS &&
                        tSock->GetTimer(tPC_SPIRITSPEAK) ==
                            0) // GM/Counselors can see ghosts talking always Seers?
                    {
                        if (mChar->IsDead() &&
                            !tChar->IsDead()) // Ghost can talk normally to other ghosts
                        {
                            tSock->Send(ghostedText);
                        }
                        else {
                            tSock->Send(txtToSend);
                        }
                    }
                    else if (tChar->GetRace() != mChar->GetRace() && !tChar->IsGM() &&
                             !tChar->IsCounselor()) {
                        SKILLVAL raceLangMin = Races->LanguageMin(mChar->GetRace());
                        if (raceLangMin > 0 &&
                            Skills->CheckSkill(tChar, SPIRITSPEAK,
                                               Races->LanguageMin(mChar->GetRace()), 1000) != 1) {
                            tSock->Send(ghostedText);
                        }
                        else {
                            tSock->Send(txtToSend);
                        }
                    }
                    else {
                        std::string mCharName = GetNpcDictName(mChar, nullptr, NRS_SPEECH);

                        if (mChar->GetVisible() == VT_TEMPHIDDEN ||
                            mChar->GetVisible() == VT_INVISIBLE ||
                            mChar->GetVisible() == VT_PERMHIDDEN) {
                            if ((tChar->IsGM() || tChar->IsCounselor()) ||
                                (Type() == WHISPER || Type() == ASCIIWHISPER)) {
                                tSock->SysMessage(1794, mCharName.c_str()); // (Whisper from %s)
                                tSock->Send(txtToSend);
                            }
                        }
                        else {
                            if ((Type() == YELL || Type() == ASCIIYELL) &&
                                !ObjInRange(tChar, mChar, DIST_SAMESCREEN)) {
                                tSock->SysMessage(1795, mCharName.c_str()); // (Yelled by %s)
                                tSock->Send(txtToSend);
                            }
                            else {
                                tSock->Send(txtToSend);
                            }
                        }
                    }
                }
            }

            delete uniTxtToSend;
            delete asciiTxtToSend;
            delete uniGhostedText;
            delete asciiGhostedText;
        }
    }
    return true;
}

CSpeechQueue::CSpeechQueue() : pollTime(100), runAsThread(false) {
    speechList.resize(0);
    // InitializeLookup();
}
auto CSpeechQueue::Startup() -> void { InitializeLookup(); }
CSpeechQueue::~CSpeechQueue() {
    for (SPEECHLIST_ITERATOR slIter = speechList.begin(); slIter != speechList.end(); ++slIter) {
        delete (*slIter);
        (*slIter) = nullptr;
    }
    speechList.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpeechQueue::SayIt()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends out specified speech entry to users
// o------------------------------------------------------------------------------------------------o
void CSpeechQueue::SayIt(CSpeechEntry &toSay) {
    CPacketSpeech toSend = toSay;
    CChar *thisChar = nullptr;
    CItem *thisItem = nullptr;
    CBaseObject *thisObj = nullptr;
    CSocket *thisSock = nullptr;
    if (toSay.Speaker() > BASEITEMSERIAL) {
        thisItem = CalcItemObjFromSer(toSay.Speaker());
        thisObj = thisItem;
    }
    else {
        thisChar = CalcCharObjFromSer(toSay.Speaker());
        thisObj = thisChar;
    }
    CChar *sChar = nullptr;
    CSocket *mSock = nullptr;
    switch (toSay.TargType()) {
    case SPTRG_INDIVIDUAL: // aimed at individual person
        if (ValidateObject(thisChar)) {
            thisSock = thisChar->GetSocket();
            if (thisSock != nullptr) {
                thisSock->Send(&toSend);
            }
        }
        sChar = CalcCharObjFromSer(toSay.SpokenTo());
        if (ValidateObject(sChar) && sChar != thisChar) {
            mSock = sChar->GetSocket();
            if (mSock != nullptr) {
                mSock->Send(&toSend);
            }
        }
        break;
    case SPTRG_PCNPC: // all NPCs and PCs in range
    case SPTRG_PCS:   // all PCs in range
    {
        if (!ValidateObject(thisObj))
            break;

        if (ValidateObject(thisItem) &&
            thisItem->GetCont() != nullptr) // not on ground, can't guarantee speech
            break;

        for (auto &mSock : FindPlayersInVisrange(thisObj)) {
            CChar *mChar = mSock->CurrcharObj();
            if (ValidateObject(mChar)) {
                if (mChar->GetCommandLevel() >= toSay.CmdLevel()) {
                    mSock->Send(&toSend);
                }
            }
        }
        break;
    }
    case SPTRG_BROADCASTPC: // ALL PCs everywhere + NPCs in range
    case SPTRG_BROADCASTALL: {
        for (auto &mSock : Network->connClients) {
            if (mSock) {
                auto mChar = mSock->CurrcharObj();
                if (ValidateObject(mChar)) {
                    if (mChar->GetCommandLevel() >= toSay.CmdLevel()) {
                        mSock->Send(&toSend);
                    }
                }
            }
        }
        break;
    }
    case SPTRG_ONLYRECEIVER: // only the receiver gets the message
        sChar = CalcCharObjFromSer(toSay.SpokenTo());
        if (ValidateObject(sChar) && sChar != thisChar) {
            mSock = sChar->GetSocket();
            if (mSock != nullptr) {
                mSock->Send(&toSend);
            }
        }
        break;
    case SPTRG_NULL:
        break;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	bool InternalPoll( void )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends out any pending speech, returning true if entries were sent
// o------------------------------------------------------------------------------------------------o
bool CSpeechQueue::InternalPoll(void) {
    bool retVal = false;
    CSpeechEntry *toCheck = nullptr;

    SPEECHLIST_ITERATOR slIter = speechList.begin();
    while (slIter != speechList.end()) {
        toCheck = (*slIter);

        if (toCheck->At() == -1 ||
            static_cast<std::uint32_t>(toCheck->At()) <= cwmWorldState->GetUICurrentTime()) {
            retVal = true;
            SayIt((*toCheck));

            delete (*slIter);
            (*slIter) = nullptr;

            slIter = speechList.erase(slIter);
        }
        else {
            ++slIter;
        }
    }
    return retVal;
}

bool CSpeechQueue::Poll(void) {
    if (RunAsThread()) {
        while (RunAsThread()) {
            InternalPoll();
            std::this_thread::sleep_for(std::chrono::milliseconds(
                pollTime)); // so that it's never 0 (and number of milliseconds)
        }
        return true;
    }
    else {
        return InternalPoll();
    }
}

CSpeechEntry &CSpeechQueue::Add(void) // Make space in queue, and return pointer to new entry
{
    size_t iSize = speechList.size();
    speechList.push_back(new CSpeechEntry);
    return (*speechList[iSize]);
}

std::int32_t CSpeechQueue::PollTime(void) const { return pollTime; }

void CSpeechQueue::PollTime(std::int32_t value) { pollTime = value; }

bool CSpeechQueue::RunAsThread(void) const { return runAsThread; }

void CSpeechQueue::RunAsThread(bool value) { runAsThread = value; }

void CSpeechQueue::DumpInFile(void) {
    std::string speechFile = cwmWorldState->ServerData()->Directory(CSDDP_LOGS) + "speechdump.txt";
    std::ofstream speechDestination(speechFile.c_str());
    if (!speechDestination) {
        Console::shared().Error(util::format("Failed to open %s for writing", speechFile.c_str()));
        return;
    }
    SPEECHLIST_ITERATOR toWrite;
    for (toWrite = speechList.begin(); toWrite != speechList.end(); ++toWrite) {
        speechDestination << "Time: " << (*toWrite)->At() << std::endl;
        speechDestination << "nColour: " << (*toWrite)->Colour() << std::endl;
        speechDestination << "nFont: " << (*toWrite)->Font() << std::endl;
        speechDestination << "nLanguage: " << (*toWrite)->Language() << std::endl;
        speechDestination << "nSpeech: " << (*toWrite)->Speech() << std::endl;
        speechDestination << "nSpeaker: " << (*toWrite)->SpeakerName() << std::endl << std::endl;
    }
    speechDestination.close();
}
