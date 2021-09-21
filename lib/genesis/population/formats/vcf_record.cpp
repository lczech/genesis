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

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_record.hpp"

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_header.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Typedefs and Enums
// =================================================================================================

// VariantType
static_assert(
    static_cast<int>( VcfRecord::VariantType::kRef ) == VCF_REF,
    "Definitions of VCF_REF in htslib and of VariantType::kRef in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfRecord::VariantType::kSnp ) == VCF_SNP,
    "Definitions of VCF_SNP in htslib and of VariantType::kSnp in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfRecord::VariantType::kMnp ) == VCF_MNP,
    "Definitions of VCF_MNP in htslib and of VariantType::kMnp in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfRecord::VariantType::kIndel ) == VCF_INDEL,
    "Definitions of VCF_INDEL in htslib and of VariantType::kIndel in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfRecord::VariantType::kOther ) == VCF_OTHER,
    "Definitions of VCF_OTHER in htslib and of VariantType::kOther in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfRecord::VariantType::kBreakend ) == VCF_BND,
    "Definitions of VCF_BND in htslib and of VariantType::kBreakend in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfRecord::VariantType::kOverlap ) == VCF_OVERLAP,
    "Definitions of VCF_OVERLAP in htslib and of VariantType::kOverlap in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

VcfRecord::VcfRecord()
{
    record_ = ::bcf_init();
    if( ! record_ ) {
        throw std::runtime_error( "Failed to default-initialize VcfRecord bcf1_t data structure." );
    }
}

VcfRecord::VcfRecord( VcfHeader& header )
{
    header_ = &header;
    record_ = ::bcf_init();
    if( ! record_ ) {
        throw std::runtime_error( "Failed to initialize VcfRecord bcf1_t data structure." );
    }
}

VcfRecord::VcfRecord( VcfHeader& header, ::bcf1_t* bcf1 )
{
    header_ = &header;
    record_ = ::bcf_dup( bcf1 );
    if( ! record_ ) {
        throw std::runtime_error( "Failed to copy-initialize VcfRecord bcf1_t data structure." );
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

VcfRecord::VcfRecord( VcfRecord&& other )
{
    // We swap everything, so that once `other` gets destroyed, our current data of `this` gets
    // also destroyed with it.
    this->swap( other );
}

VcfRecord& VcfRecord::operator= ( VcfRecord&& other )
{
    if( this == &other ) {
        return *this;
    }
    this->swap( other );
    return *this;
}

void VcfRecord::swap( VcfRecord& other )
{
    std::swap( header_,            other.header_ );
    std::swap( record_,            other.record_ );
    std::swap( info_dest_string_,  other.info_dest_string_ );
    std::swap( info_dest_float_,   other.info_dest_float_ );
    std::swap( info_dest_int_,     other.info_dest_int_ );
    std::swap( info_ndest_string_, other.info_ndest_string_ );
    std::swap( info_ndest_float_,  other.info_ndest_float_ );
    std::swap( info_ndest_int_,    other.info_ndest_int_ );
}

// =================================================================================================
//     Simple Fixed Columns
// =================================================================================================

void VcfRecord::unpack() const
{
    ::bcf_unpack( record_, BCF_UN_STR );
}

std::string VcfRecord::get_chromosome() const
{
    std::string chr = ::bcf_hdr_id2name( header_->data(), record_->rid );
    if( chr.empty() ) {
        throw std::runtime_error(
            "Malformed VCF file: empty chromosome name"
        );
    }
    return chr;
}

size_t VcfRecord::get_position() const
{
    // This one time, htslib wants to be smart and make the position 0-based. While we appreciate
    // their effort, in that case, this leads to inconcruencies for users who are not aware of this.
    // Hence, we "fix" this back to the original number as given in the 1-based VCF/BCF file.
    assert( record_->pos >= 0 );
    return record_->pos + 1;
}

std::string VcfRecord::get_id() const
{
    ::bcf_unpack( record_, BCF_UN_STR );
    return std::string( record_->d.id );
}

std::string VcfRecord::at() const
{
    auto const pos_id = std::string( get_id() != "." ? " (" + get_id() + ")" : "" );
    return get_chromosome() + ":" + std::to_string( get_position() ) + pos_id;
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
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    auto ret = std::vector<std::string>( record_->n_allele - 1 );
    for( size_t i = 1; i < record_->n_allele; ++i ) {
        ret[ i - 1 ] = std::string( record_->d.allele[i] );
    }
    return ret;
}

std::string VcfRecord::get_alternative( size_t index ) const
{
    // The ALT alleles are stored in allele[1..n], so we need to re-index into our result vector.
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    if( index + 1 >= record_->n_allele ) {
        throw std::invalid_argument(
            "Cannot retrieve alternative at index " + std::to_string(index) + ", as the record " +
            "line only has " + std::to_string( record_->n_allele - 1 ) + " alternative alleles."
        );
    }
    assert( index + 1 < record_->n_allele );
    return record_->d.allele[ index + 1 ];
}

size_t VcfRecord::get_alternatives_count() const
{
    // Even if there are no alternatives (that is, set to "."), there has to be at least the REF
    // allele, which we assert here, so that the re-indexing is ensured to work.
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    return record_->n_allele - 1;
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

std::string VcfRecord::get_variant( size_t index ) const
{
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    if( index >= record_->n_allele ) {
        throw std::invalid_argument(
            "Cannot retrieve variant at index " + std::to_string(index) + ", as the record " +
            "line only has " + std::to_string( record_->n_allele ) + " variants (reference + " +
            "alternative alleles)."
        );
    }
    assert( index < record_->n_allele );
    return record_->d.allele[ index ];
}

size_t VcfRecord::get_variant_count() const
{
    ::bcf_unpack( record_, BCF_UN_STR );
    assert( record_->n_allele > 0 );
    return record_->n_allele;
}

VcfRecord::VariantType VcfRecord::get_variant_types() const
{
    return static_cast<VariantType>( ::bcf_get_variant_types( record_ ));
}

VcfRecord::VariantType VcfRecord::get_variant_type( size_t alt_index ) const
{
    // Nope, `bcf_dec_t.n_var` is NOT the number of variants that this record has. It is the
    // allocated size, which might be bigger if the `bcf1_t` is re-used between records.
    // So, we have to use `bcf1_t.n_allele` to get to the number of actual valid entries...
    // Furthermore, `bcf_dec_t.var_type` is not necessarily one type, but the or'ed value
    // of all types of the variant alleles. Of course. Well documented, htslib!
    if( alt_index >= record_->n_allele ) {
        throw std::runtime_error(
            "Alternative allele index " + std::to_string( alt_index ) +
            " out of bounds of the number of alleles " + std::to_string( record_->n_allele ) +
            " of the record."
        );
    }
    return static_cast<VariantType>( ::bcf_get_variant_type( record_, static_cast<int>( alt_index )));
}

bool VcfRecord::is_snp() const
{
    return ::bcf_is_snp( record_ );
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
        ret.push_back( std::string( bcf_hdr_int2id( header_->data(), BCF_DT_ID, record_->d.flt[i] )));
    }
    return ret;
}

bool VcfRecord::has_filter( std::string const& filter ) const
{
    // htslib expects a non-const pointer, as it potentially mutates the string...
    char* cstr = new char[ filter.length() + 1] ;
	std::strcpy( cstr, filter.c_str() );

    // Make the call.
    int const res = ::bcf_has_filter( header_->data(), record_, cstr );

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
    return ::bcf_has_filter( header_->data(), record_, pass );
}

// =================================================================================================
//     Info Column
// =================================================================================================

std::vector<std::string> VcfRecord::get_info_ids() const
{
    ::bcf_unpack( record_, BCF_UN_INFO );
    auto ret = std::vector<std::string>( record_->n_info );
    for( size_t i = 0; i < static_cast<size_t>( record_->n_info ); ++i ) {
        ret[i] = std::string( bcf_hdr_int2id( header_->data(), BCF_DT_ID, record_->d.info[i].key ));
    }
    return ret;
}

bool VcfRecord::has_info( std::string const& id ) const
{
    return has_info( id.c_str() );
}

bool VcfRecord::has_info( char const* id ) const
{
    return ::bcf_get_info( header_->data(), record_, id ) != nullptr;

    // The below code seems to return whether the field exists at all in the header... not what we want.
    // int const id = bcf_hdr_id2int( header_->data(), BCF_DT_ID, id.c_str() );
    // return bcf_hdr_idinfo_exists( header_->data(), BCF_HL_INFO, id );
}

void VcfRecord::assert_info( std::string const& id ) const
{
    assert_info( id.c_str() );
}

void VcfRecord::assert_info( char const* id ) const
{
    if( ! ::bcf_get_info( header_->data(), record_, id )) {
        throw std::runtime_error(
            "Required INFO tag " + std::string( id ) + " is not present in the record at " + at()
        );
    }
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

std::vector<std::string> VcfRecord::get_format_ids() const
{
    ::bcf_unpack( record_, BCF_UN_FMT );
    auto ret = std::vector<std::string>( record_->n_fmt );
    for( size_t i = 0; i < static_cast<size_t>( record_->n_fmt ); ++i ) {
        ret[i] = std::string( bcf_hdr_int2id( header_->data(), BCF_DT_ID, record_->d.fmt[i].id ));
    }
    return ret;
}

bool VcfRecord::has_format( std::string const& id ) const
{
    return has_format( id.c_str() );

}

bool VcfRecord::has_format( char const* id ) const
{
    return ::bcf_get_fmt( header_->data(), record_, id ) != nullptr;

}

void VcfRecord::assert_format( std::string const& id ) const
{
    assert_format( id.c_str() );
}

void VcfRecord::assert_format( char const* id ) const
{
    if( ! ::bcf_get_fmt( header_->data(), record_, id )) {
        throw std::runtime_error(
            "Required FORMAT tag " + std::string( id ) + " is not present in the record at " + at()
        );
    }
}

// =================================================================================================
//     Sample Columns
// =================================================================================================

VcfFormatIteratorGenotype VcfRecord::begin_format_genotype() const
{
    return VcfFormatIteratorGenotype( header_->data(), record_, "GT", VcfValueType::kInteger );
}

VcfFormatIteratorGenotype VcfRecord::end_format_genotype() const
{
    return VcfFormatIteratorGenotype();
}

genesis::utils::Range<VcfFormatIteratorGenotype> VcfRecord::get_format_genotype() const {
    return {
        VcfFormatIteratorGenotype( header_->data(), record_, "GT", VcfValueType::kInteger ),
        VcfFormatIteratorGenotype()
    };
}

VcfFormatIteratorString VcfRecord::begin_format_string( std::string const& id ) const
{
    return VcfFormatIteratorString( header_->data(), record_, id, VcfValueType::kString );
}

VcfFormatIteratorString VcfRecord::end_format_string() const
{
    return VcfFormatIteratorString();
}

genesis::utils::Range<VcfFormatIteratorString> VcfRecord::get_format_string(
    std::string const& id
) const {
    return {
        VcfFormatIteratorString( header_->data(), record_, id, VcfValueType::kString ),
        VcfFormatIteratorString()
    };
}

VcfFormatIteratorInt VcfRecord::begin_format_int( std::string const& id ) const
{
    return VcfFormatIteratorInt( header_->data(), record_, id, VcfValueType::kInteger );
}

VcfFormatIteratorInt VcfRecord::end_format_int() const
{
    return VcfFormatIteratorInt();
}

genesis::utils::Range<VcfFormatIteratorInt> VcfRecord::get_format_int(
    std::string const& id
) const {
    return {
        VcfFormatIteratorInt( header_->data(), record_, id, VcfValueType::kInteger ),
        VcfFormatIteratorInt()
    };
}

VcfFormatIteratorFloat VcfRecord::begin_format_float( std::string const& id ) const
{
    return VcfFormatIteratorFloat( header_->data(), record_, id, VcfValueType::kFloat );
}

VcfFormatIteratorFloat VcfRecord::end_format_float() const
{
    return VcfFormatIteratorFloat();
}

genesis::utils::Range<VcfFormatIteratorFloat> VcfRecord::get_format_float(
    std::string const& id
) const {
    return {
        VcfFormatIteratorFloat( header_->data(), record_, id, VcfValueType::kFloat ),
        VcfFormatIteratorFloat()
    };
}

// =================================================================================================
//     Modifiers
// =================================================================================================

bool VcfRecord::read_next( HtsFile& source )
{
    bool const good = ( ::bcf_read1( source.data(), header_->data(), record_ ) == 0 );
    // if( good ) {
    //     ::bcf_unpack( record_ , BCF_UN_ALL );
    // }
    return good;
}

// =================================================================================================
//     Internal Members
// =================================================================================================

int VcfRecord::get_info_ptr_( std::string const& id, int ht_type, void** dest, int* ndest) const
{
    // Call the htslib function, and call our function to check the return value, which encodes
    // for errors as well (if negative). If there was an error, that function call throws
    // an exception.
    int const len = ::bcf_get_info_values( header_->data(), record_, id.c_str(), dest, ndest, ht_type );
    VcfHeader::check_value_return_code_( header_->data(), id, ht_type, BCF_HL_INFO, len );

    // Assert that if ndest is used (for all but flags), it has a valid value.
    assert( !ndest || ( *ndest >= 0 && *ndest >= len ));
    return len;
}

} // namespace population
} // namespace genesis

#endif // htslib guard
