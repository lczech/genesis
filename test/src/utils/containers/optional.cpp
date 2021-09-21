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

#include "src/common.hpp"

#include "genesis/utils/containers/optional.hpp"

#include <string>
#include <vector>

using namespace genesis::utils;

// Test cases adapted from https://github.com/martinmoene/optional-bare/blob/master/test/optional.t.cpp
// See our Optional implementation in genesis/lib/genesis/utils/containers/optional.hpp
// for details on the copyright and license.
//
// Copyright 2017-2019 by Martin Moene
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// optional bare is derived from optional lite. optional lite is inspired on std::optional
// by Fernando Cacciola and  Andrzej Krzemienski and on expected lite by Martin Moene.

TEST( Containers, OptionalConstruction )
{
    // Test output so that we see what we are using.
    #if GENESIS_OPTIONAL_USES_STD_OPTIONAL

        LOG_DBG << "Using C++17 std::optional";

    #else // GENESIS_OPTIONAL_USES_STD_OPTIONAL

        LOG_DBG << "Using optional-bare";

    #endif

    // Allows to default construct an empty optional
    {
        Optional<int> a;
        EXPECT_TRUE( !a );
    }

    // Allows to explicitly construct a disengaged, empty optional via nullopt
    {
        Optional<int> a( nullopt );
        EXPECT_TRUE( !a );
    }

    // Allows to copy-construct from empty optional
    {
        Optional<int> a;
        Optional<int> b( a );
        EXPECT_FALSE( b );
    }

    // Allows to copy-construct from non-empty optional
    {
        Optional<int> a( 7 );
        Optional<int> b( a );
        EXPECT_TRUE(  b      );
        EXPECT_TRUE( *b == 7 );
    }

    // Allows to copy-construct from literal value
    {
        Optional<int> a = 7;
        EXPECT_TRUE(  a      );
        EXPECT_TRUE( *a == 7 );
    }

    // Allows to copy-construct from value
    {
        const int i = 7;
        Optional<int> a( i );
        EXPECT_TRUE(  a      );
        EXPECT_TRUE( *a == 7 );
    }

    // Allows to copy-construct from optional with different value type
    {
        Optional<int> a( Optional<char>( 7 ) );
        EXPECT_TRUE(  a      );
        EXPECT_TRUE( *a == 7 );
    }

    // Allows to copy-construct from empty optional with different value type
    {
        Optional<char> a;
        Optional<int> b( a );
        EXPECT_FALSE( b );
    }

    // Allows to move-construct from value type
    {
        std::string test = "hello";
        Optional<std::string> a( std::move( test ));

        EXPECT_TRUE( a );
        EXPECT_EQ( "hello", *a );
    }

    // Allows to move-construct from other optional
    {
        Optional<std::string> a( "hello" );
        Optional<std::string> b( std::move(a) );

        EXPECT_TRUE( b );
        EXPECT_EQ( "hello", *b );
    }
}

TEST( Containers, OptionalAssignment )
{
    // Allows to assign nullopt to disengage
    {
        Optional<int>  a( 7 );
        a = nullopt;
        EXPECT_TRUE( !a );
    }

    // Allows to copy-assign from/to engaged and disengaged optionals
    {
        Optional<int> d1;
        Optional<int> d2;
        Optional<int> e1( 123 );
        Optional<int> e2( 987 );

        // a disengaged optional assigned nullopt remains empty
        d1 = nullopt;
        EXPECT_TRUE( !d1 );

        // a disengaged optional assigned an engaged optional obtains its value
        d1 = e1;
        EXPECT_TRUE(  d1 );
        EXPECT_TRUE( *d1 == 123 );

        // an engaged optional assigned an engaged optional obtains its value
        e1 = e2;
        EXPECT_TRUE(  e1 );
        EXPECT_TRUE( *e1 == 987 );

        // an engaged optional assigned nullopt becomes empty
        e1 = nullopt;
        EXPECT_TRUE( !e1 );

        // a disengaged optional assigned a disengaged optional remains empty
        d1 = d2;
        EXPECT_TRUE( !d1 );
    }

    // Allows to copy-assign from literal value
    {
        Optional<int> a;
        a = 7;
        EXPECT_TRUE( a );
        EXPECT_TRUE( *a == 7 );
    }

    // Allows to copy-assign from value
    {
        const int i = 7;
        Optional<int> a;
        a = i;
        EXPECT_TRUE( a );
        EXPECT_TRUE( *a == i );
    }

    // Allows to copy-assign from optional with different value type
    {
        Optional<int> a;
        a = Optional<char>( 7 );
        EXPECT_TRUE(  a      );
        EXPECT_TRUE( *a == 7 );
    }

    // Allows to copy-assign from empty optional with different value type
    {
        Optional<int> a;
        a = Optional<char>();
        EXPECT_FALSE( a );
    }

    // Allows to move-assign from value type
    {
        std::string test = "hello";
        Optional<std::string> a;
        a = std::move( test );

        EXPECT_TRUE( a );
        EXPECT_EQ( "hello", *a );
    }

    // Allows to move-assign from other optional
    {
        Optional<std::string> a( "hello" );
        Optional<std::string> b;
        b = std::move(a);

        EXPECT_TRUE( b );
        EXPECT_EQ( "hello", *b );
    }

    // Assign with different value typw
    {
        Optional<size_t> a;
        a = -1;
    }
}

TEST( Containers, OptionalSwapModify )
{
    // Allows to swap with other optional (member)
    {
        // swap disengaged with disengaged optional
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            d1.swap( d2 );
            EXPECT_TRUE( !d1 );
        }

        // swap engaged with engaged optional
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            e1.swap( e2 );
            EXPECT_TRUE(  e1  );
            EXPECT_TRUE(  e2 );
            EXPECT_TRUE( *e1 == 7 );
            EXPECT_TRUE( *e2 == 42 );
        }
        // swap disengaged with engaged optional" )
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            d1.swap( e1 );
            EXPECT_TRUE(  d1 );
            EXPECT_TRUE( !e1 );
            EXPECT_TRUE( *d1 == 42 );
        }
        // swap engaged with disengaged optional" )
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            e1.swap( d1 );
            EXPECT_TRUE(  d1 );
            EXPECT_TRUE( !e1 );
            EXPECT_TRUE( *d1 == 42 );
        }
    }

    // Allows to reset content
    {
        Optional<int> a = 7;
        a.reset();
        EXPECT_FALSE( a.has_value() );
    }
}

TEST( Containers, OptionalObservers )
{
    {

        struct Integer { int x; Integer(int v) : x(v) {} };
        // Allows to obtain pointer to value via operator->()
        Optional<Integer> e( Integer( 42 ) );

        // operator->() yields pointer to value (const)" )
        EXPECT_TRUE(  e->x == 42 );

        // operator->() yields pointer to value (non-const)" )
        e->x = 7;
        EXPECT_TRUE(  e->x == 7 );
    }

    // Allows to obtain value via operator*()
    {
        Optional<int> e( 42 );

        // operator*() yields value (const)" )
        EXPECT_TRUE( *e == 42 );

        // operator*() yields value (non-const)" )
        *e = 7;
        EXPECT_TRUE( *e == 7 );
    }

    // Allows to obtain engaged state via has_value()
    {
        Optional<int> a;
        Optional<int> b( 7 );
        EXPECT_FALSE( a.has_value() );
        EXPECT_TRUE(  b.has_value() );
    }

    // Allows to obtain has_value() via operator bool()
    {
        Optional<int> a;
        Optional<int> b( 7 );
        EXPECT_FALSE( a );
        EXPECT_TRUE(  b );
    }

    #if GENESIS_OPTIONAL_USES_STD_OPTIONAL && defined(__APPLE__)
    #  define opt_value( o ) *o
    #else
    #  define opt_value( o )  o.value()
    #endif

    //  Allows to obtain value via value()
    {
        Optional<int> e( 42 );

        // value() yields value (const)" )
        EXPECT_TRUE( opt_value( e ) == 42 );

        // value() yields value (non-const)" )
        opt_value( e ) = 7;
        EXPECT_TRUE( opt_value( e ) == 7 );
    }

    // Allows to obtain value or default via value_or()
    {
        Optional<int> d;
        Optional<int> e( 42 );

        // value_or( 7 ) yields value for non-empty optional" )
        EXPECT_TRUE( e.value_or( 7 ) == 42 );

        // value_or( 7 ) yields default for empty optional" )
        EXPECT_TRUE( d.value_or( 7 ) == 7 );
    }

    // Throws bad_optional_access at disengaged access
    {
        EXPECT_ANY_THROW( opt_value( Optional<int>() ) );
    }

    #undef opt_value
}


template< typename R, typename S, typename T >
void optional_relop_()
{
    Optional<R> d;
    Optional<S> e1( 6 );
    Optional<T> e2( 7 );

    /* engaged    == engaged"    */ EXPECT_TRUE(   e1 == e1  );
    /* engaged    == disengaged" */ EXPECT_TRUE( !(e1 == d ) );
    /* disengaged == engaged"    */ EXPECT_TRUE( !(d  == e1) );

    /* engaged    != engaged"    */ EXPECT_TRUE(   e1 != e2  );
    /* engaged    != disengaged" */ EXPECT_TRUE(   e1 != d   );
    /* disengaged != engaged"    */ EXPECT_TRUE(   d  != e2  );

    /* engaged    <  engaged"    */ EXPECT_TRUE(   e1 <  e2  );
    /* engaged    <  disengaged" */ EXPECT_TRUE( !(e1 <  d ) );
    /* disengaged <  engaged"    */ EXPECT_TRUE(   d  <  e2  );

    /* engaged    <= engaged"    */ EXPECT_TRUE(   e1 <= e1  );
    /* engaged    <= engaged"    */ EXPECT_TRUE(   e1 <= e2  );
    /* engaged    <= disengaged" */ EXPECT_TRUE( !(e1 <= d ) );
    /* disengaged <= engaged"    */ EXPECT_TRUE(   d  <= e2  );

    /* engaged    >  engaged"    */ EXPECT_TRUE(   e2 >  e1  );
    /* engaged    >  disengaged" */ EXPECT_TRUE(   e2 >  d   );
    /* disengaged >  engaged"    */ EXPECT_TRUE( !(d  >  e1) );

    /* engaged    >= engaged"    */ EXPECT_TRUE(   e1 >= e1  );
    /* engaged    >= engaged"    */ EXPECT_TRUE(   e2 >= e1  );
    /* engaged    >= disengaged" */ EXPECT_TRUE(   e2 >= d   );
    /* disengaged >= engaged"    */ EXPECT_TRUE( !(d  >= e1) );

    /* disengaged == nullopt"    */ EXPECT_TRUE(  (d       == nullopt) );
    /* nullopt    == disengaged" */ EXPECT_TRUE(  (nullopt == d      ) );
    /* engaged    == nullopt"    */ EXPECT_TRUE(  (e1      != nullopt) );
    /* nullopt    == engaged"    */ EXPECT_TRUE(  (nullopt != e1     ) );
    /* disengaged == nullopt"    */ EXPECT_TRUE( !(d       <  nullopt) );
    /* nullopt    == disengaged" */ EXPECT_TRUE( !(nullopt <  d      ) );
    /* disengaged == nullopt"    */ EXPECT_TRUE(  (d       <= nullopt) );
    /* nullopt    == disengaged" */ EXPECT_TRUE(  (nullopt <= d      ) );
    /* disengaged == nullopt"    */ EXPECT_TRUE( !(d       >  nullopt) );
    /* nullopt    == disengaged" */ EXPECT_TRUE( !(nullopt >  d      ) );
    /* disengaged == nullopt"    */ EXPECT_TRUE(  (d       >= nullopt) );
    /* nullopt    == disengaged" */ EXPECT_TRUE(  (nullopt >= d      ) );

    /* engaged    == value"      */ EXPECT_TRUE( e1 == 6  );
    /* value     == engaged"     */ EXPECT_TRUE(  6 == e1 );
    /* engaged   != value"       */ EXPECT_TRUE( e1 != 7  );
    /* value     != engaged"     */ EXPECT_TRUE(  6 != e2 );
    /* engaged   <  value"       */ EXPECT_TRUE( e1 <  7  );
    /* value     <  engaged"     */ EXPECT_TRUE(  6 <  e2 );
    /* engaged   <= value"       */ EXPECT_TRUE( e1 <= 7  );
    /* value     <= engaged"     */ EXPECT_TRUE(  6 <= e2 );
    /* engaged   >  value"       */ EXPECT_TRUE( e2 >  6  );
    /* value     >  engaged"     */ EXPECT_TRUE(  7 >  e1 );
    /* engaged   >= value"       */ EXPECT_TRUE( e2 >= 6  );
    /* value     >= engaged"     */ EXPECT_TRUE(  7 >= e1 );
}

TEST( Containers, OptionalNonMemberFunctions )
{
    // Allows to swap engage state and values (non-member)
    {
        // swap disengaged with disengaged optional" )
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            swap( d1, d2 );
            EXPECT_TRUE( !d1 );
        }

        // swap engaged with engaged optional" )
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            swap( e1, e2 );
            EXPECT_TRUE(  e1  );
            EXPECT_TRUE(  e2 );
            EXPECT_TRUE( *e1 == 7 );
            EXPECT_TRUE( *e2 == 42 );
        }

        // swap disengaged with engaged optional" )
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            swap( d1, e1 );
            EXPECT_TRUE(  d1 );
            EXPECT_TRUE( !e1 );
            EXPECT_TRUE( *d1 == 42 );
        }

        // swap engaged with disengaged optional" )
        {
            Optional<int> d1;
            Optional<int> d2;
            Optional<int> e1( 42 );
            Optional<int> e2( 7 );

            swap( e1, d1 );
            EXPECT_TRUE(  d1 );
            EXPECT_TRUE( !e1 );
            EXPECT_TRUE( *d1 == 42 );
        }
    }

    // Provides relational operators
    optional_relop_<int, int, int>();

    // Provides mixed-type relational operators
    optional_relop_<char, int, long>();

    // Allows to copy-construct optional
    {
        struct S
        {
            S( int v )
            : value( v ) {}

            int value;
        };

        S s( 7 );
        EXPECT_TRUE( make_optional( s )->value == 7 );
    }
}
