#ifndef GENESIS_UTILS_FORMATS_CSV_INPUT_ITERATOR_H_
#define GENESIS_UTILS_FORMATS_CSV_INPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Csv Input Iterator
// =================================================================================================

class CsvInputIterator
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    using self_type         = CsvInputIterator;
    using value_type        = CsvReader::Line;
    using pointer           = value_type&;
    using reference         = value_type*;
    using iterator_category = std::input_iterator_tag;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input.
     */
    CsvInputIterator()
        : input_stream_( nullptr )
        , reader_()
        , line_()
    {}

    /**
     * @brief Create an instance that reads from an input source, using a default CsvReader.
     */
    explicit CsvInputIterator( std::shared_ptr<utils::BaseInputSource> source )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_()
        , line_()
    {
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source,
     * using the settings of a given CsvReader.
     */
    CsvInputIterator( std::shared_ptr<utils::BaseInputSource> source, CsvReader const& settings )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( settings )
        , line_()
    {
        increment();
    }

    ~CsvInputIterator() = default;

    CsvInputIterator( CsvInputIterator const& ) = default;
    CsvInputIterator( CsvInputIterator&& )      = default;

    CsvInputIterator& operator= ( CsvInputIterator const& ) = default;
    CsvInputIterator& operator= ( CsvInputIterator&& )      = default;

    // -------------------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------------------

    bool operator == ( self_type const& other ) const
    {
        return input_stream_ == other.input_stream_;
    }

    bool operator != ( self_type const& other ) const
    {
        return !( *this == other );
    }

    /**
    * @brief Return true iff dereferencing is valid, i.e., iff there is a Csv Line available.
    */
    explicit operator bool() const
    {
        return good_;
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    value_type const& operator * () const
    {
        return line_;
    }

    value_type const* operator -> () const
    {
        return &line_;
    }

    value_type const& dereference() const
    {
        return line_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    /**
     * @brief Beginning of the iterator.
     *
     * This is a bit uncommon, as the iterator provides its own begin() and end() functions.
     * We do this to allow the easy use case of range-based for loops.
     */
    self_type& begin()
    {
        return *this;
    }

    /**
     * @brief End of the iterator.
     *
     * @copydoc begin()
     */
    self_type end()
    {
        return self_type();
    }

    self_type& operator ++ ()
    {
        increment();
        return *this;
    }

    void increment()
    {
        // Check whether the input stream is good (not end-of-stream) and can be read from.
        // If not, we reached its end, so we stop reading in the next iteration.
        if( ! input_stream_ || ! *input_stream_ ) {
            good_ = false;
            return;
        }

        line_ = reader_.parse_line( *input_stream_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    private:

    std::shared_ptr<utils::InputStream> input_stream_;

    bool            good_ = true;
    CsvReader       reader_;
    CsvReader::Line line_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
