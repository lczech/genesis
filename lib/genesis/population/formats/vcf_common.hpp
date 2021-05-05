#ifndef GENESIS_POPULATION_FORMATS_VCF_COMMON_H_
#define GENESIS_POPULATION_FORMATS_VCF_COMMON_H_

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

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
    // #include <htslib/vcf.h>
    // #include <htslib/hts.h>
}

namespace genesis {
namespace population {

// =================================================================================================
//     Typedefs and Enums
// =================================================================================================

/**
 * @brief Specification for the values determining header line types of VCF/BCF files.
 *
 * This list contains the types of header lines that htslib uses for identification,
 * as specified in the VCF header.
 * Corresponds to the `BCF_HL_*` macro constants defined by htslib. We statically assert
 * that these have the same values.
 */
enum class VcfHeaderLine : int
{
    kFilter     = 0,
    kInfo       = 1,
    kFormat     = 2,
    kContig     = 3,
    kStructured = 4, // structured header line TAG=<A=..,B=..>
    kGeneric    = 5  // generic header line
};

/**
 * @brief Specification for the data type of the values expected in key-value-pairs of VCF/BCF files.
 *
 * This list contains the types of data in values of the `INFO` and `FORMAT` key-value pairs,
 * as specified in the VCF header, and used in the record lines.
 * Corresponds to the `BCF_HT_*` macro constants defined by htslib. We statically assert
 * that these have the same values.
 */
enum class VcfValueType : int
{
    kFlag    = 0,
    kInteger = 1,
    kFloat   = 2,
    kString  = 3
};

/**
 * @brief Specification for special markers for the number of values expected for key-value-pairs
 * of VCF/BCF files.
 *
 * This list contains the special markers for the number of values of the `INFO` and `FORMAT`
 * key-value pairs, as specified in the VCF header, and used in the record lines.
 * Corresponds to the `BCF_VL_*` macro constants defined by htslib. We statically assert
 * that these have the same values.
 */
enum class VcfValueSpecial : int
{
    /**
     * @brief Fixed number of values expected. In VCF, this is denoted simply by an integer
     * number.
     *
     * This simply specifies that there is a fixed number of values to be expected;
     * we do not further define how many exaclty are expected here (the integer value).
     * This is taken care of in a separate variable that is provided whenever a fixed-size
     * value is needed, see for example VcfSpecification.
     */
    kFixed = 0,

    /**
     * @brief Variable number of possible values, or unknown, or unbounded.
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
 * @brief Collect the four required keys that describe an INFO or FORMAT sub-field  of VCF/BCF files.
 *
 * We follow the htslib usage of Number, which is stored as two variables: One for the
 * special specification of the number of values (is it variable or fixed, etc), and, if fixed,
 * the actual value for the number of expected entries, which we here call the `number`. That number
 * corresponds to the integer value given in the `Number` key of the INFO or FORMAT line.
 */
struct VcfSpecification
{
    std::string     id;
    VcfValueType    type;
    VcfValueSpecial special;
    int             number;
    std::string     description;
};

// =================================================================================================
//     Typedef and Enum Helpers
// =================================================================================================

std::string vcf_value_type_to_string( VcfValueType ht_type );
std::string vcf_value_type_to_string( int ht_type );
std::string vcf_value_special_to_string( VcfValueSpecial vl_type_num );
std::string vcf_value_special_to_string( int vl_type_num );

/**
 * @brief Internal helper function to convert htslib-internal BCF_HL_* header line type values
 * to their string representation as used in the VCF header ("FILTER", "INFO", "FORMAT", etc).
 */
std::string vcf_hl_type_to_string( int hl_type );

// =================================================================================================
//     VCF Genotype Functions
// =================================================================================================

// Forward declare.
class VcfGenotype;

/**
 * @brief Return the VCF-like string representation of a set of VcfGenotype entries.
 *
 * The VcfFormatIterator::get_values() function returns all genotype entries for a
 * given sample of a record/line. Here, we return a string representation similar to VCF of these
 * genotypes, for example `0|0` or `./1`.
 */
std::string vcf_genotype_string( std::vector<VcfGenotype> const& genotypes );

/**
 * @brief Return the sum of genotypes for a set of VcfGenotype entries, typically used to
 * construct a genotype matrix with entries 0,1,2.
 *
 * The function takes the given @p genotypes, encodes the reference as 0 and any alternative as 1,
 * and then sums this over the values. For diploid organisms, this yields possible results in
 * the range of 0 (homozygote for the reference), 1 (heterzygote), or 2 (homozygote for the
 * alternative), which is typically used in genotype matrices.
 */
size_t vcf_genotype_sum( std::vector<VcfGenotype> const& genotypes );

// =================================================================================================
//     VCF Genotype
// =================================================================================================

/**
 * @brief Simple wrapper class for one genotype field for a sample.
 *
 * The FORMAT `GT` genotype field in VCF contains entries such as
 *
 *   * `0/1` to mark an unphased diploid genotype call, where one allele is the reference (index `0`),
 *     and on alelle is the first alternative (index `1`) in the `ALT` column of the record line.
 *   * `2|1` to mark a phased diploid genotype call, where the first phased allele is the second
 *     (index `2`) alternative allele of the `ALT` column, and the second phased allele is the first
 *     (index `1`) alternative allele.
 *
 * Here, we model exactly one such entry, that is, one such index. For diploid calls, a sample hence
 * needs two such values (two instances of this class); for triploic calls, three instances, and so
 * forth.
 *
 * For simplicity, the VcfFormatIterator::get_values() function returns all genotype entries for a
 * given sample of a record/line. This can for example be used with vcf_genotype_string() to get
 * a similar string representation of all called genotypes of a record than VCF uses.
 *
 * The function variant_index() returns the index of the referenec (`0`) or alternative allele (`>0`)
 * as exemplified above. The corresponding allele can be obtained via VcfRecord::get_variant()
 * and VcfRecord::get_variants().
 */
class VcfGenotype
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    explicit VcfGenotype( int32_t genotype )
        : genotype_(genotype)
    {}

    ~VcfGenotype() = default;

    VcfGenotype( VcfGenotype const& ) = default;
    VcfGenotype( VcfGenotype&& )      = default;

    VcfGenotype& operator= ( VcfGenotype const& ) = default;
    VcfGenotype& operator= ( VcfGenotype&& )      = default;

    // -------------------------------------------------------------------------
    //     Access Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return the index of the variant set for this genotype call.
     *
     * This corresponds to the indices of the result of VcfRecord::get_variants(). If the call
     * is missing (`.` in VCF), `-1` is returned instead.
     */
    int32_t variant_index() const;

    /**
     * @brief True iff the called variant of this genotype is the `REF` allele.
     *
     * In VCF, this corresponds to an entry of `0`.
     */
    bool is_reference() const;

    /**
     * @brief True iff the called variant of this genotype is not the `REF`, but one of the `ALT`
     * alleles.
     *
     * In VCF, this corresponds to any integer entry `> 0`.
     */
    bool is_alternative() const;

    /**
     * @brief True iff the variant call is missing for this genotype.
     *
     * In VCF, this corresponds to an entry of `.`.
     */
    bool is_missing() const;

    /**
     * @brief True iff the called variant is phased.
     *
     * Note that in the VCF specification 4.2 and htslib this seems to be handled differently:
     * While the former required every genotype call to be marked as phased/unphased, the htslib
     * implementation does not set the phased information for the first genotype call.
     * Hence, if the `GT` field of a sample is `0|1|1`, only the second and the third genotype call
     * will return `true` in htslib using their `bcf_gt_is_phased()` macro. We here currently do
     * not fix this, and hence propagate the htslib behaviour.
     */
    bool is_phased() const;

    /**
     * @brief Return the raw genotype value as used by htslib.
     */
    int32_t data() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    int32_t genotype_;
};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
