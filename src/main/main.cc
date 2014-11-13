/**
 * @brief Main function of genesis.
 *
 * @file
 * @ingroup main
 */

#include "main/main.hh"

#include <iostream>
#include <string>

#include "tree/tree.hh"
#include "utils/logging.hh"

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
    Logging::AddOutputStream(std::cout);
    LOG_BOLD << print_header();
    LOG_TIME << "start";

    Tree tree;
    tree.FromNewickString("( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Innen:0.7{2}, Coyote:0.5{3} ){4};");

    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "ende";

    return 0;
}
