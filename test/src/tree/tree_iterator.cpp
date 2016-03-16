/**
 * @brief Testing Tree Iterators.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>
#include <utility>

#include "lib/tree/default/functions.hpp"
#include "lib/tree/default/newick_processor.hpp"
#include "lib/tree/io/newick/processor.hpp"
#include "lib/tree/tree.hpp"
#include "tree/iterator/eulertour.hpp"
#include "tree/iterator/postorder.hpp"
#include "tree/iterator/preorder.hpp"

using namespace genesis;
using namespace tree;

// =================================================================================================
//     Eulertour
// =================================================================================================

/*
class Eulertour : public ::testing::TestWithParam< std::pair<std::string, std::string> >
{};

TEST_P (Eulertour, Test)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    DefaultTree tree;
    DefaultTreeNewickProcessor().from_string(input, tree);

    auto node = tree.find_node(GetParam().first);
    ASSERT_NE(nullptr, node);

    for (auto it = tree.begin_eulertour(node); it != tree.end_eulertour(); ++it) {
        nodes += it.node()->name;
    }
    EXPECT_EQ(GetParam().second, nodes) << " with start node " << GetParam().first;
}

INSTANTIATE_TEST_CASE_P (TreeIterator, Eulertour, ::testing::Values(
    std::make_pair("R", "RABACDCECARFRGHGIG"),
    std::make_pair("A", "ARFRGHGIGRABACDCEC"),
    std::make_pair("B", "BACDCECARFRGHGIGRA"),
    std::make_pair("C", "CARFRGHGIGRABACDCE"),
    std::make_pair("D", "DCECARFRGHGIGRABAC"),
    std::make_pair("E", "ECARFRGHGIGRABACDC"),
    std::make_pair("F", "FRGHGIGRABACDCECAR"),
    std::make_pair("G", "GRABACDCECARFRGHGI"),
    std::make_pair("H", "HGIGRABACDCECARFRG"),
    std::make_pair("I", "IGRABACDCECARFRGHG")
));
*/

/*

    TODO this is a test for const correctness.
    currently, it is not const correct, for many reasons:
      * the free function and iterator wrapper do not handle constness
      * find_node takes a const tree, but returns a non const node
      * this again is possible since the unique ptr used in tree to store the nodes
        allows for this. thus, we need a wrapper iterator around the vec of uniq ptr
        for the tree members that does not allow this kind of conversion.
      * on the other hand, that might result in need for two versions for each
        free function like find_node: a const and non const version. this is ugly.
      * so far, i don't see a proper clean solution for that. need to consult someone
        who knows more about this issue...

    so for now, we leave it in this dirty state. be warned!

 */
void do_test(const std::string node_name, const std::string expected_nodes, DefaultTree const& tree)
{
    std::string resulting_nodes = "";

    // Find the Node for this test run.
    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    // Do a normal traversal.
    for (auto it = tree.begin_eulertour(node); it != tree.end_eulertour(); ++it) {
        resulting_nodes += it.node()->data.name;
        // it.node()->data.name = "bla";
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // Use free function iterator wrapper.
    resulting_nodes = "";
    for (auto it = eulertour(node).begin(); it != eulertour(node).end(); ++it) {
        resulting_nodes += it.node()->data.name;
        // it.node()->data.name = "bla";
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // Do range-based for loop traversal.
    resulting_nodes = "";
    // for (auto& node : eulertour(tree)) {
    for (auto& node_it : eulertour(node)) {
        resulting_nodes += node_it.data.name;
        // node.data.name = "bla";
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // LOG_DBG << "did it\n=============";
}

void TestEulertour(const std::string node_name, const std::string expected_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    // std::string resulting_nodes = "";

    // Prepare Tree.
    DefaultTree tree;
    DefaultTreeNewickProcessor().from_string(input, tree);

    do_test(node_name, expected_nodes, tree);

    /*
    // Find the Node for this test run.
    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    // Do a normal traversal.
    for (auto it = tree.begin_eulertour(node); it != tree.end_eulertour(); ++it) {
        resulting_nodes += it.node()->data.name;
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;

    // Do range-based for loop traversal.
    resulting_nodes = "";
    for (auto& it : eulertour(node)) {
        resulting_nodes += it.data.name;
    }
    EXPECT_EQ(expected_nodes, resulting_nodes) << " with start node " << node_name;
    */
}

TEST (TreeIterator, Eulertour)
{
    TestEulertour("R", "RABACDCECARFRGHGIG");
    TestEulertour("A", "ARFRGHGIGRABACDCEC");
    TestEulertour("B", "BACDCECARFRGHGIGRA");
    TestEulertour("C", "CARFRGHGIGRABACDCE");
    TestEulertour("D", "DCECARFRGHGIGRABAC");
    TestEulertour("E", "ECARFRGHGIGRABACDC");
    TestEulertour("F", "FRGHGIGRABACDCECAR");
    TestEulertour("G", "GRABACDCECARFRGHGI");
    TestEulertour("H", "HGIGRABACDCECARFRG");
    TestEulertour("I", "IGRABACDCECARFRGHG");
}

// =================================================================================================
//     Preorder
// =================================================================================================

void TestPreorder(std::string node_name, std::string out_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    DefaultTree tree;
    DefaultTreeNewickProcessor().from_string(input, tree);

    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    for (auto it = tree.begin_preorder(node); it != tree.end_preorder(); ++it) {
        nodes += it.node()->data.name;
    }
    EXPECT_EQ(out_nodes, nodes) << " with start node " << node_name;
}

TEST (TreeIterator, Preorder)
{
    TestPreorder("R", "RABCDEFGHI");
    TestPreorder("A", "ARFGHIBCDE");
    TestPreorder("B", "BACDERFGHI");
    TestPreorder("C", "CARFGHIBDE");
    TestPreorder("D", "DCEARFGHIB");
    TestPreorder("E", "ECARFGHIBD");
    TestPreorder("F", "FRGHIABCDE");
    TestPreorder("G", "GRABCDEFHI");
    TestPreorder("H", "HGIRABCDEF");
    TestPreorder("I", "IGRABCDEFH");
}

// =================================================================================================
//     Postorder
// =================================================================================================

void TestPostorder(std::string node_name, std::string out_nodes)
{
    std::string input = "((B,(D,E)C)A,F,(H,I)G)R;";
    std::string nodes = "";

    DefaultTree tree;
    DefaultTreeNewickProcessor().from_string(input, tree);

    auto node = find_node(tree, node_name);
    ASSERT_NE(nullptr, node);

    for (auto it = tree.begin_postorder(node); it != tree.end_postorder(); ++it) {
        nodes += it.node()->data.name;
    }
    EXPECT_EQ(out_nodes, nodes) << " with start node " << node_name;
}

TEST (TreeIterator, Postorder)
{
    TestPostorder("R", "BDECAFHIGR");
    TestPostorder("A", "FHIGRBDECA");
    TestPostorder("B", "DECFHIGRAB");
    TestPostorder("C", "FHIGRBADEC");
    // TestPostorder("D", "");
    // TestPostorder("E", "");
    // TestPostorder("F", "");
    // TestPostorder("G", "");
    // TestPostorder("H", "");
    // TestPostorder("I", "");
}
