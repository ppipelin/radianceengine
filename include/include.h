#pragma once
#pragma warning(disable: 4505)

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <string>
// #include <regex>
// #include <map>

typedef std::uint_fast16_t UInt;
typedef std::int_fast16_t Int;
typedef std::uint_fast8_t SUInt;

constexpr UInt BOARD_SIZE = 8;
constexpr UInt BOARD_SIZE2 = 8 * 8;
constexpr Int MAX_EVAL = INT_FAST16_MAX;
constexpr UInt MAX_PLY = 246;

static void debug(std::string str)
{
	std::cout << "DEBUG: " << str << std::endl;
}

static void warn(std::string str)
{
	std::cout << "WARN: " << str << std::endl;
}

static void err(std::string str)
{
	std::cout << "ERR: " << str << std::endl;
}
