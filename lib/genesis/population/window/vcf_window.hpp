#ifndef GENESIS_POPULATION_WINDOW_VCF_WINDOW_H_
#define GENESIS_POPULATION_WINDOW_VCF_WINDOW_H_

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

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include <cassert>
#include <functional>
#include <string>
#include <vector>

#include "genesis/population/formats/vcf_input_iterator.hpp"
#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/window.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     VCF Window Generator
// =================================================================================================

/**
 * @brief Convenience function to iterate over a whole VCF file.
 *
 * This function is convenience, and takes care of iterating a VCF file record by record
 * (that is, line by line), using a provided @p conversion function to extract the `D`/`Data`
 * from the VcfRecord. It furthermore takes care of finishing all chromosomes properly,
 * using their lengths as provided in the VCF header.
 *
 * Before calling the function, of course, all necessary plugin functions have to be set in the
 * SlidingWindowGenerator instance, so that the data is processed as intended. In particular, take
 * care of setting SlidingWindowGenerator::emit_incomplete_windows() to the desired value.
 *
 * Furthermore, the function offers a @p condition function that can be used to skip records
 * that do not fullfil a given condition. That is, if @p condition is used, it needs to return
 * `true` for records that shall be processed, and `false` for those that shall be skipped.
 */
template<class Data, class Accumulator = EmptyAccumulator>
void run_vcf_window(
    SlidingWindowGenerator<Data, Accumulator>& generator,
    std::string const& vcf_file,
    std::function<Data( VcfRecord const& )> conversion,
    std::function<bool( VcfRecord const& )> condition = {}
) {
    size_t current_chr_len = 0;
    for( auto record = VcfInputIterator( vcf_file ); record; ++record ) {

        // Check if we want to process this record at all.
        if( condition && ! condition( *record )) {
            continue;
        }

        // Get the chromosome that this record belongs to.
        auto const rec_chromosome = record->get_chromosome();

        // We need to take care of changes of the chromosome, that is, either at the very
        // beginning of the VCF file, or at actual changes within the file.
        // When finishing a chromosome, we want to use its proper length from the VCF header.
        if( rec_chromosome != generator.chromosome() ) {

            // We store the length, and also make sure that we get the same value via two
            // different methods. Safe is safe.
            current_chr_len = record->header().get_chromosome_length( rec_chromosome );
            assert(
                record->header().get_chromosome_values( rec_chromosome ).count("length") == 0 ||
                std::stoul(
                    record->header().get_chromosome_values( rec_chromosome ).at("length")
                ) == current_chr_len
            );

            // If there has been data enqueued before, this is not the first data point of the
            // chromosome. In that case, we need to finish the previous chromosome first,
            // and do so using its length to get the full interval covered and closed.
            if( ! generator.empty() ) {

                // We might not have usable chromosome length information in the header.
                // In that case, simply finish the current chromosome where it is.
                // (As of now, finish_chromosome uses 0 as default value anyway, so we could just
                // call it directly without the extra condition here. But this way seems a bit
                // more future proof, if we need to change this and use overloads of
                // finish_chromosome instead.)
                if( current_chr_len == 0 ) {
                    generator.finish_chromosome();
                } else {
                    generator.finish_chromosome( current_chr_len );
                }
            }

        } else {
            // If we did not change chromosomes, we expect their length not to change.
            assert( record->header().get_chromosome_length( rec_chromosome ) == current_chr_len );
        }

        // Finally, enqueue the new data.
        generator.enqueue( rec_chromosome, record->get_position(), conversion( *record ));
    }

    // Now that we are done with the whole file, we also need to finish and close the
    // last remaining chromosome interval properly. Same as above.
    if( current_chr_len == 0 ) {
        generator.finish_chromosome();
    } else {
        generator.finish_chromosome( current_chr_len );
    }
}

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
