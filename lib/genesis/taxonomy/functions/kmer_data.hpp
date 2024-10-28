#ifndef GENESIS_TAXONOMY_FUNCTIONS_KMER_DATA_H_
#define GENESIS_TAXONOMY_FUNCTIONS_KMER_DATA_H_

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

#include "genesis/taxonomy/taxon_data.hpp"
#include "genesis/sequence/counts.hpp"

#include <cstddef>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Entropy Data
// =================================================================================================

/**
 * @brief Extra data to store in a Taxonomy to keep track of k-mer related quantities.
 *
 * This class is meant to be used as Taxon::data() entires, and stores additional data that we need
 * for the k-mer based reference data classification. A given Taxonomy can be converted to
 * use this data type via
 *
 *     reset_taxonomy_data<KmerTaxonData>( tax );
 *
 * after which all Taxa contain this data, which can be then accessed via
 *
 *     auto const& data = tax.data<KmerTaxonData>();
 *
 * This class is mostly meant for a special use case of k-mer based grouping or clustering.
 * It is probably not useful for much else.
 */
class KmerTaxonData : public BaseTaxonData
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~KmerTaxonData() override = default;

    // Move ctor and assignment.
    KmerTaxonData( KmerTaxonData&& )             = delete;
    KmerTaxonData& operator= ( KmerTaxonData&& ) = delete;

protected:

    KmerTaxonData() = default;

    // Copy ctor and assignment.
    KmerTaxonData( KmerTaxonData const& )             = default;
    KmerTaxonData& operator= ( KmerTaxonData const& ) = default;

public:

    static std::unique_ptr< KmerTaxonData > create()
    {
        return std::unique_ptr< KmerTaxonData >( new KmerTaxonData() );
    }

    virtual std::unique_ptr< BaseTaxonData > clone() const override
    {
        return std::unique_ptr< KmerTaxonData >( new KmerTaxonData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief Store the number of sequences (accessions) assigned to this Taxon.
     */
    size_t num_sequences = 0;

    /**
     * @brief Store the total size of all chromosomes/contigs/scaffolds of the accessions
     * assinged to this Taxon.
     */
    size_t sum_ref_bases = 0;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
