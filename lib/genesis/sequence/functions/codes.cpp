/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup sequence
 */

#include "genesis/sequence/functions/codes.hpp"

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color.hpp"

#include <algorithm>
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

static const std::map<char, utils::Color> nucleic_acid_colors_map = {
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

static const std::map<char, utils::Color> amino_acid_colors_map = {
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

std::string nucleic_acid_codes_all()
{
    return nucleic_acid_codes_plain()
         + nucleic_acid_codes_degenerated()
         + nucleic_acid_codes_undetermined();
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
    return "X*-?";
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
    auto normalized = utils::to_upper_ascii( alphabet );
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
                return utils::to_upper_ascii( code );
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
            return utils::to_upper_ascii( code );
        case 'B':
        case 'J':
        case 'Z':
        case 'b':
        case 'j':
        case 'z':
            if( accept_degenerated ) {
                return utils::to_upper_ascii( code );
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
            return '-';
        default:
            throw std::invalid_argument( "Not an amino acid code: " + std::string( 1, code ) );
    }
}

std::string reverse_complement( std::string const& sequence, bool accept_degenerated )
{
    // Dummy result string.
    auto result = std::string( sequence.size(), '-' );

    // Get rev comp char. We only need to check upper case as we normalized before.
    auto rev_comp = []( char c ){
        switch( c ) {
            case 'A':
                return 'T';
            case 'C':
                return 'G';
            case 'G':
                return 'C';
            case 'T':
                return 'A';

            case 'W':
                return 'W';
            case 'S':
                return 'S';
            case 'M':
                return 'K';
            case 'K':
                return 'M';
            case 'R':
                return 'Y';
            case 'Y':
                return 'R';

            case 'B':
                return 'V';
            case 'D':
                return 'H';
            case 'H':
                return 'D';
            case 'V':
                return 'B';

            default:
                // We already checked for invalid chars in the normalize function.
                // Just do this to be safe.
                assert( false );
                throw std::invalid_argument( "Not a nucleic acid code: " + std::string( 1, c ) );
        }
    };

    // Stupid and simple.
    for( size_t i = 0; i < sequence.size(); ++i ) {
        char c = sequence[i];

        // Slighly hacky: the normalize function turns 'N' into '-'.
        // We don't want that here, so we have to treat that special case.
        if( c == 'n' || c == 'N' ) {
            if( accept_degenerated ) {
                result[ sequence.size() - i - 1 ] = 'N';
                continue;
            } else {
                throw std::invalid_argument(
                    "Degenerated nucleic acid code not accepted: " + std::string( 1, c )
                );
            }
        }

        // First normalize, then reverse. Slighly inefficition, but saves code duplication.
        c = normalize_nucleic_acid_code( c, accept_degenerated );
        result[ sequence.size() - i - 1 ] = rev_comp( c );
    }
    return result;
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

std::map<char, utils::Color> nucleic_acid_colors()
{
    return nucleic_acid_colors_map;
}

std::map<char, utils::Color> amino_acid_colors()
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
    auto tmp = utils::to_upper_ascii( codes );
    std::sort( tmp.begin(), tmp.end() );
    tmp.erase( std::unique( tmp.begin(), tmp.end() ), tmp.end() );

    if( nucleic_acid_ambiguity_string_map.count( tmp ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid codes '" + codes + "'." );
    }
    return nucleic_acid_ambiguity_string_map.at( tmp );
}

} // namespace sequence
} // namespace genesis
