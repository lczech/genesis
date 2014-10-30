#include <string>
#include <iostream>

#include "../utils/log.hh"

using namespace genesis;

/**
 * Prints the header information on screen when starting the program.
 */
std::string print_header()
{
    return
"                                     ,    \n\
        __    __    __    __   __     __   \n\
      /   ) /___) /   ) /___) (_ ` / (_ `  \n\
     (___/ (___  /   / (___  (__) / (__)   \n\
        /                                  \n\
    (_ /            2014 by Lucas Czech    \n";
}

int main (int argc, char* argv[])
{
    Log::AddOutputStream(std::cout);

    LOG_BOLD << print_header();

    LOG_TIME << "am start";

    LOG_DBG1 << "test " << 42;
    LOG_INFO << "hallo";

    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "durch";

    return 0;
}
