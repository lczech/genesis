#ifndef GENESIS_TAXONOMY_FUNCTIONS_KMER_DATA_H_
#define GENESIS_TAXONOMY_FUNCTIONS_KMER_DATA_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include <cstddef>
#include <limits>
#include <memory>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Kmer Data
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
    //     Typedefs and Enums
    // -------------------------------------------------------------------

    /**
     * @brief Store the status for a Taxon with respect to its grouping.
     *
     * @see group_by_taxon_sizes()
     */
    enum class Status
    {
        /**
         * @brief Initial status: The Taxon has not yet been processed.
         */
        kUnprocessed,

        /**
         * @brief The Taxon and all its children are assigned to a group.
         *
         * This is used for Taxa whose sizes are within the set limits.
         */
        kGroupAssigned,

        /**
         * @brief The Taxon is not assigned to a group, but expanded into smaller groups instead.
         *
         * This is used for higher ranks, which contain too much data to be a single group.
         * Instead, its children will be assigned to separate groups of smaller sizes.
         */
        kGroupExpanded
    };

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
     * @brief Store the sum of the length of the chromosomes/contigs/scaffolds of the accessions
     * assinged to this Taxon.
     */
    size_t sum_seq_lengths = 0;

    /**
     * @brief Store the total number of sequences (accessions) of this Taxon and its children.
     *
     * @see accumulate_kmer_taxon_counts()
     */
    size_t clade_num_sequences = 0;

    /**
     * @brief Store the total the length of all chromosomes/contigs/scaffolds of the accessions
     * assinged to this Taxonn and its children.
     *
     * @see accumulate_kmer_taxon_counts()
     */
    size_t clade_sum_seq_lengths = 0;

    /**
     * @brief Status of the taxon with respect to its group or partition.
     *
     * @see group_by_taxon_sizes()
     */
    Status status = Status::kUnprocessed;

    /**
     * @brief Index of the taxon, if grouped or partitioned.
     *
     * @see group_by_taxon_sizes()
     */
    size_t index = std::numeric_limits<size_t>::max();

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
