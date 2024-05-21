/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/functions/dict.hpp"

#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Sequence Dict
// =================================================================================================

SequenceDict read_sequence_dict( std::shared_ptr< utils::BaseInputSource > source )
{
    // Prepare the result and the input stream.
    SequenceDict result;
    utils::InputStream it( source );

    // Read lines while there is data.
    // We don't need to be super efficient here, dict files typically only contain a few dozen lines.
    size_t line_cnt = 0;
    while( it ) {
        ++line_cnt;

        // Get the line and split it on tabs; do some basic format sanity checks.
        auto const line = utils::split( it.get_line(), "\t" );
        if( line.size() == 0 ) {
            continue;
        }
        if( line[0].length() != 3 || line[0][0] != '@' ) {
            throw std::runtime_error(
                "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                " does not start with a header record type code '@XX', but with '" + line[0] + "'."
            );
        }

        // Now we know that we have a valid line. Let's see if it's one that we are intersted in.
        assert( line.size() > 0 );
        assert( line[0].size() == 3 );
        assert( line[0][0] == '@' );
        if( line[0] != "@SQ" ) {
            continue;
        }

        // Now we can go through the fields of the line, and find the TAG:VALUE pairs that we want.
        std::string sn;
        size_t ln = 0;
        for( size_t i = 1; i < line.size(); ++i ) {
            auto const& field = line[i];
            if( field.size() < 3 || field[2] != ':' ) {
                throw std::runtime_error(
                    "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                    " contains an @SQ record that is not of the form 'TAG:VALUE', "
                    "but '" + field + "'."
                );
            }
            assert( field.size() >=3 );
            assert( field[2] == ':' );

            // Get the two fields we are itnerested in.
            if( utils::starts_with( field, "SN:" )) {
                sn = field.substr( 3 );
            }
            if( utils::starts_with( field, "LN:" )) {
                if( ! utils::is_convertible_to_unsigned_integer( field.substr( 3 ) )) {
                    throw std::runtime_error(
                        "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                        " contains an @SQ record with a field for the sequence length LN"
                        " whose VALUE is not a number, but '" + field.substr( 3 ) + "'."
                    );
                }
                ln = utils::convert_to_unsigned_integer( field.substr( 3 ));
            }
        }

        // We are a bit pedantic here, and throw in case of empty results.
        // Technically, those could actually occur in the data, but that would be some weird edge
        // case anyway, that would lead to other problems downstream.
        if( sn.empty() || ln == 0 ) {
            throw std::runtime_error(
                "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                " contains an @SQ record with no valid SN or LN fields."
            );
        }

        // Now we are good; add to the list.
        result.add( sn, ln );
    }

    return result;
}

SequenceDict read_sequence_fai( std::shared_ptr<utils::BaseInputSource> source )
{
    // Prepare the result and the input stream.
    SequenceDict result;
    utils::InputStream it( source );

    // Read lines while there is data.
    // We don't need to be super efficient here, fai files typically only contain a few dozen lines.
    size_t line_cnt = 0;
    while( it ) {
        ++line_cnt;

        // Get the line and split it on tabs; do some basic format sanity checks.
        auto const line = utils::split( it.get_line(), "\t" );
        if( line.size() == 0 ) {
            continue;
        }
        if( line.size() != 5 ) {
            throw std::runtime_error(
                "Invalid sequence fai file: Line " + std::to_string( line_cnt ) +
                " has " + std::to_string( line.size() ) + " columns instead of the expected 5 columns."
            );
        }

        // Now we know that we have a valid line. Let's see if it's one that we are intersted in.
        assert( line.size() == 5 );
        std::string const sn = line[0];
        if( ! utils::is_convertible_to_unsigned_integer( line[1] )) {
            throw std::runtime_error(
                "Invalid sequence fai file: Line " + std::to_string( line_cnt ) +
                " contains a record with a LENGTH field that is not a number, but '" + line[1] + "'."
            );
        }
        size_t const ln = utils::convert_to_unsigned_integer( line[1] );

        // We are a bit pedantic here, and throw in case of empty results.
        // Technically, those could actually occur in the data, but that would be some weird edge
        // case anyway, that would lead to other problems downstream.
        if( sn.empty() || ln == 0 ) {
            throw std::runtime_error(
                "Invalid sequence fai file: Line " + std::to_string( line_cnt ) +
                " contains a record with invalid NAME or LENGTH fields."
            );
        }

        // Now we are good; add to the list.
        result.add( sn, ln );
    }

    return result;
}

template<class T>
SequenceDict sequence_iterable_to_dict_( T const& input )
{
    SequenceDict result;
    for( auto const& elem : input ) {
        result.add( elem.label(), elem.length() );
    }
    return result;
}

SequenceDict sequence_set_to_dict( SequenceSet const& set )
{
    return sequence_iterable_to_dict_( set );
}

SequenceDict reference_genome_to_dict( ReferenceGenome const& rg )
{
    return sequence_iterable_to_dict_( rg );
}

// =================================================================================================
//     Reference
// =================================================================================================

bool compatible_references(
    SequenceDict const& lhs,
    SequenceDict const& rhs,
    ReferenceComparisonMode mode
) {
    // Basic check for forbidden edge case of empty strings first.
    auto check_dict_empty_names_ = []( SequenceDict const& dict ) {
        for( auto const& e : dict ) {
            if( e.name.empty() ) {
                throw std::invalid_argument( "Invalid reference sequences with empty names." );
            }
        }
    };
    check_dict_empty_names_( lhs );
    check_dict_empty_names_( rhs );

    // We have a few different combinations to test. Let's see if that works.
    switch( mode ) {
        case ReferenceComparisonMode::kStrict: {
            // Strict: Compar the sizes of both sets. Then, we only need to iterate one of them,
            // as this then needs to exactly match every entry in the other.
            if( lhs.size() != rhs.size() ) {
                return false;
            }
            for( auto const& e : lhs ) {
                if( ! rhs.contains( e.name ) || e.length != rhs.get( e.name ).length ) {
                    return false;
                }
            }
            break;
        }
        case ReferenceComparisonMode::kLeftSuperset: {
            // Left superset is equivalent to right subset, so we only need to go through everything
            // that the rhs has and compare those.
            for( auto const& e : rhs ) {
                if( ! lhs.contains( e.name ) || e.length != lhs.get( e.name ).length ) {
                    return false;
                }
            }
            break;
        }
        case ReferenceComparisonMode::kRightSuperset: {
            // Same the other way round. Bit of code duplication, but okay for now.
            for( auto const& e : lhs ) {
                if( ! rhs.contains( e.name ) || e.length != rhs.get( e.name ).length ) {
                    return false;
                }
            }
            break;
        }
        case ReferenceComparisonMode::kSharedOnly: {
            // Shared only: We ignore the ones that are not in either.
            for( auto const& e : lhs ) {
                if( ! rhs.contains( e.name )) {
                    continue;
                }
                if( e.length != rhs.get( e.name ).length ) {
                    return false;
                }
            }
            break;
        }
        default: {
            throw std::invalid_argument(
                "Invalid ReferenceComparisonMode in compatible_references()"
            );
        }
    }

    // Finally, we have exhausted all false cases above, so we can return that they are compatible.
    return true;
}

bool verify( SequenceDict const& dict, SequenceSet const& set, bool match_first_word )
{
    if( dict.size() != set.size() ) {
        return false;
    }
    for( size_t i = 0; i < dict.size(); ++i ) {
        if( dict[i].name.empty() || set[i].label().empty() ) {
            return false;
        }
        if( match_first_word ) {
            auto const s_dct = utils::split( dict[i].name, "\t " );
            auto const s_set = utils::split( set[i].label(), "\t " );
            assert( ! s_dct.empty() );
            assert( ! s_set.empty() );
            if( s_dct[0] != s_set[0] ) {
                return false;
            }
        } else if( dict[i].name != set[i].label() || dict[i].length != set[i].length() ) {
            return false;
        }
    }
    return true;
}

} // namespace sequence
} // namespace genesis
