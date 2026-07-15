/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/sequence/function/code.hpp"
#include "genesis/util/math/random.hpp"
#include "genesis/util/tool/timer.hpp"

#include <stdexcept>
#include <string>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::util;
using namespace genesis::util::math;

// =================================================================================================
//     Codes
// =================================================================================================

TEST( Sequence, CodeAmbiguities )
{
    // Get all codes that are translateble in both directions.
    auto codes = nucleic_acid_codes_plain() + nucleic_acid_codes_degenerated() + "N-";

    // See if it works.
    for( auto code : codes ) {
        auto code_string = nucleic_acid_ambiguities( code );
        auto code_char   = nucleic_acid_ambiguity_code( code_string );
        EXPECT_EQ( code, code_char );
    }
}

TEST( Sequence, CodeContainment )
{
    EXPECT_TRUE( nucleic_acid_code_containment( 'A', 'A' ));
    EXPECT_TRUE( nucleic_acid_code_containment( 'C', 'C' ));
    EXPECT_TRUE( nucleic_acid_code_containment( 'G', 'G' ));
    EXPECT_TRUE( nucleic_acid_code_containment( 'T', 'T' ));

    EXPECT_TRUE( nucleic_acid_code_containment( 'A', 'W' ));
    EXPECT_TRUE( nucleic_acid_code_containment( 'T', 'W' ));
    EXPECT_TRUE( nucleic_acid_code_containment( 'C', 'H' ));

    EXPECT_FALSE( nucleic_acid_code_containment( 'C', 'D' ));
    EXPECT_FALSE( nucleic_acid_code_containment( 'A', 'T' ));
    EXPECT_FALSE( nucleic_acid_code_containment( 'A', 'Y' ));

    EXPECT_TRUE( nucleic_acid_code_containment( '-', 'C', true ));
    EXPECT_TRUE( nucleic_acid_code_containment( '-', 'H', true ));

    EXPECT_FALSE( nucleic_acid_code_containment( '-', 'C', false ));
    EXPECT_FALSE( nucleic_acid_code_containment( '-', 'H', false ));
}

// =================================================================================================
//     Reverse Complement
// =================================================================================================

TEST( Sequence, ReverseComplementPlain )
{
    // Single bases, both cases. 'U' normalizes to 'T'.
    EXPECT_EQ( "T", reverse_complement( "A" ));
    EXPECT_EQ( "G", reverse_complement( "C" ));
    EXPECT_EQ( "C", reverse_complement( "G" ));
    EXPECT_EQ( "A", reverse_complement( "T" ));
    EXPECT_EQ( "A", reverse_complement( "U" ));
    EXPECT_EQ( "t", reverse_complement( "a" ));
    EXPECT_EQ( "g", reverse_complement( "c" ));
    EXPECT_EQ( "c", reverse_complement( "g" ));
    EXPECT_EQ( "a", reverse_complement( "t" ));

    // A longer, hand-checked example. Case is preserved in the output.
    EXPECT_EQ( "TTTTGGCA", reverse_complement( "TGCCAAAA" ));
    EXPECT_EQ( "ttttggca", reverse_complement( "tgccaaaa" ));
    EXPECT_EQ( "tGcA", reverse_complement( "TgCa" ));

    // Edge case of an empty sequence.
    EXPECT_EQ( "", reverse_complement( "" ));

    // A palindromic sequence is its own reverse complement.
    EXPECT_EQ( "GAATTC", reverse_complement( "GAATTC" ));
}

TEST( Sequence, ReverseComplementDegenerated )
{
    // All degenerated codes, and their expected (correct) complements. Accepted by default.
    EXPECT_EQ( "W", reverse_complement( "W" )); // A/T   <-> A/T
    EXPECT_EQ( "S", reverse_complement( "S" )); // C/G   <-> C/G
    EXPECT_EQ( "K", reverse_complement( "M" )); // A/C   <-> G/T
    EXPECT_EQ( "M", reverse_complement( "K" )); // G/T   <-> A/C
    EXPECT_EQ( "Y", reverse_complement( "R" )); // A/G   <-> C/T
    EXPECT_EQ( "R", reverse_complement( "Y" )); // C/T   <-> A/G
    EXPECT_EQ( "V", reverse_complement( "B" )); // C/G/T <-> A/C/G
    EXPECT_EQ( "H", reverse_complement( "D" )); // A/G/T <-> A/C/T
    EXPECT_EQ( "D", reverse_complement( "H" )); // A/C/T <-> A/G/T
    EXPECT_EQ( "B", reverse_complement( "V" )); // A/C/G <-> C/G/T

    // Lower case also works, and its case is preserved in the output.
    EXPECT_EQ( "k", reverse_complement( "m" ));

    // Not accepting degenerated codes throws by default, for both single codes and codes
    // within sequences.
    EXPECT_THROW( reverse_complement( "M", false ), std::invalid_argument );
    EXPECT_THROW( reverse_complement( "ACGTW", false ), std::invalid_argument );

    // With throw_on_invalid disabled, rejected degenerated codes are instead replaced by a
    // case-matched 'N'/'n', same as any other invalid code (see ReverseComplementInvalidSilent).
    EXPECT_EQ( "N", reverse_complement( "M", false, false ));
    EXPECT_EQ( "n", reverse_complement( "m", false, false ));
    EXPECT_EQ( "NACGT", reverse_complement( "ACGTW", false, false ));
}

TEST( Sequence, ReverseComplementUndetermined )
{
    // All undetermined codes (see nucleic_acid_codes_undetermined()) have no strand-specific
    // meaning, and so are always self-complementary (case preserved for letters), independent
    // of accept_degenerated. They never throw and are never substituted.
    for( bool accept_degenerated : { true, false } ) {
        EXPECT_EQ( "N", reverse_complement( "N", accept_degenerated ));
        EXPECT_EQ( "n", reverse_complement( "n", accept_degenerated ));
        EXPECT_EQ( "O", reverse_complement( "O", accept_degenerated ));
        EXPECT_EQ( "o", reverse_complement( "o", accept_degenerated ));
        EXPECT_EQ( "X", reverse_complement( "X", accept_degenerated ));
        EXPECT_EQ( "x", reverse_complement( "x", accept_degenerated ));
        EXPECT_EQ( ".", reverse_complement( ".", accept_degenerated ));
        EXPECT_EQ( "-", reverse_complement( "-", accept_degenerated ));
        EXPECT_EQ( "?", reverse_complement( "?", accept_degenerated ));
    }
    EXPECT_EQ( "NAT", reverse_complement( "ATN" ));
    EXPECT_EQ( "-.?", reverse_complement( "?.-" ));
}

TEST( Sequence, ReverseComplementInvalid )
{
    // Chars that are not valid nucleic acid codes at all throw by default.
    EXPECT_THROW( reverse_complement( "E" ), std::invalid_argument );
    EXPECT_THROW( reverse_complement( "1" ), std::invalid_argument );
    EXPECT_THROW( reverse_complement( "@" ), std::invalid_argument );
    EXPECT_THROW( reverse_complement( " " ), std::invalid_argument );
    EXPECT_THROW( reverse_complement( "ACGTE" ), std::invalid_argument );

    // The exception reports how many invalid codes were found, but not which ones or where.
    try {
        reverse_complement( "AEC1G@T" );
        FAIL() << "Expected reverse_complement() to throw";
    } catch( std::invalid_argument const& ex ) {
        EXPECT_NE( std::string( ex.what() ).find( "3" ), std::string::npos );
    }
}

TEST( Sequence, ReverseComplementInvalidSilent )
{
    // With throw_on_invalid disabled, invalid codes are replaced by a case-matched 'N'/'n'
    // instead, and no exception is thrown.
    EXPECT_EQ( "N", reverse_complement( "E", true, false ));
    EXPECT_EQ( "n", reverse_complement( "e", true, false ));
    EXPECT_EQ( "N", reverse_complement( "1", true, false ));
    EXPECT_EQ( "N", reverse_complement( "@", true, false ));
    EXPECT_EQ( "NACGT", reverse_complement( "ACGTE", true, false ));
}

TEST( Sequence, ReverseComplementInplaceCount )
{
    // The in-place version returns the number of invalid/rejected codes it encountered.
    std::string clean = "ACGTACGT";
    EXPECT_EQ( 0, reverse_complement_inplace( clean ));
    EXPECT_EQ( "ACGTACGT", clean );

    std::string dirty = "ACGTE";
    EXPECT_EQ( 1, reverse_complement_inplace( dirty, true, false ));
    EXPECT_EQ( "NACGT", dirty );

    std::string very_dirty = "AEC1G@T";
    EXPECT_EQ( 3, reverse_complement_inplace( very_dirty, true, false ));
}

TEST( Sequence, ReverseComplementExceptionGuarantees )
{
    // reverse_complement_inplace() only offers a weak exception guarantee: if it throws,
    // sequence has already been overwritten with the (partial, substituted) result, and must
    // not be used further. We can't predict the exact content (it depends on loop order), but
    // it must differ from the original, since at least some of it was already processed.
    std::string mutated = "ACGTE";
    EXPECT_THROW( reverse_complement_inplace( mutated ), std::invalid_argument );
    EXPECT_NE( "ACGTE", mutated );

    // reverse_complement() (the copying convenience wrapper) instead offers a strong guarantee:
    // since it only ever mutates its own internal copy, the caller's original argument is
    // untouched even if the underlying reverse_complement_inplace() call throws.
    std::string original = "ACGTE";
    std::string const backup = original;
    EXPECT_THROW( reverse_complement( original ), std::invalid_argument );
    EXPECT_EQ( backup, original );
}

TEST( Sequence, ReverseComplementDoubleIsIdentity )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time( nullptr );
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // nucleic_acid_codes_all() covers all plain, degenerated, and undetermined codes (it does
    // not include 'U', which normalizes to 'T' and so is not its own round-trip partner). All of
    // these now round-trip correctly, including the undetermined codes that used to throw.
    auto const alphabet = nucleic_acid_codes_all();

    for( size_t t = 0; t < 100; ++t ) {
        auto const len = permuted_congruential_generator( 1, 200 );
        auto sequence = std::string( len, 'A' );
        for( auto& c : sequence ) {
            c = alphabet[ permuted_congruential_generator( 0, alphabet.size() - 1 ) ];
        }

        auto const rc   = reverse_complement( sequence );
        auto const rcrc = reverse_complement( rc );
        EXPECT_EQ( sequence, rcrc );
    }
}

// Speed test for reverse_complement(), to track its throughput over time.
// TEST( Sequence, ReverseComplementSpeed )
// {
//     size_t const n = 1000000000;
//     permuted_congruential_generator_init( 42 );
//     std::string const bases = "ACGT";

//     auto sequence = std::string( n, 'A' );
//     for( size_t i = 0; i < n; ++i ) {
//         sequence[i] = bases[ permuted_congruential_generator( 3 ) ];
//     }

//     Timer timer( true );
//     auto const result = reverse_complement( sequence );
//     auto const elapsed = timer.elapsed();
//     LOG_INFO << "reverse_complement() of " << n << " bases took " << elapsed
//              << "s (" << ( n / elapsed / 1000000.0 ) << " Mbases/s)";

//     // Basic sanity check, so that the compiler cannot optimize away the call.
//     EXPECT_EQ( n, result.size() );
// }
