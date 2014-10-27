#include <iostream>

//~ #include "../utils/log.hh"

/**
 * Prints the header information on screen when starting the program.
 */
void print_header()
{
    std::cout << "genesis started." << std::endl;
}

int main (int argc, char* argv[])
{
    print_header();

    //~ Log* l = new Log();
    //~ Log logger();
//~
    //~ logger << "text to log, and a variable " << i;

    //~ LOG_DBG << "test";
//~
    //~ LOG_INFO << "hallo";

    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    return 0;
}
