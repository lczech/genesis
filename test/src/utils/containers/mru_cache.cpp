/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/containers/mru_cache.hpp"
#include "genesis/utils/core/fs.hpp"

#include <string>

using namespace genesis::utils;

TEST( Containers, MruCacheBasics )
{
    MruCache<std::string, size_t> cache { 5 };
    cache.load_function = []( std::string const& s ){
        return stoi( s );
    };

    // Test the emptiness.
    EXPECT_EQ( 0, cache.size() );
    EXPECT_TRUE( cache.empty() );
    EXPECT_EQ( 5, cache.capacity() );
    EXPECT_THROW( cache.peek( "nope" ), std::invalid_argument );
    EXPECT_FALSE( cache.check( "nope" ));

    // Fetch some elements.
    cache.fetch( "0" );
    cache.fetch( "1" );
    cache.fetch( "2" );
    cache.fetch( "3" );
    cache.fetch( "4" );
    EXPECT_EQ( 5, cache.size() );
    EXPECT_LE( cache.size(), cache.capacity() );

    // Fetch more. This removes elements 0 and 1, the oldest ones.
    cache.fetch( "5" );
    cache.fetch( "6" );
    EXPECT_EQ( 5, cache.size() );
    EXPECT_LE( cache.size(), cache.capacity() );

    // Fetch the newest one. This should not chance anything.
    EXPECT_EQ( 6, cache.fetch( "6" ));

    // Check all elements.
    EXPECT_FALSE( cache.check( "0" ));
    EXPECT_FALSE( cache.check( "1" ));
    EXPECT_TRUE(  cache.check( "2" ));
    EXPECT_TRUE(  cache.check( "3" ));
    EXPECT_TRUE(  cache.check( "4" ));
    EXPECT_TRUE(  cache.check( "5" ));
    EXPECT_TRUE(  cache.check( "6" ));

    // Bring an existing one to the front. Add more.
    cache.touch( "3" );
    cache.touch( "7" );
    cache.touch( "8" );

    // Elements 2 and four were the oldest ones and should be gone now.
    EXPECT_FALSE( cache.check( "2" ));
    EXPECT_FALSE( cache.check( "4" ));

    // Shrink. This removes all except the three that we recently touched.
    cache.capacity( 3 );
    EXPECT_EQ( 3, cache.size() );
    EXPECT_LE( cache.size(), cache.capacity() );
    EXPECT_TRUE(  cache.check( "3" ));
    EXPECT_TRUE(  cache.check( "7" ));
    EXPECT_TRUE(  cache.check( "8" ));
    EXPECT_EQ( 8, cache.fetch( "8" ));
    EXPECT_NO_THROW( cache.peek( "3" ));

    // Change to limitless.
    cache.capacity( 0 );
    EXPECT_EQ( 3, cache.size() );
    EXPECT_EQ( 0, cache.capacity() );

    // Add some more again.
    cache.touch( "9" );
    cache.touch( "10" );
    EXPECT_EQ( 5, cache.size() );

    // Touch existing ones.
    EXPECT_EQ( 8, cache.fetch( "8" ));
    EXPECT_EQ( 5, cache.size() );

    // Clear.
    cache.clear();
    EXPECT_EQ( 0, cache.size() );
    EXPECT_TRUE( cache.empty() );
}

TEST( Containers, MruCacheLimitless )
{
    MruCache<std::string, size_t> cache;
    cache.load_function = []( std::string const& s ){
        return stoi( s );
    };

    // Fetch some elements.
    cache.fetch( "0" );
    cache.fetch( "1" );
    cache.fetch( "2" );
    cache.fetch( "3" );
    cache.fetch( "4" );
    EXPECT_EQ( 5, cache.size() );

    // Fetch some existing ones again.
    EXPECT_EQ( 1, cache.fetch( "1" ));
    EXPECT_EQ( 2, cache.fetch( "2" ));
    EXPECT_EQ( 5, cache.size() );
}

TEST( Containers, MruCacheFailLoading )
{
    MruCache<std::string, size_t> cache{ 3 };
    cache.load_function = []( std::string const& s ){
        return stoi( s );
    };

    // Fetch some elements that cannot be converted to a number and thus throws.
    EXPECT_ANY_THROW( cache.fetch( "abc" ));
    EXPECT_ANY_THROW( cache.fetch( "xyz" ));

    // Those elements should not be there.
    EXPECT_FALSE( cache.check( "abc" ));
    EXPECT_FALSE( cache.check( "xyz" ));

    // Everything needs to be okay still.
    EXPECT_EQ( 0, cache.size() );
    EXPECT_TRUE( cache.empty() );
    EXPECT_EQ( 3, cache.capacity() );
    EXPECT_LE( cache.size(), cache.capacity() );

    // Now load some elements that are okay.
    cache.fetch( "0" );
    cache.fetch( "1" );
    cache.fetch( "2" );
    cache.fetch( "3" );
    cache.fetch( "4" );
    EXPECT_EQ( 3, cache.size() );
    EXPECT_TRUE( cache.check( "2" ));
    EXPECT_TRUE( cache.check( "3" ));
    EXPECT_TRUE( cache.check( "4" ));

    // And again some corrupt ones.
    EXPECT_ANY_THROW( cache.fetch( "l" ));
    EXPECT_ANY_THROW( cache.fetch( "m" ));

    // Nothing should have changed.
    EXPECT_EQ( 3, cache.size() );
    EXPECT_TRUE( cache.check( "2" ));
    EXPECT_TRUE( cache.check( "3" ));
    EXPECT_TRUE( cache.check( "4" ));
}

TEST( Containers, MruCacheFiles )
{
    // This is the example from the class description. Make sure it actually works.
    NEEDS_TEST_DATA;

    // Path to our data.
    std::string dir = environment->data_dir + "utils/json";

    // Create a cache with a capacity of 3.
    MruCache<std::string, std::string> cache { 3 };

    // Load elements from file.
    cache.load_function = [ &dir ]( std::string const& file ){
        return file_read( dir + "/" + file );
    };

    // Access some files.
    cache.fetch( "fail2.jtest" );
    cache.fetch( "fail3.jtest" );
    cache.fetch( "fail4.jtest" );
    cache.fetch( "fail5.jtest" );

    // Only the last three ones are in the cache now.
    EXPECT_EQ( 3, cache.size() );
    EXPECT_FALSE( cache.check( "fail2.jtest" ));
    EXPECT_TRUE(  cache.check( "fail3.jtest" ));
    EXPECT_TRUE(  cache.check( "fail4.jtest" ));
    EXPECT_TRUE(  cache.check( "fail5.jtest" ));

    // Load a file that does not exist. Check for exception (coming from the load function).
    EXPECT_ANY_THROW( cache.fetch( "not_there" ));

    // Nothing should have changed.
    EXPECT_EQ( 3, cache.size() );
    EXPECT_TRUE(  cache.check( "fail3.jtest" ));
    EXPECT_TRUE(  cache.check( "fail4.jtest" ));
    EXPECT_TRUE(  cache.check( "fail5.jtest" ));

}
