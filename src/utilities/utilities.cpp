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

std::string 
tab_separated(const std::vector<std::string> &columns) {
    std::string result(" ");
    for (auto column: columns) {
        if (column.empty()) {
            result += '\t';
        } else {
            result += column + "\t";
        }
    }                       
    result[result.size() - 1] = '\n'; 
    return result;
}
