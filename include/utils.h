#pragma once

#include "include.h"
#include "cMove.h"

namespace utils {
	static std::string to_string(std::vector<UInt> v)
	{
		std::string s = "";
		for (UInt i : v)
		{
			s += std::to_string(i) + " ";
		}
		return s;
	}

	static std::string to_string(std::vector<cMove> v)
	{
		std::string s = "";
		for (cMove i : v)
		{
			s += std::to_string(i.getTo()) + " ";
		}
		return s;
	}
}
