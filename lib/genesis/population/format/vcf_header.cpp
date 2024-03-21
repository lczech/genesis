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

#include "genesis/population/format/vcf_header.hpp"

#include "genesis/utils/text/string.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

// VcfHeader::VcfHeader( std::string const& file_name )
// {
//     // We could use our HtsFile class here, but as we only need this access here very locally,
//     // we directly use the htslib functions instead.
//
//     // Open the file.
//     auto file_  = ::hts_open( file_name.c_str(), "r" );
//     if( ! file_ ) {
//         throw std::runtime_error( "Failed to open VCF/BCF file " + file_name );
//     }
//
//     // Read header.
//     header_ = ::bcf_hdr_read( file_ );
//
//     // Close the file again.
//     ::hts_close( file_ );
// }

VcfHeader::VcfHeader( std::string const& mode )
{
    header_ = ::bcf_hdr_init( mode.c_str() );
    if( ! header_ ) {
        throw std::runtime_error( "Failed to initialize VcfHeader bcf_hdr_t data structure." );
    }
}

VcfHeader::VcfHeader( HtsFile& hts_file )
{
    // Read header.
    header_ = ::bcf_hdr_read( hts_file.data() );
    if( ! header_ ) {
        throw std::runtime_error(
            "Failed to initialize VcfHeader bcf_hdr_t data structure for file " +
            hts_file.file_name()
        );
    }
}

VcfHeader::VcfHeader( ::bcf_hdr_t* bcf_hdr )
{
    header_ = ::bcf_hdr_dup( bcf_hdr );
    if( ! header_ ) {
        throw std::runtime_error( "Failed to copy-initialize VcfHeader bcf_hdr_t data structure." );
    }
}

VcfHeader::~VcfHeader()
{
    if( header_ ) {
        ::bcf_hdr_destroy( header_ );
    }
}

VcfHeader::VcfHeader( VcfHeader&& other )
{
    // We need a custom move, as the explicitly defaulted one "moves" the header_ pointer, which
    // effectively copies it, and then upon destruction of the moved-from object frees the header_.
    // So, instead we swap, so that once `other` gets destroyed (as it is moved from, it will go
    // out of scope soon), our current data of `this` gets also destroyed with it.
    std::swap( header_, other.header_ );
}

VcfHeader& VcfHeader::operator= ( VcfHeader&& other )
{
    // Same reasoning as above. Need additional check for self assignment,
    // as otherwise we'd destroy the header as well.
    if( this == &other ) {
        return *this;
    }
    std::swap( header_, other.header_ );
    return *this;
}

// =================================================================================================
//     General Acccessors
// =================================================================================================

std::string VcfHeader::version() const
{
    return std::string( ::bcf_hdr_get_version( header_ ));
}

// =================================================================================================
//     Chromosomes / Contigs / Sequences
// =================================================================================================

std::vector<std::string> VcfHeader::get_chromosomes() const
{
    // bcf_hdr_seqnames returns a newly allocated array of pointers to the seq names.
    // We have to deallocate the array, but not the seqnames themselves; the number
    // of sequences is stored in the int pointer passed in as the second argument.
    // The id in each record can be used to index into the array to obtain the sequence name.
    int nseq = 0;
    const char** seqnames = ::bcf_hdr_seqnames( header_, &nseq );
    assert( nseq >= 0 );

    // If there are supposed to be names, but the array still is empty, we have an error.
    if( nseq > 0 && !seqnames ) {
        throw std::runtime_error(
            "Cannot obtain chromosome/contig/sequence names from VCF/BCF header."
        );
    }

    // Copy over to result.
    auto res = std::vector<std::string>( nseq );
    for( size_t i = 0; i < static_cast<size_t>(nseq); ++i ) {
        res[i] = std::string( seqnames[i] );

        // bcf_hdr_id2name is another way to get the name of a sequence.
        // Assert that this is identical.
        assert( res[i] == std::string( ::bcf_hdr_id2name( header_, i )));
    }

    // Clean up and return.
    if( seqnames != nullptr ) {
        free(seqnames);
    }
    return res;
}

size_t VcfHeader::get_chromosome_length( std::string const& chrom_name ) const
{
    if( chrom_name.empty() ) {
        throw std::invalid_argument( "Invalid chromosome name: empty" );
    }

    auto const id = ::bcf_hdr_name2id( header_, chrom_name.c_str() );
    if( id < 0 ) {
        throw std::invalid_argument( "Invalid chromosome name '" + chrom_name +"'" );
    }

    size_t const result = header_->id[BCF_DT_CTG][id].val->info[0];
    assert(
        get_chromosome_values( chrom_name ).count("length") == 0 ||
        std::stoul( get_chromosome_values( chrom_name ).at("length") ) == result
    );
    return result;
}

std::unordered_map<std::string, std::string> VcfHeader::get_chromosome_values( std::string const& chrom_name ) const
{
    return get_hrec_values_( BCF_HL_CTG, chrom_name );
}

// =================================================================================================
//     Filter
// =================================================================================================

std::vector<std::string> VcfHeader::get_filter_ids() const
{
    return get_hrec_ids_( BCF_HL_FLT );
}

std::unordered_map<std::string, std::string> VcfHeader::get_filter_values( std::string const& id ) const
{
    return get_hrec_values_( BCF_HL_FLT, id );
}

void VcfHeader::assert_filter( std::string const& id ) const
{
    test_hl_entry_( true, BCF_HL_FLT, id, false, {}, false, {}, false, 0 );
}

bool VcfHeader::has_filter( std::string const& id ) const
{
    return test_hl_entry_( false, BCF_HL_FLT, id, false, {}, false, {}, false, 0 );
}

// =================================================================================================
//     Info
// =================================================================================================

std::vector<std::string> VcfHeader::get_info_ids() const
{
    return get_hrec_ids_( BCF_HL_INFO );
}

VcfSpecification VcfHeader::get_info_specification( std::string const& id ) const
{
    return get_specification_( BCF_HL_INFO, id );
}

std::unordered_map<std::string, std::string> VcfHeader::get_info_values( std::string const& id ) const
{
    return get_hrec_values_( BCF_HL_INFO, id );
}

void VcfHeader::assert_info( std::string const& id ) const
{
    test_hl_entry_( true, BCF_HL_INFO, id, false, {}, false, {}, false, 0 );
}

void VcfHeader::assert_info( std::string const& id, VcfValueType type ) const
{
    test_hl_entry_( true, BCF_HL_INFO, id, true, type, false, {}, false, 0 );
}

void VcfHeader::assert_info( std::string const& id, VcfValueType type, VcfValueSpecial num ) const
{
    test_hl_entry_( true, BCF_HL_INFO, id, true, type, true, num, false, 0 );
}

void VcfHeader::assert_info( std::string const& id, VcfValueType type, size_t number ) const
{
    test_hl_entry_( true, BCF_HL_INFO, id, true, type, true, VcfValueSpecial::kFixed, true, number );
}

bool VcfHeader::has_info( std::string const& id ) const
{
    return test_hl_entry_( false, BCF_HL_INFO, id, false, {}, false, {}, false, 0 );
}

bool VcfHeader::has_info( std::string const& id, VcfValueType type ) const
{
    return test_hl_entry_( false, BCF_HL_INFO, id, true, type, false, {}, false, 0 );
}

bool VcfHeader::has_info( std::string const& id, VcfValueType type, VcfValueSpecial num ) const
{
    return test_hl_entry_( false, BCF_HL_INFO, id, true, type, true, num, false, 0 );
}

bool VcfHeader::has_info( std::string const& id, VcfValueType type, size_t number ) const
{
    return test_hl_entry_( false, BCF_HL_INFO, id, true, type, true, VcfValueSpecial::kFixed, true, number );
}

// =================================================================================================
//     Format
// =================================================================================================

std::vector<std::string> VcfHeader::get_format_ids() const
{
    return get_hrec_ids_( BCF_HL_FMT );
}

VcfSpecification VcfHeader::get_format_specification( std::string const& id ) const
{
    return get_specification_( BCF_HL_FMT, id );
}

std::unordered_map<std::string, std::string> VcfHeader::get_format_values( std::string const& id ) const
{
    return get_hrec_values_( BCF_HL_FMT, id );
}

void VcfHeader::assert_format( std::string const& id ) const
{
    test_hl_entry_( true, BCF_HL_FMT, id, false, {}, false, {}, false, 0 );
}

void VcfHeader::assert_format( std::string const& id, VcfValueType type ) const
{
    test_hl_entry_( true, BCF_HL_FMT, id, true, type, false, {}, false, 0 );
}

void VcfHeader::assert_format( std::string const& id, VcfValueType type, VcfValueSpecial num ) const
{
    test_hl_entry_( true, BCF_HL_FMT, id, true, type, true, num, false, 0 );
}

void VcfHeader::assert_format( std::string const& id, VcfValueType type, size_t number ) const
{
    test_hl_entry_( true, BCF_HL_FMT, id, true, type, true, VcfValueSpecial::kFixed, true, number );
}

bool VcfHeader::has_format( std::string const& id ) const
{
    return test_hl_entry_( false, BCF_HL_FMT, id, false, {}, false, {}, false, 0 );
}

bool VcfHeader::has_format( std::string const& id, VcfValueType type ) const
{
    return test_hl_entry_( false, BCF_HL_FMT, id, true, type, false, {}, false, 0 );
}

bool VcfHeader::has_format( std::string const& id, VcfValueType type, VcfValueSpecial num ) const
{
    return test_hl_entry_( false, BCF_HL_FMT, id, true, type, true, num, false, 0 );
}

bool VcfHeader::has_format( std::string const& id, VcfValueType type, size_t number ) const
{
    return test_hl_entry_( false, BCF_HL_FMT, id, true, type, true, VcfValueSpecial::kFixed, true, number );
}

// =================================================================================================
//     Samples
// =================================================================================================

size_t VcfHeader::get_sample_count() const
{
    assert( bcf_hdr_nsamples(header_) == header_->n[BCF_DT_SAMPLE] );
    return header_->n[BCF_DT_SAMPLE];
}

std::string VcfHeader::get_sample_name( size_t index ) const
{
    if( index >= get_sample_count() ) {
        throw std::invalid_argument(
            "Cannot get sample name for sample at index " + std::to_string(index) +
            ", as the VCF/BCF file only uses " + std::to_string( get_sample_count() ) + " samples."
        );
    }
    return header_->samples[index];
}

size_t VcfHeader::get_sample_index( std::string const& name ) const
{
    assert( bcf_hdr_nsamples(header_) == header_->n[BCF_DT_SAMPLE] );
    size_t const sample_count = header_->n[BCF_DT_SAMPLE];
    for( size_t i = 0; i < sample_count; ++i ) {
        if( strcmp( name.c_str(), header_->samples[i] ) == 0 ) {
            return i;
        }
    }
    throw std::runtime_error( "Sample name '" + name + "' not found in VCF file." );
}

std::vector<std::string> VcfHeader::get_sample_names() const
{
    assert( bcf_hdr_nsamples(header_) == header_->n[BCF_DT_SAMPLE] );
    size_t const sample_count = header_->n[BCF_DT_SAMPLE];
    auto result = std::vector<std::string>( sample_count );
    for( size_t i = 0; i < sample_count; ++i ) {
        result[i] = std::string( header_->samples[i] );
    }
    return result;
}

void VcfHeader::set_samples(
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names
) {
    // Restrict the samples that are read for each record, using the provided list of names.
    int suc = 0;
    if( sample_names.empty() ) {
        // If an empty list was supplied, either read none or all samples,
        // depending on inverse_sample_names.
        if( inverse_sample_names ) {
            // Basically, this is identical to not calling the function at all.
            // But for completeness, we still call it here.
            suc = ::bcf_hdr_set_samples( header_, "-", 0 );
        } else {
            suc = ::bcf_hdr_set_samples( header_, nullptr, 0 );
        }
    } else {
        // If an actual list of sample names is given, we build the required string from it,
        // then pass that to the htslib function, and check the result for errors.
        std::string list = ( inverse_sample_names ? "^" : "" );
        for( size_t i = 0; i < sample_names.size(); ++i ) {
            if( i > 0 ) {
                list += ",";
            }
            list += sample_names[i];
        }
        suc = ::bcf_hdr_set_samples( header_, list.c_str(), 0 );
    }

    // Check the return code of the above calls to htslib.
    // The htslib documentation is not clear on how to interpret the return values of this.
    // It states (https://github.com/samtools/htslib/blob/develop/htslib/vcf.h):
    //     Returns 0 on success, -1 on error or a positive integer if the list
    //     contains samples not present in the VCF header. In such a case, the
    //     return value is the index of the offending sample.
    // Is that index 1-based then?! Seriously? We checked the code and it seems it is. WTF.
    if( suc < 0 ) {
        throw std::runtime_error(
            "Invalid list of sample names provided that cannot be used for constricting "
            "the sample parsing of the VCF/BCF file."
        );
    } else if( suc > 0 ) {
        // Fix to use 0-based index.
        --suc;

        // Let's assume that htslib returns valid indices, and assert this.
        assert( 0 <= suc && static_cast<size_t>(suc) < sample_names.size() );
        throw std::runtime_error(
            "Provided list of sample names contains entry '" + sample_names[suc] +
            "', which is not part of the sample names in the file header, and hence cannot"
            " be used for constricting the sample parsing of the VCF/BCF file."
        );
    }
}

// =================================================================================================
//     Internal Helpers
// =================================================================================================

std::vector<std::string> VcfHeader::get_hrec_ids_( int hl_type ) const
{
    std::vector<std::string> res;
    for( int i = 0; i < header_->nhrec; ++i ) {
        // We need to scan all hrec entries, and only process the ones we are interested in...
        if( header_->hrec[i]->type != hl_type ) {
            continue;
        }
        for( int j = 0; j < header_->hrec[i]->nkeys; ++j ) {
            // Find the "ID" field, and copy it to our result.
            if( std::strcmp( header_->hrec[i]->keys[j], "ID" ) == 0 ) {
                res.push_back( std::string( header_->hrec[i]->vals[j] ));
            }
        }
    }
    return res;
}

std::unordered_map<std::string, std::string> VcfHeader::get_hrec_values_( int hl_type, std::string const& id ) const
{
    std::unordered_map<std::string, std::string> res;
    bcf_hrec_t* hrec = ::bcf_hdr_get_hrec( header_, hl_type, "ID", id.c_str(), nullptr );

    if( !hrec ) {
        throw std::runtime_error(
            vcf_hl_type_to_string(hl_type) + " tag " + id + " not defined in the VCF/BCF header."
         );
    }
    for( int i = 0; i < hrec->nkeys; ++i ) {
        res[ std::string( hrec->keys[i] )] = std::string( hrec->vals[i] );
    }
    return res;
}

VcfSpecification VcfHeader::get_specification_( int hl_type, std::string const& id) const
{
    auto const int_id = ::bcf_hdr_id2int( header_, BCF_DT_ID, id.c_str() );
    if( ! bcf_hdr_idinfo_exists( header_, hl_type, int_id )) {
        throw std::runtime_error(
            vcf_hl_type_to_string(hl_type) + " tag " + id + " not defined in the VCF/BCF header."
         );
    }

    VcfSpecification res;
    res.id = id;

    // We use the same values in our Number and Type enums than the htslib-defined macro values,
    // which is statically asserted. So here, we can simply cast to our enum values.
    res.type    = static_cast<VcfValueType>( bcf_hdr_id2type( header_, hl_type, int_id ));
    res.special  = static_cast<VcfValueSpecial>( bcf_hdr_id2length( header_, hl_type, int_id ));
    res.number   = bcf_hdr_id2number( header_, hl_type, int_id );

    // Description is a required entry, but there seems to be no macro in htslib for this.
    // We can (hopefully?!) here simply assert that the hrec exists, because we above already
    // tested that the int_id exists. Let's hope that htslib is conistent with this.
    bcf_hrec_t* hrec = ::bcf_hdr_get_hrec( header_, hl_type, "ID", id.c_str(), nullptr );
    assert( hrec );
    auto const descr_key = ::bcf_hrec_find_key( hrec, "Description" );
    if( descr_key >= 0 ) {
        // It seems that htslib leaves the quotes around the description.
        // That is ugly, let's remove!
        res.description = utils::trim( std::string( hrec->vals[descr_key] ), "\"");
    }
    return res;
}

bool VcfHeader::test_hl_entry_(
    bool throwing,
    int hl_type, std::string const& id,
    bool with_type, VcfValueType type,
    bool with_special, VcfValueSpecial special,
    bool with_number, size_t number
) const {
    // We always want to test whether the given ID is defined in the header line type (hl_type,
    // which can be BCF_HL_INFO, BCF_HL_FORMAT, etc).
    // Let's use two ways of testing this. Because why not. This assertion function is typically
    // called once per VCF file, so that doesn't cost us much, but gives more certainty that
    // we are using htslib correctly.
    bcf_hrec_t* hrec = ::bcf_hdr_get_hrec( header_, hl_type, "ID", id.c_str(), nullptr );
    if( !hrec ) {
        if( throwing ) {
            throw std::runtime_error(
                "Required " + vcf_hl_type_to_string(hl_type) + " tag " + id +
                " is not defined in the VCF/BCF header."
            );
        } else {
            return false;
        }
    }
    auto const int_id = ::bcf_hdr_id2int( header_, BCF_DT_ID, id.c_str() );
    if( ! bcf_hdr_idinfo_exists( header_, hl_type, int_id )) {
        if( throwing ) {
            throw std::runtime_error(
                "Required " + vcf_hl_type_to_string(hl_type) + " tag " + id +
                " is not defined in the VCF/BCF header."
            );
        } else {
            return false;
        }
    }

    // If requested, test that the header line sets the correct data type (Integer, String, etc;
    // this is one of the many circumstances where the word type is used over and over again in htslib).
    if( with_type ) {
        auto const def_type = bcf_hdr_id2type( header_, hl_type, int_id );
        if( static_cast<int>( def_type ) != static_cast<int>( type ) ) {
            if( throwing ) {
                throw std::runtime_error(
                    vcf_hl_type_to_string(hl_type) + " tag " + id + " is defined in the VCF/BCF header "
                    "to be of value data type '" + vcf_value_type_to_string( def_type ) +
                    "', but data type '" + vcf_value_type_to_string( type ) + "' is required instead."
                );
            } else {
                return false;
            }
        }
    }

    // Same for the number of values. Here, we test both the kind of number values (fixed or something
    // else), and the actual number, if needed. If an actual number is given, make sure
    // that the special is set to fixed.
    auto const def_special = bcf_hdr_id2length( header_, hl_type, int_id );
    if( with_special && ( static_cast<int>( def_special ) != static_cast<int>( special ))) {
        if( throwing ) {
            throw std::runtime_error(
                vcf_hl_type_to_string(hl_type) + " tag " + id + " is defined in the VCF/BCF header "
                "to have '" + vcf_value_special_to_string( def_special ) + "' number of values, but '" +
                vcf_value_special_to_string( special ) + "' is required instead."
            );
        } else {
            return false;
        }
    }
    if( with_number ) {
        if( special != VcfValueSpecial::kFixed ) {
            if( throwing ) {
                throw std::runtime_error(
                    vcf_hl_type_to_string(hl_type) + " tag " + id + " is defined in the VCF/BCF header "
                    "to have '" + vcf_value_special_to_string( def_special ) + "' number of values, but '" +
                    vcf_value_special_to_string( special ) + "' with n=" + std::to_string( number ) +
                    " is required instead."
                );
            } else {
                return false;
            }
        }
        auto const def_number = bcf_hdr_id2number( header_, hl_type, int_id );
        assert( with_special );
        assert( special == VcfValueSpecial::kFixed );
        assert( def_special == BCF_VL_FIXED );
        if( number != static_cast<size_t>( def_number )) {
            if( throwing ) {
                throw std::runtime_error(
                    vcf_hl_type_to_string(hl_type) + " tag " + id + " is defined in the VCF/BCF header "
                    "to have '" + vcf_value_special_to_string( def_special ) + "' number of values with n=" +
                    std::to_string( def_number ) + ", but n=" + std::to_string( number ) +
                    " is required instead."
                );
            } else {
                return false;
            }
        }
    }

    return true;
}

void VcfHeader::check_value_return_code_(
    ::bcf_hdr_t* header, std::string const& id, int ht_type, int hl_type, int return_value
) {
    assert( hl_type == BCF_HL_INFO || hl_type == BCF_HL_FMT );
    switch( return_value ) {
        case -1: {
            throw std::runtime_error(
                vcf_hl_type_to_string( hl_type ) + " tag " + id + " not defined in the VCF/BCF header."
            );
            break;
        }
        case -2: {
            bcf_hrec_t* hrec = ::bcf_hdr_get_hrec( header, hl_type, "ID", id.c_str(), nullptr );
            int const hrec_key = ::bcf_hrec_find_key( hrec, "Type" );
            std::string defined_type = ( hrec_key >= 0 ? std::string( hrec->vals[hrec_key] ) : "Unknown" );
            // int const tag_id = bcf_hdr_id2int( header, BCF_DT_ID, id.c_str() );

            throw std::runtime_error(
                "Clash between types defined in the header and encountered in the VCF/BCF record for " +
                vcf_hl_type_to_string( hl_type ) + " tag " + id + ": Header defines type '" + defined_type +
                "', but '" + vcf_value_type_to_string( ht_type ) + "' was requested instead."
            );
            break;
        }
        case -3: {
            throw std::runtime_error(
                vcf_hl_type_to_string( hl_type ) + " tag " + id + " not present in the VCF/BCF record."
            );
            break;
        }
        case -4: {
            throw std::runtime_error(
                vcf_hl_type_to_string( hl_type ) + " tag " + id + " retrieval could not be completed " +
                "(e.g., out of memory)."
            );
            break;
        }
        // default:
        //     (void);
    }

    // If we are here, the above part succeeded, which means, our return type could correctly be
    // retrieved. Let's assert that this is also the type that was specified in the header,
    // just to be sure that htslib does its job. Except for the genotype `GT` FORMAT field,
    // because htslib treats that as a special case that is a string that gets converted to an
    // int representation...
    auto const int_id = ::bcf_hdr_id2int( header, BCF_DT_ID, id.c_str() );
    assert( bcf_hdr_idinfo_exists( header, hl_type, int_id ) );
    assert( id == "GT" || bcf_hdr_id2type( header, hl_type, int_id ) == static_cast<uint32_t>( ht_type ));
    (void) int_id;

    // Assert that we are only left with valid, non-negative return codes.
    // All negative ones, which signify errors, are caught above.
    assert( return_value >= 0 );
}

} // namespace population
} // namespace genesis

#endif // htslib guard
