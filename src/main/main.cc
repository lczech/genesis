#include <string>
#include <iostream>

#include "../utils/log.hh"
#include "../utils/utils.hh"
#include "../utils/lexer.hh"

using namespace genesis;
using namespace genesis::utils;

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

    utils::Lexer l;
    l.Analyze(utils::ReadFile("/home/lucas/Dropbox/HITS/genesis/data/placement.json"));
    //~ l.Analyze("((A:0.2{0},B:0.09{1}):0.7{2},C:0.5{3}){4};");
    LOG_BOLD << l.Dump();
    LOG_INFO << "brackets " << l.CheckBrackets();

    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "durch";

    return 0;
}
