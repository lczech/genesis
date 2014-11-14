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
    Logging::details.level = false;
    LOG_BOLD << print_header();
    LOG_TIME << "start";

    Tree tree;
    //~ tree.FromNewickString("( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};");
    //~ tree.FromNewickString("( (] Ant:0.2, [a comment] 'Bee':0.09 )Innen:0.7, Coyote:0.5 );");

    LOG_INFO << "no nodes are named: (,,(,));";
    tree.FromNewickString("(,,(,));");

    LOG_INFO << "leaf nodes are named: (A,B,(C,D));";
    tree.FromNewickString("(A,B,(C,D));");

    LOG_INFO << "all nodes are named: (A,B,(C,D)E)F;";
    tree.FromNewickString("(A,B,(C,D)E)F;");

    LOG_INFO << "all but root node have a distance to parent: (:0.1,:0.2,(:0.3,:0.4):0.5);";
    tree.FromNewickString("(:0.1,:0.2,(:0.3,:0.4):0.5);");

    LOG_INFO << "all have a distance to parent: (:0.1,:0.2,(:0.3,:0.4):0.5):0.0;";
    tree.FromNewickString("(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;");

    LOG_INFO << "distances and leaf names (popular): (A:0.1,B:0.2,(C:0.3,D:0.4):0.5);";
    tree.FromNewickString("(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);");

    LOG_INFO << "distances and all names: (A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";
    tree.FromNewickString("(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;");

    LOG_INFO << "a tree rooted on a leaf node (rare): ((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;";
    tree.FromNewickString("((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;");

    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "ende";

    return 0;
}
