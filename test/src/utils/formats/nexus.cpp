/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include "lib/utils/formats/nexus/document.hpp"
#include "lib/utils/formats/nexus/taxa.hpp"
#include "lib/utils/formats/nexus/trees.hpp"
#include "lib/utils/formats/nexus/writer.hpp"

#include <sstream>

using namespace genesis;
using namespace genesis::utils;

TEST(Nexus, Writer)
{
    auto doc = NexusDocument();

    // TODO create a factory method for each class, just for simplicity.
    // TODO make all functions comparing block names case insensitive!

    auto trees = make_unique<NexusTrees>();
    trees->add_tree( "life", "(human, mouse, rat);" );
    doc.set_block( std::move(trees) );

    auto taxa = make_unique<NexusTaxa>();
    taxa->add_taxa({ "human", "mouse", "rat" });
    doc.set_block( std::move(taxa) );

    EXPECT_TRUE( doc.has_block("TREES") );
    EXPECT_TRUE( doc.has_block("TAXA") );
    EXPECT_FALSE( doc.has_block("stargazer") );

    // std::ostringstream buffer;
    // auto writer = NexusWriter();
    // writer.to_stream( doc, buffer );
    // std::cout << buffer.str();
}
