#ifndef GENESIS_POPULATION_FUNCTIONS_FST_POOL_KARLSSON_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_POOL_KARLSSON_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/population/functions/fst_pool_calculator.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/compensated_sum.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Pool Calculator Karlsson
// =================================================================================================

/**
 * @brief Compute the F_ST statistic for pool-sequenced data of Karlsson et al
 * as used in PoPoolation2, for two ranges of BaseCounts%s.
 *
 * The approach is called the "asymptotically unbiased" estimator in PoPoolation2 [1],
 * and follows Karlsson et al [2].
 *
 * > [1] **PoPoolation2: identifying differentiation between populations
 * > using sequencing of pooled DNA samples (Pool-Seq).**<br />
 * > Kofler R, Pandey RV, Schlotterer C.<br />
 * > Bioinformatics, 2011, 27(24), 3435–3436. https://doi.org/10.1093/bioinformatics/btr589
 *
 * > [2] **Efficient mapping of mendelian traits in dogs through genome-wide association.**<br />
 * > Karlsson EK, Baranowska I, Wade CM, Salmon Hillbertz NHC, Zody MC, Anderson N, Biagi TM,
 * > Patterson N, Pielberg GR, Kulbokas EJ, Comstock KE, Keller ET, Mesirov JP, Von Euler H,
 * > Kämpe O, Hedhammar Å, Lander ES, Andersson G, Andersson L, Lindblad-Toh K.<br />
 * > Nature Genetics, 2007, 39(11), 1321–1328. https://doi.org/10.1038/ng.2007.10
 */
class FstPoolCalculatorKarlsson final : public BaseFstPoolCalculator
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FstPoolCalculatorKarlsson() = default;

    /**
     * @brief Constructor that takes dummy pool sizes that are not used. Offered to have the same
     * interface as the other FstPoolCalculator classes.
     */
    FstPoolCalculatorKarlsson( size_t p1_poolsize, size_t p2_poolsize )
    {
        (void) p1_poolsize;
        (void) p2_poolsize;
    }

    ~FstPoolCalculatorKarlsson() = default;

    FstPoolCalculatorKarlsson( FstPoolCalculatorKarlsson const& ) = default;
    FstPoolCalculatorKarlsson( FstPoolCalculatorKarlsson&& )      = default;

    FstPoolCalculatorKarlsson& operator= ( FstPoolCalculatorKarlsson const& ) = default;
    FstPoolCalculatorKarlsson& operator= ( FstPoolCalculatorKarlsson&& )      = default;

    // -------------------------------------------------------------------------
    //     Abstract Members
    // -------------------------------------------------------------------------

private:

    void reset_() override
    {
        sum_nk_ = 0.0;
        sum_dk_ = 0.0;
    }

    void process_( BaseCounts const& p1, BaseCounts const& p2 ) override
    {
        // Get intermediate values and add them up.
        auto const counts = sorted_average_base_counts( p1, p2 );
        auto const nkdk = f_st_pool_karlsson_nkdk( counts );
        sum_nk_ += nkdk.first;
        sum_dk_ += nkdk.second;
    }

    double get_result_() override
    {
        return static_cast<double>( sum_nk_ ) / static_cast<double>( sum_dk_ );
    }

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Compute the numerator `N_k` and denominator `D_k`  needed for the asymptotically
     * unbiased F_ST estimator of Karlsson et al (2007).
     *
     * See f_st_pool_karlsson() for details. The function expects sorted base counts for the
     * two samples of which we want to compute F_ST, which are produced by sorted_average_base_counts().
     */
    static std::pair<double, double> f_st_pool_karlsson_nkdk(
        std::pair<SortedBaseCounts, SortedBaseCounts> const& sample_counts
    ) {
        // PoPoolation2 function: calculate_nk_dk
        using namespace genesis::utils;

        // Error check. We only want biallelic SNPs, so we check that the smallest two values
        // here are actually zero.
        // Update: That does not quite work corrctly if we want to filter by min counts,
        // in which cases we might have minor third and fourth alleles that are below the min count.
        // We can safely ignore this here, as those are not taken into account below anyway.
        // This is also now how PoPoolation2 handles this situation.
        // if(
        //     sample_counts.first[2].count  != 0 || sample_counts.first[3].count  != 0 ||
        //     sample_counts.second[2].count != 0 || sample_counts.second[3].count != 0
        // ) {
        //     throw std::runtime_error(
        //         "In f_st_pool_karlsson(): Expecting biallelic SNPs where only "
        //         "two nucleotide counts are > 0, but found more non-zero counts."
        //     );
        // }
        // assert( sample_counts.first[2].count  == 0 && sample_counts.first[3].count  == 0 );
        // assert( sample_counts.second[2].count == 0 && sample_counts.second[3].count == 0 );

        // Assert that the bases are in the same order in both samples.
        assert(
            sample_counts.first[0].base == sample_counts.second[0].base &&
            sample_counts.first[1].base == sample_counts.second[1].base &&
            sample_counts.first[2].base == sample_counts.second[2].base &&
            sample_counts.first[3].base == sample_counts.second[3].base
        );

        // Get the major allele count (`a` here and in PoPoolation2),
        // the minor allele count (`b` here, not used in PoPoolation2 under that name),
        // and the total coverage (`n` here and in PoPoolation2).
        auto const a_1 = static_cast<double>( sample_counts.first[0].count );
        auto const b_1 = static_cast<double>( sample_counts.first[1].count );
        auto const n_1 = a_1 + b_1;
        auto const a_2 = static_cast<double>( sample_counts.second[0].count );
        auto const b_2 = static_cast<double>( sample_counts.second[1].count );
        auto const n_2 = a_2 + b_2;

        // Edge case
        if( n_1 <= 1.0 || n_2 <= 1.0 ) {
            return { 0.0, 0.0 };
        }
        assert( n_1 > 1.0 );
        assert( n_2 > 1.0 );
        assert( a_1 <= n_1 );
        assert( a_2 <= n_2 );

        // PoPoolation2 functions: calculate_h1, calculate_h2
        double const h1 = ( a_1 * b_1 ) / ( n_1 * ( n_1 - 1.0 ));
        double const h2 = ( a_2 * b_2 ) / ( n_2 * ( n_2 - 1.0 ));

        // PoPoolation2 function: calculate_nk_dk
        double const sqr = squared(( a_1 / n_1 ) - ( a_2 / n_2 ));
        double const sub = ( h1 / n_1 + h2 / n_2 );
        double const nk = sqr - sub;
        double const dk = nk + h1 + h2;

        return { nk, dk };
    }

    // -------------------------------------------------------------------------
    //     Private Member Variables
    // -------------------------------------------------------------------------

private:

    // Result values.
    utils::NeumaierSum sum_nk_ = 0.0;
    utils::NeumaierSum sum_dk_ = 0.0;

};

} // namespace population
} // namespace genesis

#endif // include guard
