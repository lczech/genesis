#ifndef GENESIS_POPULATION_FORMAT_SYNC_INPUT_STREAM_H_
#define GENESIS_POPULATION_FORMAT_SYNC_INPUT_STREAM_H_

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

#include "genesis/population/format/sync_reader.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Sync Input Stream
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as a sync file.
 *
 * This simple wrapper allows easy iteration through the records/lines of an sync file
 * as defined by PoPoolation2, line by line. See SyncReader for details.
 *
 * Basic usage:
 *
 *     auto it = SyncInputStream( utils::from_file( infile ));
 *     while( it ) {
 *         // work with it->...
 *         ++it;
 *     }
 *
 * or
 *
 *     for( auto it = SyncInputStream( utils::from_file( infile )); it; ++it ) {
 *         // work with it->...
 *     }
 *
 * both allow to iterate each line in the file.
 *
 * Additionally, filtering of which samples (by index) to include can be provided, as a bool vector
 * that is `true` at the indices to consider. The filter does not need to contain the same number
 * of values as the Variant/line has samples. If it is shorter, all samples after its last index
 * will be ignored. If it is longer, the remaining entries are not used as a filter.
 */
class SyncInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = SyncInputStream;
    using value_type        = Variant;
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
    SyncInputStream() = default;

    /**
     * @brief Create an instance that reads from an input source, and optionally take
     * a SyncReader with settings to be used.
     */
    explicit SyncInputStream(
        std::shared_ptr< utils::BaseInputSource > source,
        SyncReader const&                         reader = {}
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( reader )
    {
        // Read the header line, if present. If not, this does nothing.
        // Then, we use the number of sample names that we found to resize the variant that
        // we read into. If there was no header, this will not do anything, so that the sync reader
        // will resize by using the size of the first line instead.
        // We init the size here with the header, which could also be 0, which would be okay.
        sample_names_ = reader_.read_header( *input_stream_ );
        sample_size_ = sample_names_.size();

        // Read the first record of the file.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source, using only the samples at the
     * indices where the @p sample_filter is true, and optionally take a SyncReader with
     * settings to be used.
     */
    SyncInputStream(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<bool> const&                  sample_filter,
        SyncReader const&                         reader = {}
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( reader )
        , sample_filter_( sample_filter )
        , use_sample_filter_( true )
    {
        // Same as above.
        sample_names_ = reader_.read_header( *input_stream_, sample_filter_ );
        sample_size_ = sample_names_.size();
        increment();
    }

    ~SyncInputStream() = default;

    SyncInputStream( self_type const& ) = default;
    SyncInputStream( self_type&& )      = default;

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

    std::vector<std::string> const& get_sample_names() const
    {
        return sample_names_;
    }

    Variant const& variant() const
    {
        return variant_;
    }

    Variant& variant()
    {
        return variant_;
    }

    Variant const* operator ->() const
    {
        return &variant_;
    }

    Variant* operator ->()
    {
        return &variant_;
    }

    Variant const& operator*() const
    {
        return variant_;
    }

    Variant& operator*()
    {
        return variant_;
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
        // We don't do any order checks here (for example, on the order of the input),
        // and leave that to downstream checkers that might want to add this on top.
        // NB: We used to have a check here, by reading into a temp instance first,
        // and checking if the chromosome and position order was working.
        // But this did not work as intended when this iterator/reader here was used with
        // make_variant_input_stream_from_sync_file(), as in that case, the chromosome name
        // is moved out of the Variant, so that we would always test against an empty moved-from
        // chromosome name, which would just never fail, and so completely miss its purpose.
        // Now, we don't do that check any more, but if we bring back that step from old commits,
        // be aware that it did not work as intended.

        // We set the size here, so that the reader checks the correct sample size every time.
        // We need to reset, as the Variant might be moved-from when we get here,
        // as for example the make_variant_input_stream_from_sync_file() iterator does that.
        // We then also set that size again after we are done - as a means of initializing
        // it in the first iteration, in case that there was no header that already did that.
        variant_.samples.resize( sample_size_ );
        if( use_sample_filter_ ) {
            good_ = reader_.parse_line( *input_stream_, variant_, sample_filter_ );
        } else {
            good_ = reader_.parse_line( *input_stream_, variant_ );
        }
        sample_size_ = variant_.samples.size();
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
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Basic iterator setup and input.
    bool good_ = false;
    std::shared_ptr<utils::InputStream> input_stream_;

    // Reading into variants
    SyncReader reader_;
    std::vector<std::string> sample_names_;
    size_t sample_size_ = 0;
    Variant variant_;

    // Sample filtering
    std::vector<bool> sample_filter_;
    bool              use_sample_filter_ = false;
};

} // namespace population
} // namespace genesis

#endif // include guard
