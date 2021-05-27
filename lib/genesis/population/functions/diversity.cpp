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

#include "genesis/population/functions/diversity.hpp"

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/function_cache.hpp"
#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <cmath>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace population {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

/**
 * @brief Local helper function to compute values for the denominator.
 */
double amnm_( // get_aMnm_buffer
    size_t poolsize,         // n
    size_t nucleotide_count, // M (coverage)
    size_t allele_frequency  // m, m_it (running variable for b .. M-b)
) {
    // The terminology in PoPoolation is confusing and differs completely from the paper,
    // their code is not well documented, and their binomial_term function combines other aspects
    // of the computation than just computing the binomial distribution,
    // hence adding to the confusion. Let's try to disentangle:
    //
    // It seems, we want a binomial distribution with n being the coverage/nucleotide_count/M,
    // and k being the allele_frequency/m, and we want p being r (1..poolsize-1) divided by poolsize,
    // using the r from the below loop (which confusingly is also called k in PoPoolation).
    // What a mess.

    static genesis::utils::FunctionCache<double, size_t, size_t, size_t> amnm_cache_{ [](
        size_t poolsize, size_t nucleotide_count, size_t allele_frequency
    ) {
        double result = 0.0;

        // #pragma omp parallel for
        for( size_t r = 1; r <= poolsize - 1; ++r ) {
            double const p = static_cast<double>( r ) / static_cast<double>( poolsize );

            // We use the "lenient" flag of the binomial distribution, so that values of
            // nucleotide_count = n > 1024 do not throw an exception, but return infinity instead.
            // That is okay, because this will lead to the theta denominator being infinity as well,
            // which is then inverted, so it becomes 0, and then added to the total theta of the
            // window. So, it just vanishes in that case, which is okay.
            double const binom = utils::binomial_distribution(
                allele_frequency, nucleotide_count, p, true
            );
            double const partial = binom / static_cast<double>( r );

            // #pragma omp atomic
            result += partial;
        }
        return result;
    }};

    return amnm_cache_( poolsize, nucleotide_count, allele_frequency );
}

// =================================================================================================
//     Diversity Estimates
// =================================================================================================

double heterozygosity( BaseCounts const& sample )
{
    using namespace genesis::utils;

    double h = 1.0;
    double const nt_cnt = static_cast<double>( nucleotide_sum( sample ));
    h -= squared( static_cast<double>( sample.a_count ) / nt_cnt );
    h -= squared( static_cast<double>( sample.c_count ) / nt_cnt );
    h -= squared( static_cast<double>( sample.g_count ) / nt_cnt );
    h -= squared( static_cast<double>( sample.t_count ) / nt_cnt );

    // Slighly slower version with powers.
    // h -= std::pow( static_cast<double>( sample.a_count ) / nt_cnt, 2 );
    // h -= std::pow( static_cast<double>( sample.c_count ) / nt_cnt, 2 );
    // h -= std::pow( static_cast<double>( sample.g_count ) / nt_cnt, 2 );
    // h -= std::pow( static_cast<double>( sample.t_count ) / nt_cnt, 2 );
    h *= nt_cnt / ( nt_cnt - 1.0 );
    return h;
}

// =================================================================================================
//     Theta Pi
// =================================================================================================

double theta_pi_pool_denominator(
    PoolDiversitySettings const& settings,
    size_t nucleotide_count  // M
) {
    // PoPoolation variable names:
    // poolsize:         n
    // min_allele_count: b
    // nucleotide_count: M

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t, size_t, size_t> denom_cache_{ [](
        size_t poolsize, size_t min_allele_count, size_t nucleotide_count
    ){
        // Boundary: if not held, we'd return zero, and that would not be a useful denominator.
        if( 2 * min_allele_count > nucleotide_count ) {
            throw std::invalid_argument(
                "Cannot compute theta_pi_pool_denominator with min_allele_count = " +
                std::to_string( min_allele_count ) + " and nucleotide_count = " +
                std::to_string( nucleotide_count )
            );
        }

        // Iterate all allele frequencies in between the min and max-min boundaries.
        double div = 0.0;

        #pragma omp parallel for
        for( size_t m_it = min_allele_count; m_it <= ( nucleotide_count - min_allele_count ); ++m_it ) {
            // We iterate from b to M-b (in PoPoolation terminology), inclusively.
            // Use double values however for the computations.
            double const m = static_cast<double>( m_it );
            double const M = static_cast<double>( nucleotide_count );

            // Compute the term. We here use the cache, which also computes results if not yet cached.
            double const term = ( 2.0 * m * ( M - m )) / ( M * ( M - 1.0 ));
            double const partial = term * amnm_( poolsize, nucleotide_count, m_it );

            #pragma omp atomic
            div += partial;
        }
        return div;
    }};

    // Simply return the cached value (which computes them first if not yet cached).
    return denom_cache_( settings.poolsize, settings.min_allele_count, nucleotide_count );
}

// =================================================================================================
//     Theta Watterson
// =================================================================================================

double theta_watterson_pool_denominator(
    PoolDiversitySettings const& settings,
    size_t nucleotide_count  // M
) {
    // PoPoolation variable names:
    // poolsize:         n
    // min_allele_count: b
    // nucleotide_count: M

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t, size_t, size_t> denom_cache_{ [](
        size_t poolsize, size_t min_allele_count, size_t nucleotide_count
    ){
        // Boundary: if not held, we'd return zero, and that would not be a useful denominator.
        if( 2 * min_allele_count > nucleotide_count ) {
            throw std::invalid_argument(
                "Cannot compute theta_watterson_pool_denominator with min_allele_count = " +
                std::to_string( min_allele_count ) + " and nucleotide_count = " +
                std::to_string( nucleotide_count )
            );
        }

        // Iterate all allele frequencies in between the min and max-min boundaries.
        double div = 0.0;

        #pragma omp parallel for
        for( size_t m_it = min_allele_count; m_it <= ( nucleotide_count - min_allele_count ); ++m_it ) {

            // Compute the term. We here use the cache, which also computes results if not yet cached.
            auto const anmn = amnm_( poolsize, nucleotide_count, m_it );

            #pragma omp atomic
            div += anmn;
        }
        return div;
    }};

    // Simply return the cached value (which computes them first if not yet cached).
    return denom_cache_( settings.poolsize, settings.min_allele_count, nucleotide_count );
}

// =================================================================================================
//     Tajima's D Local Helpers
// =================================================================================================

double a_n( size_t n ) // get_an_buffer
{
    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t> a_n_cache_{ []( size_t n ){
        double sum = 0.0;
        for( size_t i = 1; i < n; ++i ) {
            sum += 1.0 / static_cast<double>( i );
        }
        return sum;
    }};
    return a_n_cache_( n );
}

double b_n( size_t n ) // get_bn_buffer
{
    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t> b_n_cache_{ []( size_t n ){
        double sum = 0.0;
        for( size_t i = 1; i < n; ++i ) {
            sum += 1.0 / ( static_cast<double>( i ) * static_cast<double>( i ));
        }
        return sum;
    }};
    return b_n_cache_( n );
}

double f_star( double a_n, double n ) // calculate_fstar
{
    double const nd = static_cast<double>( n );
    return ( nd - 3.0 ) / ( a_n * ( nd - 1.0 ) - nd );
}

double alpha_star( double n ) // get_alphastar_calculator
{
    if( n <= 1 ) {
        throw std::invalid_argument( "Cannot compute alpha star with effective coverage n <= 1" );
    }

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, double> alpha_star_cache_{ []( double n ){
        using namespace genesis::utils;

        // Prepare some constants: n as double, a_n, and f_star.
        double const nd = static_cast<double>( n );
        double const an = a_n( n );
        double const fs = f_star( an, n );

        // Calculate individual terms (t) and subterms (ts).
        auto const t1 = squared( fs ) * ( an - ( nd / ( nd - 1.0 )));
        auto const t2s1 = an * (( 4.0 * ( nd + 1.0 )) / squared( nd - 1.0 ));
        auto const t2s2 = 2.0 * (( nd + 3.0 ) / ( nd - 1.0 ));
        auto const t2 = fs * ( t2s1 - t2s2 );
        auto const t3 = an * (( 8.0 * ( nd + 1.0 )) / ( nd * squared( nd - 1.0 )));
        auto const t4 = ( squared( nd ) + nd + 60.0 ) / ( 3.0 * nd * ( nd - 1.0 ));
        return t1 + t2 - t3 + t4;
    }};

    return alpha_star_cache_( n );
}

double beta_star( double n ) // get_betastar_calculator
{
    if( n <= 1 ) {
        throw std::invalid_argument( "Cannot compute beta star with effective coverage n <= 1" );
    }

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, double> beta_star_cache_{ []( double n ){
        using namespace genesis::utils;

        // Prepare some constants: n as double, a_n, b_n, and f_star.
        double const nd = static_cast<double>( n );
        double const an = a_n( n );
        double const bn = b_n( n );
        double const fs = f_star( an, n );

        // Calculate individual terms (t) and subterms (ts).
        auto const t1 = squared( fs ) * ( bn - (( 2.0 * ( nd - 1.0 )) / squared( nd - 1.0 )));
        auto const t2s1 = bn * ( 8.0 / ( nd - 1.0 ));
        auto const t2s2 = an * ( 4.0 / ( nd * ( nd - 1.0 )));
        auto const t2s3n = cubed( nd ) + 12.0 * squared( nd ) - 35.0 * nd + 18.0;
        auto const t2s3d = nd * squared( nd - 1.0 );
        auto const t2s3 = t2s3n / t2s3d;
        auto const t2 = fs * ( t2s1 - t2s2 - t2s3 );
        auto const t3 = bn * ( 16.0 / ( nd * ( nd - 1.0 )));
        auto const t4 = an * ( 8.0 / ( squared( nd ) * ( nd - 1.0 )));
        auto const t5s1 = 2.0 * ( std::pow( nd, 4 ) + 110.0 * squared( nd ) - 255.0 * nd + 126.0 );
        auto const t5s2 = 9.0 * ( squared( nd ) * squared( nd - 1.0 ));
        auto const t5 = t5s1 / t5s2;
        return t1 + t2 - t3 + t4 + t5;
    }};

    return beta_star_cache_( n );
}

genesis::utils::Matrix<double> pij_matrix_( // _get_pij_matrix
    size_t max_coverage, size_t poolsize
) {
    // Prepare a matrix with the needed dimensions. PoPoolation only computes this matrix
    // for min( max_coverage, poolsize ) many columns, but we go all the way and compute
    // all that is needed. Just seems cleaner. Also it avoids a bug that PoPoolation might have there.
    // auto const max_width = max_coverage < poolsize ? max_coverage : poolsize;
    auto const max_width = poolsize;
    auto result = genesis::utils::Matrix<double>( max_coverage + 1, max_width + 1 );

    // Prepare double conversion constants.
    double const poold = static_cast<double>( poolsize );

    // Init first row and column, and top left element.
    result( 0, 0 ) = 1.0;
    for( size_t i = 1; i < max_coverage + 1; ++i ) {
        result( i, 0 ) = 0.0;
    }
    for( size_t j = 1; j < max_width + 1; ++j ) {
        result( 0, j ) = 0.0;
    }

    // Compute the remaining entries.
    for( size_t i = 1; i < max_coverage + 1; ++i ) {
        for( size_t j = 1; j < max_width + 1; ++j ) {
            auto const t1s1 = (( 1.0 + poold - static_cast<double>( j )) / poold );
            auto const t1s2 = result( i - 1, j - 1 );
            auto const t2 = ( static_cast<double>( j ) / poold ) * result( i-1, j );
            result( i, j ) = t1s1 * t1s2 + t2;
        }
    }
    return result;
}

genesis::utils::Matrix<double> const& pij_matrix_resolver_( // get_nbase_matrix_resolver
    size_t max_coverage, size_t poolsize
) {
    // Here, we need to cache only by poolsize, but additionally make sure that for a given
    // poolsize, the matrix is large enough for max_coverage.
    // If it already is, we can just return it. If not, we compute a large enough matrix first.
    // We could re-use data from the smaller matrix for the computation here, but that would
    // be more complex. In terms of runtime, this amortizes pretty quickly, so probably no
    // need to do this optimization as of now.
    // We could modify FunctionCache to allow overwriting cached values from the outside,
    // but that somehow seems unclean for all other standard use cases of that class.
    // Hence, simple map here, so that we can do our own caching with overwriting.

    // Map from poolsizes to the matrix for that poolsize.
    using PijMatrix = genesis::utils::Matrix<double>;
    static std::unordered_map<size_t, PijMatrix> pij_matrix_cache_;

    // Make sure this function is called thread savely, for concurrent access to the cache.
    static std::mutex guard_;
    std::lock_guard<std::mutex> lock( guard_ );

    // Check if we already have the correct size, and re-compute if not.
    if(
        pij_matrix_cache_.count( poolsize ) == 0 ||
        max_coverage >= pij_matrix_cache_.at( poolsize ).rows() ||
        poolsize + 1 != pij_matrix_cache_.at( poolsize ).cols()
    ) {
        // Get a bit of leeway to reduce recomputation. Or maybe this is about the approximation
        // that PoPollation does. Not sure. We just copied their approach here...
        pij_matrix_cache_[ poolsize ] = pij_matrix_( 3 * max_coverage, poolsize );
    }

    assert( pij_matrix_cache_.count( poolsize ) > 0 );
    assert( max_coverage <= pij_matrix_cache_.at( poolsize ).rows() );
    assert( poolsize     <= pij_matrix_cache_.at( poolsize ).cols() );
    return pij_matrix_cache_[ poolsize ];
}

double n_base_matrix( size_t coverage, size_t poolsize ) // get_nbase_buffer
{
    // Shortcut? Boundary check? PoPoolation is not clear on this...
    // Also, might not be needed at all. Who knows. Not me.
    // News: apparently, not needed, as we cover that case below by flipping roles of the variables.
    // Not sure why. But let's do as PoPoolation does!
    // if( poolsize <= coverage ) {
    //     throw std::invalid_argument(
    //         "Cannot compute nbase with poolsize == " + std::to_string( poolsize ) +
    //         " <= coverage == " + std::to_string( coverage )
    //     );
    // }

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t, size_t> nbase_cache_{ [](
        size_t coverage, size_t poolsize
    ) {

        // Get the matrix, cached and by reference, to optimize the access.
        auto const& pij_matrix = pij_matrix_resolver_( coverage, poolsize );

        double nbase = 0.0;
        auto const minj = ( coverage < poolsize ) ? coverage : poolsize;
        for( size_t k = 1; k <= minj; ++k ) {
            assert( coverage < pij_matrix.rows() );
            assert( k < pij_matrix.cols() );
            nbase += static_cast<double>( k ) * pij_matrix( coverage, k );
        }
        return nbase;
    }};

    return nbase_cache_( coverage, poolsize );
}

double n_base( size_t coverage, size_t poolsize ) // get_nbase_buffer, but better
{
    // The following simple closed form is equivalent to the way more complicated equation given
    // in that hidden PoPoolation auxiliary equations document. See
    // https://math.stackexchange.com/questions/72223/finding-expected-number-of-distinct-values-selected-from-a-set-of-integers
    // for the proof. At the time of writing this, we are however still lacking the proof that
    // the PoPoolation equation and the PoPoolation implementation are equivalent - they never
    // show that, and instead just use their recursive dynamic programming approach (which we
    // re-implemented above) without ever showing (to the best of our knowledge) that this is
    // the same as the given equation.
    double const p = static_cast<double>( coverage );
    double const n = static_cast<double>( poolsize );
    return n * ( 1.0 - std::pow(( n - 1.0 ) / n, p ));
}

// =================================================================================================
//     Tajima's D
// =================================================================================================

double tajima_d_pool_denominator( // get_ddivisor
    PoolDiversitySettings const& settings,
    size_t snp_count,
    double theta
) {
    // PoPoolation variable names:
    // poolsize:         n
    // min_allele_count: b
    // nucleotide_count: M

    using namespace genesis::utils;

    // Edge cases.
    if( settings.min_allele_count != 2 ) {
        throw std::invalid_argument(
            "Minimum allele count needs to be set to 2 for calculating pool-corrected Tajima's D "
            "with tajima_d_pool(). In case 2 is insufficient, we recommend to subsample the reads "
            "to a smaller coverage."
        );
    }
    if( 3 * settings.min_coverage >= settings.poolsize ) {
        throw std::invalid_argument(
            "Invalid mincoverage >> poolsize (as internal aproximation we use: "
            "3 * minimumcoverage < poolsize) in tajima_d_pool()"
        );
    }

    // TODO The average of n seems to be calcualted as the expected value of picking distinct
    // individuals from a pool. This value is however not an integer. But the alpha star and
    // beta star computations assume integers. Not sure what to make of this...

    // We here re-implement two bugs from PoPoolation that massively change the results.
    // We do this in order to be able to ensure that these are the only differences between
    // our code and PoPoolation. It is weird and freaky though to conciously implement bugs...
    double avg_n;
    double alphastar;
    double betastar;
    if( settings.with_popoolation_bugs ) {
        avg_n = n_base( settings.poolsize, settings.poolsize );
        alphastar = static_cast<double>( beta_star( avg_n ));
        betastar  = alphastar;
    } else {
        avg_n = n_base( settings.min_coverage, settings.poolsize );
        alphastar = static_cast<double>( alpha_star( avg_n ));
        betastar  = static_cast<double>( beta_star( avg_n ));;
    }

    return std::sqrt(
        ( alphastar / static_cast<double>( snp_count )) * theta + betastar * squared( theta )
    );
}

} // namespace population
} // namespace genesis
