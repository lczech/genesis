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
 * @brief Implementation of Taxonomy class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/printers/nested.hpp"

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/utils/text/string.hpp"

#include <sstream>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Print
// =================================================================================================

void PrinterNested::print(
    std::ostream&   out,
    Taxonomy const& tax
) const {
    size_t lines = 0;
    print_to_ostream_( out, tax, 0, lines );
}

std::string PrinterNested::print(
    Taxonomy const& tax
) const {
    std::ostringstream res;
    print( res, tax );
    return res.str();
}

std::string PrinterNested::operator() (
    Taxonomy const& tax
) const {
    return print( tax );
}

bool PrinterNested::print_to_ostream_(
    std::ostream&   out,
    Taxonomy const& tax,
    size_t          depth,
    size_t&         lines
) const {
    // Check depth limit. The cast is always legel, because we first check.
    if( depth_limit_ > -1 && depth > static_cast< size_t >( depth_limit_ )) {
        return true;
    }

    // Indent string.
    auto in = utils::repeat( indent_string_, depth );

    bool finished = true;
    for( auto const& t : tax ) {
        if( line_limit_ > -1 && lines >= static_cast< size_t >( line_limit_ ) ) {
            return false;
        }

        // Print and count.
        out << in << t.name();
        if(( print_ranks_ && t.rank() != "" ) || ( print_ids_ && t.id() != "" ) || print_sizes_ ) {
            out << " (";
        }
        if( print_ranks_ && t.rank() != "" ) {
            out << t.rank();
        }
        if( print_ids_ && t.id() != "" ) {
            if( print_ranks_ && t.rank() != "" ) {
                out << ", ";
            }
            out << "id=" << t.id();
        }
        if( print_sizes_ ) {
            if(( print_ranks_ && t.rank() != "" ) || ( print_ids_ && t.id() != "" )) {
                out << ", ";
            }
            out << "size=" << total_taxa_count( t );
        }
        if(( print_ranks_ && t.rank() != "" ) || ( print_ids_ && t.id() != "" ) || print_sizes_ ) {
            out << ")";
        }
        out << "\n";
        ++lines;

        // Recurse.
        finished &= print_to_ostream_( out, t, depth + 1, lines );
    }

    if( depth == 0 && ! finished ) {
        out << "...\n";
    }
    return finished;
}

// =================================================================================================
//     Properties
// =================================================================================================

PrinterNested& PrinterNested::line_limit( int value )
{
    line_limit_ = value;
    return *this;
}

int PrinterNested::line_limit() const
{
    return line_limit_;
}

PrinterNested& PrinterNested::depth_limit( int value )
{
    depth_limit_ = value;
    return *this;
}

int PrinterNested::depth_limit() const
{
    return depth_limit_;
}

PrinterNested& PrinterNested::indent_string( std::string value )
{
    indent_string_ = value;
    return *this;
}

std::string PrinterNested::indent_string() const
{
    return indent_string_;
}

PrinterNested& PrinterNested::print_ranks( bool value )
{
    print_ranks_ = value;
    return *this;
}

bool PrinterNested::print_ranks() const
{
    return print_ranks_;
}

PrinterNested& PrinterNested::print_ids( bool value )
{
    print_ids_ = value;
    return *this;
}

bool PrinterNested::print_ids() const
{
    return print_ids_;
}

PrinterNested& PrinterNested::print_sizes( bool value )
{
    print_sizes_ = value;
    return *this;
}

bool PrinterNested::print_sizes() const
{
    return print_sizes_;
}

} // namespace taxonomy
} // namespace genesis
