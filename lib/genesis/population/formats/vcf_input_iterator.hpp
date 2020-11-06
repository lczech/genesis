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

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_header.hpp"
#include "genesis/population/formats/vcf_record.hpp"

#include <cassert>
#include <memory>
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
 *
 * Caveat: The iterator is an input iterator that traverses a single VCF file in one go.
 * That means, any copy of this iterator will also advance if another copy is incremented -
 * the will all point to the same VcfRecord. This is also true for the post-increment operator,
 * which returns a copy that also points to the same VcfRecord, and not to the previous one.
 */
class VcfInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = VcfInputIterator;
    using value_type        = VcfRecord;
    using pointer           = value_type const*;
    using reference         = value_type const&;
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
        , file_( std::make_shared<HtsFile>( filename ))
        , header_( std::make_shared<VcfHeader>( *file_ ))
        , record_( std::make_shared<VcfRecord>( *header_ ))
    {
        // Read the first record of the file.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input file name.
     *
     * Additionally, this constructor takes a list of @p sample_names which are used as filter so
     * that only those samples (columns of the VCF records) are evaluated and accessible - or,
     * if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
     */
    VcfInputIterator(
        std::string const& filename,
        std::vector<std::string> const& sample_names,
        bool inverse_sample_names = false
    )
        : filename_( filename )
        , file_( std::make_shared<HtsFile>( filename ))
        , header_( std::make_shared<VcfHeader>( *file_ ))
        , record_( std::make_shared<VcfRecord>( *header_ ))
    {
        // Filter sample columns by their name
        header_->set_samples( sample_names, inverse_sample_names );

        // Read the first record of the file.
        increment();
    }

    ~VcfInputIterator() = default;

    VcfInputIterator( self_type const& ) = default;
    VcfInputIterator( self_type&& )      = default;

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
        assert( file_ );
        assert( record_ );
        good_ = record_->read_next( *file_ );
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
    std::string filename_;

    // htslib structs. We use shared pointers here to allow copying this iterator.
    std::shared_ptr<HtsFile>   file_;
    std::shared_ptr<VcfHeader> header_;
    std::shared_ptr<VcfRecord> record_;
};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
