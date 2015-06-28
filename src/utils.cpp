#include "utils.h"

namespace my_utils{

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}


/**
 * Read first 5 digits from a string.
 */
std::string read_number_substring(const std::string& s)
{
	char buffer[5];
	int index = 0;
	std::string::const_iterator it = s.begin();
	while (it < s.end() && std::isdigit(*it) && index < 5){
		buffer[index] = *it;
		++index;
		++it;
	}
	buffer[index] = 0;
	std::string out( buffer );
	return out;
}

}
