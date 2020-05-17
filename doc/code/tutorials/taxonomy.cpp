/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/genesis.hpp"

int main()
{
    using namespace genesis::taxonomy;
    using namespace genesis::utils;

    // Read from the SILVA taxonomy database
    TaxonomyReader tax_reader;
    tax_reader.name_field_position( 0 );
    tax_reader.rank_field_position( 2 );
    auto taxonomy = tax_reader.read( from_file( "path/to/tax_slv_ssu_123.1.txt" ));

    // Write to a file
    TaxonomyWriter tax_writer;
    tax_writer.write_ranks( true );
    tax_writer.write( taxonomy, to_file( "path/to/file.txt" ));

    // Traverse in preorder and print content
    for( auto const& tax_it : preorder( taxonomy )) {
        auto const indent = std::string( ' ', taxon_level( tax_it.taxon() ));
        auto const rank = ( tax_it.taxon().rank().empty() ? "" : " (" + tax_it.taxon().rank() + ")" );
        LOG_INFO << indent << tax_it.taxon().name() << rank;
    }

    // Add an object that stores entropy of sequences to each taxon
    auto add_entropy_data = [&]( Taxon& taxon ){
        taxon.reset_data( EntropyTaxonData::create() );
        // ... do initialization and further processing of the taxon data as needed
    };
    preorder_for_each( taxonomy, add_entropy_data );
}
