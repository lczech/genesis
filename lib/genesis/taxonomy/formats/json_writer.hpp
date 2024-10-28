#ifndef GENESIS_TAXONOMY_FORMATS_JSON_WRITER_H_
#define GENESIS_TAXONOMY_FORMATS_JSON_WRITER_H_

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/io/output_target.hpp"

#include <functional>
#include <iosfwd>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxonomy Json Writer
// =================================================================================================

/**
 * @brief Write a Taxonomy as a JSON document.
 *
 * The format is not standardized, the implementation here is not optimized, and the produced
 * Json document is rather wasteful in terms of disk space. It is merely meant as a simple way
 * to store the data of a Taxonomy in an easy way for experimentation and aspects of an application
 * that are not performance-critical.
 *
 * By default, this creates a Json document with the structure of the Taxonomy, with names,
 * ranks, and tax IDs as present. If additionally the function `taxon_to_json` is set,
 * other data from each Taxon can be stored in the Json objects.
 */
class TaxonomyJsonWriter
{
public:

    // ---------------------------------------------------------------------
    //     Functionals
    // ---------------------------------------------------------------------

    /**
     * @brief Function to store additional data from each Taxon in the JsonDocument.
     *
     * This function is called for every Taxon, and can be used to store additional data
     * in the Json object of that Taxon.
     */
    std::function<void( Taxon const&, utils::JsonDocument::ObjectType& )> taxon_to_json;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TaxonomyJsonWriter() = default;
    ~TaxonomyJsonWriter() = default;

    TaxonomyJsonWriter( TaxonomyJsonWriter const& ) = default;
    TaxonomyJsonWriter( TaxonomyJsonWriter&& )      = default;

    TaxonomyJsonWriter& operator= ( TaxonomyJsonWriter const& ) = default;
    TaxonomyJsonWriter& operator= ( TaxonomyJsonWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    void write( Taxonomy const& tax, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    utils::JsonDocument to_document( Taxonomy const& tax ) const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    utils::JsonDocument::ObjectType taxon_to_object_( Taxon const& tax ) const;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
