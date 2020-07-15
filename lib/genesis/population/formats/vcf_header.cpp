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

#include "genesis/population/formats/vcf_header.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cassert>
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
//         throw std::runtime_error( "Cannot open VCF/BCF file " + file_name );
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
}

VcfHeader::VcfHeader( HtsFile& hts_file )
{
    // Read header.
    header_ = ::bcf_hdr_read( hts_file.data() );
}

VcfHeader::VcfHeader( ::bcf_hdr_t* bcf_hdr )
{
    header_ = ::bcf_hdr_dup( bcf_hdr );
}

VcfHeader::~VcfHeader()
{
    if( header_ ) {
        ::bcf_hdr_destroy( header_ );
    }
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

std::vector<std::string> VcfHeader::get_chroms() const
{
    // bcf_hdr_seqnames returns a newly allocated array of pointers to the seq names.
    // We have to deallocate the array, but not the seqnames themselves; the number
    // of sequences is stored in the int pointer passed in as the second argument.
    // The id in each record can be used to index into the array to obtain the sequence name.
    int nseq = 0;
    const char **seqnames = ::bcf_hdr_seqnames( header_, &nseq );
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

size_t VcfHeader::get_chrom_id( std::string const& chrom_name ) const
{
    return ::bcf_hdr_name2id( header_, chrom_name.c_str() );
}

std::string VcfHeader::get_chrom_name( size_t chrom_id ) const
{
    // For now, we simply cast to int. That can technically overflow, but if it does,
    // the whole htslib breaks anyway.
    return ::bcf_hdr_id2name( header_, static_cast<int>( chrom_id ));
}

size_t VcfHeader::get_chrom_length( std::string const& chrom_name ) const
{
    return header_->id[BCF_DT_CTG][ get_chrom_id( chrom_name )].val->info[0];
}

size_t VcfHeader::get_chrom_length( size_t chrom_id ) const
{
    auto const id = static_cast<int>( chrom_id );
    return header_->id[BCF_DT_CTG][id].val->info[0];
}

std::unordered_map<std::string, std::string> VcfHeader::get_chrom_values( std::string const& chrom_name ) const
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

// =================================================================================================
//     Info
// =================================================================================================

std::vector<std::string> VcfHeader::get_info_ids() const
{
    return get_hrec_ids_( BCF_HL_INFO );
}

std::unordered_map<std::string, std::string> VcfHeader::get_info_values( std::string const& id ) const
{
    return get_hrec_values_( BCF_HL_INFO, id );
}

// =================================================================================================
//     Format
// =================================================================================================

std::vector<std::string> VcfHeader::get_format_ids() const
{
    return get_hrec_ids_( BCF_HL_FMT );
}

std::unordered_map<std::string, std::string> VcfHeader::get_format_values( std::string const& id ) const
{
    return get_hrec_values_( BCF_HL_FMT, id );
}

// =================================================================================================
//     Samples
// =================================================================================================

size_t VcfHeader::get_sample_count() const
{
    return header_->n[BCF_DT_SAMPLE];
}

std::vector<std::string> VcfHeader::get_samples() const
{
    size_t sample_count = header_->n[BCF_DT_SAMPLE];
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

std::vector<std::string> VcfHeader::get_hrec_ids_( int type ) const
{
    std::vector<std::string> res;
    for( int i = 0; i < header_->nhrec; ++i ) {
        // We need to scan all hrec entries, and only process the ones we are interested in...
        if( header_->hrec[i]->type != type ) {
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

std::unordered_map<std::string, std::string> VcfHeader::get_hrec_values_( int type, std::string const& id ) const
{
    std::unordered_map<std::string, std::string> res;
    bcf_hrec_t* hrec = ::bcf_hdr_get_hrec( header_, type, "ID", id.c_str(), nullptr );
    for( int i = 0; i < hrec->nkeys; ++i ) {
        res[ std::string( hrec->keys[i] )] = std::string( hrec->vals[i] );
    }
    return res;
}

} // namespace population
} // namespace genesis
