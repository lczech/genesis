#ifndef GENESIS_POPULATION_STREAMS_VARIANT_INPUT_STREAM_H_
#define GENESIS_POPULATION_STREAMS_VARIANT_INPUT_STREAM_H_

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

#include "genesis/population/formats/frequency_table_input_stream.hpp"
#include "genesis/population/formats/sam_variant_input_stream.hpp"
#include "genesis/population/formats/simple_pileup_input_stream.hpp"
#include "genesis/population/formats/sync_input_stream.hpp"
#include "genesis/population/formats/sync_reader.hpp"
#include "genesis/population/formats/vcf_input_stream.hpp"
#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

// Cannot include the headers, as it itself needs the VariantInputStream to be defined first.
// We could split this whole file into two, one for the VariantInputStream definition,
// and one for the functions to create it from sources... Maybe do that in the future.
// As of now, this forward declaration means that the VariantParallelInputStream header
// needs to be included by the user at some point themselves... which they probably do anyway.
class VariantGaplessInputStream;
class VariantParallelInputStream;

// =================================================================================================
//     Generic Variant Stream
// =================================================================================================

/**
 * @brief Data storage for input-specific information when traversing a variant file.
 *
 * This is used by VariantInputStream, see there for details.
 *
 * The utils::GenericInputStream allows us to store some extra data. When traversing a file as a
 * list of Variant%s, one per genomic position, we can use this extra field to store information
 * such as the file name and the individual sample names.
 *
 * In the future, we might even want to store pointers to the underlying streams and readers
 * (useful for VCF for example), so that users can work with them when iterating. For now however,
 * we just store some basic information.
 */
struct VariantInputStreamData
{
    /**
     * @brief Full file path, when reading from a file.
     */
    std::string file_path;

    /**
     * @brief User-readable name of the input source.
     *
     * This can for example be the file base name, without path and extensions.
     */
    std::string source_name;

    /**
     * @brief Sample names, for example as found in the file header.
     *
     * Not all input file formats contain sample names. In that case, this field might be left empty,
     * or filled with names based on the source name and the number of samples contained in the file.
     * That means that downstream processing needs to check this if sample names are going to be
     * used (e.g., for output). The `make_variant_input_stream_from_...()` functions for instance
     * fill this list with names based on the source name, and a numbered list of samples, such as
     * `file-1`.
     */
    std::vector<std::string> sample_names;
};

/**
 * @brief Iterate Variant%s, using a variety of input file formats.
 *
 * This generic stream is an abstraction that is agnostic to the underlying file format,
 * and can be used with anything that can be converted to a Variant per genome position.
 * It offers to iterate a whole input file, and transform and filter the Variant as needed
 * in order to make downstream processing as easy as possible.
 *
 * This is useful for downstream processing, where we just want to work with the Variant%s along
 * the genome, but want to allow different file formats for their input. Use this stream
 * to achieve this. For example, use the `make_variant_input_stream_...()` functions to get such
 * an interator for different input file types.
 *
 * The stream furthermore offers a data field of type VariantInputStreamData, which gets
 * filled with basic data about the input file and sample names (if available in the file format).
 * Use the @link utils::GenericInputStream::data() data()@endlink function to access this data
 * while iterating.
 *
 * @see @link genesis::utils::GenericInputStream GenericInputStream@endlink for usage and details.
 */
using VariantInputStream = utils::GenericInputStream<Variant, VariantInputStreamData>;

// =================================================================================================
//     Input Sources
// =================================================================================================

// -------------------------------------------------------------------------
//     vector
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputStream to iterate the contents of `std::vector` containing Variant%s.
 *
 * This is a simple wrapper to bring a vector of in-memory Variant%s into the input stream
 * format that we use for file streaming as well. Meant as a speed-up for small files that fit into
 * memory, in cases where they for example have to processed multiple times.
 *
 * The user needs to make sure that the lifetime of the given input @p variants vector is longer
 * than the stream returned here, and that the vector is not modified after calling this function.
 */
VariantInputStream make_variant_input_stream_from_vector(
    std::vector<Variant> const& variants
);

// -------------------------------------------------------------------------
//     SAM/BAM/CRAM
// -------------------------------------------------------------------------

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Create a VariantInputStream to iterate the contents of a SAM/BAM/CRAM file as Variant%s.
 *
 * An instance of SamVariantInputStream can be provided from which the settings are copied.
 *
 * Depending on the settings used in the @p reader, this can either produce a single sample
 * (one BaseCounts object in the resulting Variant at each position in the genome),
 * or split the input file by the read group (RG) tag (potentially also allowing for an
 * "unaccounted" group of reads).
 *
 * The other `make_variant_input_stream_...` functions offer settings to sub-set (filter) the
 * samples based on their names or indices. This can be achieved here as well, but has instead to
 * be done directly in the @p reader, instead of providing the fitler arguments to this function.
 */
VariantInputStream make_variant_input_stream_from_sam_file(
    std::string const& filename,
    SamVariantInputStream const& reader = SamVariantInputStream{}
);

#endif // GENESIS_HTSLIB

// -------------------------------------------------------------------------
//     Pileup
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputStream to iterate the contents of a (m)pileup file as Variant%s.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    SimplePileupReader const& reader = SimplePileupReader{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a (m)pileup file as Variant%s.
 *
 * This uses only the samples at the zero-based indices given in the @p sample_indices list.
 * If @p inverse_sample_indices is `true`, this list is inversed, that is, all sample indices _but_
 * the ones listed are included in the output.
 *
 * For example, given a list `{ 0, 2 }` and a file with 4 samples, only the first and the third
 * sample will be in the output. When however @p inverse_sample_indices is also set, then the output
 * will contain the second and fourth sample.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices = false,
    SimplePileupReader const& reader = SimplePileupReader{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a (m)pileup file as Variant%s.
 *
 * This uses only the samples at the indices where the @p sample_filter is `true`.
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    std::vector<bool> const& sample_filter,
    SimplePileupReader const& reader = SimplePileupReader{}
);

// -------------------------------------------------------------------------
//     Sync
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputStream to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 */
VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 *
 * This uses only the samples at the zero-based indices given in the @p sample_indices list.
 * If @p inverse_sample_indices is `true`, this list is inversed, that is, all sample indices _but_
 * the ones listed are included in the output.
 *
 * For example, given a list `{ 0, 2 }` and a file with 4 samples, only the first and the third
 * sample will be in the output. When however @p inverse_sample_indices is also set, then the output
 * will contain the second and fourth sample.
 */
VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices = false
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 *
 * This uses only the samples at the indices where the @p sample_filter is `true`.
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename,
    std::vector<bool> const& sample_filter
);

// -------------------------------------------------------------------------
//     Frequency Table
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputStream to iterate the contents of a frequency table file as
 * Variant%s.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_frequency_table_file(
    std::string const& filename,
    char separator_char = '\t',
    FrequencyTableInputStream const& reader = FrequencyTableInputStream{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a frequency table file as
 * Variant%s.
 *
 * Additionally, this version of the function takes a list of @p sample_names which are used as
 * filter so that only those samples (columns of the frequency table) are evaluated and accessible -
 * or, if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_frequency_table_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names_filter,
    bool inverse_sample_names_filter = false,
    char separator_char = '\t',
    FrequencyTableInputStream const& reader = FrequencyTableInputStream{}
);

// -------------------------------------------------------------------------
//     VCF
// -------------------------------------------------------------------------

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Create a VariantInputStream to iterate the contents of a VCF file as Variant%s,
 * treating each sample as a pool of individuals.
 *
 * See convert_to_variant_as_pool( VcfRecord const& ) for details on the conversion from
 * VcfRecord to Variant.
 *
 * This function requires the VCF to have the "AD" FORMAT field. It only iterates over those VCF
 * record lines that actually have the "AD" FORMAT provided, as this is the information that we use
 * to convert the samples to Variant%s. All records without that field are skipped.
 * Only SNP records are processed; that is, all non-SNPs (indels and others) are ignord.
 *
 * If @p only_biallelic is set to `true`, this is further restricted to only contain
 * biallelic SNPs, that is, only positions with exactly one alternative allele.
 *
 * If @p only_filter_pass is set to `true` (default), only those positions are considered that
 * have the FILTER field set to "PASS". That is, all variants that did not pass a filter in
 * the VCF processing are skipped.
 *
 * @see See make_variant_input_stream_from_individual_vcf_file() for the function that instead
 * interprets the VCF as usual as a set of individuals.
 */
VariantInputStream make_variant_input_stream_from_pool_vcf_file(
    std::string const& filename,
    bool only_biallelic   = false,
    bool only_filter_pass = true
);

/**
 * @copydoc make_variant_input_stream_from_pool_vcf_file( std::string const&, bool, bool )
 *
 * Additionally, this version of the function takes a list of @p sample_names which are used as
 * filter so that only those samples (columns of the VCF records) are evaluated and accessible - or,
 * if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
 */
VariantInputStream make_variant_input_stream_from_pool_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names = false,
    bool only_biallelic       = false,
    bool only_filter_pass     = true
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a VCF file as Variant%s,
 * treating each sample as an individual, and combining them all into one BaseCounts sample.
 *
 * See convert_to_variant_as_individuals( VcfRecord const&, bool ) for details on the conversion
 * from VcfRecord to Variant. We only consider biallelic SNP positions here.
 *
 * If @p only_filter_pass is set to `true` (default), only those positions are considered that
 * have the FILTER field set to "PASS". That is, all variants that did not pass a filter in
 * the VCF processing are skipped.
 *
 * @see See make_variant_input_stream_from_pool_vcf_file() for the function that instead
 * interprets each sample (column) as a pool of individuals, e.g., from pool sequencing.
 */
VariantInputStream make_variant_input_stream_from_individual_vcf_file(
    std::string const& filename,
    bool use_allelic_depth = false,
    bool only_biallelic    = false,
    bool only_filter_pass  = true
);

/**
 * @copydoc make_variant_input_stream_from_individual_vcf_file( std::string const&, bool, bool, bool )
 *
 * Additionally, this version of the function takes a list of @p sample_names which are used as
 * filter so that only those samples (columns of the VCF records) are evaluated and accessible - or,
 * if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
 */
VariantInputStream make_variant_input_stream_from_individual_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names = false,
    bool use_allelic_depth    = false,
    bool only_biallelic       = false,
    bool only_filter_pass     = true
);

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Input Processing Streams
// =================================================================================================

// -------------------------------------------------------------------------
//     Variant Parallel Input Stream
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputStream to iterate multiple input sources at once,
 * using a VariantParallelInputStream.
 *
 * This wraps multiple input sources into one stream that traverses all of them in parallel,
 * and is here then yet again turned into a Variant per position, using
 * VariantParallelInputStream::Iterator::joined_variant() to combine all input sources into one.
 * See there for the meaning of the two `bool` parameters of this function.
 *
 * As this is iterating multiple files, we leave the VariantInputStreamData::file_path and
 * VariantInputStreamData::source_name empty, and fill the VariantInputStreamData::sample_names
 * with the sample names of the underlying input sources of the parallel stream, checking
 * for duplicates to avoid downstream trouble.
 */
VariantInputStream make_variant_input_stream_from_variant_parallel_input_stream(
    VariantParallelInputStream const& parallel_input,
    bool allow_ref_base_mismatches = false,
    bool allow_alt_base_mismatches = true
);

// -------------------------------------------------------------------------
//     Variant Gapless Input Stream
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantGaplessInputStream from a VariantInputStream @p input, and wrap it
 * agian in a VariantInputStream.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input
);

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets the reference genome for the gapless iteration.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::ReferenceGenome> ref_genome
);

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets the sequence dictionary for the gapless iteration.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::SequenceDict> seq_dict
);

/**
 * @brief Create a VariantInputStream that wraps a VariantGaplessInputStream.
 *
 * See also make_variant_gapless_input_stream()
 */
VariantInputStream make_variant_input_stream_from_variant_gapless_input_stream(
    VariantGaplessInputStream const& gapless_input
);

// -------------------------------------------------------------------------
//     Merging Input Stream
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputStream that merges samples from its underlying @p input.
 *
 * This provides an on-the-fly merging of input samples by simply summing out their BaseCounts.
 * It takes a mapping of sample names to group names, and creates a VariantInputStream with
 * the group names as new sample names, which then merge the input of their respective samples.
 *
 * If @p allow_ungrouped_samples is set to `true`, any sample that does not occur in the map
 * will be added as-is, with its original sample name, and as its own "group". By default,
 * we throw an exception in this case, in order to make sure that the behavior is intended.
 */
VariantInputStream make_variant_merging_input_stream(
    VariantInputStream const& input,
    std::unordered_map<std::string, std::string> const& sample_name_to_group,
    bool allow_ungrouped_samples = false
);

} // namespace population
} // namespace genesis

#endif // include guard
