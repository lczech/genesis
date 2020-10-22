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

#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/utils/io/input_source.hpp"

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
 * line by line. See SimplePileupReader for details.
 *
 * Basic usage:
 *
 *     auto it = SimplePileupInputIterator( utils::from_file( infile ));
 *     while( it ) {
 *         // work with it.record() or it->...
 *         ++it;
 *     }
 *
 * or
 *
 *     for( auto it = SimplePileupInputIterator( utils::from_file( infile )); it; ++it ) {
 *         // work with it
 *     }
 *
 * both allow to iterate each line in the file.
 */
class SimplePileupInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = SimplePileupInputIterator;
    using value_type        = SimplePileupReader::Record;
    using pointer           = value_type&;
    using reference         = value_type*;
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
     * @brief Create an instance that reads from an input source.
     */
    explicit SimplePileupInputIterator(
        std::shared_ptr< utils::BaseInputSource > source
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
    {
        // Read the first record of the file.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source, and takes a SimplePileupReader
     * with settings to be used.
     */
    SimplePileupInputIterator(
        std::shared_ptr< utils::BaseInputSource > source,
        SimplePileupReader const& reader
    )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( reader )
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

    SimplePileupReader::Record const& record() const
    {
        return record_;
    }

    SimplePileupReader::Record& record()
    {
        return record_;
    }

    SimplePileupReader::Record const* operator ->() const
    {
        return &record_;
    }

    SimplePileupReader::Record* operator ->()
    {
        return &record_;
    }

    SimplePileupReader::Record const& operator*() const
    {
        return record_;
    }

    SimplePileupReader::Record& operator*()
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
        good_ = reader_.parse_line( *input_stream_, record_ );
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

    bool good_ = false;
    std::shared_ptr<utils::InputStream> input_stream_;

    SimplePileupReader::Record record_;
    SimplePileupReader reader_;
};

} // namespace population
} // namespace genesis

#endif // include guard
