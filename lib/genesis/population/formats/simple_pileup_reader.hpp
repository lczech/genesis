#ifndef GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_INPUT_ITERATOR_H_

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

#include "genesis/utils/io/input_source.hpp"
#include "genesis/sequence/functions/quality.hpp"

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class InputStream;
}

namespace population {

// =================================================================================================
//     Simple (m)pileup Reader
// =================================================================================================

/**
 * @brief Reader for line-by-line assessment of pileup files.
 *
 * This simple reader processes pileup files line by line. That is, it does not take into
 * consideration which read starts at which position, but instead gives a quick and simple
 * tally of the bases of all reads that cover a given position.
 * This makes it fast in cases where no per-read information is needed.
 */
class SimplePileupReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    struct Sample
    {
        /**
         * @brief Total count of reads covering this position.
         *
         * This is the number given in the first column of each sample.
         * In a well-formed pileup file, this also corresponds to the number of actual bases
         * that are listed for the sample.
         */
        size_t read_count = 0;

        /**
         * @brief All bases (expect for indels) of the reads that cover the given position.
         */
        std::string read_bases;

        /**
         * @brief Phread-scaled scores of the bases as given in `read_bases`.
         */
        std::vector<unsigned char> phred_scores;

        size_t A = 0;
        size_t C = 0;
        size_t G = 0;
        size_t T = 0;
        size_t N = 0;
        size_t del = 0;

        /**
         * @brief Count of the nucleotide bases at this position, that is,
         * the sum of all `A`, `C`, `G`, and `T`.
         */
        size_t nucleotide_count = 0;
    };

    struct Record
    {
        std::string         chromosome;
        size_t              position;
        char                reference_base;
        std::vector<Sample> samples;
    };

    using self_type         = SimplePileupReader;
    using value_type        = Record;
    using pointer           = value_type&;
    using reference         = value_type*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SimplePileupReader() = default;
    ~SimplePileupReader() = default;

    SimplePileupReader( self_type const& ) = delete;
    SimplePileupReader( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = delete;
    self_type& operator= ( self_type&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    std::vector<Record> read( std::shared_ptr< utils::BaseInputSource > source ) const;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    bool parse_line(
        utils::InputStream& input_stream,
        Record&             record
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool with_quality_string() const
    {
        return with_quality_string_;
    }

    self_type& with_quality_string( bool value )
    {
        with_quality_string_ = value;
        return *this;
    }

    sequence::QualityEncoding quality_encoding() const
    {
        return quality_encoding_;
    }

    self_type& quality_encoding( sequence::QualityEncoding value )
    {
        quality_encoding_ = value;
        return *this;
    }

    unsigned char min_phred_score() const
    {
        return min_phred_score_;
    }

    self_type& min_phred_score( unsigned char value )
    {
        min_phred_score_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    bool parse_line_(
        utils::InputStream& input_stream,
        Record&             record
    ) const;

    void parse_sample_(
        utils::InputStream& input_stream,
        Record&             record,
        size_t              index
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
    unsigned char min_phred_score_ = 0;

};

} // namespace population
} // namespace genesis

#endif // include guard
