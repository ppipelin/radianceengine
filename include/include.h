#pragma once
#pragma warning(disable: 4505)

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <chrono>

using UInt = std::uint_fast16_t;
using Key = uint64_t;
using Int = std::int_fast16_t;
using Value = Int;

constexpr UInt BOARD_SIZE = 8;
constexpr UInt BOARD_SIZE2 = 8 * 8;
constexpr Value MAX_EVAL = 100000;
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
