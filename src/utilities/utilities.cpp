#include <string>
#include <vector>



std::string 
comma_separated(const std::vector<std::string> &columns) {
    std::string result(" ");
    for (auto column: columns) {
        result += column + ",";
    }                       
    result[result.size() - 1] = ' '; 
    return result;
}
