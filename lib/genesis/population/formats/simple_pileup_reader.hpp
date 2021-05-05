#ifndef GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_READER_H_
#define GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include "genesis/sequence/functions/quality.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Simple (m)pileup Reader
// =================================================================================================

/**
 * @brief Reader for line-by-line assessment of (m)pileup files.
 *
 * This simple reader processes (m)pileup files line by line. That is, it does not take into
 * consideration which read starts at which position, but instead gives a quick and simple
 * tally of the bases of all reads that cover a given position.
 * This makes it fast in cases where only per-position, but no per-read information is needed.
 *
 * For each processed line, a SimplePileupReader::Record is produced, which captures the basic
 * information of the line, as well as a tally for each sample in the line, collected in
 * SimplePileupReader::Sample. One such sample consists of two or more columns in the file.
 * The number of columns per sample depends on the additional information contained in the file.
 * As we have no way of deciding this automatically, these columns have to be activated beforehand:
 *
 *   - Quality scores, set with_quality_string()
 *   - Ancestral alles, set with_ancestral_base()
 *
 * More columns might be needed in the future, and potentially their ordering might need to be
 * adapted. But for now, we only have these use cases.
 */
class SimplePileupReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief One sample in a pileup line/record.
     *
     * Each sample in a pileup file corresponds to the reads of one sample that cover a certain
     * position on a chromosome, and consists of two or three entries/columns in the file:
     *
     *  1. A read count.
     *  2. A list of bases (and some other information on read start and end etc) from the reads
     *     that cover the given position on the chromosome.
     *  3. (Optionally) A list of phread-scaled ASCII-encoded quality scores for the list of bases.
     *
     * As this here is a simple reader, we ignore the information on read starts/ends, as well as
     * potential insertions and deletions (indels), and instead simply tally up the number of
     * actual bases of the reads that cover a position. This Sample struct here collects this
     * information.
     */
    struct Sample
    {
        /**
         * @brief Total count of reads covering this position.
         *
         * This is the number given in the first column of each sample.
         * In a well-formed pileup file, this also corresponds to the number of actual bases
         * that are listed for the sample, that is `read_bases.size()`.
         *
         * In our simple reader here, this value is almost identical to the sum of all other
         * counters here (that is, `a_count + c_count + g_count + t_count + n_count + d_count`),
         * with the exception of RNA symbols (`<>`) in the string, that we ignore.
         * If those are needed as well, @ref read_bases can be scanned again to count them.
         */
        size_t read_coverage = 0;

        /**
         * @brief All bases (expect for indels) of the reads that cover the given position.
         *
         * These are the data of the second column of the sample, but without the read start/end
         * and indel data. Furthermore, the pileup notation for using the reference base
         * (`.` and `,`) is replaced by the actual reference base here.
         */
        std::string read_bases;

        /**
         * @brief Phread-scaled scores of the bases as given in `read_bases`.
         *
         * This is the data from the third column of the sample. It is only parsed and filled in
         * if with_quality_string() is set to `true` (default), in which case this data is expected
         * to be present in the file.
         */
        std::vector<unsigned char> phred_scores;

        /**
         * @brief Base of the ancestral allele.
         *
         * Only read if with_ancestral_base() is set to `true`. See there for details.
         */
        char ancestral_base = '\0';
    };

    /**
     * @brief Single line/record from a pileup file.
     *
     * A line consists of a chromosome and position, the reference base at that position,
     * as well as one or more Sample%s, that is, information on reads (and optionally, their quality
     * scores) that cover that position. See Sample for details on the latter.
     */
    struct Record
    {
        std::string         chromosome;
        size_t              position;
        char                reference_base;
        std::vector<Sample> samples;
    };

    using self_type         = SimplePileupReader;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SimplePileupReader() = default;
    ~SimplePileupReader() = default;

    SimplePileupReader( self_type const& ) = default;
    SimplePileupReader( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read an (m)pileup file line by line.
     */
    std::vector<Record> read( std::shared_ptr< utils::BaseInputSource > source ) const;

    /**
     * @brief Read an (m)pileup file line by line, but only the samples at the given indices.
     */
    std::vector<Record> read(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<size_t> const&                sample_indices
    ) const;

    /**
     * @brief Read an (m)pileup file line by line, but only the samples at which the
     * @p sample_filter is `true`.
     *
     * This filter does not need to contain the same number of values as the record has samples.
     * If it is shorter, all samples after its last index will be ignored. If it is longer,
     * the remaining entries are not used as a filter.
     */
    std::vector<Record> read(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<bool> const&                  sample_filter
    ) const;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    /**
     * @brief Read an (m)pileup line.
     */
    bool parse_line(
        utils::InputStream& input_stream,
        Record&             record
    ) const;

    /**
     * @brief Read an (m)pileup line, but only the samples at which the @p sample_filter is `true`.
     *
     * This filter does not need to contain the same number of values as the record has samples.
     * If it is shorter, all samples after its last index will be ignored. If it is longer,
     * the remaining entries are not used as a filter.
     */
    bool parse_line(
        utils::InputStream&      input_stream,
        Record&                  record,
        std::vector<bool> const& sample_filter
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool with_quality_string() const
    {
        return with_quality_string_;
    }

    /**
     * @brief Set whether to expect a phred-scaled, ASCII-encoded quality code string per
     * sample.
     *
     * A typical line from a pileup file looks like
     *
     *     seq1 272 T 24  ,.$.....,,.,.,...,,,.,..^+. <<<+;<<<<<<<<<<<=<;<;7<&
     *
     * with the last field being quality codes. However, this last field is optional, and hence
     * we offer this option. If `true` (default), the field is expected to be there; if `false`,
     * it is expected not to be there. That is, at the moment, we have no automatic setting for this.
     *
     * See quality_encoding() for changing the encoding that is used in this column. Default
     * is Sanger encoding. See ::genesis::sequence::QualityEncoding for details.
     */
    self_type& with_quality_string( bool value )
    {
        with_quality_string_ = value;
        return *this;
    }

    sequence::QualityEncoding quality_encoding() const
    {
        return quality_encoding_;
    }

    /**
     * @brief Set the type of encoding for the quality code string.
     *
     * If with_quality_string() is set to `true` (default), this encoding is used to transform
     * the ASCII-encoded string into actual phred-scaled scores. See
     * sequence::quality_decode_to_phred_score() for details.
     */
    self_type& quality_encoding( sequence::QualityEncoding value )
    {
        quality_encoding_ = value;
        return *this;
    }

    bool with_ancestral_base() const
    {
        return with_ancestral_base_;
    }

    /**
     * @brief Set whether to expect the base of the ancestral allele as the last part of each
     * sample in a record line.
     *
     * This is a pipeup extension used by Pool-HMM (Boitard et al 2013) to denote the ancestral
     * allele of each position directly within the pipleup file. Set to true when this is present
     * in the input.
     *
     * A typical line from a pileup file looks like
     *
     *     2L	30	A	15	aaaAaaaAaAAaaAa	PY\aVO^`ZaaV[_S	A
     *
     * which contains the three fixed columns, and then four columns for the sample, with the
     * last one `A` being the ancestral allele for that sample.
     */
    self_type& with_ancestral_base( bool value )
    {
        with_ancestral_base_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Helper function to create a sample filter from a list of sample indices.
     */
    static std::vector<bool> make_sample_filter( std::vector<size_t> const& indices );

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    bool parse_line_(
        utils::InputStream&      input_stream,
        Record&                  record,
        std::vector<bool> const& sample_filter,
        bool                     use_sample_filter
    ) const;

    void process_sample_(
        utils::InputStream& input_stream,
        Record&             record,
        Sample&             sample
    ) const;

    void skip_sample_(
        utils::InputStream& input_stream
    ) const;

    void next_field_(
        utils::InputStream& input_stream
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Set whether the file contains the base quality score column, and if so, how its encoded
    // (we default to Sanger with offset 33), and if we want to skip low quality bases.
    bool with_quality_string_ = true;
    sequence::QualityEncoding quality_encoding_ = sequence::QualityEncoding::kSanger;

    // Set whether the last part of the sample line contains the base of the ancestral allele.
    bool with_ancestral_base_ = false;
};

} // namespace population
} // namespace genesis

#endif // include guard
