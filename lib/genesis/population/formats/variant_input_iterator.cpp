/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/functions/variant.hpp"
#include "genesis/utils/core/fs.hpp"

#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     SAM/BAM/CRAM
// =================================================================================================

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

VariantInputIterator make_variant_input_iterator_from_sam_file(
    std::string const& filename,
    SamVariantInputIterator const& reader
) {
    // Make an iterator over sam/bam/cram, using the given reader to take over its settings.
    // We wrap this in a shared pointer so that this very instance can stay alive
    // when being copied over to the lambda that we return from this function.
    auto input = std::make_shared<SamVariantInputIterator>( reader );
    input->input_file( filename );

    // Get the iterators. We store them by copy in the lambda, and these copies are kept alive
    // when returning from this function.
    assert( input );
    auto begin = input->begin();
    auto end   = input->end();

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".sam", ".sam.gz", ".bam", ".cram" });

    // Get the sample names from the read group tags. If they are empty, because the reader
    // was not set up to split by read group tags, we instead use the source name as sample name,
    // as in that case, the file name itself is the sample name.
    data.sample_names = begin.rg_tags();
    if( data.sample_names.empty() ) {
        data.sample_names.push_back( data.source_name );
    }

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputIterator(
        [ input, begin, end ]( Variant& variant ) mutable {
            if( begin != end ) {
                variant = std::move( *begin );
                ++begin;
                return true;
            } else {
                return false;
            }
        },
        std::move( data )
    );
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Pileup
// =================================================================================================

/**
 * @brief Create a VariantInputIterator to iterate the contents of a (m)pileup file as Variant%s.
 */
VariantInputIterator make_variant_input_iterator_from_pileup_file(
    std::string const& filename,
    SimplePileupReader const& reader
) {
    // Prepare a reader. This instance is only alive here, but it's copy lives on.
    auto input = std::make_shared<SimplePileupInputIterator<Variant>>(
        utils::from_file( filename ),
        reader
    );

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename(
        filename, { ".gz", ".plp", ".mplp" ".pileup", ".mpileup" }
    );
    // No sample names in pileup... nothing to do for this here.

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputIterator(
        [ input ]( Variant& variant ) mutable -> bool {
            auto& it = *input;
            if( it ) {
                variant = std::move( *it );
                ++it;
                return true;
            } else {
                return false;
            }
        },
        std::move( data )
    );
}

// =================================================================================================
//     Sync
// =================================================================================================

/**
 * @brief Create a VariantInputIterator to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 */
VariantInputIterator make_variant_input_iterator_from_sync_file(
    std::string const& filename
) {
    auto input = std::make_shared<SyncInputIterator>( utils::from_file( filename ));

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".sync" });
    // No sample names in sync... nothing to do for this here.

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputIterator(
        [ input ]( Variant& variant ) mutable {
            auto& sync_it = *input;
            if( sync_it ) {
                variant = std::move( *sync_it );
                ++sync_it;
                return true;
            } else {
                return false;
            }
        },
        std::move( data )
    );
}

// =================================================================================================
//     VCF
// =================================================================================================

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Local helper function that takes care of both main functions below.
 */
VariantInputIterator make_variant_input_iterator_from_vcf_file_(
    // File input
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names,

    // Settings
    bool pool_samples,
    bool use_allelic_depth,
    bool only_biallelic,
    bool only_filter_pass
) {
    // Make an iterator over vcf, and check that the necessary format field AD is present
    // and of the correct form. We wrap this in a shared pointer so that this very instance
    // can stay alive when being copied over to the lambda that we return from this function.
    auto input = std::make_shared<VcfInputIterator>( filename, sample_names, inverse_sample_names );
    if(
        use_allelic_depth &&
        ! input->header().has_format( "AD", VcfValueType::kInteger, VcfValueSpecial::kReference )
    ) {
        throw std::runtime_error(
            "Cannot iterate over VCF file " + filename + " using the \"AD\" FORMAT " +
            "field to count allelic depths, as that field is not part of the VCF file."
        );
    }

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".vcf", ".bcf" });
    data.sample_names = input->header().get_sample_names();

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputIterator(
        [ input, pool_samples, use_allelic_depth, only_biallelic, only_filter_pass ]
        ( Variant& variant ) mutable {
            auto& vcf_it = *input;

            // Only use the lines that have the "AD" field, and are biallelic.
            // Also test for the extra conditions. If any test fails, skip this position.
            // For simplicity, we code that as a sequence of conditions; the compiler will optimize.
            for( ; vcf_it; ++vcf_it ) {
                if( ! vcf_it->has_format( "AD" ) || ! vcf_it->is_snp() ) {
                    continue;
                }
                if( only_biallelic && vcf_it->get_alternatives_count() != 1 ) {
                    continue;
                }
                if( only_filter_pass && ! vcf_it->pass_filter() ) {
                    continue;
                }
                break;
            }

            // Now we are either at a record that fits our needs, or at the end of the input.
            if( vcf_it ) {
                assert( vcf_it->has_format( "AD" ) );
                assert( vcf_it->is_snp() );

                // Depending on what type of conversion we want to do (which in turn depends on
                // the wrapper function that this local function was called from), we switch
                // between pools and individuals here.
                if( pool_samples ) {
                    variant = convert_to_variant_as_pool( *vcf_it );
                } else {
                    variant = convert_to_variant_as_individuals( *vcf_it, use_allelic_depth );
                }

                // Move on to the next input, so that it is ready when this lambda is called again.
                ++vcf_it;
                return true;
            } else {
                // If we reached the end of the input, return false to signal this.
                return false;
            }
        },
        std::move( data )
    );
}

VariantInputIterator make_variant_input_iterator_from_pool_vcf_file(
    std::string const& filename,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_iterator_from_pool_vcf_file(
        filename, std::vector<std::string>{}, false,
        only_biallelic, only_filter_pass
    );
}

VariantInputIterator make_variant_input_iterator_from_pool_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_iterator_from_vcf_file_(
        filename, sample_names, inverse_sample_names,
        true, true, only_biallelic, only_filter_pass
    );
}

VariantInputIterator make_variant_input_iterator_from_individual_vcf_file(
    std::string const& filename,
    bool use_allelic_depth,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_iterator_from_individual_vcf_file(
        filename, std::vector<std::string>{}, false,
        use_allelic_depth, only_biallelic, only_filter_pass
    );
}

VariantInputIterator make_variant_input_iterator_from_individual_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names,
    bool use_allelic_depth,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_iterator_from_vcf_file_(
        filename, sample_names, inverse_sample_names,
        false, use_allelic_depth, only_biallelic, only_filter_pass
    );
}

#endif // GENESIS_HTSLIB

} // namespace population
} // namespace genesis
