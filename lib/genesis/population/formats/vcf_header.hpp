#ifndef GENESIS_POPULATION_FORMATS_VCF_HEADER_H_
#define GENESIS_POPULATION_FORMATS_VCF_HEADER_H_

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

#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

// =================================================================================================
//     VCF/BCF Header
// =================================================================================================

namespace genesis {
namespace population {

/**
 * @brief Capture the information from a header of a VCF/BCF file.
 */
class VcfHeader
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default (empty) instance.
     */
    VcfHeader() = default;

    // /**
    //  * @brief Create an instance that reads from an input file name.
    //  *
    //  * The header is read from the file, which is subsequently closed again. Hence, this
    //  * cannot be used to further operate on that file. It is merely meant for cases where
    //  * quick access to the VCF header information is needed, without parsing its body.
    //  */
    // explicit VcfHeader( std::string const& file_name );

    /**
     * @brief Create an instance with a specific mode.
     *
     * For the @p mode param, see the ::hts_open() documentation of htslib.
     */
    explicit VcfHeader( std::string const& mode );

    /**
     * @brief Create an instance given an HtsFile object.
     *
     * The HtsFile has to be newly created and cannot have been used to read from before,
     * otherwise its internal file pointer is already past the header.
     */
    explicit VcfHeader( HtsFile& hts_file );

    /**
     * @brief Create an instance given a pointer to the htslib-internal header struct.
     *
     * This copies the headers using ::bcf_hdr_dup() from htslib.
     */
    explicit VcfHeader( ::bcf_hdr_t* bcf_hdr );

    ~VcfHeader();

    VcfHeader( VcfHeader const& ) = delete;
    VcfHeader( VcfHeader&& )      = default;

    VcfHeader& operator= ( VcfHeader const& ) = delete;
    VcfHeader& operator= ( VcfHeader&& )      = default;

    // -------------------------------------------------------------------------
    //     General Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the internal htslib data struct.
     */
    ::bcf_hdr_t* data()
    {
        return header_;
    }

    /**
     * @brief Return the VCF/BCF version string, e.g. "VCFv4.2".
     */
    std::string version() const;

    // -------------------------------------------------------------------------
    //     Chromosomes / Contigs / Sequences
    // -------------------------------------------------------------------------

    /**
     * @brief Get a list of the chromosome/contig/sequence names used in the file.
     *
     * These correspond to the names of the entries in the `CHROM` column of the records.
     */
    std::vector<std::string> get_chroms() const;

    /**
     * @brief Get the ID of a chromosome/contig/sequence, given its name.
     */
    size_t get_chrom_id( std::string const& chrom_name ) const;

    /**
     * @brief Get the name of a chromosome/contig/sequence, given its ID.
     */
    std::string get_chrom_name( size_t chrom_id ) const;

    /**
     * @brief Get the length of a chromosome/contig/sequence, given its name.
     */
    size_t get_chrom_length( std::string const& chrom_name ) const;

    /**
     * @brief Get the length of a chromosome/contig/sequence, given its ID.
     */
    size_t get_chrom_length( size_t chrom_id ) const;

    /**
     * @brief Get all key-value paris describing a particular chromosome/contig/sequence, given its name.
     *
     * For example, if the header contains a line
     *
     *     ##contig=<ID=20,length=62435964,assembly=B36,md5=f126cdf8a6e0c7f379d618ff66beb2da,species="Homo sapiens",taxonomy=x>
     *
     * then the result will contain entries that map from "ID" to "20", from "length" to "62435964",
     * and so forth. (Note that ID is also included in the result, for simplicity.)
     */
    std::unordered_map<std::string, std::string> get_chrom_values( std::string const& chrom_name ) const;

    // -------------------------------------------------------------------------
    //     Filter
    // -------------------------------------------------------------------------

    /**
     * @brief Get a list of the ID names of all `FILTER` fields in the header.
     *
     * For example, if the header contains a line
     *
     *     ##FILTER=<ID=q10,Description="Quality below 10">
     *
     * then the list contains an entry `q10`.
     */
    std::vector<std::string> get_filter_ids() const;

    /**
     * @brief Get all key-value pairs describing a particular filter field, given its ID.
     *
     * For example, if the header contains a line
     *
     *     ##FILTER=<ID=q10,Description="Quality below 10">
     *
     * then the result will contain entries that map from "ID" to "q10",
     * and from "Description" to "Quality below 10". (Note that ID is also included in the result,
     * for simplicity.)
     */
    std::unordered_map<std::string, std::string> get_filter_values( std::string const& id ) const;

    // -------------------------------------------------------------------------
    //     Info
    // -------------------------------------------------------------------------

    /**
     * @brief Get a list of the ID names of all `INFO` fields in the header.
     *
     * For example, if the header contains a line
     *
     *     ##INFO=<ID=DP,Number=1,Type=Integer,Description="Total Depth">
     *
     * then the list contains an entry `DP`.
     */
    std::vector<std::string> get_info_ids() const;

    /**
     * @brief Get all key-value pairs describing a particular info field, given its ID.
     *
     * For example, if the header contains a line
     *
     *     ##INFO=<ID=DP,Number=1,Type=Integer,Description="Total Depth">
     *
     * then the result will contain entries that map from "ID" to "DP",
     * from "Number" to "1", and so forth. (Note that ID is also included in the result,
     * for simplicity.)
     */
    std::unordered_map<std::string, std::string> get_info_values( std::string const& id ) const;

    // -------------------------------------------------------------------------
    //     Format
    // -------------------------------------------------------------------------

    /**
     * @brief Get a list of the ID names of all `FORMAT` fields in the header.
     *
     * For example, if the header contains a line
     *
     *     ##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
     *
     * then the list contains an entry `GT`.
     */
    std::vector<std::string> get_format_ids() const;

    /**
     * @brief Get all key-value pairs describing a particular format field, given its ID.
     *
     * For example, if the header contains a line
     *
     *     ##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">
     *
     * then the result will contain entries that map from "ID" to "GT",
     * from "Number" to "1", and so forth. (Note that ID is also included in the result,
     * for simplicity.)
     */
    std::unordered_map<std::string, std::string> get_format_values( std::string const& id ) const;

    // -------------------------------------------------------------------------
    //     Samples
    // -------------------------------------------------------------------------

    /**
     * @brief Get the number of samples (columns) in the file.
     */
    size_t get_sample_count() const;

    /**
     * @brief Return a list of the sample names (column headers) of the VCF/BCF file.
     */
    std::vector<std::string> get_samples() const;

    void set_samples(
        std::vector<std::string> const& sample_names,
        bool inverse_sample_names = false
    );

    // -------------------------------------------------------------------------
    //     Internal Helpers
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Get the ID names for a given hrec type, e.g., INFO, FORMAT, or FILTER.
     */
    std::vector<std::string> get_hrec_ids_( int type ) const;

    /**
     * @brief Get the key value pairs for a given type (e.g., INFO, FORMAT, or FILTER) and ID name.
     */
    std::unordered_map<std::string, std::string> get_hrec_values_( int type, std::string const& id ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    ::bcf_hdr_t* header_ = nullptr;
};

} // namespace population
} // namespace genesis

#endif // include guard
