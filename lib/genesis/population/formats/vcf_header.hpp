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

    /**
     * @brief Specification for the data type of the values expected in key-value-pairs.
     */
    enum class ValueType
    {
        kFlag    = 0,
        kInteger = 1,
        kFloat   = 2,
        kString  = 3
    };

    /**
     * @brief Specification for special markers for the number of values expected for key-value-pairs.
     */
    enum class ValueSpecial
    {
        /**
         * @brief Fixed number of values expected. In VCF, this is denoted simply by an integer
         * number.
         *
         * This simply specifies that there is a fixed number of values to be expected;
         * we do not further define how many exaclty are expected here (the integer value).
         * This is taken care of in a separate variable that is provided whenever a fixed-size
         * value is needed, see for example VcfHeader::Specification.
         */
        kFixed = 0,

        /**
        * @brief Variable number of possible values, or unknown or unbounded.
        * In VCF, this is denoted by '.'.
        */
        kVariable = 1,

        /**
         * One value per alternate allele. In VCF, this is denoted as 'A'.
         */
        kAllele = 2,

        /**
        * One value for each possible genotype (more relevant to the FORMAT tags).
        * In VCF, this is denoated as 'G'.
        */
        kGenotype = 3,

        /**
         * One value for each possible allele (including the reference).
         * In VCF, this is denoted as 'R'.
         */
        kReference = 4,
    };

    /**
     * @brief Collect the four required keys that describe an INFO or FORMAT sub-field.
     *
     * We follow the htslib usage of Number, which is stored as two variables: One for the
     * specification of the number of values (is it variable or fixed, etc), and, if fixed,
     * the actual value for the number of expected entries, which we here call the `number`.
     */
    struct Specification
    {
        std::string  id;
        ValueType    type;
        ValueSpecial special;
        int          number;
        std::string  description;
    };

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
     * This copies the header using ::bcf_hdr_dup() from htslib, and then manages the livetime
     * of the newly created `::bcf_hdr_t` instance only.
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
     * @brief Return the internal htslib `::bcf_hdr_t` data struct.
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
     * These correspond to the field entries in the `CHROM` column of the records.
     */
    std::vector<std::string> get_chroms() const;

    /**
     * @brief Get the length of a chromosome/contig/sequence, given its name.
     *
     * This information is potentially stored auto-magically in the htslib struct.
     */
    size_t get_chrom_length( std::string const& chrom_name ) const;

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
    std::unordered_map<std::string, std::string> get_chrom_values( std::string const& chrom_name ) const;

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
    Specification get_info_specification( std::string const& id ) const;

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
    void assert_info( std::string const& id, ValueType type ) const;

    /**
     * @brief Assert that an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    void assert_info( std::string const& id, ValueType type, ValueSpecial special ) const;

    /**
     * @brief Assert that an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    void assert_info( std::string const& id, ValueType type, size_t number ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file.
     */
    bool has_info( std::string const& id ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * and whether its value(s) has/have a specified data type.
     */
    bool has_info( std::string const& id, ValueType type ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    bool has_info( std::string const& id, ValueType type, ValueSpecial special ) const;

    /**
     * @brief Return whether an INFO entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    bool has_info( std::string const& id, ValueType type, size_t number ) const;

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
    Specification get_format_specification( std::string const& id ) const;

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
    void assert_format( std::string const& id, ValueType type ) const;

    /**
     * @brief Assert that an FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    void assert_format( std::string const& id, ValueType type, ValueSpecial special ) const;

    /**
     * @brief Assert that an FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * that its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    void assert_format( std::string const& id, ValueType type, size_t number ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file.
     */
    bool has_format( std::string const& id ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * and whether its value(s) has/have a specified data type.
     */
    bool has_format( std::string const& id, ValueType type ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified special type of number of values.
     *
     * The last check for the kind of number of values is typically used to require one of the special
     * cases (number of values depending on number of alleles, etc).
     */
    bool has_format( std::string const& id, ValueType type, ValueSpecial special ) const;

    /**
     * @brief Return whether a FORMAT entry with a given ID is defined in the header of the VCF/BCF file,
     * whether its value(s) has/have a specified data type, and a specified number of values.
     *
     * This check requires that the number of values is fixed to the given @p number.
     * That is, here, we do not use any of the special cases (number of values depending on number
     * of alleles, etc), but require a fixed given number instead.
     */
    bool has_format( std::string const& id, ValueType type, size_t number ) const;

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

    /**
     * @brief Speficy a subset of samples to be parsed.
     *
     * Only the specified set of sample columns are parsed and used to populate the VcfRecord%s
     * when reading the VCF/BCF file (or, if @p inverse_sample_names is `true`, only the specified
     * samples are excluded). This can yield drastical speedups in parsing large files.
     *
     * This function wraps the bcf_hdr_set_samples() function of htslib, see there for details.
     */
    void set_samples(
        std::vector<std::string> const& sample_names,
        bool inverse_sample_names = false
    );

    // -------------------------------------------------------------------------
    //     Typedef and Enum Helpers
    // -------------------------------------------------------------------------

    static std::string value_type_to_string( ValueType type );
    static std::string value_type_to_string( int type );
    static std::string value_special_to_string( ValueSpecial num );
    static std::string value_special_to_string( int num );

    // -------------------------------------------------------------------------
    //     Internal Helpers
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Convert htslib-internal BCF_HL_* header line type values to their string representation
     * as used in the VCF header ("FILTER", "INFO", "FORMAT", etc).
     */
    std::string hl_to_string_( int hl_type ) const;

    /**
     * @brief Get the ID names for a given hrec header line type
     * (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER).
     */
    std::vector<std::string> get_hrec_ids_( int hl_type ) const;

    /**
     * @brief Get all key-value-pairs for a given header line type
     * (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER) and ID name.
     */
    std::unordered_map<std::string, std::string> get_hrec_values_( int hl_type, std::string const& id ) const;

    /**
     * @brief Get the required key-value-pairs for a given header line type
     * (one of BCF_HL_*, e.g., INFO, FORMAT, or FILTER) and ID name.
     */
    Specification get_specification_( int hl_type, std::string const& id) const;

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
        bool with_type, ValueType type,
        bool with_special, ValueSpecial special,
        bool with_number, size_t number
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    ::bcf_hdr_t* header_ = nullptr;
};

} // namespace population
} // namespace genesis

#endif // include guard
