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

#include "genesis/population/stream/variant_input_stream_sources.hpp"

#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/bitvector/helper.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Local Helpers
// =================================================================================================

/**
 * @brief Local helper function template that takes care of intilizing an input stream,
 * and setting the sample filters, for those streams for which we do not know the number
 * of samples prior to starting the file iteration.
 *
 * The template arguments are: `T` the returned type of input stream, and `R` the underlying
 * reader type. This is very specific for the use case here, and currently is only meant for how
 * we work with the SimplePileupReader and the SyncReader and their streams. Both their
 * streams accept a reader to take settings from.
 */
template<class T, class R>
std::shared_ptr<T> make_input_stream_with_sample_filter_(
    std::string const& filename,
    R const& reader,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices,
    std::vector<bool> const& sample_filter
) {
    // Prepare a reader. We switch depending on the type of filter.
    // Not both can be given by the way that this function is called; assert that.
    assert( sample_indices.empty() || sample_filter.empty() );

    std::shared_ptr<T> input;
    if( ! sample_indices.empty() ) {

        // When we have indices given, we need to open the file once to get the number of samples
        // in the file, then create our correctly sized bool vector, and then open the file again
        // to start iterating with the filter. Cumbersome, but an unfortunate detail of the
        // current implementation. Might need fixing later.
        input = std::make_shared<T>( utils::from_file( filename ), reader );
        auto const smp_cnt = (*input)->samples.size();

        // Make the filter. We check the condition that the make function checks here as well,
        // as the error message is super not helpful for users otherwise. See there for details.
        auto max_it = std::max_element( sample_indices.begin(), sample_indices.end() );
        if( *max_it + 1 > smp_cnt ) {
            throw std::invalid_argument(
                "In " + filename + ": "
                "Cannot create sample filter for the input file, as the filter index list contains "
                "entries for " + std::to_string( *max_it + 1 ) + " samples, "
                "while the input file only contains " + std::to_string( smp_cnt ) + " samples."
            );
        }

        // Now make a bool filter, inverse as needed, and restart the file with it.
        auto smp_flt = utils::make_bool_vector_from_indices( sample_indices, smp_cnt );
        if( inverse_sample_indices ) {
            smp_flt.flip();
        }
        input = std::make_shared<T>(
            utils::from_file( filename ),
            smp_flt,
            reader
        );

    } else if( ! sample_filter.empty() ) {
        input = std::make_shared<T>(
            utils::from_file( filename ),
            sample_filter,
            reader
        );
    } else {
        input = std::make_shared<T>(
            utils::from_file( filename ),
            reader
        );
    }
    assert( input );
    return input;
}

/**
 * @brief Local helper to fill the sample names of file formats without sample names.
 *
 * We want to use a standardized format for that: the file base name, followed by consecutive
 * numbers for each sample, separated by a character.
 */
std::vector<std::string> make_sample_name_list_( std::string const& source_name, size_t size )
{
    std::vector<std::string> result;
    result.reserve( size );
    for( size_t i = 0; i < size; ++i ) {
        result.push_back( source_name + "." + std::to_string( i + 1 ));
    }
    return result;
}

// =================================================================================================
//     vector
// =================================================================================================

VariantInputStream make_variant_input_stream_from_vector(
    std::vector<Variant> const& variants
) {

    // Prepare the iterator data.
    VariantInputStreamData data;
    data.source_name = "std::vector";

    // No sample names in a vector... so we just use numbered entries.
    if( ! variants.empty() ) {
        data.sample_names = make_sample_name_list_( data.source_name, variants[0].samples.size() );
    }

    // Get iterators to the data.
    auto cur = variants.begin();
    auto end = variants.end();

    // The iterators are copied over to the lambda,
    // and those copies are kept alive when returning from this function.
    return VariantInputStream(
        [ cur, end ]( Variant& variant ) mutable {
            if( cur != end ) {
                // We make copies of the data here, as we do not want to modify the vector.
                variant = *cur;
                ++cur;
                return true;
            } else {
                return false;
            }
        },
        std::move( data )
    );
}

// =================================================================================================
//     SAM/BAM/CRAM
// =================================================================================================

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

VariantInputStream make_variant_input_stream_from_sam_file(
    std::string const& filename,
    SamVariantInputStream const& reader
) {
    // Make an iterator over sam/bam/cram, using the given reader to take over its settings.
    // We wrap this in a shared pointer so that this very instance can stay alive
    // when being copied over to the lambda that we return from this function.
    auto input = std::make_shared<SamVariantInputStream>( reader );
    input->input_file( filename );

    // Get the iterators. We store them by copy in the lambda, and these copies are kept alive
    // when returning from this function. Unfortunately, at the moment, we need to open the file
    // here already and start the iteration, as we need access to some information from the file
    // content itself, namely, the rg tags. The way that the sam reading is currently implemented,
    // we have to do this. Might fix in the future, to avoid starting the iteration here already.
    assert( input );
    auto cur = input->begin();
    auto end = input->end();

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputStreamData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".sam", ".sam.gz", ".bam", ".cram" });

    // Get the sample names from the read group tags. If they are empty, because the reader
    // was not set up to split by read group tags, we instead use an empty name, to indicate that
    // there is one unnamed sample.
    data.sample_names = cur.rg_tags();
    if( data.sample_names.empty() ) {
        // We could have an input file where we want to split by RG, but no RG are set in the
        // header. When not using unaccounted RG, we would end up with no samples.
        // Take this into account, and create as many empty (unnamed) samples as needed.
        // This cannot be more than one though, as it can be the unaccounted or none,
        // or, if we do not split by RG at all, just the one sample were every read ends up in.
        // data.sample_names = make_sample_name_list_( data.source_name, cur.sample_size() );
        // assert( data.sample_names.size() <= 1 );

        // Scratch that. If we treat the file as a single sample anyway, we just use the file name
        // as the sample name. Way more intuitive. Unfortunately, there is then the inconsistency
        // in naming, but it's more in line with what e.g. the sync does if a header is provided.
        assert( cur.sample_size() <= 1 );
        if( cur.sample_size() == 1 ) {
            data.sample_names = std::vector<std::string>{ data.source_name };
        }
    } else {
        assert( reader.split_by_rg() == true );
    }

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputStream(
        [ input, cur, end ]( Variant& variant ) mutable {
            if( cur != end ) {
                variant = std::move( *cur );
                ++cur;
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
 * @brief Local helper function that takes care of the three functions below.
 */
VariantInputStream make_variant_input_stream_from_pileup_file_(
    std::string const& filename,
    SimplePileupReader const& reader,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices,
    std::vector<bool> const& sample_filter
) {
    // Get the input, taking care of the filters.
    auto input = make_input_stream_with_sample_filter_<
        SimplePileupInputStream<Variant>, SimplePileupReader
    >(
        filename, reader, sample_indices, inverse_sample_indices, sample_filter
    );

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputStreamData data;
    data.file_path = filename;
    data.source_name = utils::file_basename(
        filename, { ".gz", ".plp", ".mplp", ".pileup", ".mpileup" }
    );

    // No sample names in pileup, use numbers instead.
    data.sample_names = make_sample_name_list_( data.source_name, (*input)->samples.size() );

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputStream(
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

VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    SimplePileupReader const& reader
) {
    return make_variant_input_stream_from_pileup_file_(
        filename, reader, std::vector<size_t>{}, false, std::vector<bool>{}
    );
}

VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices,
    SimplePileupReader const& reader
) {
    return make_variant_input_stream_from_pileup_file_(
        filename, reader, sample_indices, inverse_sample_indices, std::vector<bool>{}
    );
}

VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    std::vector<bool> const& sample_filter,
    SimplePileupReader const& reader
) {
    return make_variant_input_stream_from_pileup_file_(
        filename, reader, std::vector<size_t>{}, false, sample_filter
    );
}

// =================================================================================================
//     Sync
// =================================================================================================

VariantInputStream make_variant_input_stream_from_sync_file_(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices,
    std::vector<bool> const& sample_filter
) {
    // Get the input, taking care of the filters. We use a default reader here,
    // as sync currently does not have any settings that a reader would neeed to take care of.
    auto input = make_input_stream_with_sample_filter_<
        SyncInputStream, SyncReader
    >(
        filename, SyncReader(), sample_indices, inverse_sample_indices, sample_filter
    );
    // auto input = std::make_shared<SyncInputStream>( utils::from_file( filename ));

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputStreamData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".sync" });

    if( input->get_sample_names().size() > 0 ) {
        // If we have sample names, using our ad-hoc extension, use these.
        data.sample_names = input->get_sample_names();
    } else {
        // No sample names given, so we use numbers instead.
        data.sample_names = make_sample_name_list_( data.source_name, (*input)->samples.size() );
    }

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputStream(
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

VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename
) {
    return make_variant_input_stream_from_sync_file_(
        filename, std::vector<size_t>{}, false, std::vector<bool>{}
    );
}

VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices
) {
    return make_variant_input_stream_from_sync_file_(
        filename, sample_indices, inverse_sample_indices, std::vector<bool>{}
    );
}

VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename,
    std::vector<bool> const& sample_filter
) {
    return make_variant_input_stream_from_sync_file_(
        filename, std::vector<size_t>{}, false, sample_filter
    );
}

// =================================================================================================
//     Frequency Table
// =================================================================================================

VariantInputStream make_variant_input_stream_from_frequency_table_file(
    std::string const& filename,
    char separator_char,
    FrequencyTableInputStream const& reader
) {
    return make_variant_input_stream_from_frequency_table_file(
        filename, std::vector<std::string>{}, false, separator_char, reader
    );
}

VariantInputStream make_variant_input_stream_from_frequency_table_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names_filter,
    bool inverse_sample_names_filter,
    char separator_char,
    FrequencyTableInputStream const& reader
) {
    // Make an iterator over, using the given reader to take over its settings.
    // We wrap this in a shared pointer so that this very instance can stay alive
    // when being copied over to the lambda that we return from this function.
    auto input = std::make_shared<FrequencyTableInputStream>( reader );
    input->input_source( utils::from_file( filename ));
    input->sample_names_filter(
        std::unordered_set<std::string>( sample_names_filter.begin(), sample_names_filter.end() )
    );
    input->inverse_sample_names_filter( inverse_sample_names_filter );
    input->separator_char( separator_char );

    // Get the iterators. We store them by copy in the lambda, and these copies are kept alive
    // when returning from this function. Similar to the sam function above, we here already need
    // to start the iteration, to have access to data from the file. Might fix in the future.
    assert( input );
    auto cur = input->begin();
    auto end = input->end();

    // Get the data, using the file base name without path and potential extensions as source.
    VariantInputStreamData data;
    data.file_path = filename;
    data.source_name = utils::file_basename(
        filename, { ".csv", ".csv.gz", ".tsv", ".tsv.gz", ".txt" }
    );

    // Get the sample names from the iterator.
    data.sample_names = cur.sample_names();

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputStream(
        [ input, cur, end ]( Variant& variant ) mutable {
            if( cur != end ) {
                // The deref operator of the iterator is const, so this falls back to copy ctor.
                // We keep the move here though, in case we change this behaviour later,
                // and allow to move from the iterator.
                variant = std::move( *cur );
                ++cur;
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
VariantInputStream make_variant_input_stream_from_vcf_file_(
    std::string const& filename,
    VariantInputStreamFromVcfParams const& params,
    bool pool_samples,
    bool use_allelic_depth
) {
    // We do not expect order by default here. Just to keep it simple. If needed, activate again.
    const bool expect_ordered = false;

    // Make an iterator over vcf, and check that the necessary format field AD is present
    // and of the correct form. We wrap this in a shared pointer so that this very instance
    // can stay alive when being copied over to the lambda that we return from this function.
    auto input = std::make_shared<VcfInputStream>(
        filename, params.sample_names, params.inverse_sample_names, expect_ordered
    );
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
    VariantInputStreamData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".vcf", ".bcf" });
    data.sample_names = input->header().get_sample_names();

    // The input is copied over to the lambda, and that copy is kept alive
    // when returning from this function.
    return VariantInputStream(
        [ input, pool_samples, use_allelic_depth, params ]
        ( Variant& variant ) mutable {
            auto& vcf_it = *input;

            // Only use the lines that have the "AD" field (if needed), and fit the other criteria.
            // If any test fails, skip this position. In particular, we need to skip everything
            // that is not a single change (contains ref or alt that is not a single nucleotide)
            // when working with Pool data.
            for( ; vcf_it; ++vcf_it ) {
                if( use_allelic_depth && ! vcf_it->has_format( "AD" ) ) {
                    continue;
                }
                if( pool_samples && ! vcf_it->is_snp_or_alt_del() ) {
                    continue;
                }
                if( params.only_snps && ! vcf_it->is_snp() ) {
                    continue;
                }
                if( params.only_biallelic_snps && vcf_it->get_alternatives_count() != 1 ) {
                    continue;
                }
                if( params.only_filter_pass && ! vcf_it->pass_filter() ) {
                    continue;
                }
                break;
            }

            // Now we are either at a record that fits our needs, or at the end of the input.
            if( vcf_it ) {
                assert( ! use_allelic_depth || vcf_it->has_format( "AD" ) );
                assert( ! params.only_snps || vcf_it->is_snp() );
                assert( ! params.only_biallelic_snps || vcf_it->get_alternatives_count() == 1 );
                assert( ! params.only_filter_pass || vcf_it->pass_filter() );

                // Depending on what type of conversion we want to do (which in turn depends on
                // the wrapper function that this local function was called from), we switch
                // between pools and individuals here.
                if( pool_samples ) {
                    variant = convert_to_variant_as_pool( *vcf_it );
                } else {
                    variant = convert_to_variant_as_individuals( *vcf_it, use_allelic_depth );
                }

                // Set the filter tag, if needed. We need to use reset() here instead of set(),
                // as the conversion functions might already set the filter status, but we want
                // the filter setting here to have precedence over, e.g., missing data.
                if( ! vcf_it->pass_filter() ) {
                    variant.status.reset( VariantFilterTag::kNotPassed );
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

VariantInputStream make_variant_input_stream_from_pool_vcf_file(
    std::string const& filename,
    VariantInputStreamFromVcfParams const& params
) {
    return make_variant_input_stream_from_vcf_file_(
        filename, params, true, true
    );
}

VariantInputStream make_variant_input_stream_from_individual_vcf_file(
    std::string const& filename,
    VariantInputStreamFromVcfParams const& params,
    bool use_allelic_depth
) {
    return make_variant_input_stream_from_vcf_file_(
        filename, params, false, use_allelic_depth
    );
}

#endif // GENESIS_HTSLIB

} // namespace population
} // namespace genesis
