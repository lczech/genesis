#ifndef GENESIS_POPULATION_FUNCTIONS_VARIANT_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FUNCTIONS_VARIANT_INPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/functions/genome_locus.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/functions/dict.hpp"
#include "genesis/sequence/sequence_dict.hpp"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Input Interator Helper Functions
// =================================================================================================

/**
 * @brief Helper function to check that some Variant input is sorted properly.
 *
 * The function creates a `std::function` that can be used with a VariantInputIterator to check
 * the order (and length) of the Variant%s being processed.
 *
 * By default, the different types of VariantInputIterator%s that we create for different file types
 * with the `make_variant_input_iterator_from_...()` simply iterate over their respective input
 * files as they are. However, we might want to check that their order is correct, or that their
 * lengths fit our expectation.
 *
 * This function by default checks this, using lexicographical order for the chromosomes, and
 * numerical order for the positions within chromosomes. This however might not always be the order
 * as present in the input source. In order to provide a custom order, the function optinally takes
 * a @link ::genesis::sequence::SequenceDict SequenceDict@endlink, which is used for the order
 * instead. See there for details on how to create such a dictionary from, e.g., `.fai`, `.dict`,
 * or reference genome `.fasta` files.
 *
 * Furthermore, when a @p sequence_dict is provided, using the @p check_sequence_lengths, we can
 * also check that the positions within each chromosome that we encounter in the input source fit
 * with the expectations of that dictionary. This serves as an additional sanity check of the
 * input files.
 *
 * If any of these checks fail, an exception is thrown.
 *
 * Exemplary usage:
 *
 *     // Get a sequence dict from a fai file
 *     auto sequence_dict = read_sequence_fai( from_file( fai_file ));
 *
 *     // Create a VariantInputIterator, for example from a sync file
 *     auto variant_iterator = make_variant_input_iterator_from_sync_file( sync_file );
 *
 *     // Add the visitor that checks order, using a
 *     variant_iterator.add_visitor(
 *         make_variant_input_iterator_sequence_order_visitor(
 *             sequence_dict, true
 *         )
 *     );
 *
 *     // Use the iterator as usual.
 *     for( auto const& variant : variant_iterator ) {
 *         // ...
 *     }
 *
 * @see See @link ::genesis::utils::LambdaIterator::add_visitor() LambdaIterator::add_visitor()@endlink
 * for the function of the underlying iterator that accepts the returned function from here.
 *
 * @see make_variant_input_iterator_from_vector(),
 * make_variant_input_iterator_from_sam_file(),
 * make_variant_input_iterator_from_pileup_file(),
 * make_variant_input_iterator_from_sync_file(),
 * make_variant_input_iterator_from_frequency_table_file(),
 * make_variant_input_iterator_from_pool_vcf_file(),
 * make_variant_input_iterator_from_individual_vcf_file(),
 * make_variant_input_iterator_from_variant_parallel_input_iterator()
 */
inline std::function<void(Variant const&)> make_variant_input_iterator_sequence_order_visitor(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = {},
    bool check_sequence_lengths = true
) {
    // We create copies of stuff here using lambda capture.
    // Those will then live on once the lambda is in use.
    GenomeLocus current_locus;
    return [ sequence_dict, check_sequence_lengths, current_locus ]( Variant const& variant ) mutable {
        // When a dict is provided, we need to take care of the special empty case for the chromosome
        // name, which will happen in the very first time that this callback is used.
        // The empty string won't be part of the dict chromosomes. For simplicity,
        // we here always check that case. We later check that the variant itself does not have
        // an empty chromsome name, in order to not regress to this starting condition.
        if( ! current_locus.empty() ) {

            // If a dict is provided, but the chromosome is not in there, the following check throws.
            if( ! locus_greater( variant.chromosome, variant.position, current_locus, sequence_dict )) {
                throw std::runtime_error(
                    "Invalid sorting order of input Variants. By default, we expect lexicographical "
                    "sorting of chromosomes, and then sorting by position within chromosomes. "
                    "Alternatively, when a sequence dictionary is specified (such as from a .dict "
                    "or .fai file, or from a reference genome .fasta file), we expect the order of "
                    "chromosomes as specified there. "
                    "Offending input going from " + to_string( current_locus ) + " to " +
                    to_string( GenomeLocus{ variant.chromosome, variant.position })
                );
            }
        }

        // Now also check the length, potentially.
        if( check_sequence_lengths && sequence_dict ) {
            auto const& entry = sequence_dict->get( variant.chromosome );
            if( variant.position > entry.length ) {
                throw std::runtime_error(
                    "The current position " +
                    to_string( GenomeLocus{ variant.chromosome, variant.position }) +
                    " of the input Variant is greater than the length of the chromosome "
                    "as specified by the SequenceDict, which is " + std::to_string( entry.length )
                );
            }
        }

        // Finally, update the current locus according to the variant.
        // We also check that the current variant is a valid, so that we do not run into
        // the initial condition of an empty chromosome again.
        current_locus = GenomeLocus( variant.chromosome, variant.position );
        if( current_locus.chromosome.empty() || current_locus.position == 0 ) {
            throw std::runtime_error(
                "Invalid empty chromosome or position 0 found in input Variant."
            );
        }
    };
}

/**
 * @brief Helper function to check that some Variant input has positions that agree with those
 * reported in a SequenceDict.
 *
 * Similar to make_variant_input_iterator_sequence_order_visitor(), but without the sequence order
 * check. Meant for situations where this order check is either not necessary, or already done in
 * some other way, for example in a VariantParallelInputIterator.
 *
 * See make_variant_input_iterator_sequence_order_visitor() for details on usage.
 */
inline std::function<void(Variant const&)> make_variant_input_iterator_sequence_length_visitor(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict
) {
    return [ sequence_dict ]( Variant const& variant ) {
        // Assumes the chromosome is in the dict. If not, the get function throws.
        auto const& entry = sequence_dict->get( variant.chromosome );
        if( variant.position > entry.length ) {
            throw std::runtime_error(
                "The current position " +
                to_string( GenomeLocus{ variant.chromosome, variant.position }) +
                " of the input Variant is greater than the length of the chromosome "
                "as specified by the SequenceDict, which is " + std::to_string( entry.length )
            );
        }
    };
}

} // namespace population
} // namespace genesis

#endif // include guard
