#ifndef GENESIS_TAXONOMY_FORMATS_JSON_READER_H_
#define GENESIS_TAXONOMY_FORMATS_JSON_READER_H_

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
#include "genesis/utils/io/input_source.hpp"

#include <functional>
#include <iosfwd>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxonomy Json Writer
// =================================================================================================

/**
 * @brief Read a Taxonomy from a JSON document.
 *
 * See JsonWriter for the caveats on this format. This reader is equally not meant for
 * a standardized way of storing a Taxonomy: We barly do any error checking during the reading
 * and the code is far from optimized.
 */
class TaxonomyJsonReader
{
public:

    // ---------------------------------------------------------------------
    //     Functionals
    // ---------------------------------------------------------------------

    /**
     * @brief Function to read additional data from each Taxon in the JsonDocument.
     *
     * This function is called for every Taxon, and can be used to read the additional data
     * in the Json object of that Taxon that was stored there by the JsonWriter.
     */
    std::function<void( utils::JsonDocument::ObjectType const&, Taxon& )> json_to_taxon;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TaxonomyJsonReader() = default;
    ~TaxonomyJsonReader() = default;

    TaxonomyJsonReader( TaxonomyJsonReader const& ) = default;
    TaxonomyJsonReader( TaxonomyJsonReader&& )      = default;

    TaxonomyJsonReader& operator= ( TaxonomyJsonReader const& ) = default;
    TaxonomyJsonReader& operator= ( TaxonomyJsonReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    /**
     * @brief Read a Taxonomy from an input source.
     */
    Taxonomy read( std::shared_ptr<utils::BaseInputSource> source ) const;

    /**
     * @brief Read a Taxonomy from an input JsonDocument.
     *
     * This is destructive, to save at least some memory while reading.
     */
    Taxonomy read( utils::JsonDocument& doc ) const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    void object_to_taxon_( utils::JsonDocument::ObjectType& obj, Taxon& tax ) const;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
