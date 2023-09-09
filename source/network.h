#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <list>
#include <map>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif

#include "typedefs.h"

// o------------------------------------------------------------------------------------------------o
//  ByteBufferBounds
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
struct ByteBufferBounds : public std::out_of_range {
    int offset;
    int amount;
    int buffersize;
    std::string _msg;
    explicit ByteBufferBounds(int offset, int amount, int size);
    auto what() const noexcept -> const char * override;
};

// o------------------------------------------------------------------------------------------------o
//  Radix that are supported
//  Normally part of strutil
enum class radix_t { dec = 10, oct = 8, hex = 16, bin = 2 };

// o------------------------------------------------------------------------------------------------o
//  ByteBuffer
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
class ByteBuffer {
    // These are compatability to "look" like a the original CPacketStream
    //=============================================================================================
  public:
    auto ReserveSize(size_t len) -> void { this->size(static_cast<int>(len), 0); }
    auto WriteByte(size_t pos, std::uint8_t toWrite) -> void {
        this->write(static_cast<int>(pos), toWrite);
    }
    // Why isn't thist std::int16_t?
    auto WriteShort(size_t pos, std::int32_t toWrite) -> void {
        this->write(static_cast<int>(pos), static_cast<int16_t>(toWrite));
    }
    auto WriteLong(size_t pos, std::uint32_t toWrite) -> void {
        this->write(static_cast<int>(pos), toWrite);
    }
    auto WriteString(size_t pos, const std::string &toWrite, size_t len) -> void {
        this->write(static_cast<int>(pos), toWrite, static_cast<int>(len), false);
    }
    auto WriteArray(size_t pos, const std::uint8_t *toWrite, size_t len) -> void {
        this->write(static_cast<int>(pos), toWrite, static_cast<int>(len), false);
    }
    auto GetByte(size_t pos) const -> std::uint8_t {
        try {
            return this->read<std::uint8_t>(static_cast<int>(pos));
        } catch (...) {
            return 0;
        }
    }
    auto GetShort(size_t pos) const -> std::int16_t {
        try {
            return this->read<std::int16_t>(static_cast<int>(pos));
        } catch (...) {
            return 0;
        }
    }
    auto GetUShort(size_t pos) const -> std::uint16_t {
        try {
            return this->read<std::uint16_t>(static_cast<int>(pos));
        } catch (...) {
            return 0;
        }
    }
    auto GetLong(size_t pos) const -> std::int32_t {
        try {
            return this->read<std::int32_t>(static_cast<int>(pos));
        } catch (...) {
            return 0;
        }
    }
    auto GetULong(size_t pos) const -> std::uint32_t {
        try {
            return this->read<std::uint32_t>(static_cast<int>(pos));
        } catch (...) {
            return 0;
        }
    }

    auto GetBuffer() const -> const std::uint8_t * { return this->raw(); }

    auto GetSize() const -> size_t { return this->size(); }

    // End compatability of original CPacketStream
    // o------------------------------------------------------------------------------------------------o

    // o------------------------------------------------------------------------------------------------o
    //  Normally part of strutil, but included here for stand alone
  public:
    // The maximum characters in a string number for conversion sake
    static constexpr auto max_characters_in_number = 12;

    // Dumps a byte buffer, formatted to a provided stream.
    // The entries_line indicate how many bytes to display per line.
    static auto DumpByteBuffer(std::ostream &output, const std::uint8_t *buffer, std::size_t length,
                               radix_t radix = radix_t::hex, std::size_t entries_line = 8) -> void;

  private:
    // Convert a bool to a string
    // the true_value/false_value are returned based on the bool
    static auto ntos(bool value, const std::string &true_value = "true",
                     const std::string &false_value = "false") -> std::string;

    // Convert a number to a string, with options on radix, prefix, size, pad
    // Radix indicates the radix the string will represent
    // prefix indicates if the "radix" indicator will be present at the front of the string
    // size is the minimum size the string must be (if the number is not large enough
    // it will use the "pad" character prepended to the number
    template <typename T>
    static auto ntos(T value, radix_t radix = radix_t::dec, bool prefix = false, int size = 0,
                     char pad = '0') -> std::string {
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
            // first, thing we need to convert the value to a string
            std::array<char, max_characters_in_number> str;

            if (auto [pc, ec] = std::to_chars(str.data(), str.data() + str.size(), value,
                                              static_cast<int>(radix));
                ec == std::errc()) {
                // How many characters did this number take
                auto numchars = static_cast<int>(std::distance(str.data(), pc));
                // what is larger, that is the size of our string
                auto sizeofstring = std::max(numchars, size);
                // where do we start adding the number into our string ?
                auto index = sizeofstring - numchars;
                if (prefix) {
                    // We have a prefix, so we add two characters to the beginning
                    sizeofstring += 2;
                    index += 2;
                }
                auto rValue = std::string(sizeofstring, pad);
                // copy the value into the string
                std::copy(str.data(), pc, rValue.begin() + index);
                // do we need our prefix?
                if (prefix) {
                    switch (static_cast<int>(radix)) {
                    case static_cast<int>(radix_t::dec):
                        // We dont add anything for decimal!
                        break;
                    case static_cast<int>(radix_t::hex):
                        rValue[0] = '0';
                        rValue[1] = 'x';
                        break;
                    case static_cast<int>(radix_t::oct):
                        rValue[0] = '0';
                        rValue[1] = 'o';
                        break;
                    case static_cast<int>(radix_t::bin):
                        rValue[0] = '0';
                        rValue[1] = 'b';
                        break;

                    default:
                        break;
                    }
                }
                return rValue;
            }
            else {
                // The conversion was not successful, so we return an empty string
                return std::string();
            }
        }
    }
    // End of strutil inclusion
    //=============================================================================================

  protected:
    mutable int _index;
    std::vector<std::uint8_t> _bytedata;
    auto Exceeds(int offset, int bytelength) const -> bool;
    auto Exceeds(int offset, int bytelength, bool expand) -> bool;

  public:
    ByteBuffer(int size = 0, int reserve = 0);

    auto size() const -> size_t;
    auto size(int value, std::uint8_t fill = 0) -> void;

    auto index() const -> int;
    auto index(int value) -> void;

    auto raw() const -> const std::uint8_t *;
    auto raw() -> std::uint8_t *;

    auto operator[](int index) const -> const std::uint8_t &;
    auto operator[](int index) -> std::uint8_t &;

    auto Fill(std::uint8_t value, int offset, int length) -> void;
    auto LogByteBuffer(std::ostream &output, radix_t radix = radix_t::hex,
                       int entries_line = 8) const -> void;

    // we need to read :integral/floating, vectors/list/strings
    template <typename T>
    auto read(int offset = -1, [[maybe_unused]] int amount = -1, bool reverse = true) const -> T {
        if (offset < 0) {
            offset = _index;
        }
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            // we ignore amount for integrals and floating point
            auto size = static_cast<int>(sizeof(T));
            if constexpr (std::is_same_v<T, bool>) {
                size = 1;
            }
            if (Exceeds(offset, size)) {
                throw ByteBufferBounds(offset, size, static_cast<int>(_bytedata.size()));
            }
            T value(0);
            std::copy(_bytedata.data() + offset, _bytedata.data() + offset + size,
                      reinterpret_cast<std::uint8_t *>(&value));
            if (reverse && (size > 1)) {
                std::reverse(reinterpret_cast<std::uint8_t *>(&value),
                             reinterpret_cast<std::uint8_t *>(&value) + size);
            }
            _index = offset + size;
            return value;
        }
        else if constexpr (std::is_class_v<T>) {
            if constexpr (std::is_integral_v<typename T::value_type>) {
                // It is a supported container (hopefully) they want back!
                // what is the size of the entry in the container
                auto entry_size = static_cast<int>(sizeof(typename T::value_type));
                if (amount < 0) {
                    // what we do is calcualate how many entries are left in the buffer
                    amount = (static_cast<int>(_bytedata.size()) - offset) / entry_size;
                }
                auto requested_size = entry_size * amount;
                // are we exceeding that?
                if (Exceeds(offset, requested_size)) {
                    throw ByteBufferBounds(offset, requested_size,
                                              static_cast<int>(_bytedata.size()));
                }
                // we need to loop through and read a "character" at a time
                typename T::value_type character = 0;
                T rValue;

                for (auto i = 0; i < amount; ++i) {
                    std::copy(_bytedata.data() + offset + i * entry_size,
                              _bytedata.data() + offset + (i + 1) * entry_size,
                              reinterpret_cast<std::uint8_t *>(&character));
                    // should we "reverse it" ?
                    if ((entry_size > 1) && reverse) {
                        std::reverse(reinterpret_cast<std::uint8_t *>(&character),
                                     reinterpret_cast<std::uint8_t *>(&character) + entry_size);
                    }
                    // If this is a string, we stop at a null terminator.  Do we really want to do
                    // this?
                    if constexpr (std::is_same_v<std::string, T> ||
                                  std::is_same_v<std::wstring, T> ||
                                  std::is_same_v<std::u16string, T> ||
                                  std::is_same_v<std::u32string, T>) {
                        if (character == 0) {
                            break;
                        }
                    }
                    rValue.push_back(character);
                }
                _index = offset + requested_size;
                return rValue;
            }
        }
    }

    // This reads into the buffer supplied, only for integral types;
    template <typename T>
    auto read(int offset, T *value, int amount = -1, bool reverse = true) const -> void {
        if (offset < 0) {
            offset = _index;
        }
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            auto entry_size = static_cast<int>(sizeof(T));
            if constexpr (std::is_same_v<T, bool>) {
                entry_size = 1;
            }
            auto size = amount * entry_size;
            if (amount < 0) {
                size = entry_size;
                amount = 1;
            }
            if (amount > 0) {
                if (Exceeds(offset, size)) {
                    throw ByteBufferBounds(offset, size, static_cast<int>(_bytedata.size()));
                }
                // We now get to read
                T input;
                for (auto i = 0; i < amount; ++i) {
                    std::copy(_bytedata.begin() + offset + (i * entry_size),
                              _bytedata.begin() + offset + ((i + 1) * entry_size),
                              reinterpret_cast<std::uint8_t *>(&input));
                    if (reverse) {
                        std::reverse(reinterpret_cast<std::uint8_t *>(&input),
                                     reinterpret_cast<std::uint8_t *>(&input) + entry_size);
                    }
                    // Now put it into the data stream
                    std::copy(reinterpret_cast<std::uint8_t *>(&input),
                              reinterpret_cast<std::uint8_t *>(&input) + entry_size,
                              value + (i * entry_size));
                }
                _index = offset + amount * entry_size;
            }
        }
    }

    template <typename T>
    auto write(int offset, const T *value, int amount = -1, bool reverse = true, bool expand = true)
        -> ByteBuffer & {
        if (offset < 0) {
            offset = _index;
        }
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            auto entry_size = static_cast<int>(sizeof(T));
            if constexpr (std::is_same_v<T, bool>) {
                entry_size = 1;
            }
            auto size = amount * entry_size;
            if (amount < 0) {
                size = entry_size;
                amount = 1;
            }
            if (amount > 0) {
                if (Exceeds(offset, size, expand)) {
                    throw ByteBufferBounds(offset, size, static_cast<int>(_bytedata.size()));
                }
                // we need to write it
                for (auto i = 0; i < amount; ++i) {
                    auto input = value[i];
                    if (reverse && (entry_size > 1)) {
                        std::reverse(reinterpret_cast<std::uint8_t *>(&input),reinterpret_cast<std::uint8_t *>(&input) + entry_size);
                    }
                    std::copy(reinterpret_cast<std::uint8_t *>(&input),reinterpret_cast<std::uint8_t *>(&input) + entry_size,_bytedata.begin() + offset + (i * entry_size));
                }
                _index = offset + (entry_size * amount);
            }
            return *this; // we put this here, versue at the end, for we want a compile error if a
                          // type not caught with if constexpr. So a return in each if constexpr at
                          // the top level
        }
    }

    template <typename T>
    auto write(int offset, const T &value, int amount = -1, bool reverse = true, bool expand = true)
        -> ByteBuffer & {
        if (offset < 0) {
            offset = _index;
        }

        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            // we ignore amount for integrals and floating point
            auto size = static_cast<int>(sizeof(T));
            if constexpr (std::is_same_v<T, bool>) {
                size = 1;
            }
            if (Exceeds(offset, size, expand)) {

                throw ByteBufferBounds(offset, size, static_cast<int>(_bytedata.size()));
            }
            // we need to write it
            T temp = value;
            if (reverse) {
                std::reverse(reinterpret_cast<std::uint8_t *>(&temp),
                             reinterpret_cast<std::uint8_t *>(&temp) + size);
            }
            std::copy(reinterpret_cast<std::uint8_t *>(&temp),
                      reinterpret_cast<std::uint8_t *>(&temp) + size, _bytedata.begin() + offset);

            _index = offset + size;
            return *this; // we put this here, versue at the end, for we want a compile error if a
                          // type not caught with if constexpr. So a return in each if constexpr at
                          // the top level
        }
        else if constexpr (std::is_class_v<T>) {
            if constexpr (std::is_integral_v<typename T::value_type>) {
                // It is a supported container (hopefully) they want back!
                // what is the size of the entry in the container
                auto entry_size = static_cast<int>(sizeof(typename T::value_type));
                auto container_size = static_cast<int>(value.size());
                auto fill_size = 0;
                auto write_size = container_size;
                if (amount < 0) {
                    amount = container_size;
                }
                else {
                    write_size = amount;
                    if (amount > container_size) {
                        fill_size = amount - container_size;
                        write_size = container_size;
                    }
                }
                auto requested_size = (write_size + fill_size) * entry_size;
                // are we exceeding that?
                if (Exceeds(offset, requested_size, expand)) {
                    throw ByteBufferBounds(offset, requested_size,
                                              static_cast<int>(_bytedata.size()));
                }
                // Ok, so now we get to go and do our thing
                for (auto i = 0; i < write_size; ++i) {
                    auto entry = value[i];
                    if (reverse && (entry_size > 1)) {
                        std::reverse(reinterpret_cast<std::uint8_t *>(&entry),
                                     reinterpret_cast<std::uint8_t *>(&entry) + entry_size);
                    }
                    std::copy(reinterpret_cast<std::uint8_t *>(&entry),
                              reinterpret_cast<std::uint8_t *>(&entry) + entry_size,
                              _bytedata.data() + offset + i * entry_size);
                }
                _index = offset + (write_size * entry_size);

                // Now we need to do the fill if anyway
                if (fill_size > 0) {
                    std::fill(_bytedata.data() + _index,
                              _bytedata.data() + _index + (fill_size * entry_size), 0);
                }
                _index += fill_size * entry_size;
            }
            return *this;
        }
        else if constexpr (std::is_array_v<T>) {
            if constexpr (std::is_integral_v<typename std::remove_extent<T>::type> ||
                          std::is_floating_point_v<typename std::remove_extent<T>::type>) {
                // It is an array!
                auto entry_size = static_cast<int>(sizeof(typename std::remove_extent<T>::type));
                if (amount < 0) {
                    amount = 1;
                }
                auto requested_size = amount * entry_size;

                if (Exceeds(offset, requested_size, expand)) {
                    throw ByteBufferBounds(offset, requested_size,
                                              static_cast<int>(_bytedata.size()));
                }
                // We need to check and loop through if we are reversing;

                // we need to write it
                for (auto i = 0; i < amount; ++i) {
                    auto input = value[i];
                    if (reverse && (entry_size > 1)) {
                        std::reverse(reinterpret_cast<std::uint8_t *>(&input),
                                     reinterpret_cast<std::uint8_t *>(&input) + entry_size);
                    }
                    std::copy(reinterpret_cast<std::uint8_t *>(&input),
                              reinterpret_cast<std::uint8_t *>(&input) + entry_size,
                              _bytedata.begin() + offset + (i * entry_size));
                }
                _index = offset + (entry_size * amount);
                return *this;
            }
        }
    }
};

class socket_error : public std::runtime_error {
  private:
    std::uint32_t errorNum;

  public:
    socket_error(const std::string &what_arg);
    socket_error(const std::uint32_t errorNumber);
    socket_error();
    std::uint32_t ErrorNumber() const;
    const char *what() const throw();
};

class CPUOXBuffer {
  private:
    std::vector<std::uint8_t> packedBuffer;
    bool isPacked;
    std::uint32_t packedLength;

  protected:
    ByteBuffer pStream;

    virtual void InternalReset();

  public:
    CPUOXBuffer();
    virtual ~CPUOXBuffer();
    CPUOXBuffer(CPUOXBuffer *initBuffer);
    CPUOXBuffer &operator=(CPUOXBuffer &copyFrom);

    std::uint32_t Pack();
    virtual bool ClientCanReceive(CSocket *mSock);
    ByteBuffer &GetPacketStream();

    std::uint32_t PackedLength() const;
    const std::uint8_t *PackedPointer() const;

    virtual void log(std::ostream &outStream, bool fullHeader = true);
};

class CPInputBuffer {
  protected:
    CSocket *tSock;

  public:
    CPInputBuffer();
    CPInputBuffer(CSocket *input);
    virtual ~CPInputBuffer() {}

    virtual void Receive() = 0;
    virtual void log(std::ostream &outStream, bool fullHeader = true);
    virtual bool Handle();
    void SetSocket(CSocket *toSet);
    CSocket *GetSocket() const;
};

class CNetworkStuff {

  public:
    CNetworkStuff();
    ~CNetworkStuff();
    auto startup() -> void;
    void Disconnect(uoxsocket_t s);
    void Disconnect(CSocket *s);
    void ClearBuffers();
    void CheckLoginMessage();
    void CheckMessage();
    void SockClose();
    void SetLastOn(CSocket *s);
    CSocket *GetSockPtr(uoxsocket_t s);
    uoxsocket_t FindNetworkPtr(CSocket *toFind);

    void CheckConnections();
    void CheckMessages();

    void Transfer(CSocket *s);

    size_t PeakConnectionCount() const;

    // Login Specific
    void LoginDisconnect(uoxsocket_t s);
    void LoginDisconnect(CSocket *s);

    void RegisterPacket(std::uint8_t packet, std::uint8_t subCmd, std::uint16_t scriptId);

    // We don't want to do this, but given we have outside classes
    // we can either friend a lot of things, or just put it out here
    std::mutex internallock;
    std::vector<CSocket *> connClients, loggedInClients;

  private:
    struct FirewallEntry_st {
        std::int16_t b[4];
        FirewallEntry_st(std::int16_t p1, std::int16_t p2, std::int16_t p3, std::int16_t p4) {
            b[0] = p1;
            b[1] = p2;
            b[2] = p3;
            b[3] = p4;
        }
    };

    std::map<std::uint16_t, std::uint16_t> packetOverloads;
    std::vector<FirewallEntry_st> slEntries;
    SOCKET a_socket;

    struct sockaddr_in client_addr;

    size_t peakConnectionCount;

    void LoadFirewallEntries();
    void GetMsg(uoxsocket_t s);
    void sockInit();
    void GetLoginMsg(uoxsocket_t s);
    uoxsocket_t FindLoginPtr(CSocket *s);

    void CheckConn();
    void LogOut(CSocket *s);

    bool IsFirewallBlocked(std::uint8_t part[4]);
};

extern CNetworkStuff *Network;

#endif
