#ifndef GENESIS_TAXONOMY_H_
#define GENESIS_TAXONOMY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2026 Lucas Czech

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
 * @brief This header includes all headers in 'taxonomy'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include <genesis/taxonomy/accession_lookup.hpp>
#include <genesis/taxonomy/format/accession_lookup_reader.hpp>
#include <genesis/taxonomy/format/json_reader.hpp>
#include <genesis/taxonomy/format/json_writer.hpp>
#include <genesis/taxonomy/format/ncbi.hpp>
#include <genesis/taxonomy/format/taxonomy_reader.hpp>
#include <genesis/taxonomy/format/taxonomy_writer.hpp>
#include <genesis/taxonomy/format/taxopath_generator.hpp>
#include <genesis/taxonomy/format/taxopath_parser.hpp>
#include <genesis/taxonomy/function/entropy_data.hpp>
#include <genesis/taxonomy/function/entropy.hpp>
#include <genesis/taxonomy/function/kmer_data.hpp>
#include <genesis/taxonomy/function/kmer_grouping.hpp>
#include <genesis/taxonomy/function/kmer.hpp>
#include <genesis/taxonomy/function/kmer_partitioning.hpp>
#include <genesis/taxonomy/function/operator.hpp>
#include <genesis/taxonomy/function/ranks.hpp>
#include <genesis/taxonomy/function/taxonomy.hpp>
#include <genesis/taxonomy/function/taxopath.hpp>
#include <genesis/taxonomy/function/tree.hpp>
#include <genesis/taxonomy/iterator/levelorder.hpp>
#include <genesis/taxonomy/iterator/postorder.hpp>
#include <genesis/taxonomy/iterator/preorder.hpp>
#include <genesis/taxonomy/printer/nested.hpp>
#include <genesis/taxonomy/taxon_data.hpp>
#include <genesis/taxonomy/taxon.hpp>
#include <genesis/taxonomy/taxonomy.hpp>
#include <genesis/taxonomy/taxopath.hpp>

#endif // include guard
