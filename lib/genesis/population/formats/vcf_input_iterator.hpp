#ifndef GENESIS_POPULATION_FORMATS_VCF_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_VCF_INPUT_ITERATOR_H_

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

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_header.hpp"
#include "genesis/population/formats/vcf_record.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     VCF/BCF Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as a VCF/BCF file.
 *
 * This simple wrapper allows easy iteration through the records/lines of a VCF/BCF file, and
 * takes care of setting up the HtsFile, VcfHeader, and VcfRecord.
 *
 * Basic usage:
 *
 *     auto it = VcfInputIterator( infile );
 *     while( it ) {
 *         // work with it.record() or it->...
 *         ++it;
 *     }
 *
 * or
 *
 *     for( auto it = VcfInputIterator( infile );  it; ++it ) {
 *         // work with it
 *     }
 *
 * For details on working with the records/lines, see VcfRecord and VcfFormatIterator.
 */
class VcfInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = VcfInputIterator;
    using value_type        = VcfRecord;
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
    VcfInputIterator() = default;

    /**
     * @brief Create an instance that reads from an input file name.
     */
    explicit VcfInputIterator(
        std::string const& filename
    )
        : filename_( filename )
        , file_( filename )
        , header_( file_ )
        , record_( header_ )
    {
        // Read the first record of the file.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input file name.
     */
    VcfInputIterator(
        std::string const& filename,
        std::vector<std::string> const& sample_names,
        bool inverse_sample_names = false
    )
        : filename_( filename )
        , file_( filename )
        , header_( file_ )
        , record_( header_ )
    {
        // Filter sample columns by their name
        header_.set_samples( sample_names, inverse_sample_names );

        // Read the first record of the file.
        increment();
    }

    ~VcfInputIterator() = default;

    VcfInputIterator( self_type const& ) = delete;
    VcfInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = delete;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------------------

    /**
    * @brief Return true iff dereferencing is valid, i.e., iff there is a VCF record available.
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

    std::string const& filename() const
    {
        return filename_;
    }

    HtsFile const& hts_file() const
    {
        return file_;
    }

    HtsFile& hts_file()
    {
        return file_;
    }

    VcfHeader const& header() const
    {
        return header_;
    }

    VcfHeader& header()
    {
        return header_;
    }

    VcfRecord const& record() const
    {
        return record_;
    }

    VcfRecord& record()
    {
        return record_;
    }

    VcfRecord const* operator ->() const
    {
        return &record_;
    }

    VcfRecord* operator ->()
    {
        return &record_;
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
        good_ = record_.read_next( file_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool good_ = true;
    std::string filename_;

    // htslib structs
    HtsFile   file_;
    VcfHeader header_;
    VcfRecord record_;
};

} // namespace population
} // namespace genesis

#endif // include guard
