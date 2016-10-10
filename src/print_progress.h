

#include <iostream>
#include <string>

template < typename T1 , typename T2>
void
print_progress(T1 wantProgress, T2 currentProgress) {
    int length = 50;
    double percent = static_cast <double> (currentProgress) /  static_cast <double>(wantProgress);
    int fillerLenght = static_cast<int>(percent * length);

    std::string bar = "[";
    for (int i = 0; i < fillerLenght; i++) {
        bar += "*";
    }

    bar += "|";
    for (int i = 0; i < length - fillerLenght; i++) {
        bar += " ";
    }

    bar += "]";
    std::cout << "\r"
        << bar
        << " (" << static_cast<int>(100 * percent) << "%)"
        << std::flush;
}
