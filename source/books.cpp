// o------------------------------------------------------------------------------------------------o
//|	File		-	books.cpp
//|	Date		-	12/05/1999
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writable and Pre-Defined Book Handling
// o------------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|
//|					0.9			12/05/1999
//|					Initial implimentation
//|
//|					1.0			12/07/1999
//|					Wrapped everything in a class, added customizable number of
// max-pages
//|
//|					1.1			12/12/1999
//|					Nasty bug fixed (item # used as "key" instead of serial #) and
//a few very small bugfixes
//|
//|					2.0			5th November, 2005
//|					Rewrote much of the original .bok handling to improve
// performance
//|					Fixed several issues with the fstream handling to ensure
//validity of data from .bok files.
//|					Slimmed down the cBook class making use of packet classes
//for handling data from the client.
//|					Added support for the new book header packet removing support
//for the old book header packet.
// o------------------------------------------------------------------------------------------------o

#include "books.h"

#include <fstream>

#include "cchar.h"
#include "citem.h"
#include "csocket.h"

#include "cpacketsend.h"
#include "cserverdefinitions.h"
#include "funcdecl.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"

#include "osunique.hpp"

using namespace std::string_literals;

extern CBooks worldBook ;
extern CServerDefinitions worldFileLookup ;

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPINewBookHeader::Handle()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the .bok file with changes to the author or title
// o------------------------------------------------------------------------------------------------o
bool CPINewBookHeader::Handle() {
    if (tSock != nullptr) {
        const serial_t bookSer = tSock->GetDWord(3);
        CItem *mBook = CalcItemObjFromSer(bookSer);
        if (!ValidateObject(mBook))
            return true;
        
        auto fileName = ServerConfig::shared().directoryFor(dirlocation_t::BOOK) / std::filesystem::path(util::ntos(bookSer, 16) + ".bok"s);
        
        if (!std::filesystem::exists(fileName)) {
            worldBook.CreateBook(fileName, tSock->CurrcharObj(), mBook);
        }
        
        std::fstream file(fileName.string(), std::ios::in | std::ios::out | std::ios::binary);
        
        if (file.is_open()) { // If it isn't open now, our create and open failed
            
            const size_t titleLen = tSock->GetWord(11);
            const size_t authorLen = tSock->GetWord(13 + titleLen);
            
            char titleBuff[62];
            char authBuff[32];
            
            memset(titleBuff, 0x00, 62);
            memset(authBuff, 0x00, 32);
            
            memcpy(titleBuff, &tSock->Buffer()[13], (titleLen > 61 ? 61 : titleLen));
            memcpy(authBuff, &tSock->Buffer()[15 + titleLen], (authorLen > 31 ? 31 : authorLen));
            
            mBook->SetName(titleBuff);
            
            file.seekp(0, std::ios::beg);
            if (!file.fail()) {
                file.write(titleBuff, 62);
                file.write(authBuff, 32);
                
                if (file.fail()) {
                    Console::shared().error(util::format("Couldn't write to book file %s", fileName.string().c_str()));
                }
            }
            else {
                Console::shared().error(util::format("Failed to seek to book file %s", fileName.string().c_str()));
            }
            file.close();
        }
        else {
            Console::shared().error(util::format("Couldn't write to book file %s for book 0x%X",fileName.string().c_str(), bookSer));
        }
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBooks::OpenPreDefBook()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens "Read Only" Books defined in /misc/books.dfn
// o------------------------------------------------------------------------------------------------o
auto CBooks::OpenPreDefBook(CSocket *mSock, CItem *i) -> void {
    if (mSock) {
        auto temp = "BOOK "s + util::ntos(i->GetTempVar(CITV_MORE));
        auto book = worldFileLookup.FindEntry(temp, misc_def);
        if (book) {
            CPNewBookHeader toSend;
            toSend.Serial(i->GetSerial());
            toSend.Flag1(0);
            toSend.Flag2(0);
            
            bool part1 = false, part2 = false, part3 = false;
            for (const auto &sec : book->collection()) {
                auto tag = sec->tag;
                auto UTag = util::upper(tag);
                auto data = sec->data;
                if (UTag == "PAGES") {
                    part1 = true;
                    toSend.Pages(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                }
                else if (UTag == "TITLE") {
                    part2 = true;
                    toSend.Title(data);
                }
                else if (UTag == "AUTHOR") {
                    part3 = true;
                    toSend.Author(data);
                }
                if (part1 && part2 && part3) {
                    break;
                }
            }
            toSend.Finalize();
            mSock->Send(&toSend);
        }
        
        ReadPreDefBook(mSock, i, 1);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBooks::OpenBook()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens Writable books based on their .bok file
// o------------------------------------------------------------------------------------------------o
void CBooks::OpenBook(CSocket *mSock, CItem *mBook, bool isWriteable) {
    if (mSock != nullptr) {
        CPBookPage cpbpToSend;
        CPNewBookHeader bInfo;
        bInfo.Serial(mBook->GetSerial());
        
        std::uint16_t numPages = 0;
        
        std::string bookTitle, bookAuthor;
        auto fileName = ServerConfig::shared().directoryFor(dirlocation_t::BOOK) / std::filesystem::path(util::ntos(mBook->GetSerial(), 16) + ".bok"s);
        
        std::ifstream file(fileName.string(), std::ios::in | std::ios::binary);
        
        bool bookExists = (file.is_open());
        if (bookExists) {
            file.seekg(0, std::ios::beg);
            
            if (!file.fail()) {
                char rBuffer[2];
                char tempString[62];
                
                file.read(tempString, 62);
                bookTitle = tempString;
                
                file.read(tempString, 32);
                bookAuthor = tempString;
                
                file.read(rBuffer, 2);
                numPages = static_cast<std::uint16_t>((rBuffer[0] << 8) + rBuffer[1]);
                
                cpbpToSend.Serial(mBook->GetSerial());
                for (std::uint16_t pageNum = 0; pageNum < numPages; ++pageNum) {
                    std::uint8_t blankLineCtr = 0;
                    std::vector<std::string> tempLines;
                    tempLines.resize(0);
                    for (std::uint8_t lineNum = 0; lineNum < 8; ++lineNum) {
                        file.read(tempString, 34);
                        tempLines.push_back(tempString);
                        if (tempString[0] == 0x00) {
                            ++blankLineCtr;
                        }
                    }
                    if (blankLineCtr == 8) {
                        tempLines.clear();
                    }
                    cpbpToSend.NewPage(-1, &tempLines);
                }
                cpbpToSend.Finalize();
            }
            else {
                Console::shared().error(util::format("Failed to seek to book file %s", fileName.string().c_str()));
            }
            
            file.close();
        }
        else {
            numPages = static_cast<std::uint16_t>(mBook->GetTempVar(CITV_MOREY)); // if new book get number of maxpages ...
            if (numPages < 1 || numPages > 255) {
                numPages = 16;
            }
        }
        
        if (bookAuthor.empty()) {
            CChar *mChar = mSock->CurrcharObj();
            if (ValidateObject(mChar)) {
                bookAuthor = mChar->GetName();
            }
        }
        if (bookTitle.empty()) {
            bookTitle = "a book";
        }
        
        bInfo.Pages(numPages);
        
        if (isWriteable) {
            bInfo.Flag1(1);
            bInfo.Flag2(1);
        }
        else {
            bInfo.Flag1(0);
            bInfo.Flag2(0);
        }
        
        bInfo.Author(bookAuthor);
        bInfo.Title(bookTitle);
        
        bInfo.Finalize();
        mSock->Send(&bInfo);
        
        if (bookExists) { // dont send book contents if the file doesnt exist (yet)!
            
            mSock->Send(&cpbpToSend);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBooks::ReadPreDefBook()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends pager number "p" of a Pre-defined Book to the socket
// o------------------------------------------------------------------------------------------------o
auto CBooks::ReadPreDefBook(CSocket *mSock, CItem *i, std::uint16_t p) -> void {
    if (mSock) {
        auto temp = "BOOK "s + util::ntos(i->GetTempVar(CITV_MORE));
        CScriptSection *book = worldFileLookup.FindEntry(temp, misc_def);
        if (book) {
            std::uint16_t curPage = p;
            for (const auto &sec : book->collection()) {
                auto tag = sec->tag;
                if (tag == "PAGE") {
                    --curPage;
                    if (curPage == 0) // we found our page
                    {
                        temp = "PAGE "s + sec->data;
                        auto page = worldFileLookup.FindEntry(temp, misc_def);
                        if (page) {
                            CPBookPage cpbpSend((*i));
                            cpbpSend.NewPage(p);
                            for (const auto &sec2 : page->collection()) {
                                temp = sec2->data;
                                cpbpSend.AddLine(temp);
                            }
                            cpbpSend.Finalize();
                            mSock->Send(&cpbpSend);
                        }
                        break;
                    }
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIBookPage::Handle()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the .bok file with changes made on a specific page.
// o------------------------------------------------------------------------------------------------o
bool CPIBookPage::Handle() {
    if (tSock != nullptr) {
        CItem *mBook = CalcItemObjFromSer(tSock->GetDWord(3));
        if (!ValidateObject(mBook))
            return true;
        
        const std::uint16_t pageNum = tSock->GetWord(9);
        
        if (mBook->GetTempVar(CITV_MOREX) != 666) // Just incase, make sure it is a writable book
        {
            if (mBook->GetTempVar(CITV_MOREX) != 999) {
                worldBook.ReadPreDefBook(tSock, mBook, pageNum);
            }
            return true;
        }
        
        auto fileName = ServerConfig::shared().directoryFor(dirlocation_t::BOOK) / std::filesystem::path(util::ntos(mBook->GetSerial(), 16) + ".bok"s);
        std::uint16_t totalLines = tSock->GetWord(11);
        
        // Cap amount of lines sent in one go at 8 per page
        if (totalLines > 8) {
            totalLines = 8;
        }
        
        // Each page has 8 lines of 34 bytes for each line
        char tempLines[8][34];
        
        std::uint16_t bufferCount = 0;
        for (std::uint8_t lineNum = 0; lineNum < totalLines; ++lineNum) {
            memset(tempLines[lineNum], 0x00, 34);
            
            std::uint8_t i = 0;
            for (i = 0; i < 34; ++i) {
                tempLines[lineNum][i] = tSock->Buffer()[13 + bufferCount];
                ++bufferCount;
                if (tempLines[lineNum][i] == 0x00)
                    break;
            }
            if (tempLines[lineNum][i] != 0x00) {
                tempLines[lineNum][i] = 0x00;
            }
        }
        
        if (!std::filesystem::exists(fileName)) {
            worldBook.CreateBook(fileName, tSock->CurrcharObj(), mBook);
        }
        
        std::fstream file(fileName.string(), std::ios::in | std::ios::out | std::ios::binary);
        if (file.is_open()) {
            file.seekp((((static_cast<size_t>(pageNum) - 1) * 272) + 32 + 62 + 2), std::ios::beg);
            if (!file.fail()) {
                for (std::uint8_t j = 0; j < totalLines; ++j) {
                    file.write((const char *)&tempLines[j], 34);
                }
            }
            else {
                Console::shared().error(util::format("Failed to seek to book file %s", fileName.string().c_str()));
            }
            file.close();
        }
        else {
            Console::shared().error(util::format("Couldn't write to book file %s", fileName.string().c_str()));
        }
    }
    return true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBooks::DeleteBook()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes the .bok file associated with the book item
// o------------------------------------------------------------------------------------------------o
void CBooks::DeleteBook(CItem *id) {
    auto fileName = ServerConfig::shared().directoryFor(dirlocation_t::BOOK) / std::filesystem::path(util::ntos(id->GetSerial(), 16) + ".bok"s);
    if (std::filesystem::exists(fileName)){
        std::filesystem::remove(fileName);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CBooks::CreateBook()
//|	Date		-	11/5/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Formats a newly created .bok file, this must be done with any new
// book file
// o------------------------------------------------------------------------------------------------o
void CBooks::CreateBook(const std::filesystem::path &fileName, CChar *mChar, CItem *mBook) {
    char wBuffer[2];
    char line[34];
    char titleBuff[62];
    char authBuff[32];
    
    std::uint16_t maxpages = static_cast<std::uint16_t>(mBook->GetTempVar(CITV_MOREY));
    if (maxpages < 1 || maxpages > 255) {
        maxpages = 16;
    }
    
    const std::string author = mChar->GetName();
    const std::string title = "a book";
    
    memset(line, 0x00, 34);
    memset(titleBuff, 0x00, 62);
    memset(authBuff, 0x00, 32);
    
    strncopy(titleBuff, 62, title.c_str(), 61);
    strncopy(authBuff, 32, author.c_str(), 31);
    
    std::ofstream file(fileName.string(), std::ios::out | std::ios::binary | std::ios::trunc);
    
    file.seekp(0, std::ios::beg);
    
    file.write(titleBuff, 62);
    file.write(authBuff, 32);
    
    wBuffer[0] = static_cast<std::int8_t>(maxpages >> 8);
    wBuffer[1] = static_cast<std::int8_t>(maxpages % 256);
    file.write((const char *)&wBuffer, 2);
    
    for (std::uint16_t i = 0; i < maxpages; ++i) {
        for (std::uint8_t lineNum = 0; lineNum < 8; ++lineNum) {
            file.write((const char *)&line, 34);
        }
    }
    
    file.close();
}
