#ifndef GENESIS_POPULATION_FORMATS_VCF_INPUT_STREAM_H_
#define GENESIS_POPULATION_FORMATS_VCF_INPUT_STREAM_H_

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

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_header.hpp"
#include "genesis/population/formats/vcf_record.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     VCF/BCF Input Stream
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as a VCF/BCF file.
 *
 * This simple wrapper allows easy iteration through the records/lines of a VCF/BCF file, and
 * takes care of setting up the HtsFile, VcfHeader, and VcfRecord.
 *
 * Basic usage:
 *
 *     auto it = VcfInputStream( infile );
 *     while( it ) {
 *         // work with it.record() or it->...
 *         ++it;
 *     }
 *
 * or
 *
 *     for( auto it = VcfInputStream( infile );  it; ++it ) {
 *         // work with it
 *     }
 *
 * For details on working with the records/lines, see VcfRecord and VcfFormatIterator.
 */
class VcfInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = VcfInputStream;
    using value_type        = VcfRecord;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input. This is also the past-the-end iterator.
     */
    VcfInputStream() = default;

    /**
     * @brief Create an instance that reads from an input file name.
     *
     * By default, we expect the input to be ordered by chromosome and position, and throw an
     * exception if this is not the case. Set @p expect_ordered to `false` to deactivate this check,
     * and instead allow VcfRecord%s to appear in any order of chromosomes and positions.
     */
    explicit VcfInputStream(
        std::string const& filename,
        bool expect_ordered = true
    )
        // Call the other constuctor, to avoid code duplication.
        : VcfInputStream( filename, std::vector<std::string>{}, false, expect_ordered )
    {}

    /**
     * @brief Create an instance that reads from an input file name.
     *
     * Additionally, this constructor takes a list of @p sample_names which are used as filter so
     * that only those samples (columns of the VCF records) are evaluated and accessible - or,
     * if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
     *
     * @copydetails VcfInputStream( std::string const&, bool )
     */
    VcfInputStream(
        std::string const& filename,
        std::vector<std::string> const& sample_names,
        bool inverse_sample_names = false,
        bool expect_ordered = true
    )
        : filename_( filename )
        , expect_ordered_( expect_ordered )
        , file_(   std::make_shared<HtsFile>( filename ))
        , header_( std::make_shared<VcfHeader>( *file_ ))
        , record_( std::make_shared<VcfRecord>( *header_ ))
    {
        // Filter sample columns by their name.
        if( ! sample_names.empty() ) {
            header_->set_samples( sample_names, inverse_sample_names );
        }

        // Read the first record of the file.
        increment_();
    }

    ~VcfInputStream() = default;

    VcfInputStream( self_type const& ) = default;
    VcfInputStream( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------------------

    /**
    * @brief Return true iff dereferencing is valid, i.e., iff there is a VCF record available.
    */
    explicit operator bool() const
    {
        return static_cast<bool>( file_ );
    }

    bool good() const
    {
        return static_cast<bool>( file_ );
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
        // Here and below we assert the existence of a pointed-to object in the shared pointer,
        // which does not hold true if the default constructor of this class was used, in which
        // case any of these dereferencing functions here are supposed to be invalid - so, by
        // using assertions here, we can fail a bit more gracefully in such cases, instead of
        // getting seg faults due to nullptrs.
        assert( file_ );
        return *file_;
    }

    HtsFile& hts_file()
    {
        assert( file_ );
        return *file_;
    }

    VcfHeader const& header() const
    {
        assert( header_ );
        return *header_;
    }

    VcfHeader& header()
    {
        assert( header_ );
        return *header_;
    }

    VcfRecord const& record() const
    {
        assert( record_ );
        return *record_;
    }

    VcfRecord& record()
    {
        assert( record_ );
        return *record_;
    }

    VcfRecord const* operator ->() const
    {
        assert( record_ );
        return &*record_;
    }

    VcfRecord* operator ->()
    {
        assert( record_ );
        return &*record_;
    }

    VcfRecord const& operator*() const
    {
        assert( record_ );
        return *record_;
    }

    VcfRecord& operator*()
    {
        assert( record_ );
        return *record_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    self_type& operator ++ ()
    {
        increment_();
        return *this;
    }

    self_type& operator ++(int)
    {
        increment_();
        return *this;
    }

    bool operator==( self_type const& other ) const
    {
        // We want equality between iterators that are copies of each other, and inequality
        // for non copies that were not default constructed. This also works for the default
        // constructed iterator, which serves as the past-the-end marker.
        return file_ == other.file_;
    }

    bool operator!=( self_type const& other ) const
    {
        return !(*this == other);
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    void increment_();

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string filename_;
    bool expect_ordered_ = true;

    // htslib structs. We use shared pointers here to allow copying this iterator.
    std::shared_ptr<HtsFile>   file_;
    std::shared_ptr<VcfHeader> header_;
    std::shared_ptr<VcfRecord> record_;

};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
