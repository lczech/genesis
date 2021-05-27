#ifndef GENESIS_POPULATION_FORMATS_VARIANT_PILEUP_READER_H_
#define GENESIS_POPULATION_FORMATS_VARIANT_PILEUP_READER_H_

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

#include "genesis/population/variant.hpp"
#include "genesis/sequence/functions/quality.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant (m)pileup Reader
// =================================================================================================

/**
 * @brief Reader for Variant information of (m)pileup files.
 *
 * This reader is a specialized version of the SimplePileupReader, which instead of yielding
 * per-line tallies bases and phred quality scores, directly yields their summed up counts
 * of bases per line.
 *
 * For each line in the file, a Variant is produced. One such sample in an (m)pileup file consists
 * of two or more columns in the file.
 * The number of columns per sample depends on the additional information contained in the file.
 * As we have no way of deciding this automatically, these columns have to be activated beforehand:
 *
 *   - Quality scores, set with_quality_string()
 *   - Ancestral alles, set with_ancestral_base()
 *
 * More columns might be needed in the future, and potentially their ordering might need to be
 * adapted. But for now, we only have these use cases.
 */
class VariantPileupReader
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    VariantPileupReader() = default;
    ~VariantPileupReader() = default;

    VariantPileupReader( VariantPileupReader const& ) = default;
    VariantPileupReader( VariantPileupReader&& )      = default;

    VariantPileupReader& operator= ( VariantPileupReader const& ) = default;
    VariantPileupReader& operator= ( VariantPileupReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read an (m)pileup file line by line.
     */
    std::vector<Variant> read( std::shared_ptr< utils::BaseInputSource > source ) const;

    /**
     * @brief Read an (m)pileup file line by line, but only the samples at the given indices.
     */
    std::vector<Variant> read(
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
    std::vector<Variant> read(
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
        Variant&            variant
    ) const;

    /**
     * @brief Read an (m)pileup line, but only the samples at which the @p sample_filter is `true`.
     *
     * This filter does not need to contain the same number of values as the line has samples.
     * If it is shorter, all samples after its last index will be ignored. If it is longer,
     * the remaining entries are not used as a filter.
     */
    bool parse_line(
        utils::InputStream&      input_stream,
        Variant&                 variant,
        std::vector<bool> const& sample_filter
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool strict_bases() const
    {
        return strict_bases_;
    }

    /**
     * @brief Set whether to strictly require bases to be in `ACGTN`.
     *
     * If set to `true`, we expect bases to be `ACGTN`, and throw otherwise.
     * If set to `false`, we will change any other base to be `N`.
     */
    VariantPileupReader& strict_bases( bool value )
    {
        strict_bases_ = value;
        return *this;
    }

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
    VariantPileupReader& with_quality_string( bool value )
    {
        with_quality_string_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set quality encoding for the phred-scaled quality scores.
     */
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
    VariantPileupReader& quality_encoding( sequence::QualityEncoding value )
    {
        quality_encoding_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set minimum phred quality score that a base needs to have
     * to be added to the Variant BaseCounts for a sample.
     */
    size_t min_phred_score() const
    {
        return min_phred_score_;
    }

    /**
     * @brief Set the minimum phred quality score that a base needs to have
     * to be added to the Variant BaseCounts for a sample.
     *
     * Bases below this quality score are ignored when summing up the counts per sample.
     */
    VariantPileupReader& min_phred_score( size_t value )
    {
        min_phred_score_ = value;
        return *this;
    }

    /**
     * @brief Get whether currently the reader is set to read ancestral bases for the samples.
     */
    bool with_ancestral_base() const
    {
        return with_ancestral_base_;
    }

    /**
     * @brief Set whether to expect the base of the ancestral allele as the last part of each
     * sample in a record line.
     *
     * This is a pileup extension used by Pool-HMM (Boitard et al 2013) to denote the ancestral
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
    VariantPileupReader& with_ancestral_base( bool value )
    {
        with_ancestral_base_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    bool parse_line_(
        utils::InputStream&      input_stream,
        Variant&                 variant,
        std::vector<bool> const& sample_filter,
        bool                     use_sample_filter
    ) const;

    void process_sample_(
        utils::InputStream& input_stream,
        Variant const&      variant,
        BaseCounts&         base_counts
    ) const;

    void skip_sample_(
        utils::InputStream& input_stream
    ) const;

    void next_field_(
        utils::InputStream& input_stream
    ) const;

    void tally_base_(
        utils::InputStream& input_stream,
        BaseCounts& base_count,
        char b
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // If set, we expect bases to be ACGTN. If not set, we will fix any that are not to N.
    bool strict_bases_ = false;

    // Set whether the file contains the base quality score column, and if so, how its encoded
    // (we default to Sanger with offset 33), and if we want to skip low quality bases.
    bool with_quality_string_ = true;
    sequence::QualityEncoding quality_encoding_ = sequence::QualityEncoding::kSanger;
    size_t min_phred_score_ = 0;

    // Set whether the last part of the sample line contains the base of the ancestral allele.
    bool with_ancestral_base_ = false;

    // Internal buffer to read bases into. Used for speedup to avoid reallocations.
    mutable std::string base_buffer_;
};

} // namespace population
} // namespace genesis

#endif // include guard
