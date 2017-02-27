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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/sequence/functions/codes.hpp"

using namespace genesis;
using namespace genesis::sequence;

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
