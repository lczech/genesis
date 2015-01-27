/**
 * @brief Main function of genesis.
 *
 * @file
 * @ingroup main
 */

#include "main/main.hpp"

#include <iostream>
#include <string>

#include "placement/placements.hpp"
#include "placement/jplace_parser.hpp"
#include "tree/tree.hpp"
#include "utils/json_document.hpp"
#include "utils/json_lexer.hpp"
#include "utils/json_parser.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

using namespace genesis;

/**
 * Prints the header information on screen when starting the program.
 */
std::string print_header()
{
    return "\
                                     ,     \n\
        __    __    __    __   __     __   \n\
      /   ) /___) /   ) /___) (_ ` / (_ `  \n\
     (___/ (___  /   / (___  (__) / (__)   \n\
        /                                  \n\
    (__/       2014-2015 by Lucas Czech    \n";
}

int main (int argc, char* argv[])
{
    // -----------------------------------------------------
    //     Program startup routines
    // -----------------------------------------------------
    Logging::AddOutputStream(std::cout);
    //~ Logging::details.level = false;
    //~ Logging::details.file = true;
    //~ Logging::details.line = true;
    LOG_BOLD << print_header();
    LOG_TIME << "start";

    // -----------------------------------------------------
    //     Test cases for tree iterators
    // -----------------------------------------------------
    /*
    Tree<> tree;
    TreeNode<DefaultNodeData, DefaultEdgeData>* n;
    tree.FromNewickString("((A,((B,C,D)E,F)G)H,((I,J,K)L,M,N)O,P,Q)R;");
    //~ tree.FromNewickString("((A,((B,C)D,E)F)G,((H,((I,J)K,L)M)N,O)P,Q)R;");
    LOG_DBG << tree.DumpAll();

    // test roundtrip
    LOG_DBG << "Test Roundtrip at root";
    for (Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip(); it != tree.EndRoundtrip(); ++it) {
        LOG_DBG1 << it->Node()->Dump();
        if (it->Node()->data.name == "L") {
            n = it->Node();
        }
    }
    LOG_DBG << "Test Roundtrip at " + n->data.name;
    for (Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip(n); it != tree.EndRoundtrip(); ++it) {
        LOG_DBG1 << it->Node()->Dump();
    }

    // test preorder
    LOG_DBG << "Test Preorder at root";
    for (Tree<>::IteratorPreorder it = tree.BeginPreorder(); it != tree.EndPreorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Preorder at " + n->data.name;
    for (Tree<>::IteratorPreorder it = tree.BeginPreorder(n); it != tree.EndPreorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }

    // * /
    // test inorder
    LOG_DBG << "Test Inorder at root";
    for (Tree<>::IteratorInorder it = tree.BeginInorder(); it != tree.EndInorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "G") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Inorder at " + n->data.name;
    for (Tree<>::IteratorInorder it = tree.BeginInorder(n); it != tree.EndInorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }
    * /

    // test postorder
    LOG_DBG << "Test Postorder at root";
    for (Tree<>::IteratorPostorder it = tree.BeginPostorder(); it != tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Postorder at " + n->data.name;
    for (Tree<>::IteratorPostorder it = tree.BeginPostorder(n); it != tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }

    / *
    // test levelorder
    LOG_DBG << "Test Levelorder at root";
    for (Tree<>::IteratorLevelorder it = tree.BeginLevelorder(); it != tree.EndLevelorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Levelorder at " + n->data.name;
    for (Tree<>::IteratorLevelorder it = tree.BeginLevelorder(n); it != tree.EndLevelorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }
    //*/

    // -----------------------------------------------------
    //     Test cases for newick parser
    // -----------------------------------------------------
    //~ tree.FromNewickString("((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;");

    //~ tree.FromNewickString("( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};");
    //~ tree.FromNewickString("( (] Ant:0.2, [a comment] 'Bee':0.09 )Innen:0.7, Coyote:0.5 );");

    //~ LOG_INFO << "no nodes are named: (,,(,));";
    //~ tree.FromNewickString("(,,(,));");
//~
    //~ LOG_INFO << "leaf nodes are named: (A,B,(C,D));";
    //~ tree.FromNewickString("(A,B,(C,D));");
//~
    //~ LOG_INFO << "all nodes are named: (A,B,(C,D)E)F;";
    //~ tree.FromNewickString("(A,B,(C,D)E)F;");
//~
    //~ LOG_INFO << "all but root node have a distance to parent: (:0.1,:0.2,(:0.3,:0.4):0.5);";
    //~ tree.FromNewickString("(:0.1,:0.2,(:0.3,:0.4):0.5);");
//~
    //~ LOG_INFO << "all have a distance to parent: (:0.1,:0.2,(:0.3,:0.4):0.5):0.0;";
    //~ tree.FromNewickString("(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;");
//~
    //~ LOG_INFO << "distances and leaf names (popular): (A:0.1,B:0.2,(C:0.3,D:0.4):0.5);";
    //~ tree.FromNewickString("(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);");
//~
    //~ LOG_INFO << "distances and all names: (A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";
    //~ tree.FromNewickString("(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;");
//~
    //~ LOG_INFO << "a tree rooted on a leaf node (rare): ((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;";
    //~ tree.FromNewickString("((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;");

    // -----------------------------------------------------
    //     Test for json parser
    // -----------------------------------------------------
    //~ JsonLexer lex;
    //~ lex.ProcessString(FileRead("test/data/placement.jplace"));
    //~ LOG_INFO << lex.Dump();

    //~ JsonDocument doc;
    //~ JsonParser::ProcessLexer(lex, doc);
    //~ LOG_INFO << doc.ToString();

    //~ JsonDocument doc;

    // -----------------------------------------------------
    //     Test for placements, earth movers distance, center of gravity
    // -----------------------------------------------------
    Placements place_a, place_b;
    JplaceParser::ProcessFile("test/data/test_a.jplace", place_a);
    JplaceParser::ProcessFile("test/data/test_b.jplace", place_b);
    //~ LOG_DBG << place_a.DumpAll() << "\nvalid: " << place_a.Validate();
    //~ place_a.EMD(place_b);
    //~ place_a.COG();
    LOG_DBG << place_a.DumpPqueries();
    LOG_DBG << place_b.DumpPqueries();
    place_a.Merge(place_b);
    place_b.pqueries[1]->names[0].name = "huhu";
    LOG_DBG << place_a.DumpPqueries();
    LOG_DBG << place_b.DumpPqueries();

    // -----------------------------------------------------
    //     Program exit routines
    // -----------------------------------------------------
    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "ende";

    return 0;
}
