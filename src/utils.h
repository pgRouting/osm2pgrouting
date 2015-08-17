#include <string>

namespace my_utils{

//! return whether the giveb string is a number or not
bool is_number(const std::string& s);

/**
 * Read first 5 digits from a string.
 */
std::string read_number_substring(const std::string& s);

}
