#ifndef GENESIS_POPULATION_FORMATS_VCF_COMMON_H_
#define GENESIS_POPULATION_FORMATS_VCF_COMMON_H_

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

#include <string>
#include <vector>

// extern "C" {
//     #include <htslib/hts.h>
//     #include <htslib/vcf.h>
// }

// =================================================================================================
//     VCF/BCF Common
// =================================================================================================

namespace genesis {
namespace population {

// -------------------------------------------------------------------------
//     Typedefs and Enums
// -------------------------------------------------------------------------

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


// -------------------------------------------------------------------------
//     Typedef and Enum Helpers
// -------------------------------------------------------------------------

std::string vcf_value_type_to_string( VcfValueType type );
std::string vcf_value_type_to_string( int type );
std::string vcf_value_special_to_string( VcfValueSpecial num );
std::string vcf_value_special_to_string( int num );

/**
 * @brief Internal helper function to convert htslib-internal BCF_HL_* header line type values
 * to their string representation as used in the VCF header ("FILTER", "INFO", "FORMAT", etc).
 */
std::string vcf_hl_to_string( int hl_type );

} // namespace population
} // namespace genesis

#endif // include guard
