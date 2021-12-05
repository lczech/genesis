/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include <algorithm>
#include <cmath>
#include <ctime>
#include <functional>
#include <limits>
#include <list>
#include <random>

#include "src/common.hpp"

#include "genesis/utils/containers/interval_tree.hpp"

using namespace genesis::utils;

// =================================================================================================
//     Test Utilities
// =================================================================================================

template <typename ContainedT>
struct TestIntervalTypes
{
    using value_type = ContainedT;
    using interval_type = Interval <EmptyIntervalData, ContainedT>;
    using tree_type = IntervalTree <EmptyIntervalData, ContainedT>;
    using iterator_type = typename tree_type::iterator;
    using const_iterator_type = typename tree_type::const_iterator;
};

template <typename TreeT>
void test_red_black_property_violation_(TreeT const& tree)
{
    // Warning this function is very expensive.

    // root is always black.
    EXPECT_EQ(tree.root().color(), RedBackColor::kBlack);

    // check that all nodes have red or black coloring. (seems obvious, but is not on bug)
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        EXPECT_EQ(true, i.color() == RedBackColor::kBlack || i.color() == RedBackColor::kRed);
    }

    // check for (red children = black) property:
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        auto nodeColor = i.color();
        if (nodeColor == RedBackColor::kRed)
        {
            if (i.left() != std::end(tree))
            {
                EXPECT_EQ(i.left().color(), RedBackColor::kBlack);
            }
            if (i.right() != std::end(tree))
            {
                EXPECT_EQ(i.right().color(), RedBackColor::kBlack);
            }
        }
    }

    auto leafCollector = [&](typename TreeT::const_iterator root)
    {
        std::list <typename TreeT::const_iterator> leaves{};
        std::function <void(typename std::list <typename TreeT::const_iterator>::iterator)> recursiveLeafFinder;
        recursiveLeafFinder = [&](typename std::list <typename TreeT::const_iterator>::iterator self)
        {
            if (self->left() != std::end(tree))
            {
                recursiveLeafFinder(leaves.insert(self, self->left()));
            }
            if (self->right() != std::end(tree))
            {
                *self = self->right();
                recursiveLeafFinder(self);
            }
        };
        leaves.push_back(root);
        recursiveLeafFinder(leaves.begin());
        return leaves;
    };

    // Test that for every node, on the path to its leaves, has the same number of black nodes.
    for (auto i = tree.cbegin(); i != tree.cend(); ++i)
    {
        auto leaves = leafCollector(i);
        int comparisonCounter{0};
        for (auto const& leaf : leaves)
        {
            auto p = leaf;
            int counter{0};
            do
            {
                if (p.color() == RedBackColor::kBlack)
                    ++counter;
                p = p.parent();
            } while (p != i && p != std::end(tree));
            if (comparisonCounter == 0)
                comparisonCounter = counter;
            else
            {
                EXPECT_EQ(comparisonCounter, counter);
            }
        }
    }
}

template <typename TreeT>
void test_max_property_(TreeT const& tree)
{
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
    {
        if (i->left())
        {
            EXPECT_LE(i->left()->max(), i->max());
        }
        if (i->right())
        {
            EXPECT_LE(i->right()->max(), i->max());
        }
        EXPECT_GE(i->max(), i->interval().high());
    }
}

template <typename TreeT>
void test_tree_height_health_(TreeT const& tree)
{
    int treeSize = tree.size();

    size_t maxHeight{0};
    for (auto i = std::begin(tree); i != std::end(tree); ++i)
        maxHeight = std::max(maxHeight, i->height());

    EXPECT_LE(maxHeight, 2 * std::log2(treeSize + 1));
}

// =================================================================================================
//     Interval Tests
// =================================================================================================

class IntervalTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;
};

class OverlapTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;
};

class ContainmentTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;
};

class DistanceTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;
};

TEST_F(IntervalTests, FailBadBorders)
{
    auto f = []()
    {
        [[maybe_unused]] auto ival = types::interval_type{1, 0};
    };

    EXPECT_ANY_THROW(f());
}

TEST_F(IntervalTests, ShallCreateInterval)
{
    auto ival = types::interval_type{1, 24};
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval2)
{
    auto ival = types::interval_type{-23, 24};
    EXPECT_EQ(ival.low(), -23);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval3)
{
    auto ival = types::interval_type{-21, -12};
    EXPECT_EQ(ival.low(), -21);
    EXPECT_EQ(ival.high(), -12);
}

TEST_F(IntervalTests, ShallCreateInterval4)
{
    auto ival = types::interval_type{1, 24};
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 24);
}

TEST_F(IntervalTests, ShallCreateInterval5)
{
    auto ival = types::interval_type{1, 1};
    EXPECT_EQ(ival.low(), 1);
    EXPECT_EQ(ival.high(), 1);
}

TEST_F(IntervalTests, TestLimits)
{
    auto ival = types::interval_type{std::numeric_limits<types::value_type>::min(), std::numeric_limits<types::value_type>::max()};
    EXPECT_EQ(ival.low(), std::numeric_limits<types::value_type>::min());
    EXPECT_EQ(ival.high(), std::numeric_limits<types::value_type>::max());
}

TEST_F(IntervalTests, TestIntervalSize)
{
    auto ival = types::interval_type{0, 5};
    EXPECT_EQ(ival.size(), 5);

    auto ival2 = types::interval_type{-21, 5};
    EXPECT_EQ(ival2.size(), 26);

    auto ival3 = types::interval_type{-20, -5};
    EXPECT_EQ(ival3.size(), 15);

    auto ival4 = types::interval_type{100, 125};
    EXPECT_EQ(ival4.size(), 25);
}

TEST_F(OverlapTests, ShallOverlapItself)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps(base), true);
}

TEST_F(OverlapTests, ShallOverlapItself2)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({0, 5}), true);
}

TEST_F(OverlapTests, ShallOverlapRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({3, 16}), true);
}

TEST_F(OverlapTests, ShallOverlapLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({-8, 1}), true);
}

TEST_F(OverlapTests, ShallEncompassCompletely)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({-99, 16}), true);
}

TEST_F(OverlapTests, ShallBeContainedIn)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({3, 4}), true);
}

TEST_F(OverlapTests, ExpectDisjunct)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({7, 19}), false);
}

TEST_F(OverlapTests, ShallBarelyOverlapLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({-3, 0}), true);
}

TEST_F(OverlapTests, ShallBarelyOverlapRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps({5, 10}), true);
}

TEST_F(OverlapTests, ShallNotOverlapExclusiveLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({-7, 0}), false);
}

TEST_F(OverlapTests, ShallNotOverlapExclusiveRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({5, 10}), false);
}

TEST_F(OverlapTests, ShallOverlapExclusiveRight)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({4, 10}), true);
}

TEST_F(OverlapTests, ShallOverlapExclusiveLeft)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({-4, 2}), true);
}

TEST_F(OverlapTests, ShallOverlapExclusiveEncompass)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({-6, 10}), true);
}

TEST_F(OverlapTests, ShallOverlapExclusiveContained)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({1, 4}), true);
}

TEST_F(OverlapTests, ExpectDisjunctExclusive)
{
    auto base = types::interval_type{0, 5};
    EXPECT_EQ(base.overlaps_exclusive({99, 101}), false);
}

TEST_F(ContainmentTests, ShallSingleBeWithin)
{
    auto base = types::interval_type{-86, 35};
    EXPECT_EQ(base.within(3), true);
    EXPECT_EQ(base.within(-3), true);
    EXPECT_EQ(base.within(-86), true);
    EXPECT_EQ(base.within(35), true);
}

TEST_F(ContainmentTests, ExpectIntervalWithinOther)
{
    auto base = types::interval_type{-100, 100};
    EXPECT_EQ(base.within({-23, 10}), true);
    EXPECT_EQ(base.within({-100, 100}), true);
    EXPECT_EQ(base.within({12, 30}), true);
    EXPECT_EQ(base.within({-73, -23}), true);
    EXPECT_EQ(base.within({-100, -100}), true);
    EXPECT_EQ(base.within({100, 100}), true);
    EXPECT_EQ(base.within({0, 0}), true);
}

TEST_F(ContainmentTests, ExpectIntervalNotWithinOther)
{
    auto base = types::interval_type{-100, 100};
    EXPECT_EQ(base.within({-101, -100}), false);
    EXPECT_EQ(base.within({-100, 101}), false);
    EXPECT_EQ(base.within({-200, 0}), false);
    EXPECT_EQ(base.within({100, 102}), false);
    EXPECT_EQ(base.within({-200, -101}), false);
    EXPECT_EQ(base.within({200, 300}), false);
}

TEST_F(DistanceTests, DistanceIsZeroOnOverlap)
{
    auto base = types::interval_type{-35, 96};
    auto other = types::interval_type{-20, 600};
    EXPECT_EQ(base - other, 0);
}

TEST_F(DistanceTests, DistanceLeftSide)
{
    auto base = types::interval_type{5, 10};
    auto other = types::interval_type{0, 1};
    EXPECT_EQ(base - other, 4);
}

TEST_F(DistanceTests, DistanceRightSide)
{
    auto base = types::interval_type{5, 10};
    auto other = types::interval_type{15, 18};
    EXPECT_EQ(base - other, 5);
}

TEST_F(DistanceTests, DistanceAdjacent)
{
    auto base = types::interval_type{5, 10};
    auto other = types::interval_type{10, 18};
    EXPECT_EQ(base - other, 0);
}

TEST_F(DistanceTests, DistanceAdjacent2)
{
    auto base = types::interval_type{5, 10};
    auto other = types::interval_type{0, 5};
    EXPECT_EQ(base - other, 0);
}

// =================================================================================================
//     Insert Tests
// =================================================================================================

class InsertTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;

protected:
    TestIntervalTypes <int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(InsertTests, InsertIntoEmpty1)
{
    auto inserted_interval = types::interval_type{0, 16};

    tree.insert(inserted_interval);
    EXPECT_EQ(*tree.begin(), inserted_interval);
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(InsertTests, InsertIntoEmpty2)
{
    auto inserted_interval = types::interval_type{-45, 16};

    tree.insert(inserted_interval);
    EXPECT_EQ(*tree.begin(), inserted_interval);
    EXPECT_EQ(tree.size(), 1);
}

TEST_F(InsertTests, InsertMultipleIntoEmpty)
{
    auto firstInterval = types::interval_type{0, 16};
    auto secondInterval = types::interval_type{5, 13};

    tree.insert(firstInterval);
    tree.insert(secondInterval);

    EXPECT_EQ(tree.size(), 2);

    EXPECT_EQ(*tree.begin(), firstInterval);
    EXPECT_EQ(*(++tree.begin()), secondInterval);
}

TEST_F(InsertTests, TreeHeightHealthynessTest)
{
    constexpr int amount = 100000;

    for (int i = 0; i != amount; ++i)
        tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

    test_tree_height_health_(tree);
}

TEST_F(InsertTests, MaxValueTest1)
{
    constexpr int amount = 100000;

    for (int i = 0; i != amount; ++i)
        tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

    test_max_property_(tree);
}

TEST_F(InsertTests, RBPropertyInsertTest)
{
    constexpr int amount = 1000;

    for (int i = 0; i != amount; ++i)
        tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

    test_red_black_property_violation_(tree);
}

// =================================================================================================
//     Find Tests
// =================================================================================================

class FindTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;
protected:
    TestIntervalTypes <int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(FindTests, WillReturnEndIfTreeIsEmpty)
{
    EXPECT_EQ(tree.find({2, 7}), std::end(tree));
}

TEST_F(FindTests, WillNotFindRootIfItIsntTheSame)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.find({2, 7}), std::end(tree));
}

TEST_F(FindTests, WillFindRoot)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.find({0, 1}), std::begin(tree));
}

TEST_F(FindTests, WillFindRootOnConstTree)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.find({0, 1}), std::begin(tree));
}

TEST_F(FindTests, WillFindInBiggerTree)
{
    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({0, 3});
    tree.insert({6, 10});
    tree.insert({19, 20});
    auto iter = tree.find({15, 23});
    EXPECT_NE(iter, std::end(tree));
    EXPECT_EQ(iter->low(), 15);
    EXPECT_EQ(iter->high(), 23);
}

TEST_F(FindTests, WillFindAllInTreeWithDuplicates)
{
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    int findCount = 0;
    tree.find_all({5, 8}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{5, 8}));
        return true;
    });
    EXPECT_EQ(findCount, tree.size());
}

TEST_F(FindTests, WillFindAllCanExitPreemptively)
{
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    tree.insert({5, 8});
    int findCount = 0;
    tree.find_all({5, 8}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{5, 8}));
        return findCount < 3;
    });
    EXPECT_EQ(findCount, 3);
}

TEST_F(FindTests, CanFindAllElementsBack)
{
    constexpr int amount = 10000;

    std::vector <decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = make_safe_interval(distLarge(gen), distLarge(gen));
        intervals.emplace_back(interval);
        tree.insert(interval);
    }
    for (auto const& ival : intervals)
    {
        ASSERT_NE(tree.find(ival), std::end(tree));
    }
}

TEST_F(FindTests, CanFindAllElementsBackInStrictlyAscendingNonOverlappingIntervals)
{
    constexpr int amount = 10000;

    std::vector <decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = make_safe_interval(i * 2,  i * 2 + 1);
        intervals.emplace_back(interval);
        tree.insert(interval);
    }
    for (auto const& ival : intervals)
    {
        ASSERT_NE(tree.find(ival), std::end(tree));
    }
}

TEST_F(FindTests, CanFindAllElementsBackInStrictlyAscendingOverlappingIntervals)
{
    constexpr int amount = 10000;

    std::vector <decltype(tree)::interval_type> intervals;
    intervals.reserve(amount);
    for (int i = 0; i != amount; ++i)
    {
        const auto interval = make_safe_interval(i - 1,  i + 1);
        intervals.emplace_back(interval);
        tree.insert(interval);
    }
    for (auto const& ival : intervals)
    {
        ASSERT_NE(tree.find(ival), std::end(tree));
    }
}

TEST_F(FindTests, CanFindAllOnConstTree)
{
    const auto targetInterval = make_safe_interval(16, 21);
    tree.insert(targetInterval);
    tree.insert({8, 9});
    tree.insert({25, 30});
    std::vector <decltype(tree)::interval_type> intervals;
    auto findWithConstTree = [&intervals, &targetInterval](decltype( tree ) const& tree)
    {
        tree.find_all(targetInterval, [&intervals](types::const_iterator_type const& iter) {
            intervals.emplace_back(*iter);
            return true;
        });
    };
    findWithConstTree(tree);

    ASSERT_EQ(intervals.size(), 1);
    EXPECT_EQ(intervals[0], targetInterval);
}

// =================================================================================================
//     Overlap Tests
// =================================================================================================

class OverlapFindTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;
protected:
    TestIntervalTypes <int>::tree_type tree;
};

TEST_F(OverlapFindTests, WillReturnEndIfTreeIsEmpty)
{
    EXPECT_EQ(tree.overlap_find({2, 7}), std::end(tree));
}

TEST_F(OverlapFindTests, WillNotFindOverlapWithRootIfItDoesntOverlap)
{
    tree.insert({0, 1});
    EXPECT_EQ(tree.overlap_find({2, 7}), std::end(tree));
}

TEST_F(OverlapFindTests, WillFindOverlapWithRoot)
{
    tree.insert({2, 4});
    EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
}

TEST_F(OverlapFindTests, WillFindOverlapWithRootOnConstTree)
{
    tree.insert({2, 4});
    [](decltype(tree) const& tree) {
        EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
    }(tree);
}

TEST_F(OverlapFindTests, WillFindOverlapWithRootIfMatchingExactly)
{
    tree.insert({2, 7});
    EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
}

TEST_F(OverlapFindTests, WillFindOverlapWithRootIfTouching)
{
    tree.insert({2, 7});
    EXPECT_EQ(tree.overlap_find({7, 9}), std::begin(tree));
}

TEST_F(OverlapFindTests, WillNotFindOverlapIfNothingOverlaps)
{
    tree.insert({0, 5});
    tree.insert({5, 10});
    tree.insert({10, 15});
    tree.insert({15, 20});
    EXPECT_EQ(tree.overlap_find({77, 99}), std::end(tree));
}

TEST_F(OverlapFindTests, WillNotFindOverlapOnBorderIfExclusive)
{
    tree.insert({0, 5});
    tree.insert({5, 10});
    tree.insert({10, 15});
    tree.insert({15, 20});
    EXPECT_EQ(tree.overlap_find({5, 5}, true), std::end(tree));
}

TEST_F(OverlapFindTests, WillFindMultipleOverlaps)
{
    tree.insert({0, 5});
    tree.insert({5, 10});
    tree.insert({10, 15});
    tree.insert({15, 20});

    std::vector <decltype(tree)::interval_type> intervals;
    tree.overlap_find_all({5, 5}, [&intervals](types::iterator_type const& iter) {
        intervals.push_back(*iter);
        return true;
    });
    // using ::testing::UnorderedElementsAre;
    // ASSERT_THAT(intervals, UnorderedElementsAre(decltype(tree)::interval_type{0, 5}, decltype(tree)::interval_type{5, 10}));
}

TEST_F(OverlapFindTests, FindAllWillFindNothingIfEmpty)
{
    int findCount = 0;
    tree.overlap_find_all({2, 7}, [&findCount](types::iterator_type const&){
        ++findCount;
        return true;
    });
    EXPECT_EQ(findCount, 0);
}

TEST_F(OverlapFindTests, FindAllWillFindNothingIfNothingOverlaps)
{
    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    int findCount = 0;
    tree.overlap_find_all({1000, 2000}, [&findCount](types::iterator_type const&){
        ++findCount;
        return true;
    });
    EXPECT_EQ(findCount, 0);
}

TEST_F(OverlapFindTests, FindAllWillFindAllWithLotsOfDuplicates)
{
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});

    int findCount = 0;
    tree.overlap_find_all({2, 3}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
        return true;
    });
    EXPECT_EQ(findCount, tree.size());
}

TEST_F(OverlapFindTests, CanExitPreemptivelyByReturningFalse)
{
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});
    tree.insert({0, 5});

    int findCount = 0;
    tree.overlap_find_all({2, 3}, [&findCount](decltype(tree)::iterator iter){
        ++findCount;
        EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
        return findCount < 3;
    });
    EXPECT_EQ(findCount, 3);
}

TEST_F(OverlapFindTests, WillFindSingleOverlapInBiggerTree)
{
    tree.insert({16, 21});
    tree.insert({8, 9});
    tree.insert({25, 30});
    tree.insert({5, 8});
    tree.insert({15, 23});
    tree.insert({17, 19});
    tree.insert({26, 26});
    tree.insert({1000, 2000});
    tree.insert({6, 10});
    tree.insert({19, 20});
    auto iter = tree.overlap_find({1000, 1001});
    EXPECT_NE(iter, std::end(tree));
    EXPECT_EQ(iter->low(), 1000);
    EXPECT_EQ(iter->high(), 2000);
}

TEST_F(FindTests, CanOverlapFindAllOnConstTree)
{
    const auto targetInterval = make_safe_interval(16, 21);
    tree.insert(targetInterval);
    tree.insert({8, 9});
    tree.insert({25, 30});
    std::vector <decltype(tree)::interval_type> intervals;
    auto findWithConstTree = [&intervals, &targetInterval](types::tree_type const& tree)
    {
        tree.overlap_find_all(targetInterval, [&intervals](types::const_iterator_type const& iter) {
            intervals.emplace_back(*iter);
            return true;
        });
    };
    findWithConstTree(tree);

    ASSERT_EQ(intervals.size(), 1);
    EXPECT_EQ(intervals[0], targetInterval);
}

// =================================================================================================
//     Float Overlap Tests
// =================================================================================================

class FloatOverlapFindTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <double>;
protected:
    TestIntervalTypes <double>::tree_type tree;
};

TEST_F(FloatOverlapFindTests, FloatOverlapTest)
{
    auto mk_safe_dbl_ival = []( double l, double h ){
        return make_safe_interval<EmptyIntervalData, double>( l, h );
    };

    tree.insert(mk_safe_dbl_ival(-1.483529864195180e+00, -1.296053859335657e+00));
    tree.insert(mk_safe_dbl_ival(-1.308996938995747e+00, -1.127801743538376e+00));
    tree.insert(mk_safe_dbl_ival(-1.134464013796314e+00, -9.562870818388700e-01));
    tree.insert(mk_safe_dbl_ival(-9.599310885968813e-01, -7.834918877708545e-01));
    tree.insert(mk_safe_dbl_ival(-7.853981633974484e-01, -6.090750919515169e-01));
    tree.insert(mk_safe_dbl_ival(-6.108652381980154e-01, -4.348738075675338e-01));
    tree.insert(mk_safe_dbl_ival(-4.363323129985824e-01, -2.608478200480425e-01));
    tree.insert(mk_safe_dbl_ival(-2.617993877991495e-01, -8.693606119038631e-02));
    tree.insert(mk_safe_dbl_ival(-8.726646259971654e-02, 8.726646259971654e-02));
    tree.insert(mk_safe_dbl_ival(8.693606119038631e-02, 2.617993877991493e-01));
    tree.insert(mk_safe_dbl_ival(2.608478200480422e-01, 4.363323129985823e-01));
    tree.insert(mk_safe_dbl_ival(4.348738075675337e-01, 6.108652381980154e-01));
    tree.insert(mk_safe_dbl_ival(6.090750919515169e-01, 7.853981633974484e-01));
    tree.insert(mk_safe_dbl_ival(7.834918877708545e-01, 9.599310885968813e-01));
    tree.insert(mk_safe_dbl_ival(9.562870818388700e-01, 1.134464013796314e+00));
    tree.insert(mk_safe_dbl_ival(1.127801743538376e+00, 1.308996938995747e+00));
    tree.insert(mk_safe_dbl_ival(1.296053859335657e+00, 1.483529864195180e+00));

    double lat0 = 1.040893537045970;
    double lat1 = 1.570796326794897;

    std::vector <std::pair<double, double>> vecOverlapsA;
    Interval <EmptyIntervalData, double> intSource({lat0, lat1});
    for (auto const& iter : tree)
    {
        if (iter.overlaps(intSource))
            vecOverlapsA.push_back({iter.low(), iter.high()});
    }

    std::vector <std::pair<double, double>> vecOverlapsB;
    tree.overlap_find_all
    (
        {lat0, lat1},
        [&vecOverlapsB]( IntervalTree<EmptyIntervalData, double>::iterator iter)
        {
            vecOverlapsB.push_back({iter->low(), iter->high()});
            return true;
        },
        false
    );

    std::sort(std::begin(vecOverlapsA), std::end(vecOverlapsA));
    std::sort(std::begin(vecOverlapsB), std::end(vecOverlapsB));

    ASSERT_EQ(vecOverlapsA.size(), vecOverlapsB.size());
    // EXPECT_THAT(vecOverlapsA, ::testing::ContainerEq(vecOverlapsB));

    auto it_a = vecOverlapsA.begin();
    auto it_b = vecOverlapsB.begin();
    auto end_a = vecOverlapsA.end();
    auto end_b = vecOverlapsB.end();
    while( it_a != end_a && it_b != end_b ){
        EXPECT_TRUE( *it_a == *it_b );
        ++it_a;
        ++it_b;
    }
}

// =================================================================================================
//     Erase Tests
// =================================================================================================

class EraseTests
    : public ::testing::Test
{
public:
    using types = TestIntervalTypes <int>;

protected:
    TestIntervalTypes <int>::tree_type tree;
    std::default_random_engine gen;
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};
};

TEST_F(EraseTests, EraseSingleElement)
{
    auto inserted_interval = types::interval_type{0, 16};

    tree.insert(inserted_interval);

    tree.erase(tree.begin());

    EXPECT_EQ(tree.empty(), true);
    EXPECT_EQ(tree.size(), 0);
}

TEST_F(EraseTests, ManualClearTest)
{
    constexpr int amount = 10000;

    for (int i = 0; i != amount; ++i)
        tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

    for (auto i = std::begin(tree); i != std::end(tree);)
        i = tree.erase(i);

    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, ClearTest)
{
    constexpr int amount = 10000;

    for (int i = 0; i != amount; ++i)
        tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

    tree.clear();

    EXPECT_EQ(tree.empty(), true);
}

TEST_F(EraseTests, RandomEraseTest)
{
    constexpr int amount = 10000;
    constexpr int deleteAmount = 20;

    for (int i = 0; i != amount; ++i)
        tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

    for (int i = 0; i != deleteAmount; ++i)
    {
        std::uniform_int_distribution <int> dist{0, amount - i - 1};
        auto end = dist(gen);
        auto iter = tree.begin();
        for (int j = 0; j != end; ++j)
            ++iter;
    }

    test_max_property_(tree);
    test_red_black_property_violation_(tree);
    test_tree_height_health_(tree);
}
