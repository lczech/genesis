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

#include "genesis/population/formats/vcf_record.hpp"

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_header.hpp"

#include "genesis/utils/core/logging.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

VcfRecord::VcfRecord()
{
    record_ = ::bcf_init();
    if( ! record_ ) {
        throw std::runtime_error( "Cannot default-initialize VcfRecord bcf1_t data structure." );
    }
}

VcfRecord::VcfRecord( VcfHeader& header )
{
    header_ = header.data();
    record_ = ::bcf_init();
    if( ! record_ ) {
        throw std::runtime_error( "Cannot initialize VcfRecord bcf1_t data structure." );
    }
}

VcfRecord::VcfRecord( VcfHeader& header, ::bcf1_t* bcf1 )
{
    header_ = header.data();
    record_ = ::bcf_dup( bcf1 );
    if( ! record_ ) {
        throw std::runtime_error( "Cannot copy-initialize VcfRecord bcf1_t data structure." );
    }
}

VcfRecord::~VcfRecord()
{
    if( record_ ) {
        ::bcf_destroy( record_ );
    }
    free( info_dest_string_ );
    free( info_dest_float_ );
    free( info_dest_int_ );
}

// =================================================================================================
//     Simple Fixed Columns
// =================================================================================================

std::string VcfRecord::get_chrom() const
{
    return ::bcf_hdr_id2name( header_, record_->rid );
}

size_t VcfRecord::get_position() const
{
    // This one time, htslib wants to be smart and make the position 0-based. While we appreciate
    // their effort, in that case, this leads to inconcruencies for users who are not aware of this.
    // Hence, we "fix" this back to the original number as given in the 1-based VCF/BCF file.
    return record_->pos + 1;
}

std::string VcfRecord::get_id() const
{
    ::bcf_unpack( record_, BCF_UN_STR );
    return std::string( record_->d.id );
}

std::string VcfRecord::get_reference() const
{
    // The REF allele is stored as allele[0], and its length is also stored in rlen, in addition to
    // it being null terminated anyway. Let's all use this, and assert this.
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    assert( std::strlen(record_->d.allele[0]) == static_cast<size_t>( record_->rlen ));
    return std::string( record_->d.allele[0] );
}

std::vector<std::string> VcfRecord::get_alternatives() const
{
    // The ALT alleles are stored in allele[1..n], so we need to re-index into our result vector.
    // There needs to be at least one REF and one ALT allele.
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    auto ret = std::vector<std::string>( record_->n_allele - 1 );
    for( size_t i = 1; i < record_->n_allele; ++i ) {
        ret[ i - 1 ] = std::string( record_->d.allele[i] );
    }
    return ret;
}

std::vector<std::string> VcfRecord::get_variants() const
{
    // Bit of code duplication here, but this is just short enough to not require
    // an extra function, at least for now.
    ::bcf_unpack( record_, BCF_UN_STR );
    auto ret = std::vector<std::string>( record_->n_allele );
    for( size_t i = 0; i < record_->n_allele; ++i ) {
        ret[i] = std::string( record_->d.allele[i] );
    }
    return ret;
}

double VcfRecord::get_quality() const
{
    return record_->qual;
}

// =================================================================================================
//     Filter Column
// =================================================================================================

std::vector<std::string> VcfRecord::get_filter_ids() const
{
    ::bcf_unpack( record_, BCF_UN_FLT );
    auto ret = std::vector<std::string>();
    for( size_t i = 0; i < static_cast<size_t>( record_->d.n_flt ); ++i ) {
        ret.push_back( std::string( bcf_hdr_int2id( header_, BCF_DT_ID, record_->d.flt[i] )));
    }
    return ret;
}

bool VcfRecord::has_filter( std::string const& filter ) const
{
    // htslib expects a non-const pointer, as it potentially mutates the string...
    char* cstr = new char[ filter.length() + 1] ;
	std::strcpy( cstr, filter.c_str() );

    // Make the call.
    int const res = ::bcf_has_filter( header_, record_, cstr );

    // Clean up and check result. Free the string before the potential exception!
    delete [] cstr;
    if( res == -1 ) {
        throw std::runtime_error( "Filter '" + filter + "' not defined in VCF/BCF header." );
    }
    return res;
}

bool VcfRecord::pass_filter() const
{
    // We here take a shortcut to avoid the above string copy in has_filter().
    // However, we still have to explicitly create the char array, as the htslib function
    // expects a non-const char*, which does not work by simply passing "PASS" as a string literal.
    char pass[] = "PASS";
    return ::bcf_has_filter( header_, record_, pass );
}

// =================================================================================================
//     Info Column
// =================================================================================================

std::vector<std::string> VcfRecord::get_info_ids() const
{
    ::bcf_unpack( record_, BCF_UN_INFO );
    auto ret = std::vector<std::string>( record_->n_info );
    for( size_t i = 0; i < static_cast<size_t>( record_->n_info ); ++i ) {
        ret[i] = std::string( bcf_hdr_int2id( header_, BCF_DT_ID, record_->d.info[i].key ));
    }
    return ret;
}

bool VcfRecord::has_info( std::string const& id ) const
{
    return ::bcf_get_info( header_, record_, id.c_str() ) != nullptr;

    // The below code seems to return whether the fild exists at all in the header... not what we want.
    // int const id = bcf_hdr_id2int( header_, BCF_DT_ID, id.c_str() );
    // return bcf_hdr_idinfo_exists( header_, BCF_HL_INFO, id );
}

std::string VcfRecord::get_info_string( std::string const& id ) const
{
    std::string result;
    get_info_string( id, result );
    return result;
}

void VcfRecord::get_info_string( std::string const& id, std::string& destination ) const
{
    // Load the result into our buffer that we use to avoid reallocating memory all the time.
    auto const len = get_info_ptr_(
        id, BCF_HT_STR, reinterpret_cast<void**>( &info_dest_string_ ), &info_ndest_string_
    );
    assert( len >= 0 );
    assert( info_ndest_string_ >= 0 );
    assert( info_ndest_string_ >= len );

    // Copy into our destination string.
    // destination.clear();
    // destination.reserve( info_ndest_string_ );
    // for( int i = 0; i < info_ndest_string_; ++i ) {
    //     if( destination.size() ) {
    //         destination += "***";
    //     }
    //     destination.append( std::string( static_cast<char*>( info_dest_string_ )));
    // }

    destination.assign( static_cast<char*>( info_dest_string_ ), len );
}

std::vector<double> VcfRecord::get_info_float( std::string const& id ) const
{
    std::vector<double> result;
    get_info_float( id, result );
    return result;
}

void VcfRecord::get_info_float( std::string const& id, std::vector<double>& destination ) const
{
    // Load the result into our buffer that we use to avoid reallocating memory all the time.
    auto const len = get_info_ptr_(
        id, BCF_HT_REAL, reinterpret_cast<void**>( &info_dest_float_ ), &info_ndest_float_
    );
    assert( len >= 0 );
    assert( info_ndest_float_ >= 0 );
    assert( info_ndest_float_ >= len );

    // Copy over into vector.
    destination.resize( len );
    for( int i = 0; i < len; ++i ) {
        destination[i] = static_cast<double>( static_cast<float*>(info_dest_float_)[i] );
    }
}

std::vector<int32_t> VcfRecord::get_info_int( std::string const& id ) const
{
    std::vector<int32_t> result;
    get_info_int( id, result );
    return result;
}

void VcfRecord::get_info_int( std::string const& id, std::vector<int32_t>& destination ) const
{
    // Load the result into our buffer that we use to avoid reallocating memory all the time.
    auto const len = get_info_ptr_(
        id, BCF_HT_INT, reinterpret_cast<void**>( &info_dest_int_ ), &info_ndest_int_
    );
    assert( len >= 0 );
    assert( info_ndest_int_ >= 0 );
    assert( info_ndest_int_ >= len );

    // Copy over into vector.
    destination.resize( len );
    for( int i = 0; i < len; ++i ) {
        destination[i] = static_cast<int32_t*>(info_dest_int_)[i];
    }
}

bool VcfRecord::get_info_flag( std::string const& id ) const
{
    // For flags, the destination pointer is not used, and instead the value is immediately returned.
    return get_info_ptr_( id, BCF_HT_FLAG, nullptr, nullptr );
}

// =================================================================================================
//     Format  Column
// =================================================================================================

// =================================================================================================
//     Sample Columns
// =================================================================================================

// =================================================================================================
//     Modifiers
// =================================================================================================

bool VcfRecord::read( HtsFile& source )
{
    bool const good = ( ::bcf_read1( source.data(), header_, record_ ) == 0 );
    // if( good ) {
    //     ::bcf_unpack( record_ , BCF_UN_ALL );
    // }
    return good;
}

// =================================================================================================
//     Internal Members
// =================================================================================================

/**
 * @brief Local helper function that does the repetetive part of the work of loading the info
 * data from a record.
 */
int VcfRecord::get_info_ptr_( std::string const& id, int ht_type, void** dest, int* ndest) const
{
    int const len = ::bcf_get_info_values( header_, record_, id.c_str(), dest, ndest, ht_type );
    switch( len ) {
        case -1: {
            throw std::runtime_error(
                "INFO tag " + id + " not defined in the VCF/BCF header."
            );
            break;
        }
        case -2: {
            bcf_hrec_t* hrec = ::bcf_hdr_get_hrec( header_, BCF_HL_INFO, "ID", id.c_str(), nullptr );
            int const hrec_key = ::bcf_hrec_find_key( hrec, "Type" );
            std::string defined_type = ( hrec_key >= 0 ? std::string( hrec->vals[hrec_key] ) : "Unknown" );
            // int const tag_id = bcf_hdr_id2int( header_, BCF_DT_ID, id.c_str() );

            throw std::runtime_error(
                "Clash between types defined in the header and encountered in the VCF/BCF record for "
                "INFO tag " + id + ": Header defines type '" + defined_type + "', but '" +
                VcfHeader::value_type_to_string( ht_type ) + "' was requested instead."
            );
            break;
        }
        case -3: {
            throw std::runtime_error(
                "INFO tag " + id + " not present in the VCF/BCF record."
            );
            break;
        }
        case -4: {
            throw std::runtime_error(
                "INFO tag " + id + " retrieval could not be completed (e.g., out of memory)."
            );
            break;
        }
        // default:
        //     (void);
    }

    // If we are here, the above part succeeded, which means, our return type could correctly be
    // retrieved. Let's assert that this is also the type that was specified in the header,
    // just to be sure that htslib does its job.
    auto const int_id = ::bcf_hdr_id2int( header_, BCF_DT_ID, id.c_str() );
    assert( bcf_hdr_idinfo_exists( header_, BCF_HL_INFO, int_id ) );
    assert( bcf_hdr_id2type( header_, BCF_HL_INFO, int_id ) == static_cast<uint32_t>( ht_type ));
    (void) int_id;

    // Assert that we are only left with valid, non-negative return codes.
    // All negative ones, which signify errors, are caught above.
    // Also, assert that if ndest is used (for all but flags), it has a valid value.
    assert( len >= 0 );
    assert( !ndest || ( *ndest >= 0 && *ndest >= len ));
    return len;
}

} // namespace population
} // namespace genesis
