#ifndef GENESIS_POPULATION_FORMATS_VCF_COMMON_H_
#define GENESIS_POPULATION_FORMATS_VCF_COMMON_H_

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

#include <cstdint>
#include <string>
#include <vector>

#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/genome_region_list.hpp"

extern "C" {
    // #include <htslib/vcf.h>
    // #include <htslib/hts.h>
}

namespace genesis {
namespace population {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

struct Variant;
class VcfRecord;

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
//     Conversion Functions
// =================================================================================================

/**
 * @brief Convert a VcfRecord to a Variant, treating each sample column as a pool of individuals.
 *
 * This assumes that the data that was used to create the VCF file was actually a pool of
 * individuals (e.g., from pool sequencing) for each sample (column) of the VCF file.
 * We do not actually recommend to use variant calling software on pool-seq data, as it induces
 * frequency shifts due to the statistical models employed by variant calles that were not built
 * for pool sequencing data. It however seems to be a commonly used approach, and hence we offer
 * this function here. For this type of data, the VCF allelic depth ("AD") information contains
 * the counts of the reference and alternative base, which in this context can be interpreted as
 * describing the allele frequencines of each pool of individuals.
 * This requires the VCF to have the "AD" FORMAT field.
 *
 * Only SNP data (no indels) are allowed in this function; use VcfRecord::is_snp() to test this.
 *
 * @see See make_variant_input_stream_from_pool_vcf_file() for an example where this is used.
 * @see See convert_to_variant_as_individuals() for the function that instead
 * interprets the VCF as usual as a set of individuals.
 */
Variant convert_to_variant_as_pool(
    VcfRecord const& record
);

/**
 * @brief Convert a VcfRecord to a Variant, treating each sample as an individual,
 * and combining them all into one BaseCounts sample.
 *
 * In this function, we assume that the data that was used to create the VCF file was the typical
 * use case of VCF, where each sample (column) in the file corresponds to an individual.
 * When using this function, all samples (individuals) are combined into one, as our targeted
 * output type Variant is used to describe allele counts of several individual (e.g., in a pool).
 * As all columns are combined, the resulting Variant only contains a single BaseCounts object.
 * We only consider biallelic SNP positions here.
 *
 * We offer two ways of combining the samples (columns) of the input VCF record into the BaseCounts:
 *
 *   1. When @p use_allelic_depth is `false` (default), individuals simply contribute to the
 *      BaseCounts according to their polidy. That is, an individual with genotype `A/T` will
 *      contribute one count each for `A` and `T`.
 *   2. When @p use_allelic_depth is `true` instead, we use the "AD" FORMAT field instead, to
 *      obtain the actual counts for the reference and alterantive allele, and use these to
 *      sum up the BaseCounts data.
 *
 * @see See make_variant_input_stream_from_individual_vcf_file() for an example where this is used.
 * @see See convert_to_variant_as_pool() for the alterantive function that instead
 * interprets each sample (column) as a pool of individuals, e.g., from pool sequencing.
 */
Variant convert_to_variant_as_individuals(
    VcfRecord const& record,
    bool use_allelic_depth = false
);

/**
 * @brief Read a VCF file, and use its positions to create a GenomeLocusSet.
 *
 * This is for example useful to restrict some analysis to the loci of known variants.
 * Note that the whole file has to be read still; it can hence be better to only do this once
 * and convert to a faster file format, such as simple genome region lists, see GenomeRegionReader.
 *
 * This ignores all sample information, and simply uses the `CHROM`	and `POS` data to construct
 * the resulting set. The VCF file does not have to be sorted for this.
 *
 * @see genome_region_list_from_vcf_file()
 */
GenomeLocusSet genome_locus_set_from_vcf_file( std::string const& file );

/**
 * @brief Read a VCF file, and use its positions to create a GenomeRegionList.
 *
 * This is for example useful to restrict some analysis to the loci of known variants;
 * however, for that use case, it is recommended to use genome_locus_set_from_vcf_file() instead,
 * as testing genome coordinate coverage is way faster with that.
 *
 * Note that the whole file has to be read still; it can hence be better to only do this once
 * and convert to a faster file format, such as simple genome region lists, see GenomeRegionReader.
 *
 * This ignores all sample information, and simply uses the `CHROM`	and `POS` data to construct
 * intervals of consecutive positions along the chromsomes, i.e., if the file contains positions
 * `1`, `2`, and `3`, but not `4`, an interval spanning `1-3` is inserted into the list.
 *
 * The VCF file does not have to be sorted for this.
 *
 * @see genome_locus_set_from_vcf_file()
 */
GenomeRegionList genome_region_list_from_vcf_file( std::string const& file );

/**
 * @brief Read a VCF file, and add its positions to an existing GenomeRegionList.
 *
 * @copydetails genome_region_list_from_vcf_file( std::string const& )
 * The regions are merged into the existing ones, potentially changing existing starts and ends
 * of intervals if they overlap with regions found in the VCF.
 */
void genome_region_list_from_vcf_file( std::string const& file, GenomeRegionList& target );

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
