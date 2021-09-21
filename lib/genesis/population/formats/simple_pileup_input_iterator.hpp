#ifndef GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_INPUT_ITERATOR_H_

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

#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/math/bitvector/helper.hpp"

#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     Simple Pileup Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as a (m)pileup file.
 *
 * This simple wrapper allows easy iteration through the records/lines of an (m)pileup file
 * line by line. See SimplePileupReader for details. The class is templated, so that it can
 * be either used to produce SimplePileupReader::Record or Variant objects per line of the input
 * mpileup file.
 *
 * Basic usage:
 *
 *     auto it = SimplePileupInputIterator<>( utils::from_file( infile ));
 *     while( it ) {
 *         // work with it.record() or it->...
 *         ++it;
 *     }
 *
 * or
 *
 *     for( auto it = SimplePileupInputIterator<Variant>( utils::from_file( infile )); it; ++it ) {
 *         // work with it
 *     }
 *
 * both allow to iterate each line in the file. The first instanciates an iterator over
 * SimplePileupReader::Record, while the second over Variant.
 *
 * Additionally, filtering of which samples (by index) to include can be provided, either as a vector
 * of indices to consider, or as a bool vector that is `true` at the indices to consider. The latter
 * filter does not need to contain the same number of values as the record has samples.
 * If it is shorter, all samples after its last index will be ignored. If it is longer,
 * the remaining entries are not used as a filter.
 */
template<class T = SimplePileupReader::Record>
class SimplePileupInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = SimplePileupInputIterator;
    using value_type        = T;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input.
     */
    SimplePileupInputIterator() = default;

    /**
     * @brief Create an instance that reads from an input source, and optionally take
     * a SimplePileupReader with settings to be used.
     */
    explicit SimplePileupInputIterator(
        std::shared_ptr< utils::BaseInputSource > source,
        SimplePileupReader const&                 reader = {}
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( reader )
    {
        // Read the first record of the file.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source, using only the samples at the
     * indices given in the @p sample_indices, and optionally take a SimplePileupReader with
     * settings to be used.
     */
    SimplePileupInputIterator(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<size_t> const&                sample_indices,
        SimplePileupReader const&                 reader = {}
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( reader )
        , use_sample_filter_( true )
    {
        // Prepare the sample filter from the indices.
        sample_filter_ = utils::make_bool_vector_from_indices( sample_indices );

        // Read the first record of the file.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source, using only the samples at the
     * indices where the @p sample_filter is true, and optionally take a SimplePileupReader with
     * settings to be used.
     */
    SimplePileupInputIterator(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<bool> const&                  sample_filter,
        SimplePileupReader const&                 reader = {}
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( reader )
        , sample_filter_( sample_filter )
        , use_sample_filter_( true )
    {
        // Read the first record of the file.
        increment();
    }

    ~SimplePileupInputIterator() = default;

    SimplePileupInputIterator( self_type const& ) = default;
    SimplePileupInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------------------

    /**
    * @brief Return true iff dereferencing is valid, i.e., iff there is a line/record available.
    */
    explicit operator bool() const
    {
        return good_;
    }

    bool good() const
    {
        return good_;
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    T const& record() const
    {
        return record_;
    }

    T& record()
    {
        return record_;
    }

    T const* operator ->() const
    {
        return &record_;
    }

    T* operator ->()
    {
        return &record_;
    }

    T const& operator*() const
    {
        return record_;
    }

    T& operator*()
    {
        return record_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    self_type& operator ++ ()
    {
        increment();
        return *this;
    }

    self_type& operator ++(int)
    {
        increment();
        return *this;
    }

    void increment()
    {
        increment_();
    }

    bool operator==( self_type const& it ) const
    {
        return good_ == it.good_;
    }

    bool operator!=( self_type const& it ) const
    {
        return !(*this == it);
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void increment_();

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Basic iterator setup and input.
    bool good_ = false;
    std::shared_ptr<utils::InputStream> input_stream_;

    // Reading into records
    T record_;
    SimplePileupReader reader_;

    // Sample filtering
    std::vector<bool> sample_filter_;
    bool              use_sample_filter_ = false;
};

// -------------------------------------------------------------------------
//     Explicit Specialiyations in Namespace Scope
// -------------------------------------------------------------------------

template<>
inline void SimplePileupInputIterator<SimplePileupReader::Record>::increment_()
{
    // Read into temp object, so that we have the previous one still available.
    SimplePileupReader::Record tmp;
    if( use_sample_filter_ ) {
        good_ = reader_.parse_line_record( *input_stream_, tmp, sample_filter_ );
    } else {
        good_ = reader_.parse_line_record( *input_stream_, tmp );
    }

    // Make sure that the input is sorted.
    if( good_ &&
        (
            ( tmp.chromosome  < record_.chromosome ) ||
            ( tmp.chromosome == record_.chromosome && tmp.position <= record_.position )
        )
    ) {
        throw std::runtime_error(
            "Malformed pileup " + input_stream_->source_name() + " at " + input_stream_->at() +
            ": unordered chromosomes and positions"
        );
    }
    record_ = std::move( tmp );
}

template<>
inline void SimplePileupInputIterator<Variant>::increment_()
{
    // Read into temp object, so that we have the previous one still available.
    Variant tmp;
    if( use_sample_filter_ ) {
        good_ = reader_.parse_line_variant( *input_stream_, tmp, sample_filter_ );
    } else {
        good_ = reader_.parse_line_variant( *input_stream_, tmp );
    }

    // Make sure that the input is sorted.
    if( good_ &&
        (
            ( tmp.chromosome  < record_.chromosome ) ||
            ( tmp.chromosome == record_.chromosome && tmp.position <= record_.position )
        )
    ) {
        throw std::runtime_error(
            "Malformed pileup " + input_stream_->source_name() + " at " + input_stream_->at() +
            ": unordered chromosomes and positions"
        );
    }
    record_ = std::move( tmp );
}

} // namespace population
} // namespace genesis

#endif // include guard
