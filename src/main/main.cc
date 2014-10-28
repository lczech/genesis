#include <string>
#include <iostream>

#include "../utils/log.hh"

/**
 * Prints the header information on screen when starting the program.
 */
std::string print_header()
{
    return std::string (
        "   ___   ____ __  __  ____  __  __  __ \n") +
        "  // \\\\ ||    ||\\ || ||    (( \\ || (( \\\n" +
        " (( ___ ||==  ||\\\\|| ||==   \\\\  ||  \\\\ \n" +
        "  \\\\_|| ||___ || \\|| ||___ \\_)) || \\_))\n";
}

int main (int argc, char* argv[])
{
    Log::AddOutputStream(std::cout);
    Log::AddOutputFile("/home/lucas/test.log");

    LOG_MSG << print_header();

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
