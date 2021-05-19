#ifndef GENESIS_POPULATION_FORMATS_VCF_HEADER_H_
#define GENESIS_POPULATION_FORMATS_VCF_HEADER_H_

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

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_common.hpp"

#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
    // #include <htslib/hts.h>
    // #include <htslib/vcf.h>

    struct bcf_hdr_t;
}

namespace genesis {
namespace population {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class VcfRecord;

template< typename S, typename T >
class VcfFormatIterator;

// =================================================================================================
//     VCF/BCF Header
// =================================================================================================

/**
 * @brief Capture the information from a header of a VCF/BCF file.
 *
 * Unfortunately, the terminology used in htslib and in the
 * [VCF v4.2 specification](http://samtools.github.io/hts-specs/VCFv4.2.pdf) to describe different
 * parts of a VCF file seems diffuse; in particular, the words "ID", "key", and "tag" are overloaded,
 * as well as "column", "field", and "sub-field", and the usage of "type" for almost anything.
 *
 * Given the excerpt from a VCF file
 *
 *     ##INFO=<ID=NS,Number=1,Type=Integer,Description="Number of Samples With Data">
 *     ...
 *     #CHROM POS   ID        REF ALT QUAL FILTER INFO                    FORMAT      NA00001
 *     20     14370 rs6054257 G   A   29   PASS   NS=3;DP=14;AF=0.5;DB;H2 GT:GQ:DP:HQ 0|0:48:1:51,51
 *
 * let's try to untangle the terminology. We mark the terms used in our wrapper classes in **bold**.
 *
 *   * The main body of a VCF file consists of 8 tab-separated fixed **columns** per record:
 *     "CHROM", "POS", "ID", "REF", "ALT", "QUAL", "FILTER", "INFO", as well as an optional "FORMAT"
 *     column followed by an arbitrary number of sample columns ("NA00001" in the example above).
 *   * Each record line consists of **fields** that correspond to these columns; that is,
 *     a field is the particular entry for a given column and a given record line.
 *   * Some fields (typically, corresponding to the columns "INFO", "FILTER", and "FORMAT") are
 *     further divided into sub-fields (also called "fields" in the VCF standard) that are
 *     identified by their **ID** (not to be confused with the field/column "ID", which just happens
 *     to have the same name for maximum confuson).
 *   * Some of these sub-fields furthermore form **key**-**value** pairs, where the key is given by an ID;
 *     in the example above, "NS" is the ID of an INFO sub-field, and contains the key-value-pair
 *     "NS=3" in the given record line.
 *     IDs can however also be simple **flags**, in which case they are not followed by a value.
 *     In htslib, the ID/key of a sub-field/field is also sometimes calld the `tag`.
 *   * We are aware that we here use ID and key as two terms for a similar thing. This is our attempt
 *     to bridge the gap between intuition and htslib: an ID here is used as a key for a key-value-pair.
 *
 * Hopefully, this helps to make sense of the terminology.
 */
class VcfHeader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    // VcfRecord and VcfFormatIterator both need access to some htslib functions/definitions
    // for which we'd have to include htslib headers in our headers, which we want to avoid.
    // So instead, we wrap htslib internal functions here.
    // We could make those free functions, but it seems too specialzied for general usage,
    // so let's keep it in private scope for now, and make the classes friends instead.

    friend class VcfRecord;

    template< typename S, typename T >
    friend class VcfFormatIterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default (empty) instance.
     */
    VcfHeader() = default;

    // /* *
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
     * For the @p mode param, see the <code>\::hts_open()</code> documentation of htslib.
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
     * This copies the header using <code>\::bcf_hdr_dup()</code> from htslib, and then manages the
     * livetime of the newly created <code>\::bcf_hdr_t</code> instance only.
     */
    explicit VcfHeader( ::bcf_hdr_t* bcf_hdr );

    ~VcfHeader();

    VcfHeader( VcfHeader const& ) = delete;
    VcfHeader( VcfHeader&& other );

    VcfHeader& operator= ( VcfHeader const& ) = delete;
    VcfHeader& operator= ( VcfHeader&& other );

    // -------------------------------------------------------------------------
    //     General Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the internal htslib <code>\::bcf_hdr_t</code> data struct pointer.
     */
    ::bcf_hdr_t* data()
    {
        return header_;
    }

    /**
     * @brief Return the internal htslib <code>\::bcf_hdr_t</code> data struct pointer.
     */
    ::bcf_hdr_t const* data() const
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
     * These correspond to the field entries in the `CHROM` column of the records.
     */
    std::vector<std::string> get_chromosomes() const;

    /**
     * @brief Get the length of a chromosome/contig/sequence, given its name.
     *
     * This information is potentially stored auto-magically in the htslib struct.
     */
    size_t get_chromosome_length( std::string const& chrom_name ) const;

    /**
     * @brief Get all key-value-pairs describing a particular chromosome/contig/sequence, given its name.
     *
     * For example, if the header contains a line
     *
     *     ##contig=<ID=20,length=62435964,assembly=B36,md5=f126cdf8a6e0c7f379d618ff66beb2da,species="Homo sapiens",taxonomy=x>
     *
     * then the result will contain entries that map from "ID" to "20", from "length" to "62435964",
     * and so forth. (Note that ID is also included in the result, for simplicity.)
     */
    std::unordered_map<std::string, std::string> get_chromosome_values( std::string const& chrom_name ) const;

    // -------------------------------------------------------------------------
    //     Filter
    // -------------------------------------------------------------------------

    /**
     * @brief Get a list of the ID names of all `FILTER` entries in the header.
     *
     * For example, if the header contains a line
     *
     *     ##FILTER=<ID=q10,Description="Quality below 10">
     *
     * then the list contains an entry `q10`.
     */
    std::vector<std::string> get_filter_ids() const;

    /**
     * @brief Get all key-value pairs describing a particular filter header line, given its ID.
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

    /**
     * @brief Assert that a FILTER entry with a given ID is defined in the header of the VCF/BCF file.
     */
    void assert_filter( std::string const& id ) const;

    /**
     * @brief Return whether a FILTER entry with a given ID is defined in the header of the VCF/BCF file.
     */
    bool has_filter( std::string const& id ) const;

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
     * @brief Get the required specification key-value-pairs for a given INFO entry.
     *
     * See also get_info_values() for a function that returns all given key-value-paris
     * of the INFO entry.
     */
    VcfSpecification get_info_specification( std::string const& id ) const;

    /**
     * @brief Get all key-value pairs describing a particular info header line, given its ID.
     *
     * For example, if the header contains a line
     *
     *     ##INFO=<ID=DP,Number=1,Type=Integer,Description="Total Depth">
     *
     * then the result will contain entries that map from "ID" to "DP",
     * from "Number" to "1", and so forth. (Note that ID is also included in the result,
     * for simplicity.)
     *
     * See also get_info_specification() for a function that returns only the required
     * key-value-pairs for INFO entries.
     */
    std::unordered_map<std::string, std::string> get_info_values( std::string const& id ) const;

    /**
     * @brief Assert that an INFO entry with a given ID is defined in the header of the VCF/BCF file.
     */
    void assert_info( std::string const& id ) const;

    /**
     * @brief Assert that an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * and that its value(s) has/have a specified data type.
     */
    void assert_info( std::string const& id, VcfValueType type ) const;

    /**
     * @brief Assert that an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    void assert_info( std::string const& id, VcfValueType type, VcfValueSpecial special ) const;

    /**
     * @brief Assert that an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    void assert_info( std::string const& id, VcfValueType type, size_t number ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file.
     */
    bool has_info( std::string const& id ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * and whether its value(s) has/have a specified data type.
     */
    bool has_info( std::string const& id, VcfValueType type ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    bool has_info( std::string const& id, VcfValueType type, VcfValueSpecial special ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    bool has_info( std::string const& id, VcfValueType type, size_t number ) const;

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
     * @brief Get the required specification key-value-pairs for a given FORMAT entry.
     *
     * See also get_format_values() for a function that returns all given key-value-paris
     * of the FORMAT entry.
     */
    VcfSpecification get_format_specification( std::string const& id ) const;

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
     *
     * See also get_format_specification() for a function that returns only the required
     * key-value-pairs for FORMAT entries.
     */
    std::unordered_map<std::string, std::string> get_format_values( std::string const& id ) const;

    /**
     * @brief Assert that an FORMAT entry with a given ID is defined in the header of the VCF/BCF file.
     */
    void assert_format( std::string const& id ) const;

    /**
     * @brief Assert that an FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * and that its value(s) has/have a specified data type.
     */
    void assert_format( std::string const& id, VcfValueType type ) const;

    /**
     * @brief Assert that an FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    void assert_format( std::string const& id, VcfValueType type, VcfValueSpecial special ) const;

    /**
     * @brief Assert that an FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    void assert_format( std::string const& id, VcfValueType type, size_t number ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file.
     */
    bool has_format( std::string const& id ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * and whether its value(s) has/have a specified data type.
     */
    bool has_format( std::string const& id, VcfValueType type ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    bool has_format( std::string const& id, VcfValueType type, VcfValueSpecial special ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    bool has_format( std::string const& id, VcfValueType type, size_t number ) const;

    // -------------------------------------------------------------------------
    //     Samples
    // -------------------------------------------------------------------------

    /**
     * @brief Get the number of samples (columns) in the file.
     */
    size_t get_sample_count() const;

    /**
     * @brief Get the name of a sample given its index.
     *
     * This corresponds to the name given in the <code>\#CHROM</code> line of the VCF file,
     * using indices in the range `[ 0, get_sample_count() )`.
     */
    std::string get_sample_name( size_t index ) const;

    /**
     * @brief Get the index of a sample, given its name.
     *
     * If the sample @p name does not exist in the VCF file, an exception is thrown.
     */
    size_t get_sample_index( std::string const& name ) const;

    /**
     * @brief Return a list of the sample names (column headers) of the VCF/BCF file.
     *
     * These are the names that correspond to the column headers for samples in the
     * <code>\#CHROM</code> line of a VCF file.
     */
    std::vector<std::string> get_sample_names() const;

    /**
     * @brief Speficy a subset of samples to be parsed.
     *
     * Only the specified set of sample columns are parsed and used to populate the VcfRecord%s
     * when reading the VCF/BCF file (or, if @p inverse_sample_names is `true`, only the specified
     * samples are excluded). This can yield drastical speedups in parsing large files.
     *
     * This function wraps the <code>bcf_hdr_set_samples()</code> function of htslib,
     * see there for details.
     */
    void set_samples(
        std::vector<std::string> const& sample_names,
        bool inverse_sample_names = false
    );

    // -------------------------------------------------------------------------
    //     Internal Helpers
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Get the ID names for a given hrec header line type
     * (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER).
     */
    std::vector<std::string> get_hrec_ids_( int hl_type ) const;

    /**
     * @brief Get all key-value-pairs for a given header line type
     * (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER) and ID name.
     */
    std::unordered_map<std::string, std::string> get_hrec_values_(
        int hl_type, std::string const& id
    ) const;

    /**
     * @brief Get the required key-value-pairs for a given header line type
     * (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER) and ID name.
     */
    VcfSpecification get_specification_( int hl_type, std::string const& id) const;

    /**
     * @brief Test whether a particualar header line entry exists and follows the specifications given.
     *
     * This function is used for the assert_*() and has_*() functions, and hence serves a dual
     * purpose, to avoid code duplication: If the entry is not following the specifications,
     * an exception is thrown, or a bool is returned, depending on the setting of @p throwing.
     *
     * For this, we use the header line type (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER) and
     * ID name to test whether that ID exists in the given header line.
     * Furthermore, we can test whether it also has a particular data type and number of data entries.
     * As this is an internal function, we use a quick-and-dirty approach here, and use bool flags
     * to specify whether we want to test these as well.
     */
    bool test_hl_entry_(
        bool throwing,
        int hl_type, std::string const& id,
        bool with_type, VcfValueType type,
        bool with_special, VcfValueSpecial special,
        bool with_number, size_t number
    ) const;

    /**
     * @brief Internal helper function to check the result of the htslib function
     * bcf_get_info_values() and bcf_get_format_values(), and report errors if necessary.
     *
     * Both these functions yield a @p return_value where negative values encode for errors,
     * and positive numbers correspond to the actual number of entries returned, that is, the number
     * of values of the INFO or FORMAT fields of the record. Here, we check this @p return_value,
     * and throw an exception with a meaningful error message if negative.
     *
     * For this, we need the @p id (the two-letter combination of the key-value pair of the INFO or
     * FORMAT field), as well as the @p ht_type (one of BCF_HT_*: FLAG, INT, REAL, STR - don't ask
     * why here it's suddently called REAL and not FLOAT in htslib... we don't know either),
     * and the @p hl_type (one of BCF_HL_*: here, INFO or FMT). The latter are needed to create
     * fitting error messages.
     *
     * The function is actually not needed for the header itself, but only used by VcfRecord and
     * VcfFormatIterator. It however seems too specialized to our purpose to make it a free function.
     * So we keep it here and make both classes friends, so that they have access to it.
     */
    static void check_value_return_code_(
        ::bcf_hdr_t* header, std::string const& id, int ht_type, int hl_type, int return_value
    );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    ::bcf_hdr_t* header_ = nullptr;
};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
