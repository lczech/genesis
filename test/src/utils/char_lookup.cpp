/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/tools/char_lookup.hpp"

using namespace genesis::utils;

TEST(CharLookup, Simple)
{
    auto cl = CharLookup();

    cl.set_selection( "abc" );
    EXPECT_TRUE(  cl.get('a') );
    EXPECT_FALSE( cl.get('A') );

    cl.set_range( 'G', 'L' );
    EXPECT_FALSE( cl.get('F') );
    EXPECT_TRUE(  cl.get('I') );

    cl.set_char( 'I', false );
    EXPECT_FALSE( cl.get('I') );
}
