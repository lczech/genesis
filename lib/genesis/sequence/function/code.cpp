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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <genesis/sequence/function/code.hpp>

#include <genesis/util/text/char.hpp>
#include <genesis/util/text/string.hpp>
#include <genesis/util/color/color.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Name Lists
// =================================================================================================

static const std::unordered_map<char, std::string> nucleic_acid_code_to_name = {
    { 'A', "Adenine" },
    { 'C', "Cytosine" },
    { 'G', "Guanine" },
    { 'T', "Thymine" },
    { 'U', "Uracil" },

    { 'W', "Weak" },
    { 'S', "Strong" },
    { 'M', "aMino" },
    { 'K', "Keto" },
    { 'R', "puRine" },
    { 'Y', "pYrimidine" },

    { 'B', "not A" },
    { 'D', "not C" },
    { 'H', "not G" },
    { 'V', "not T" },

    { 'N', "any" },
    { 'O', "omitted" },
    { 'X', "masked" },
    { '.', "gap" },
    { '-', "gap" },
    { '?', "gap" }
};

static const std::unordered_map<char, std::string> amino_acid_code_to_name = {
    { 'A', "Alanine" },
    { 'B', "Aspartic acid or Asparagine" },
    { 'C', "Cysteine" },
    { 'D', "Aspartic acid" },
    { 'E', "Glutamic acid" },
    { 'F', "Phenylalanine" },
    { 'G', "Glycine" },
    { 'H', "Histidine" },
    { 'I', "Isoleucine" },
    { 'J', "Leucine or Isoleucine" },
    { 'K', "Lysine" },
    { 'L', "Leucine" },
    { 'M', "Methionine" },
    { 'N', "Asparagine" },
    { 'O', "Pyrrolysine" },
    { 'P', "Proline" },
    { 'Q', "Glutamine" },
    { 'R', "Arginine" },
    { 'S', "Serine" },
    { 'T', "Threonine" },
    { 'U', "Selenocysteine" },
    { 'V', "Valine" },
    { 'W', "Tryptophan" },
    { 'Y', "Tyrosine" },
    { 'Z', "Glutamic acid or Glutamine" },

    { 'X', "any" },
    { '*', "translation stop" },
    { '-', "gap" },
    { '?', "gap" }
};

// =================================================================================================
//     Color Lists
// =================================================================================================

static const std::map<char, std::string> nucleic_acid_text_colors_map = {
    { 'A', "Red" },
    { 'C', "Green" },
    { 'G', "Yellow" },
    { 'T', "Blue" },
    { 'U', "Blue" },

    { 'W', "DarkGray" },
    { 'S', "DarkGray" },
    { 'M', "DarkGray" },
    { 'K', "DarkGray" },
    { 'R', "DarkGray" },
    { 'Y', "DarkGray" },

    { 'B', "DarkGray" },
    { 'D', "DarkGray" },
    { 'H', "DarkGray" },
    { 'V', "DarkGray" },

    { 'N', "DarkGray" },
    { 'O', "DarkGray" },
    { 'X', "DarkGray" },
    { '.', "DarkGray" },
    { '-', "DarkGray" },
    { '?', "DarkGray" }
};

static const std::map<char, std::string> amino_acid_text_colors_map = {
    { 'A', "Blue" },
    { 'B', "DarkGray" },
    { 'C', "LightMagenta" },
    { 'D', "Magenta" },
    { 'E', "Magenta" },
    { 'F', "Blue" },
    { 'G', "LightRed" },
    { 'H', "Cyan" },
    { 'I', "Blue" },
    { 'J', "DarkGray" },
    { 'K', "Red" },
    { 'L', "Blue" },
    { 'M', "Blue" },
    { 'N', "Green" },
    { 'O', "DarkGray" },
    { 'P', "Yellow" },
    { 'Q', "Green" },
    { 'R', "Red" },
    { 'S', "Green" },
    { 'T', "Green" },
    { 'U', "DarkGray" },
    { 'V', "Blue" },
    { 'W', "Blue" },
    { 'Y', "Cyan" },
    { 'Z', "DarkGray" },

    { 'X', "DarkGray" },
    { '*', "DarkGray" },
    { '-', "DarkGray" },
    { '?', "DarkGray" }
};

static const std::map<char, genesis::util::color::Color> nucleic_acid_colors_map = {
    { 'A', { 1.0, 0.0, 0.0 } },
    { 'C', { 0.0, 1.0, 0.0 } },
    { 'G', { 1.0, 1.0, 0.0 } },
    { 'T', { 0.0, 0.0, 1.0 } },
    { 'U', { 0.0, 0.0, 1.0 } },

    { 'W', { 0.376, 0.376, 0.376 } },
    { 'S', { 0.376, 0.376, 0.376 } },
    { 'M', { 0.376, 0.376, 0.376 } },
    { 'K', { 0.376, 0.376, 0.376 } },
    { 'R', { 0.376, 0.376, 0.376 } },
    { 'Y', { 0.376, 0.376, 0.376 } },

    { 'B', { 0.5, 0.5, 0.5 } },
    { 'D', { 0.5, 0.5, 0.5 } },
    { 'H', { 0.5, 0.5, 0.5 } },
    { 'V', { 0.5, 0.5, 0.5 } },

    { 'N', { 0.5, 0.5, 0.5 } },
    { 'O', { 0.5, 0.5, 0.5 } },
    { 'X', { 0.5, 0.5, 0.5 } },
    { '.', { 0.5, 0.5, 0.5 } },
    { '-', { 0.5, 0.5, 0.5 } },
    { '?', { 0.5, 0.5, 0.5 } }
};

static const std::map<char, genesis::util::color::Color> amino_acid_colors_map = {
    { 'A', { 0.098, 0.500, 1.000 } },
    { 'B', { 0.376, 0.376, 0.376 } },
    { 'C', { 0.902, 0.500, 0.500 } },
    { 'D', { 0.800, 0.302, 0.800 } },
    { 'E', { 0.800, 0.302, 0.800 } },
    { 'F', { 0.098, 0.500, 1.000 } },
    { 'G', { 0.902, 0.600, 0.302 } },
    { 'H', { 0.098, 0.702, 0.702 } },
    { 'I', { 0.098, 0.500, 1.000 } },
    { 'J', { 0.376, 0.376, 0.376 } },
    { 'K', { 0.902, 0.200, 0.098 } },
    { 'L', { 0.098, 0.500, 1.000 } },
    { 'M', { 0.098, 0.500, 1.000 } },
    { 'N', { 0.098, 0.800, 0.098 } },
    { 'O', { 0.376, 0.376, 0.376 } },
    { 'P', { 0.800, 0.800, 0.000 } },
    { 'Q', { 0.098, 0.800, 0.098 } },
    { 'R', { 0.902, 0.200, 0.098 } },
    { 'S', { 0.098, 0.800, 0.098 } },
    { 'T', { 0.098, 0.800, 0.098 } },
    { 'U', { 0.376, 0.376, 0.376 } },
    { 'V', { 0.098, 0.500, 1.000 } },
    { 'W', { 0.098, 0.500, 1.000 } },
    { 'Y', { 0.098, 0.702, 0.702 } },
    { 'Z', { 0.376, 0.376, 0.376 } },

    { 'X', { 0.5, 0.5, 0.5 } },
    { '*', { 0.5, 0.5, 0.5 } },
    { '-', { 0.5, 0.5, 0.5 } },
    { '?', { 0.5, 0.5, 0.5 } }
};

// =================================================================================================
//     Ambiguity Lists
// =================================================================================================

static const std::unordered_map<char, std::string> nucleic_acid_ambiguity_char_map = {
    { 'A', "A" },
    { 'C', "C" },
    { 'G', "G" },
    { 'T', "T" },
    { 'U', "T" },

    { 'W', "AT" },
    { 'S', "CG" },
    { 'M', "AC" },
    { 'K', "GT" },
    { 'R', "AG" },
    { 'Y', "CT" },

    { 'B', "CGT" },
    { 'D', "AGT" },
    { 'H', "ACT" },
    { 'V', "ACG" },

    { 'N', "ACGT" },
    { 'O', "-" },
    { 'X', "-" },
    { '.', "-" },
    { '-', "-" },
    { '?', "-" }
};

static const std::unordered_map< std::string, char > nucleic_acid_ambiguity_string_map = {
    { "A", 'A' },
    { "C", 'C' },
    { "G", 'G' },
    { "T", 'T' },

    { "AT", 'W' },
    { "CG", 'S' },
    { "AC", 'M' },
    { "GT", 'K' },
    { "AG", 'R' },
    { "CT", 'Y' },

    { "CGT", 'B' },
    { "AGT", 'D' },
    { "ACT", 'H' },
    { "ACG", 'V' },

    { "ACGT", 'N' },
    { "-", '-' }
};

// =================================================================================================
//     Codes
// =================================================================================================

// ---------------------------------------------------------------------
//     Nucleic Acids
// ---------------------------------------------------------------------

std::string nucleic_acid_codes_plain()
{
    return "ACGT";
}

std::string nucleic_acid_codes_degenerated()
{
    return "WSMKRYBDHV";
}

std::string nucleic_acid_codes_undetermined()
{
    return "NOX.-?";
}

std::string nucleic_acid_codes_undetermined_letters()
{
    return "NOX";
}

std::string nucleic_acid_codes_all()
{
    return nucleic_acid_codes_plain()
         + nucleic_acid_codes_degenerated()
         + nucleic_acid_codes_undetermined();
}

std::string nucleic_acid_codes_all_letters()
{
    return nucleic_acid_codes_plain()
         + nucleic_acid_codes_degenerated()
         + nucleic_acid_codes_undetermined_letters();
}

// ---------------------------------------------------------------------
//     Amino Acids
// ---------------------------------------------------------------------

std::string amino_acid_codes_plain()
{
    return "ACDEFGHIKLMNOPQRSTUVWY";
}

std::string amino_acid_codes_degenerated()
{
    return "BJZ";
}

std::string amino_acid_codes_undetermined()
{
    return "X*-?.";
}

std::string amino_acid_codes_all()
{
    return amino_acid_codes_plain()
         + amino_acid_codes_degenerated()
         + amino_acid_codes_undetermined();
}

// ---------------------------------------------------------------------
//     Misc
// ---------------------------------------------------------------------

std::string normalize_code_alphabet( std::string const& alphabet )
{
    // Uppercase, sort, uniq the alphabet.
    auto normalized = genesis::util::text::to_upper( alphabet );
    std::sort( normalized.begin(), normalized.end() );
    normalized.erase( std::unique( normalized.begin(), normalized.end() ), normalized.end() );
    return normalized;
}

char normalize_nucleic_acid_code( char code, bool accept_degenerated )
{
    switch( code ) {
        case 'u':
        case 'U':
        case 't':
        case 'T':
            return 'T';
        case 'a':
        case 'A':
            return 'A';
        case 'c':
        case 'C':
            return 'C';
        case 'g':
        case 'G':
            return 'G';
        case 'w':
        case 'W':
        case 's':
        case 'S':
        case 'm':
        case 'M':
        case 'k':
        case 'K':
        case 'r':
        case 'R':
        case 'y':
        case 'Y':
        case 'b':
        case 'B':
        case 'd':
        case 'D':
        case 'h':
        case 'H':
        case 'v':
        case 'V':
            if( accept_degenerated ) {
                return genesis::util::text::to_upper( code );
            } else {
                throw std::invalid_argument(
                    "Degenerated nucleic acid code not accepted: " + std::string( 1, code )
                );
            }
        case 'n':
        case 'N':
        case 'o':
        case 'O':
        case 'x':
        case 'X':
        case '.':
        case '-':
        case '?':
            return '-';
        default:
            throw std::invalid_argument( "Not a nucleic acid code: " + std::string( 1, code ) );
    }
}

char normalize_amino_acid_code( char code, bool accept_degenerated )
{
    switch( code ) {
        case 'A':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'Y':
            return code;
        case 'a':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'y':
            return genesis::util::text::to_upper( code );
        case 'B':
        case 'J':
        case 'Z':
        case 'b':
        case 'j':
        case 'z':
            if( accept_degenerated ) {
                return genesis::util::text::to_upper( code );
            } else {
                throw std::invalid_argument(
                    "Degenerated amino acid code not accepted: " + std::string( 1, code )
                );
            }
        case 'X':
        case 'x':
        case '*':
        case '-':
        case '?':
        case '.':
            return '-';
        default:
            throw std::invalid_argument( "Not an amino acid code: " + std::string( 1, code ) );
    }
}

// ---------------------------------------------------------------------
//     Reverse Complement Tables
// ---------------------------------------------------------------------

namespace {

// Per-byte lookup tables driving reverse_complement_inplace(): comp[c] is the char to write for
// input byte c, and invalid[c] tells whether c was not an accepted nucleic acid code (in which
// case comp[c] is the case-matched 'N'/'n' sentinel instead of a real complement).
struct ReverseComplementTable
{
    std::array<char, 256> complement;
    std::array<bool, 256> invalid;
};

ReverseComplementTable make_reverse_complement_table( bool accept_degenerated )
{
    ReverseComplementTable table;

    // Default fill: every byte is invalid, substituted by a case-matched 'N'/'n'.
    for( size_t i = 0; i < table.complement.size(); ++i ) {
        bool const is_lower = ( i >= 'a' && i <= 'z' );
        table.complement[i]    = is_lower ? 'n' : 'N';
        table.invalid[i] = true;
    }

    // Mark a single byte as a valid, accepted code with the given complement.
    auto set = [&]( char from, char to ) {
        table.complement[ static_cast<unsigned char>( from ) ] = to;
        table.invalid[ static_cast<unsigned char>( from ) ] = false;
    };

    // Mark an upper/lower case pair of bytes as valid, accepted codes.
    auto set_case = [&]( char from_upper, char to_upper, char from_lower, char to_lower ) {
        set( from_upper, to_upper );
        set( from_lower, to_lower );
    };

    // Plain bases. 'U' (RNA) behaves exactly like 'T' for complementing purposes.
    set_case( 'A', 'T', 'a', 't' );
    set_case( 'C', 'G', 'c', 'g' );
    set_case( 'G', 'C', 'g', 'c' );
    set_case( 'T', 'A', 't', 'a' );
    set_case( 'U', 'A', 'u', 'a' );

    // Undetermined codes have no strand-specific meaning, and so are always self-complementary,
    // independent of accept_degenerated.
    set_case( 'N', 'N', 'n', 'n' );
    set_case( 'O', 'O', 'o', 'o' );
    set_case( 'X', 'X', 'x', 'x' );
    set( '.', '.' );
    set( '-', '-' );
    set( '?', '?' );

    // Degenerated codes. If not accepted, leave them at the default invalid/sentinel entry.
    if( accept_degenerated ) {
        set_case( 'W', 'W', 'w', 'w' );
        set_case( 'S', 'S', 's', 's' );
        set_case( 'M', 'K', 'm', 'k' );
        set_case( 'K', 'M', 'k', 'm' );
        set_case( 'R', 'Y', 'r', 'y' );
        set_case( 'Y', 'R', 'y', 'r' );
        set_case( 'B', 'V', 'b', 'v' );
        set_case( 'D', 'H', 'd', 'h' );
        set_case( 'H', 'D', 'h', 'd' );
        set_case( 'V', 'B', 'v', 'b' );
    }

    return table;
}

ReverseComplementTable const& reverse_complement_table( bool accept_degenerated )
{
    static ReverseComplementTable const permissive = make_reverse_complement_table( true );
    static ReverseComplementTable const strict     = make_reverse_complement_table( false );
    return accept_degenerated ? permissive : strict;
}

} // anonymous namespace

// ---------------------------------------------------------------------
//     Reverse Complement
// ---------------------------------------------------------------------

size_t reverse_complement_inplace(
    std::string& sequence,
    bool accept_degenerated,
    bool throw_on_invalid
) {
    auto const& table = reverse_complement_table( accept_degenerated );
    size_t invalid_count = 0;

    // Two-pointer in-place swap. No branches in the hot loop: both the complement and the
    // invalid-count contribution come from unconditional table lookups.
    size_t const n = sequence.size();
    size_t i = 0;
    size_t j = ( n > 0 ) ? n - 1 : 0;
    while( i < j ) {
        auto const ci = static_cast<unsigned char>( sequence[i] );
        auto const cj = static_cast<unsigned char>( sequence[j] );

        invalid_count += table.invalid[ci];
        invalid_count += table.invalid[cj];

        sequence[i] = table.complement[cj];
        sequence[j] = table.complement[ci];

        ++i;
        --j;
    }
    if( n > 0 && i == j ) {
        auto const c = static_cast<unsigned char>( sequence[i] );
        invalid_count += table.invalid[c];
        sequence[i] = table.complement[c];
    }

    if( invalid_count > 0 && throw_on_invalid ) {
        throw std::invalid_argument(
            "Sequence contains " + std::to_string( invalid_count ) +
            " invalid or rejected nucleic acid code(s)"
        );
    }
    return invalid_count;
}

std::string reverse_complement(
    std::string_view sequence,
    bool accept_degenerated,
    bool throw_on_invalid
) {
    auto result = std::string( sequence );
    reverse_complement_inplace( result, accept_degenerated, throw_on_invalid );
    return result;
}

char nucleic_acid_transition( char code )
{
    switch( code ) {
        case 'a':
        case 'A':
            return 'G';
        case 'c':
        case 'C':
            return 'T';
        case 'g':
        case 'G':
            return 'A';
        case 'u':
        case 'U':
        case 't':
        case 'T':
            return 'C';
        default:
            throw std::invalid_argument(
                "No transition base for nucleic acid code: " + std::string( 1, code )
            );
    }
}

bool nucleic_acid_code_containment( char a, char b, bool undetermined_matches_all )
{
    // This is slightly bad, because we are not actually encoding binary,
    // but using hex here (for simplicity of writing...). Should still work as expected.
    auto binary_code_ = [ undetermined_matches_all ]( char c ){
        switch( c ) {
            case 'A':
                return 0x0001;
            case 'C':
                return 0x0010;
            case 'G':
                return 0x0100;
            case 'T':
                return 0x1000;

            case 'W':
                return 0x1001;
            case 'S':
                return 0x0110;
            case 'M':
                return 0x0011;
            case 'K':
                return 0x1100;
            case 'R':
                return 0x0101;
            case 'Y':
                return 0x1010;

            case 'B':
                return 0x1110;
            case 'D':
                return 0x1101;
            case 'H':
                return 0x1011;
            case 'V':
                return 0x0111;

            case '-': {
                if( undetermined_matches_all ) {
                    return 0x1111;
                } else {
                    return 0x0000;
                }
            }

            default:
                // We already checked for invalid chars in the normalize function.
                // Just do this to be safe.
                assert( false );
                throw std::invalid_argument( "Not a nucleic acid code: " + std::string( 1, c ) );
        }
    };

    auto const an = normalize_nucleic_acid_code( a, true );
    auto const bn = normalize_nucleic_acid_code( b, true );

    auto const ab = binary_code_( an );
    auto const bb = binary_code_( bn );

    return ( ab & bb ) > 0;
}

// =================================================================================================
//     Color Codes
// =================================================================================================

std::map<char, std::string> nucleic_acid_text_colors()
{
    return nucleic_acid_text_colors_map;
}

std::map<char, std::string> amino_acid_text_colors()
{
    return amino_acid_text_colors_map;
}

std::map<char, genesis::util::color::Color> nucleic_acid_colors()
{
    return nucleic_acid_colors_map;
}

std::map<char, genesis::util::color::Color> amino_acid_colors()
{
    return amino_acid_colors_map;
}

// =================================================================================================
//     Translate Codes
// =================================================================================================

std::string nucleic_acid_name( char code )
{
    auto ucode = toupper(code);
    if( nucleic_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid code '" + std::string( 1, code ) + "'." );
    }
    return nucleic_acid_code_to_name.at( ucode );
}

std::string amino_acid_name( char code )
{
    auto ucode = toupper(code);
    if( amino_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid amino acid code '" + std::string( 1, code ) + "'." );
    }
    return amino_acid_code_to_name.at( ucode );
}

std::string nucleic_acid_ambiguities( char code )
{
    auto ucode = toupper(code);
    if( nucleic_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid code '" + std::string( 1, code ) + "'." );
    }
    return nucleic_acid_ambiguity_char_map.at( ucode );
}

char nucleic_acid_ambiguity_code( std::string codes )
{
    // Uppercase, sort, uniq the codes.
    auto tmp = genesis::util::text::to_upper( codes );
    std::sort( tmp.begin(), tmp.end() );
    tmp.erase( std::unique( tmp.begin(), tmp.end() ), tmp.end() );

    if( nucleic_acid_ambiguity_string_map.count( tmp ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid codes '" + codes + "'." );
    }
    return nucleic_acid_ambiguity_string_map.at( tmp );
}

} // namespace sequence
} // namespace genesis
