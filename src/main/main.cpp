/**
 * @brief Main function of genesis.
 *
 * @file
 * @ingroup main
 */

#include "main/main.hpp"

#include <iostream>
#include <string>

#include "placement/jplace_processor.hpp"
#include "placement/placements.hpp"
#include "tree/newick_processor.hpp"
#include "tree/phyloxml_processor.hpp"

#include "utils/json_document.hpp"
#include "utils/json_processor.hpp"

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
    // =============================================================================
    //     Program startup routines
    // =============================================================================

    Logging::AddOutputStream(std::cout);
    //~ Logging::details.level = false;
    //~ Logging::details.file = true;
    //~ Logging::details.line = true;
    //~ Logging::details.function = true;
    //~ Logging::details.time = true;
    Logging::details.runtime = true;
    //~ Logging::details.rundiff = true;
    LOG_BOLD << print_header();
    LOG_TIME << "started";

    // =============================================================================
    //     Test cases for tree iterators
    // =============================================================================

    /*
    Tree<> tree;
    TreeNode<DefaultNodeData, DefaultEdgeData>* n;
    tree.FromNewickString("((A,((B,C,D)E,F)G)H,((I,J,K)L,M,N)O,P,Q)R;");
    //~ tree.FromNewickString("((A,((B,C)D,E)F)G,((H,((I,J)K,L)M)N,O)P,Q)R;");
    LOG_DBG << tree.Dump();

    // test roundtrip
    LOG_DBG << "Test Roundtrip at root";
    for (Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip(); it != tree.EndRoundtrip(); ++it) {
        LOG_DBG1 << it->Node()->Dump();
        if (it->Node()->name == "L") {
            n = it->Node();
        }
    }
    LOG_DBG << "Test Roundtrip at " + n->name;
    for (Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip(n); it != tree.EndRoundtrip(); ++it) {
        LOG_DBG1 << it->Node()->Dump();
    }

    // test preorder
    LOG_DBG << "Test Preorder at root";
    for (Tree<>::IteratorPreorder it = tree.BeginPreorder(); it != tree.EndPreorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
        if (it->name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Preorder at " + n->name;
    for (Tree<>::IteratorPreorder it = tree.BeginPreorder(n); it != tree.EndPreorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
    }

    // * /
    // test inorder
    LOG_DBG << "Test Inorder at root";
    for (Tree<>::IteratorInorder it = tree.BeginInorder(); it != tree.EndInorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
        if (it->name == "G") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Inorder at " + n->name;
    for (Tree<>::IteratorInorder it = tree.BeginInorder(n); it != tree.EndInorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
    }
    * /

    // test postorder
    LOG_DBG << "Test Postorder at root";
    for (Tree<>::IteratorPostorder it = tree.BeginPostorder(); it != tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
        if (it->name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Postorder at " + n->name;
    for (Tree<>::IteratorPostorder it = tree.BeginPostorder(n); it != tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
    }

    / *
    // test levelorder
    LOG_DBG << "Test Levelorder at root";
    for (Tree<>::IteratorLevelorder it = tree.BeginLevelorder(); it != tree.EndLevelorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
        if (it->name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Levelorder at " + n->name;
    for (Tree<>::IteratorLevelorder it = tree.BeginLevelorder(n); it != tree.EndLevelorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name
                 + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
    }
    //*/

    // =============================================================================
    //     Test cases for newick parser and printer
    // =============================================================================

    //~ std::string ts = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    //~ LOG_DBG << "In tree:  " << ts;
    //~ Tree<> tree;
    //~ tree.FromNewickString(ts);
    //~ NewickProcessor::FromString(ts, tree);
//~
    //~ for (
        //~ Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip();
        //~ it != tree.EndRoundtrip();
        //~ ++it
    //~ ) {
        //~ LOG_DBG1 << "Round " << it.Node()->name;
    //~ }
    //~ LOG_DBG << "Out tree: " << NewickProcessor::ToString(tree);

    //~ tree.Export();
    //~ NewickProcessor::ToFile("/home/file.nw", tree);


    //~ tree.FromNewickString("(((F,(G,H)I)J,K)L,(A,(B,C)D)E)R;");
    //~ LOG_DBG << "tree: " << tree.ToNewickString();

    //~ std::string ts = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    //~ LOG_DBG << "In tree:  " << ts;
    //~ TreeBroker broker;
    //~ NewickParser::ProcessString(ts, broker);
    //~ LOG_DBG << "Broker:   " << broker.Dump();
    //~ LOG_DBG << "Out tree: " << NewickPrinter::ToString(broker);

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

    // =============================================================================
    //     Test cases for phyloxml parser and printer
    // =============================================================================

    //~ std::string ts = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    //~ std::string ts = "((B,(D,E)C)A,F,(H,I)G)R;";
    //~ LOG_DBG << "In tree:  " << ts;
    //~ Tree<> tree;
    //~ NewickProcessor::FromString(ts, tree);
    //~ LOG_DBG << "Out tree: " << PhyloXmlProcessor::ToString(tree);

    // =============================================================================
    //     Test for json parser
    // =============================================================================

    //~ JsonDocument doc;
    //~ JsonLexer lex;
    //~ lex.ProcessString(FileRead("test/data/placement.jplace"));
    //~ JsonParser::ProcessLexer(lex, doc);
    //~ LOG_INFO << lex.Dump();

    //~ JsonDocument doc;
    //~ JsonProcessor::FromFile("test/data/placement.jplace", doc);
    //~ LOG_INFO << doc.ToString();
    //~ LOG_INFO << JsonProcessor::ToString(doc);

    // =============================================================================
    //     Test for placements, earth movers distance, center of gravity
    // =============================================================================
    /*
    NewickProcessor::print_names          = true;
    NewickProcessor::print_branch_lengths = true;
    NewickProcessor::print_comments       = true;
    NewickProcessor::print_tags           = false;

    LOG_DBG << "Test 1";
    Placements place_a, place_b;
    JplaceProcessor::FromFile("test/data/test_a.jplace", place_a);
    JplaceProcessor::FromFile("test/data/test_b.jplace", place_b);
    LOG_DBG << "Valid A: " << place_a.Validate();
    LOG_DBG << "Valid B: " << place_b.Validate();
    LOG_DBG << "Dump A:\n" << place_a.Dump();
    LOG_DBG << "Dump B:\n" << place_b.Dump();
//~
    //~ LOG_DBG << "EMD: " << Placements::EMD(place_a, place_b);
    // * /
//~
    LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    place_a.RestrainToMaxWeightPlacements();
    place_b.RestrainToMaxWeightPlacements();
    LOG_DBG << "Valid A: " << place_a.Validate();
    LOG_DBG << "Valid B: " << place_b.Validate();
    LOG_DBG << "Dump A:\n" << place_a.Dump();
    LOG_DBG << "Dump B:\n" << place_b.Dump();
//~
    //~ LOG_DBG << "Tree A: " << NewickProcessor::ToString(place_a.tree);
    //~ LOG_DBG << "Tree B: " << NewickProcessor::ToString(place_b.tree);

    //~ place_a.COG();
    LOG_DBG << "Variance: " << place_a.Variance();

    //~ LOG_DBG << "================================";
    //~ LOG_DBG << "Test 2";
    //~ Placements place;
    //~ place.FromJplaceFile("test/data/placement.jplace");
    //~ LOG_DBG << "valid: " << place.Validate();
    //~ LOG_DBG << place.Dump();
    //~ LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    //~ place.RestrainToMaxWeightPlacements();
    //~ LOG_DBG << "valid: " << place.Validate();
    //~ LOG_DBG << place.Dump();
    //~ LOG_DBG << "Tree: " << NewickProcessor::ToString(place.tree);
    //*/

    // =============================================================================
    //     Further tests for placements and validation
    // =============================================================================

    //*
    Placements place_a, place_b;
    LOG_DBG << "Reading file...";
    //~ JplaceProcessor::FromFile("test/data/test_a.jplace", place_a);
    JplaceProcessor::FromFile("test/data/RAxML_portableTree.split_0.jplace", place_a);
    //~ JplaceProcessor::FromFile("test/data/RAxML_portableTree.split_1.jplace", place_b);
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();

    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_0.newick", place_a.tree);
    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_1.newick", place_b.tree);

    //~ LOG_DBG << "tree a edge count " << place_a.tree.EdgesSize();

    //~ LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();
    //~ LOG_DBG << "Merging A and B...";
    //~ place_a.Merge(place_b);
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();
    //~ LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();

    LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    place_a.RestrainToMaxWeightPlacements();
    //~ place_b.RestrainToMaxWeightPlacements();
    //~ LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();
    LOG_DBG << "Calculating variance...";
    LOG_DBG << "Variance " << place_a.Variance();

    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_both.newick", place_a.tree);
    //*/
    /*
    // test postorder
    PlacementTree::NodeType* n;
    LOG_DBG << "Test Postorder at root";
    for (PlacementTree::IteratorPostorder it = place_a.tree.BeginPostorder(); it != place_a.tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
        if (it->name == "G") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Postorder at " + n->name;
    for (PlacementTree::IteratorPostorder it = place_a.tree.BeginPostorder(n); it != place_a.tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->name + "' to '" + it.Edge()->SecondaryNode()->name + "'" : "");
    }
    //*/

    // =============================================================================
    //     Using placements on real data for tropcial soils project
    // =============================================================================

    // CAVEAT: paths are hardcoded - this is for testing only!

    /*
    NewickProcessor::print_names          = true;
    NewickProcessor::print_branch_lengths = true;
    NewickProcessor::print_comments       = true;
    NewickProcessor::print_tags           = false;

    std::string path = "/home/lucas/Projects/tropical-soils/04_EPA/papara_splits/";
    LOG_DBG << "Running on data in " << path;
    std::vector<std::string> list;
    DirListFiles(path, list);
    if (list.size() == 0) {
        LOG_WARN << "No files found.";
        return 0;
    }
    LOG_DBG << "Found " << list.size() << " files.";
    //~ for (std::string s : list) {
        //~ LOG_DBG1 << s;
    //~ }

    Placements place, place_tmp;
    LOG_DBG << "Reading file " << list[0] << "...";
    JplaceProcessor::FromFile(path + list[0], place);
    //~ LOG_DBG << "Valid: " << place.Validate();
    LOG_DBG << "with " << place.PlacementCount() << " placements.";

    for (size_t i = 1; i < list.size(); ++i) {
        LOG_DBG << "Reading file " << list[i] << "...";
        JplaceProcessor::FromFile(path + list[i], place_tmp);
        LOG_DBG << "with " << place_tmp.PlacementCount() << " placements.";

        LOG_DBG << "... and merging it.";
        place.Merge(place_tmp);
    }

    LOG_DBG << "Total of " << place.PlacementCount() << " placements.";
    //~ LOG_DBG << "Validating...";
    //~ LOG_DBG << "Validity: " << place.Validate();

    LOG_DBG << "Applying RestrainToMaxWeightPlacements...";
    place.RestrainToMaxWeightPlacements();
    LOG_DBG << "Total of " << place.PlacementCount() << " placements.";
    //~ LOG_DBG << "Validating...";
    //~ LOG_DBG << "Validity: " << place.Validate();

    NewickProcessor::ToFile(path + "total_tree.newick", place.tree);
    //*/

    // =============================================================================
    //     Program exit routines
    // =============================================================================

    // anti compiler complaints
    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "finished";

    return 0;
}
