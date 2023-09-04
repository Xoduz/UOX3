#ifndef console_hpp
#define console_hpp

// In essence, Console is our nice generic class for outputting display
// As long as the interface is kept the same, we can display whatever we wish to
// We store a set of coordinates being the window corner and size, for systems with windowing
// support, which describes the window we are in
#include <bitset>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "typedefs.h"

class CBaseObject;
class CEndL;
// o------------------------------------------------------------------------------------------------o
constexpr auto MAX_CONSOLE_BUFF = 512;

const std::uint8_t CNORMAL = 0;
const std::uint8_t CBLUE = 1;
const std::uint8_t CRED = 2;
const std::uint8_t CGREEN = 3;
const std::uint8_t CYELLOW = 4;
const std::uint8_t CBWHITE = 5;

// o------------------------------------------------------------------------------------------------o
//  Console
// o------------------------------------------------------------------------------------------------o
class Console {
    Console();

  public:
    Console(const Console &) = delete;
    Console(Console &&) = delete;
    auto operator=(const Console &) -> Console & = delete;
    auto operator=(Console &&) -> Console & = delete;
    static auto shared() -> Console &;

    ~Console();
    auto Initialize() -> void;

    auto Flush() -> void { std::cout.flush(); }
    // streaming overloads, we take pointers and references

    auto operator<<(const char *output) -> Console &;
    auto operator<<(const std::int8_t *output) -> Console &;
    auto operator<<(const std::uint8_t *output) -> Console &;

    auto operator<<(const std::uint8_t &output) -> Console &;
    auto operator<<(const std::int8_t &output) -> Console &;
    auto operator<<(const std::uint16_t &output) -> Console &;
    auto operator<<(const std::int16_t &output) -> Console &;
    auto operator<<(const std::uint32_t &output) -> Console &;
    auto operator<<(const std::int32_t &output) -> Console &;
    auto operator<<(const std::uint64_t &output) -> Console &;
    auto operator<<(const std::int64_t &output) -> Console &;
    auto operator<<(const float &output) -> Console &;
    auto operator<<(const double &output) -> Console &;
    auto operator<<(const long double &output) -> Console &;

    auto operator<<(CBaseObject *output) -> Console &;
    auto operator<<(const CBaseObject *output) -> Console &;
    auto operator<<(const CBaseObject &output) -> Console &;

    auto operator<<(const std::string &outPut) -> Console &;

    auto operator<<(CEndL &myObj) -> Console &;

    auto Print(const std::string &toPrint) -> void;
    auto Log(const std::string &msg, const std::string &filename) -> void;
    auto Log(const std::string &msg) -> void;
    auto Error(const std::string &msg) -> void;
    auto Warning(const std::string &msg) -> void;

    auto CurrentMode() const -> std::uint8_t;
    auto CurrentMode(std::uint8_t value) -> void;

    auto PrintSectionBegin() -> void;

    auto TurnYellow() -> void;
    auto TurnRed() -> void;
    auto TurnGreen() -> void;
    auto TurnBlue() -> void;
    auto TurnNormal() -> void;
    auto TurnBrightWhite() -> void;
    auto PrintDone() -> void;
    auto PrintFailed() -> void;
    auto PrintPassed() -> void;

    auto ClearScreen() -> void;
    auto Start(const std::string &temp) -> void;
    auto PrintBasedOnVal(bool value) -> void;
    auto MoveTo(std::int32_t x, std::int32_t y = -1) -> void; // y=-1 will move on the current line

    auto LogEcho() const -> bool;
    auto LogEcho(bool value) -> void;
    auto PrintSpecial(std::uint8_t color, const std::string &msg) -> void;
    auto Poll() -> void;

    auto RegisterKey(std::int32_t key, std::string cmdName, std::uint16_t scriptId) -> void;
    auto RegisterFunc(const std::string &key, const std::string &cmdName, std::uint16_t scriptId) -> void;
    auto SetKeyStatus(std::int32_t key, bool isEnabled) -> void;
    auto SetFuncStatus(const std::string &key, bool isEnabled) -> void;
    auto Registration() -> void;

  private:
    auto Reset() -> void;
    auto WindowSize() -> std::pair<int, int>;
    auto DoClearScreen() -> void;
    auto SetTitle(const std::string &title) -> void;
    auto SendCMD(const std::string &cmd) -> Console &;

    auto PrintStartOfLine() -> void;
    auto StartOfLineCheck() -> void;
    auto cl_getch() -> std::int32_t;
    auto Process(std::int32_t c) -> void;
    auto DisplaySettings() -> void;

    struct JSConsoleEntry_st {
        std::uint16_t scriptId;
        bool isEnabled;
        std::string cmdName;
        JSConsoleEntry_st() : scriptId(0), isEnabled(true), cmdName("") {}
        JSConsoleEntry_st(std::uint16_t id, const std::string &cName)
            : scriptId(id), isEnabled(true), cmdName(cName) {}
    };


    std::map<std::int32_t, JSConsoleEntry_st> JSKeyHandler;
    std::map<std::string, JSConsoleEntry_st> JSConsoleFunctions;
    std::uint16_t width, height; // for differing windows
    std::uint16_t curLeft, curTop;
    std::bitset<16> filterSettings;
    std::uint8_t currentMode;
    std::uint8_t previousColour;
    bool logEcho;
    bool is_initialized;

    static const std::string ESC;
    static const std::string CSI;
    static const std::string BEL;
    //  Command sequences
    static const std::string ATTRIBUTE; // find/replace on # with attribute number
    static const std::string MOVE;      // we are going to find/replace on ROW/COL
    static const std::string HORIZMOVE;
    static const std::string VERTMOVE;
    static const std::string OFFCURSOR;
    static const std::string ONCURSOR;

    static const std::string ONGRAPHIC;
    static const std::string OFFGRAPHIC;

    static const std::string SETTITLE;

    static const std::string CLEAR;
};

// o------------------------------------------------------------------------------------------------o
//  CEndL
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
class CEndL {
  public:
    void Action(Console &test) {
        test << "\n";
        test.Flush();
    }
};
// o------------------------------------------------------------------------------------------------o
extern CEndL myendl;

#endif
