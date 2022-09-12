#pragma once

#include <string>
#include <vector>
#include <map>

typedef std::nullptr_t Null;
typedef bool Boolean;
typedef std::int8_t Byte;
typedef std::uint8_t UByte;
typedef std::int16_t Short;
typedef std::uint16_t UShort;
typedef std::int32_t Int;
typedef std::uint32_t UInt;
typedef std::int64_t Long;
typedef std::uint64_t ULong;
typedef std::size_t Size;
typedef float Float;
typedef double Double;
typedef std::string String;

template <typename T>
using Vector = std::vector<T>;

template <typename T1, typename T2>
using Map = std::map<T1, T2>;

typedef std::exception Exception;