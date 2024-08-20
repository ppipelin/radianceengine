#pragma once

#include "include.h"

#include <map>
#include <string>
#include <deque>

class BoardParser;
class cMove;
class Search;

namespace UCI {
	class Option;

	/// Define a custom comparator, because the UCI options should be case-insensitive
	struct CaseInsensitiveLess
	{
		bool operator() (const std::string &, const std::string &) const;
	};

	/// The options container is defined as a std::map
	using OptionsMap = std::map<std::string, Option, CaseInsensitiveLess>;

	/// The Option class implements each option as specified by the UCI protocol
	class Option
	{
	public:
		Option();
		Option(bool v);
		Option(const char *v);
		Option(double v, int minv, int maxv);
		Option(const char *v, const char *cur);

		Option &operator=(const std::string &);
		void operator<<(const Option &);
		operator int() const;
		operator std::string() const;
		bool operator==(const char *) const;

	private:
		friend std::ostream &operator<<(std::ostream &, const UCI::OptionsMap &);

		std::string defaultValue, currentValue, type;
		int min, max;
		size_t idx = 0; // Order of Option in the OptionsMap
	};

	void init(UCI::OptionsMap &);
	void loop(int argc, char *argv[]);
	std::string square(UInt s);
	std::string move(cMove m);
	cMove to_move(const BoardParser &pos, std::string &str);
	std::string pv(const Search &s, UInt depth);
} // namespace UCI

extern UCI::OptionsMap g_options;
