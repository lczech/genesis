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

TEST(Interval, Basics)
{
    using types = TestIntervalTypes <int>;

    // Fail Bad Borders
    auto make_bad_interval = [](){
        return types::interval_type{1, 0};
    };
    EXPECT_ANY_THROW( make_bad_interval() );

    // Create Interval
    {
        auto ival = types::interval_type{1, 24};
        EXPECT_EQ(ival.low(), 1);
        EXPECT_EQ(ival.high(), 24);

        EXPECT_EQ( "[ 1, 24 ]", ival.to_string() );
    }

    // Create Interval 2
    {
        auto ival = types::interval_type{-23, 24};
        EXPECT_EQ(ival.low(), -23);
        EXPECT_EQ(ival.high(), 24);
    }

    // Create Interval 3
    {
        auto ival = types::interval_type{-21, -12};
        EXPECT_EQ(ival.low(), -21);
        EXPECT_EQ(ival.high(), -12);
    }

    // Create Interval 4
    {
        auto ival = types::interval_type{1, 24};
        EXPECT_EQ(ival.low(), 1);
        EXPECT_EQ(ival.high(), 24);
    }

    // Create Interval 5
    {
        auto ival = types::interval_type{1, 1};
        EXPECT_EQ(ival.low(), 1);
        EXPECT_EQ(ival.high(), 1);
    }

    // Limits
    {
        auto ival = types::interval_type{std::numeric_limits<types::value_type>::min(), std::numeric_limits<types::value_type>::max()};
        EXPECT_EQ(ival.low(), std::numeric_limits<types::value_type>::min());
        EXPECT_EQ(ival.high(), std::numeric_limits<types::value_type>::max());
    }

    // Interval Size
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
}

TEST(Interval, Overlap)
{
    using types = TestIntervalTypes <int>;

    // Overlap Itself
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps(base), true);
    }

    // Overlap Itself 2
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({0, 5}), true);
    }

    // Overlap Right
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({3, 16}), true);
    }

    // Overlap Left
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({-8, 1}), true);
    }

    // Encompass Completely
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({-99, 16}), true);
    }

    // Be Contained In
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({3, 4}), true);
    }

    // Disjunct
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({7, 19}), false);
    }

    // Barely Overlap Left
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({-3, 0}), true);
    }

    // Barely Overlap Right
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps({5, 10}), true);
    }

    // Not Overlap Exclusive Left
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({-7, 0}), false);
    }

    // Not Overlap Exclusive Right
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({5, 10}), false);
    }

    // Overlap Exclusive Right
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({4, 10}), true);
    }

    // Overlap Exclusive Left
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({-4, 2}), true);
    }

    // Overlap Exclusive Encompass
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({-6, 10}), true);
    }

    // Overlap Exclusive Contained
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({1, 4}), true);
    }

    // Disjunct Exclusive
    {
        auto base = types::interval_type{0, 5};
        EXPECT_EQ(base.overlaps_exclusive({99, 101}), false);
    }
}

TEST(Interval, Containment)
{
    using types = TestIntervalTypes <int>;

    // Single Be Within
    {
        auto base = types::interval_type{-86, 35};
        EXPECT_EQ(base.within(3), true);
        EXPECT_EQ(base.within(-3), true);
        EXPECT_EQ(base.within(-86), true);
        EXPECT_EQ(base.within(35), true);
    }

    // Expect Interval Within Other
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

    // Expect Interval Not Within Other
    {
        auto base = types::interval_type{-100, 100};
        EXPECT_EQ(base.within({-101, -100}), false);
        EXPECT_EQ(base.within({-100, 101}), false);
        EXPECT_EQ(base.within({-200, 0}), false);
        EXPECT_EQ(base.within({100, 102}), false);
        EXPECT_EQ(base.within({-200, -101}), false);
        EXPECT_EQ(base.within({200, 300}), false);
    }
}

TEST(Interval, Distance)
{
    using types = TestIntervalTypes <int>;

    // Distance Is Zero On Overlap
    {
        auto base = types::interval_type{-35, 96};
        auto other = types::interval_type{-20, 600};
        EXPECT_EQ(base - other, 0);
    }

    // Distance Left Side
    {
        auto base = types::interval_type{5, 10};
        auto other = types::interval_type{0, 1};
        EXPECT_EQ(base - other, 4);
    }

    // Distance Right Side
    {
        auto base = types::interval_type{5, 10};
        auto other = types::interval_type{15, 18};
        EXPECT_EQ(base - other, 5);
    }

    // Distance Adjacent
    {
        auto base = types::interval_type{5, 10};
        auto other = types::interval_type{10, 18};
        EXPECT_EQ(base - other, 0);
    }

    // Distance Adjacent 2
    {
        auto base = types::interval_type{5, 10};
        auto other = types::interval_type{0, 5};
        EXPECT_EQ(base - other, 0);
    }
}

// =================================================================================================
//     Insert Tests
// =================================================================================================

TEST(IntervalTree, Insert)
{
    using types = TestIntervalTypes <int>;

    std::default_random_engine gen;
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};

    // Insert Into Empty 1
    {
        TestIntervalTypes <int>::tree_type tree;
        auto inserted_interval = types::interval_type{0, 16};

        tree.insert(inserted_interval);
        EXPECT_EQ(*tree.begin(), inserted_interval);
        EXPECT_EQ(tree.size(), 1);
    }

    // Insert Into Empty 2
    {
        TestIntervalTypes <int>::tree_type tree;
        auto inserted_interval = types::interval_type{-45, 16};

        tree.insert(inserted_interval);
        EXPECT_EQ(*tree.begin(), inserted_interval);
        EXPECT_EQ(tree.size(), 1);
    }

    // Insert Multiple Into Empty
    {
        TestIntervalTypes <int>::tree_type tree;
        auto firstInterval = types::interval_type{0, 16};
        auto secondInterval = types::interval_type{5, 13};

        tree.insert(firstInterval);
        tree.insert(secondInterval);

        EXPECT_EQ(tree.size(), 2);

        EXPECT_EQ(*tree.begin(), firstInterval);
        EXPECT_EQ(*(++tree.begin()), secondInterval);
    }

    // Tree Height Healthyness Test
    // Max Value Test 1
    // RB Property Insert Test
    {
        TestIntervalTypes <int>::tree_type tree;
        constexpr int amount = 100000;

        for (int i = 0; i != amount; ++i) {
            tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));
        }

        EXPECT_EQ(tree.size(), amount);
        test_tree_height_health_(tree);
        test_max_property_(tree);
        test_red_black_property_violation_(tree);
    }

    // Iterate in ascending order of low
    {
        TestIntervalTypes <int>::tree_type tree;
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

        std::vector<int> lows;
        for( auto const& ival : tree ) {
            // LOG_DBG << ival.to_string();
            lows.push_back(ival.low());
        }
        EXPECT_TRUE( std::is_sorted( lows.begin(), lows.end() ) );
    }
}

// =================================================================================================
//     Find Tests
// =================================================================================================

TEST(IntervalTree, Find)
{
    using types = TestIntervalTypes <int>;

    std::default_random_engine gen;
    std::uniform_int_distribution <int> distLarge{-50000, 50000};

    // Return End If Tree Is Empty
    {
        TestIntervalTypes <int>::tree_type tree;
        EXPECT_EQ(tree.find({2, 7}), std::end(tree));
    }

    // Not Find Root If It Isnt The Same
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 1});
        EXPECT_EQ(tree.find({2, 7}), std::end(tree));
    }

    // Find Root
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 1});
        EXPECT_EQ(tree.find({0, 1}), std::begin(tree));
    }

    // Find Root On Const Tree
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 1});
        EXPECT_EQ(tree.find({0, 1}), std::begin(tree));
    }

    // Find In Bigger Tree
    {
        TestIntervalTypes <int>::tree_type tree;
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

        EXPECT_EQ(  0, tree.lowest() );
        EXPECT_EQ( 30, tree.highest() );
    }

    // Find All In Tree With Duplicates
    {
        TestIntervalTypes <int>::tree_type tree;
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

    // Find All Can Exit Preemptively
    {
        TestIntervalTypes <int>::tree_type tree;
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
        EXPECT_EQ( 5, tree.lowest() );
        EXPECT_EQ( 8, tree.highest() );
    }

    // Find All Elements Back
    {
        TestIntervalTypes <int>::tree_type tree;
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

    // Find All Elements Back In Strictly Ascending Non Overlapping Intervals
    {
        TestIntervalTypes <int>::tree_type tree;
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

    // Find All Elements Back In Strictly Ascending Overlapping Intervals
    {
        TestIntervalTypes <int>::tree_type tree;
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

    // Find All On Const Tree
    {
        TestIntervalTypes <int>::tree_type tree;
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
        EXPECT_EQ(  8, tree.lowest() );
        EXPECT_EQ( 30, tree.highest() );
    }

    // Overlap Find All On Const Tree
    {
        TestIntervalTypes <int>::tree_type tree;
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
}

// =================================================================================================
//     Overlap Tests
// =================================================================================================

TEST(IntervalTree, OverlapFind)
{
    using types = TestIntervalTypes <int>;

    // Return End If Tree Is Empty
    {
        TestIntervalTypes <int>::tree_type tree;
        EXPECT_EQ(tree.overlap_find({2, 7}), std::end(tree));
        EXPECT_EQ(tree.overlap_find(2), std::end(tree));
    }

    // Not Find Overlap With Root If It Doesnt Overlap
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 1});
        EXPECT_EQ(tree.overlap_find({2, 7}), std::end(tree));
        EXPECT_EQ(tree.overlap_find(2), std::end(tree));
    }

    // Find Overlap With Root
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({2, 4});
        EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
        EXPECT_EQ(tree.overlap_find(2), std::begin(tree));
    }

    // Find Overlap With Root On Const Tree
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({2, 4});
        [](decltype(tree) const& tree) {
            EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
            EXPECT_EQ(tree.overlap_find(2), std::begin(tree));
        }(tree);
    }

    // Find Overlap With Root If Matching Exactly
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({2, 7});
        EXPECT_EQ(tree.overlap_find({2, 7}), std::begin(tree));
    }

    // Find Overlap With Root If Touching
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({2, 7});
        EXPECT_EQ(tree.overlap_find({7, 9}), std::begin(tree));
        EXPECT_EQ(tree.overlap_find(7), std::begin(tree));
        EXPECT_EQ(tree.overlap_find(9), std::end(tree));
    }

    // Not Find Overlap If Nothing Overlaps
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 5});
        tree.insert({5, 10});
        tree.insert({10, 15});
        tree.insert({15, 20});
        EXPECT_EQ(tree.overlap_find({77, 99}), std::end(tree));
        EXPECT_EQ(tree.overlap_find(77), std::end(tree));
    }

    // Not Find Overlap On Border If Exclusive
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 5});
        tree.insert({5, 10});
        tree.insert({10, 15});
        tree.insert({15, 20});
        EXPECT_EQ(tree.overlap_find({5, 5}, true), std::end(tree));
        EXPECT_EQ(tree.overlap_find({20, 25}, true), std::end(tree));
    }

    // Find Multiple Overlaps
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 5});
        tree.insert({5, 10});
        tree.insert({10, 15});
        tree.insert({15, 20});

        EXPECT_EQ(  0, tree.lowest() );
        EXPECT_EQ( 20, tree.highest() );

        std::vector <decltype(tree)::interval_type> intervals;
        tree.overlap_find_all({5, 5}, [&intervals](types::iterator_type const& iter) {
            intervals.push_back(*iter);
            return true;
        });
        EXPECT_EQ( 2, intervals.size() );

        // using ::testing::UnorderedElementsAre;
        // ASSERT_THAT(intervals, UnorderedElementsAre(decltype(tree)::interval_type{0, 5}, decltype(tree)::interval_type{5, 10}));

        // With numerical type
        intervals.clear();
        tree.overlap_find_all(5, [&intervals](types::iterator_type const& iter) {
            intervals.push_back(*iter);
            return true;
        });
        EXPECT_EQ( 2, intervals.size() );
    }

    // Find All Will Find Nothing If Empty
    {
        TestIntervalTypes <int>::tree_type tree;
        int findCount = 0;
        tree.overlap_find_all({2, 7}, [&findCount](types::iterator_type const&){
            ++findCount;
            return true;
        });
        tree.overlap_find_all(2, [&findCount](types::iterator_type const&){
            ++findCount;
            return true;
        });
        EXPECT_EQ(findCount, 0);
    }

    // Find All Will Find Nothing If Nothing Overlaps
    {
        TestIntervalTypes <int>::tree_type tree;
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
        EXPECT_EQ(  5, tree.lowest() );
        EXPECT_EQ( 30, tree.highest() );
    }

    // Find All Will Find All With Lots Of Duplicates
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 5});
        tree.insert({0, 5});
        tree.insert({0, 5});
        tree.insert({0, 5});
        tree.insert({0, 5});

        // With interval
        int findCount = 0;
        tree.overlap_find_all({2, 3}, [&findCount](decltype(tree)::iterator iter){
            ++findCount;
            EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
            return true;
        });
        EXPECT_EQ(findCount, tree.size());

        // With numerical type
        findCount = 0;
        tree.overlap_find_all(2, [&findCount](decltype(tree)::iterator iter){
            ++findCount;
            EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
            return true;
        });
        EXPECT_EQ(findCount, tree.size());
    }

    // Can Exit Preemptively By Returning False
    {
        TestIntervalTypes <int>::tree_type tree;
        tree.insert({0, 5});
        tree.insert({0, 5});
        tree.insert({0, 5});
        tree.insert({0, 5});
        tree.insert({0, 5});

        // With interval
        int findCount = 0;
        tree.overlap_find_all({2, 3}, [&findCount](decltype(tree)::iterator iter){
            ++findCount;
            EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
            return findCount < 3;
        });
        EXPECT_EQ(findCount, 3);

        // With numerical type
        findCount = 0;
        tree.overlap_find_all(2, [&findCount](decltype(tree)::iterator iter){
            ++findCount;
            EXPECT_EQ(*iter, (decltype(tree)::interval_type{0, 5}));
            return findCount < 3;
        });
        EXPECT_EQ(findCount, 3);
    }

    // Will Find Single Overlap In Bigger Tree
    {
        TestIntervalTypes <int>::tree_type tree;
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
        EXPECT_EQ( 5, tree.lowest() );
        EXPECT_EQ( 2000, tree.highest() );
    }
}

// =================================================================================================
//     Float Overlap Tests
// =================================================================================================

TEST(IntervalTree, FloatOverlapFind)
{
    // using types = TestIntervalTypes <double>;
    TestIntervalTypes <double>::tree_type tree;

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

    EXPECT_FLOAT_EQ( -1.483529864195180e+00, tree.lowest() );
    EXPECT_FLOAT_EQ(  1.483529864195180e+00, tree.highest() );

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

TEST(IntervalTree, Erase)
{
    using types = TestIntervalTypes <int>;

    std::default_random_engine gen;
    std::uniform_int_distribution <int> distSmall{-500, 500};
    std::uniform_int_distribution <int> distLarge{-50000, 50000};

    // Erase Single Element
    {
        TestIntervalTypes <int>::tree_type tree;
        auto inserted_interval = types::interval_type{0, 16};

        tree.insert(inserted_interval);

        tree.erase(tree.begin());

        EXPECT_EQ(tree.empty(), true);
        EXPECT_EQ(tree.size(), 0);
    }

    // Manual Clear Test
    {
        TestIntervalTypes <int>::tree_type tree;
        constexpr int amount = 10000;

        for (int i = 0; i != amount; ++i)
            tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

        for (auto i = std::begin(tree); i != std::end(tree);)
            i = tree.erase(i);

        EXPECT_EQ(tree.empty(), true);
    }

    // Clear Test
    {
        TestIntervalTypes <int>::tree_type tree;
        constexpr int amount = 10000;

        for (int i = 0; i != amount; ++i)
            tree.insert(make_safe_interval(distSmall(gen), distSmall(gen)));

        tree.clear();

        EXPECT_EQ(tree.empty(), true);
    }

    // Random Erase Test
    {
        TestIntervalTypes <int>::tree_type tree;
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
}
