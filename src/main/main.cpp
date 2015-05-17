/**
 * @brief Main function of genesis.
 *
 * @file
 * @ingroup main
 */

#include <iostream>
#include <fstream>
#include <string>

#include "utils/options.hpp"

#include "alignment/sequence_set.hpp"
#include "alignment/fasta_processor.hpp"
#include "alignment/phylip_processor.hpp"

#include "placement/jplace_processor.hpp"
#include "placement/placement_map.hpp"
#include "tree/bipartitions.hpp"
#include "tree/newick_processor.hpp"
#include "tree/phyloxml_processor.hpp"

#include "utils/json_document.hpp"
#include "utils/json_processor.hpp"

#include "utils/bitvector.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

#include "plausibility/plausibility.hpp"

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

    Logging::LogToStdout();
    //~ Logging::details.level = true;
    //~ Logging::details.file = true;
    //~ Logging::details.line = true;
    //~ Logging::details.function = true;
    //~ Logging::details.time = true;
    //~ Logging::details.date = true;
    //~ Logging::details.runtime = true;
    //~ Logging::details.rundiff = true;
    LOG_BOLD << print_header();
    Options::Init(argc, argv);
    LOG_TIME << "started";

    //~ Plausibility p;
    //~ p.SpiderpigFunction("largetree.txt", "reference-two-big.txt");

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

    */
    /*

    //~ LOG_DBG << "================================";
    //~ LOG_DBG << "Test 2";
    PlacementMap place;
    JplaceProcessor::FromFile("test/data/placement.jplace", place);
    LOG_DBG << "valid: " << place.Validate();
    LOG_DBG << place.Dump();
    LOG_DBG << "json " << JplaceProcessor::ToString(place);
    //~ LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    //~ place.RestrainToMaxWeightPlacements();
    //~ LOG_DBG << "valid: " << place.Validate();
    //~ LOG_DBG << place.Dump();
    //~ LOG_DBG << "Tree: " << NewickProcessor::ToString(place.tree);
    //*/

    // =============================================================================
    //     Further tests for placements and validation
    // =============================================================================

    /*
    PlacementMap place_a, place_b;
    LOG_DBG << "Reading file...";
    JplaceProcessor::FromFile("test/data/test_40K.jplace", place_a);
    //~ JplaceProcessor::FromFile("test/data/test_a.jplace", place_a);
    //~ JplaceProcessor::FromFile("test/data/RAxML_portableTree.split_0.jplace", place_a);
    //~ JplaceProcessor::FromFile("test/data/RAxML_portableTree.split_1.jplace", place_b);
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();

    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_0.newick", place_a.tree);
    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_1.newick", place_b.tree);

    // ----------------------------
    //     Debug
    // ----------------------------

    //~ LOG_DBG << "tree a edge count " << place_a.tree.EdgeCount();

    //~ LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();
    //~ LOG_DBG << "Merging A and B...";
    //~ place_a.Merge(place_b);
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();
    //~ LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();

    //~ LOG_DBG << "Copying a to b...";
    //~ place_b = place_a;
    //~ LOG_DBG << "Validating a...";
    //~ LOG_DBG << "Valid a: " << place_a.Validate();
    //~ LOG_DBG << "Validating b...";
    //~ LOG_DBG << "Valid b: " << place_b.Validate();

    // ----------------------------
    //     Restrain To Max Weight Placements
    // ----------------------------

    LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    place_a.RestrainToMaxWeightPlacements();
    //~ place_b.RestrainToMaxWeightPlacements();
    //~ LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();
    LOG_DBG << "count a " << place_a.PlacementCount();
    //*/

    // ----------------------------
    //     Variance, EMD
    // ----------------------------

    /*
    LOG_DBG << "Calculating LeafDepthHistogram...";
    std::vector<int> depth_hist = place_a.ClosestLeafDepthHistogram();
    for (unsigned int d = 0; d < depth_hist.size(); ++d) {
        LOG_DBG1 << "Depth " << d << " with " << depth_hist[d] << " placements.";
    }

    LOG_DBG << "Calculating LeafDistanceHistogram...";
    double min = 0, max = 3;
    std::vector<int> dist_hist = place_a.ClosestLeafDistanceHistogram(min, max, 50);
    LOG_DBG << "min(" << min << "), max(" << max << ")";
    for (unsigned int d = 0; d < dist_hist.size(); ++d) {
        LOG_DBG1 << "Distance bin " << d << " with " << dist_hist[d] << " placements.";
    }

    LOG_DBG << "Calculating LeafDistanceHistogramAuto...";
    double mina, maxa;
    int binsa = 10;
    std::vector<int> dist_hista = place_a.ClosestLeafDistanceHistogramAuto(mina, maxa, binsa);
    double bin_size = (maxa - mina) / binsa;
    LOG_DBG << "min(" << mina << "), max(" << maxa << ")";
    for (unsigned int d = 0; d < dist_hista.size(); ++d) {
        LOG_DBG1 << "Distance bin " << d << " [" << d*bin_size << "; " << (d+1)*bin_size << ") has " << dist_hista[d] << " placements.";
    }
    //*/

    //~ LOG_DBG << "Calculating variance...";
    //~ LOG_DBG << "Variance " << place_a.Variance();

    //~ LOG_DBG << "EMD " << place_a.EMD(place_a);

    // ----------------------------
    //     Debug
    // ----------------------------

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
    //     Testing leaf depth, distance, bipartitions
    // =============================================================================

    /*
    //~ std::string ts = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    //~ LOG_DBG << "In tree:  " << ts;
    Tree<> tree;
    //~ NewickProcessor::FromString(ts, tree);
    NewickProcessor::FromFile("test/data/best_tree.newick", tree);

    NewickBroker broker;
    NewickProcessor::ToBroker(broker, tree);
    broker.AssignRanks();
    //~ LOG_DBG << "Broker dump:\n" << broker.Dump();
    //~ LOG_DBG << "tree dump:\n" << tree.Dump();

    //~ Tree<>::NodeIntVectorType vec = tree.ClosestLeafDepthVector();
    //~ int i = 0;
    //~ for (std::pair<const Tree<>::NodeType*, int> p : vec) {
        //~ LOG_DBG << "node " << i << " name(" << tree.NodeAt(i)->name << ") has closest " << p.first->Index() << " name(" << tree.NodeAt(p.first->Index())->name << ") width depth " << p.second;
        //~ ++i;
    //~ }

    Bipartitions<DefaultNodeData, DefaultEdgeData> bi = Bipartitions<DefaultNodeData, DefaultEdgeData>(&tree);
    Tree<>::NodeType* n1 = tree.FindNode("AY919771_clone_LG25_05_Alveolata");
    Tree<>::NodeType* n2 = tree.FindNode("AB000912_Tridacna_parasite_Apicomplexa");
    if (n1 == nullptr) {
        LOG_DBG << "coundnt find n1";
    }
    if (n2 == nullptr) {
        LOG_DBG << "coundnt find n2";
    }
    LOG_DBG << "found nodes: " << n1->name << " and " << n2->name;
    std::vector<Tree<>::NodeType*> l;
    l.push_back(n1);
    l.push_back(n2);
    bi.GetSubtreeEdges(l);
    //~ bi.Make();
    //~ LOG_DBG << bi.Dump();

    //*/

    // =============================================================================
    //     Bitvector tests
    // =============================================================================

    //~ Bitvector bv1(86, true);
    //~ Bitvector bv2(12);
    //~ bv1.Flip(3);
    //~ bv1 = ~bv1;
    //~ bv2.Set(5);
    //~ bv2 |= bv1;
    //~ LOG_DBG << "1 " << bv1.Dump() << "\ncount " << bv1.Count();
    //~ LOG_DBG << "2 " << bv2.Dump() << "\ncount " << bv2.Count();

    // =============================================================================
    //     Using placements on real data for tropcial soils project
    // =============================================================================

    // CAVEAT: paths are hardcoded - this is for testing only!

    // --------------------------------------------------------
    //     Read and merge splitted placements
    // --------------------------------------------------------

    /*

    std::string  inpath = "/home/lucas/Dropbox/HITS/tropical-soils/pipe_03/04_EPA/papara_splits/";
    std::string outpath = "/home/lucas/Dropbox/HITS/tropical-soils/pipe_03/05_genesis/";

    LOG_DBG << "Running on data in " << inpath;
    std::vector<std::string> list;
    DirListFiles(inpath, list);
    if (list.size() == 0) {
        LOG_WARN << "No files found.";
        return 0;
    }
    LOG_DBG << "Found " << list.size() << " files.";
    //~ for (std::string s : list) {
        //~ LOG_DBG1 << s;
    //~ }

    PlacementMap place, place_tmp;
    LOG_DBG;
    LOG_DBG << "Reading file " << list[0] << "...";
    JplaceProcessor::FromFile(inpath + list[0], place);
    //~ LOG_DBG << "Valid: " << place.Validate();
    LOG_DBG << "with " << place.PlacementCount() << " placements.";
    LOG_DBG;

    for (size_t i = 1; i < list.size(); ++i) {
        LOG_DBG1 << "Reading file " << list[i] << "...";
        JplaceProcessor::FromFile(inpath + list[i], place_tmp);
        LOG_DBG1 << "...with " << place_tmp.PlacementCount() << " placements...";

        LOG_DBG1 << "... and merging it.";
        place.Merge(place_tmp);
        LOG_DBG1;
    }

    LOG_DBG << "Total of " << place.PlacementCount() << " placements.";
    //~ LOG_DBG << "Validating...";
    //~ LOG_DBG << "Validity: " << place.Validate();

    LOG_DBG << "Applying RestrainToMaxWeightPlacements...";
    place.RestrainToMaxWeightPlacements();
    LOG_DBG << "Total of " << place.PlacementCount() << " placements.";
    //~ LOG_DBG << "Validating...";
    //~ LOG_DBG << "Validity: " << place.Validate();

    LOG_DBG << "Writing jplace file to " << outpath;
    JplaceProcessor::ToFile(outpath + "all_placements.jplace", place);

    LOG_DBG << "Writing count tree...";
    NewickProcessor::print_names          = true;
    NewickProcessor::print_branch_lengths = true;
    NewickProcessor::print_comments       = true;
    NewickProcessor::print_tags           = false;
    NewickProcessor::ToFile(outpath + "total_tree.newick", place.tree);

    //*/

    // --------------------------------------------------------
    //     Process big placement
    // --------------------------------------------------------

    /*

    std::string  inpath = "/home/lucas/Dropbox/HITS/tropical-soils/pipe_03/05_genesis/";
    std::string outpath = "/home/lucas/Dropbox/HITS/tropical-soils/pipe_03/05_genesis/";

    LOG_DBG << "Reading file " << "all_placements.jplace" << "...";
    PlacementMap place;
    JplaceProcessor::FromFile(inpath + "all_placements.jplace", place);
    LOG_DBG << "with " << place.PlacementCount() << " placements.";
    LOG_DBG;

    //~ LOG_DBG << "Writing count tree...";
    //~ NewickProcessor::print_names          = true;
    //~ NewickProcessor::print_branch_lengths = true;
    //~ NewickProcessor::print_comments       = true;
    //~ NewickProcessor::print_tags           = false;
    //~ NewickProcessor::ToFile(outpath + "total_tree_x.newick", place.tree);

    //*/

    /*
    LOG_DBG << "Calculating LeafDepthHistogram...";
    std::vector<int> depth_hist = place.ClosestLeafDepthHistogram();
    for (unsigned int d = 0; d < depth_hist.size(); ++d) {
        LOG_DBG1 << "Depth \t" << d << "\t with \t" << depth_hist[d] << "\t placements.";
    }

    LOG_DBG << "Calculating LeafDistanceHistogramAuto...";
    double mina, maxa;
    int binsa = 25;
    std::vector<int> dist_hista = place.ClosestLeafDistanceHistogramAuto(mina, maxa, binsa);
    double bin_sizea = (maxa - mina) / binsa;
    LOG_DBG << "min(" << mina << "), max(" << maxa << ")";
    for (unsigned int d = 0; d < dist_hista.size(); ++d) {
        //~ LOG_DBG1 << "Distance bin \t" << d << "\t [\t" << d*bin_sizea << "\t; \t" << (d+1)*bin_sizea << "\t) has \t" << dist_hista[d] << "\t placements.";
        LOG_DBG1 << "\t"  << d << "\t" << d*bin_sizea << "\t" << (d+1)*bin_sizea << "\t" << dist_hista[d];
    }

    LOG_DBG << "Calculating LeafDistanceHistogram...";
    double min = 0, max = 2;
    int bins = 25;
    double bin_size = (max - min) / bins;
    std::vector<int> dist_hist = place.ClosestLeafDistanceHistogram(min, max, bins);
    LOG_DBG << "min(" << min << "), max(" << max << ")";
    for (unsigned int d = 0; d < dist_hist.size(); ++d) {
        //~ LOG_DBG1 << "Distance bin " << d << " with " << dist_hist[d] << " placements.";
        LOG_DBG1 << "\t" << d << "\t" << d*bin_size << "\t" << (d+1)*bin_size << "\t" << dist_hist[d];
    }

    LOG_DBG << "deepest dist: " << place.tree.DeepestDistance();

    //*/
    /*

    LOG_DBG << "Using bipartitions for subtree anaysis.";
    PlacementTree::NodeType* n1 = place.tree.FindNode("AY919771_clone_LG25_05_Alveolata");
    PlacementTree::NodeType* n2 = place.tree.FindNode("AB000912_Tridacna_parasite_Apicomplexa");
    if (n1 == nullptr) {
        LOG_DBG << "coundnt find n1";
        return 0;
    }
    if (n2 == nullptr) {
        LOG_DBG << "coundnt find n2";
        return 0;
    }
    LOG_DBG << "Found nodes: " << n1->name << " and " << n2->name;

    LOG_DBG << "Extracting reads on subtree...";
    std::vector<PlacementTree::NodeType*> l;
    l.push_back(n1);
    l.push_back(n2);

    Bipartitions<PlacementNodeData, PlacementEdgeData> bi = Bipartitions<PlacementNodeData, PlacementEdgeData>(&place.tree);
    Bipartition<PlacementNodeData, PlacementEdgeData>* subtree = bi.FindSmallestSubtree(l);
    std::vector<const PlacementTree::EdgeType*> subedges = bi.GetSubtreeEdges(subtree->Link());
    std::string readnames = "";
    for (const PlacementTree::EdgeType* e : subedges) {
        for (PqueryPlacement* p : e->placements) {
            if (p->pquery->names.size() != 1) {
                LOG_DBG << "placement with names " << p->pquery->names.size();
                continue;
            }
            readnames += p->pquery->names[0]->name + "\n";
        }
    }
    FileWrite(outpath + "reads", readnames);
    //~ bi.Make();
    //~ LOG_DBG << bi.Dump();

    //*/

    // --------------------------------------------------------
    //     Alignments and Sequences
    // --------------------------------------------------------

    //*

    //~ std::string  inpath = "/home/lucas/Dropbox/HITS/tropical-soils/pipe_04/";
    //~ std::string outpath = "/home/lucas/Dropbox/HITS/tropical-soils/pipe_04/";

    //~ std::string  inpath = "/home/lucas/Dropbox/HITS/tropical-soils/data_raw/";
//~
    //~ SequenceSet aln;
    //~ FastaProcessor::FromFile(inpath + "Apis_all_4_mafft_clean.fasta", aln);
    //~ aln.Replace('N', '-');
    //~ PhylipProcessor::ToFile(inpath + "Apis_all_4_mafft_clean.phy", aln);
//~
    //~ return 0;
//~
    //~ SequenceSet reduced_aln;
    //~ std::ifstream infile(inpath + "subtree_taxa");
    //~ std::string line;
    //~ while (std::getline(infile, line))
    //~ {
        //~ reduced_aln.sequences.push_back(aln.FindSequence(line));
    //~ }
//~
    //~ FastaProcessor::line_length = 0;
    //~ FastaProcessor::ToFile(inpath + "512.fasta", aln);
    //~ FastaProcessor::ToFile(inpath + "512_subtree.fasta", reduced_aln);
//~
    //~ for (Sequence* s : reduced_aln.sequences) {
        //~ s->RemoveGaps();
    //~ }
    //~ FastaProcessor::ToFile(inpath + "512_subtree_unaligned.fasta", reduced_aln);

    /*
    SequenceSet aln;
    LOG_DBG << "reading fas";
    FastaProcessor::FromFile(inpath + "third_20_samples_LaSelva_protists.fas", aln);

    LOG_DBG << "remove all but subtree_reads";
    //~ SequenceSet reduced_aln;
    std::vector<std::string> subtree_reads;
    std::ifstream infile(inpath + "subtree_reads");
    std::string line;
    while (std::getline(infile, line))
    {
        //~ reduced_aln.sequences.push_back(aln.FindSequence(line));
        subtree_reads.push_back(line);
    }
    aln.RemoveList(subtree_reads, true);

    LOG_DBG << "write new fas";
    FastaProcessor::line_length = 0;
    FastaProcessor::ToFile(inpath + "subtree_reads2.fasta", aln);

    //*/

    // =============================================================================
    //     Program exit routines
    // =============================================================================
    LOG_TIME << "finished";

    return 0;
}
