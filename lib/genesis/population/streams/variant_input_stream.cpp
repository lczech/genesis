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

#include "genesis/population/streams/variant_input_stream.hpp"

#include "genesis/population/streams/variant_gapless_input_stream.hpp"
#include "genesis/population/streams/variant_parallel_input_stream.hpp"
#include "genesis/population/functions/functions.hpp"
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
        data.sample_names = make_sample_name_list_( data.source_name, cur.sample_size() );
        assert( data.sample_names.size() <= 1 );
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
    // We do not expect order by default here. Just to keep it simple. If needed, activate again.
    const bool expect_ordered = false;

    // Make an iterator over vcf, and check that the necessary format field AD is present
    // and of the correct form. We wrap this in a shared pointer so that this very instance
    // can stay alive when being copied over to the lambda that we return from this function.
    auto input = std::make_shared<VcfInputStream>(
        filename, sample_names, inverse_sample_names, expect_ordered
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

VariantInputStream make_variant_input_stream_from_pool_vcf_file(
    std::string const& filename,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_stream_from_pool_vcf_file(
        filename, std::vector<std::string>{}, false,
        only_biallelic, only_filter_pass
    );
}

VariantInputStream make_variant_input_stream_from_pool_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_stream_from_vcf_file_(
        filename, sample_names, inverse_sample_names,
        true, true, only_biallelic, only_filter_pass
    );
}

VariantInputStream make_variant_input_stream_from_individual_vcf_file(
    std::string const& filename,
    bool use_allelic_depth,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_stream_from_individual_vcf_file(
        filename, std::vector<std::string>{}, false,
        use_allelic_depth, only_biallelic, only_filter_pass
    );
}

VariantInputStream make_variant_input_stream_from_individual_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names,
    bool use_allelic_depth,
    bool only_biallelic,
    bool only_filter_pass
) {
    return make_variant_input_stream_from_vcf_file_(
        filename, sample_names, inverse_sample_names,
        false, use_allelic_depth, only_biallelic, only_filter_pass
    );
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Variant Parallel Input Stream
// =================================================================================================

VariantInputStream make_variant_input_stream_from_variant_parallel_input_stream(
    VariantParallelInputStream const& parallel_input,
    bool allow_ref_base_mismatches,
    bool allow_alt_base_mismatches
) {
    // As before, make a shared pointer (with a copy of the input) that stays alive.
    auto input = std::make_shared<VariantParallelInputStream>( parallel_input );

    // Get the iterators.
    VariantParallelInputStream::Iterator cur;
    VariantParallelInputStream::Iterator end;
    bool has_started = false;

    // We do not have a single file here, so make a list of all sample names from the inputs.
    // Leave file_path and source_name at their empty defaults.
    VariantInputStreamData data;
    std::unordered_set<std::string> uniq_names;
    for( auto const& source : input->inputs() ) {
        // auto const& source_name = source.data().source_name;
        // for( auto const& sample_name : source.data().sample_names ) {
        //     data.sample_names.push_back( source_name + source_sample_separator + sample_name );
        // }

        for( auto const& sample_name : source.data().sample_names ) {
            if( uniq_names.count( sample_name ) > 0 ) {
                throw std::runtime_error(
                    "Cannot iterate input sources in parallel, as sample name \"" + sample_name +
                    "\" occurs multiple times in the inputs."
                );
            }
            uniq_names.insert( sample_name );
            data.sample_names.push_back( sample_name );
        }
    }
    assert( uniq_names.size() == data.sample_names.size() );

    // The input is copied over to the lambda, and that copy is kept alive.
    return VariantInputStream(
        [ input, cur, end, has_started, allow_ref_base_mismatches, allow_alt_base_mismatches ]
        ( Variant& variant ) mutable {
            if( ! has_started ) {
                assert( input );
                cur = input->begin();
                end = input->end();
                has_started = true;
            }
            if( cur != end ) {
                variant = cur.joined_variant(
                    allow_ref_base_mismatches, allow_alt_base_mismatches, true
                );
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
//     Variant Gapless Input Stream
// =================================================================================================

VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input
) {
    auto gapless_input = VariantGaplessInputStream( input );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::ReferenceGenome> ref_genome
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.reference_genome( ref_genome );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::SequenceDict> seq_dict
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.sequence_dict( seq_dict );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

VariantInputStream make_variant_input_stream_from_variant_gapless_input_stream(
    VariantGaplessInputStream const& gapless_input
) {
    // As before, make a shared pointer (with a copy of the input) that stays alive.
    auto input = std::make_shared<VariantGaplessInputStream>( gapless_input );

    // Get the iterators.
    VariantGaplessInputStream::Iterator cur;
    VariantGaplessInputStream::Iterator end;
    bool has_started = false;

    // The input is copied over to the lambda, and that copy is kept alive.
    // The VariantInputStreamData is simply copied over.
    return VariantInputStream(
        [ input, cur, end, has_started ]
        ( Variant& variant ) mutable {
            if( ! has_started ) {
                assert( input );
                cur = input->begin();
                end = input->end();
                has_started = true;
            }
            if( cur != end ) {
                variant = std::move( *cur );
                ++cur;
                return true;
            } else {
                return false;
            }
        },
        input->input().data()
    );
}

// =================================================================================================
//     Merging Input Stream
// =================================================================================================

/**
 * @brief Internal helper to keep information needed for make_variant_merging_input_stream()
 */
struct VariantMergeGroupAssignment
{
    /**
     * @brief Assignment of samples (indicies in the vector) to their group (values).
     */
    std::vector<size_t> group_assignments;

    /**
     * @brief Names of the groups.
     *
     * The entries in the group_assignment refer to this list of groups.
     */
    std::vector<std::string> group_names;
};

/**
 * @brief Helper function to create a mapping from sample indices to group indices.
 */
VariantMergeGroupAssignment make_variant_merging_input_stream_group_assignment_(
    VariantInputStream const& variant_input,
    std::unordered_map<std::string, std::string> const& sample_name_to_group,
    bool allow_ungrouped_samples
) {
    // Shorthands and checks
    auto const& sample_names = variant_input.data().sample_names;
    if( sample_names.size() == 0 ) {
        throw std::runtime_error( "Cannot merge sample groups if no sample names are provided" );
    }

    // Make a vector assinging sample indices to group indices.
    // We also keep track of the group names (and their indices) and samples names we have processed.
    VariantMergeGroupAssignment grouping;
    grouping.group_assignments = std::vector<size_t>( sample_names.size() );
    std::unordered_map<std::string, size_t> group_to_index;
    std::unordered_set<std::string> uniq_sample_names;

    // Do the assignment
    for( size_t i = 0; i < sample_names.size(); ++i ) {
        auto const& sample_name = sample_names[i];
        if( sample_name.empty() ) {
            throw std::runtime_error( "Cannot merge sample groups with empty sample names." );
        }

        // Check uniqueness of names. We also use that set later, so it's okay to have these
        // extra copies or the names around.
        if( uniq_sample_names.count( sample_name ) > 0 ) {
            throw std::runtime_error(
                "Cannot merge sample groups with duplicate sample names. Sample name \"" +
                sample_name + "\" occurs multiple times in the input."
            );
        }
        uniq_sample_names.insert( sample_name );

        // Get the group name for the sample. If it's in the given list, we use that.
        // If not, we either fail, or use the sample name as the group name.
        // In the latter case, there is some scenario where some provided sample names is also
        // a group name - that could be a bit chaotic and have weird consequences, but should
        // work, and so we just leave that as a user error and do not check for this here.
        std::string group_name;
        if( sample_name_to_group.count( sample_name ) > 0 ) {
            group_name = sample_name_to_group.at( sample_name );
            if( group_name.empty() ) {
                throw std::runtime_error(
                    "Cannot merge sample groups, as sample name \"" + sample_name + "\" has an " +
                    "empty group name assigned in the provided mapping of sample names to group names."
                );
            }
        } else if( allow_ungrouped_samples ) {
            group_name = sample_name;
        } else {
            throw std::runtime_error(
                "Cannot merge sample groups, as sample name \"" + sample_name +
                "\" does not occur in the provided mapping of sample names to group names."
            );
        }
        assert( !group_name.empty() );

        // Now we have a group name. Check if we already have an index for this group,
        // or whether this is a new group, so that we need to give it the next index.
        // Then assign this as the group index for the sample.
        if( group_to_index.count( group_name ) == 0 ) {
            auto const next_idx = group_to_index.size();
            group_to_index[ group_name ] = next_idx;
            grouping.group_names.push_back( group_name );
        }
        auto const group_idx = group_to_index.at( group_name );
        assert( group_idx < group_to_index.size() );
        assert( group_idx < grouping.group_names.size() );
        grouping.group_assignments[i] = group_idx;
    }
    assert( grouping.group_names.size() > 0 );
    assert( grouping.group_names.size() == group_to_index.size() );
    assert( uniq_sample_names.size() == sample_names.size() );

    // Finally, we warn about any names that have an assignment to a group, but did not appear.
    // Might indicate that something is wrong, but might also just mean that the user has some
    // larger mapping that does not always contain all names. Either way, better to tell.
    std::unordered_set<std::string> samples_names_to_warn;
    for( auto const& ng : sample_name_to_group ) {
        if( uniq_sample_names.count( ng.first ) == 0 ) {
            samples_names_to_warn.insert( ng.first );
        }
    }
    if( !samples_names_to_warn.empty() ) {
        LOG_WARN << "In the provided list of samples to merge into groups, there were "
                 << std::to_string( samples_names_to_warn.size() )
                 << " sample names that did not occur in the input sample names:\n"
                 << "  - " << genesis::utils::join( samples_names_to_warn, "  - " )
        ;
    }

    return grouping;
}

VariantInputStream make_variant_merging_input_stream(
    VariantInputStream const& variant_input,
    std::unordered_map<std::string, std::string> const& sample_name_to_group,
    bool allow_ungrouped_samples
) {

    // Make a mapping from sample indices to group indices. That is, each entry in the returned
    // vector corresponds to a sample (by its index) of the input, and the value at each entry
    // is the group number we assign to this.
    auto const grouping = make_variant_merging_input_stream_group_assignment_(
        variant_input, sample_name_to_group, allow_ungrouped_samples
    );

    // As before, make a shared pointer (with a copy of the input) that stays alive.
    auto input = std::make_shared<VariantInputStream>( variant_input );

    // Get iterators to the data.
    VariantInputStream::Iterator cur;
    VariantInputStream::Iterator end;
    bool has_started = false;

    // We copy the original variant data, but replace the sample names by our group names.
    auto data = variant_input.data();
    data.sample_names = grouping.group_names;

    // Now we can create our iterator. As before, the iterators are copied over to the lambda,
    // and those copies are kept alive when returning from this function.
    return VariantInputStream(
        [ input, cur, end, has_started, grouping ]( Variant& variant ) mutable {
            if( ! has_started ) {
                assert( input );
                cur = input->begin();
                end = input->end();
                has_started = true;
            }

            // Nothing to do if we are at the end. Indicate that to the caller.
            if( cur == end ) {
                return false;
            }

            // For efficiency, we do not want to make a full copy of the input variant,
            // as that would entail an unncessary copy of the full samples vector.
            // However, we do want a copy of all other members of Variant, and listing them here
            // is a bit tedious. So instead we trick around a bit: We move out the samples,
            // then copy the rest, and then move the samples back. From the point of view of
            // the underlying input, that should appear as if nothing happened.
            auto& cur_var = *cur;
            auto const sample_count = cur_var.samples.size();
            auto tmp_samples = std::move( cur_var.samples );
            variant = cur_var;
            cur_var.samples = std::move( tmp_samples );
            assert( cur_var.samples.size() == sample_count );

            // Consistency check the number of samples in the input.
            if( sample_count != grouping.group_assignments.size() ) {
                throw std::runtime_error(
                    "Based on sample names and groups, " +
                    std::to_string( grouping.group_assignments.size() ) +
                    " samples are expected to be found in the input, but " +
                    std::to_string( sample_count ) + " samples were found at " +
                    cur_var.chromosome + ":" + std::to_string( cur_var.position )
                );
            }

            // Now we create a new samples vector (no way around that), with BaseCount instances
            // that are all initialized to 0 at all counts, and merge the samples into it,
            // as given by our group assignment.
            variant.samples = std::vector<BaseCounts>( grouping.group_names.size() );
            for( size_t i = 0; i < sample_count; ++i ) {
                auto const group_idx = grouping.group_assignments[i];

                // Validity check. Pretty sure that this could just be an assertion, as this
                // is all internal and not dependent on user input, but seems important enough
                // to just always check.
                if( group_idx >= variant.samples.size() ) {
                    throw std::runtime_error(
                        "Invalid group index " + std::to_string( group_idx ) +
                        " in Variant with " + std::to_string( variant.samples.size() ) +
                        " samples."
                    );
                }
                merge_inplace( variant.samples[group_idx], cur_var.samples[i] );
            }

            // We are done, move to the next position in the input,
            // and signal to the caller that we found data.
            ++cur;
            return true;
        },
        std::move( data )
    );
}

} // namespace population
} // namespace genesis
