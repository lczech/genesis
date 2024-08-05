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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/function/diversity_pool_functions.hpp"

#include "genesis/utils/containers/function_cache.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/math/binomial.hpp"
#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

// #ifdef GENESIS_OPENMP
// #   include <omp.h>
// #endif

namespace genesis {
namespace population {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

/**
 * @brief Local helper function to compute values for the denominator.
 *
 * This computes the sum over all r poolsizes of 1/r times a binomial:
 *
 * \f$ \sum_{m=b}^{C-b} \frac{1}{k} {C \choose m} \left(\frac{k}{n}\right)^m \left(\frac{n-k}{n}\right)^{C-m} \f$
 *
 * This is needed in the pool seq correction denoinators of Theta Pi and Theta Watterson.
 */
double amnm_( // get_aMnm_buffer
    size_t poolsize,         // n
    size_t nucleotide_count, // M (coverage/read depth)
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

    // PoPoolation uses a function caching mechanism here, which we also initially did:
    // We used to have the following function cache here, but it turned out that this actually
    // was doing more harm than good. It uses a lot of memory (prohibitively so for datasets
    // with large unfiltered/un-subsampled read depth), and even adds a little bit of extra runtime
    // due to the cache lookups. At the same time, there is not really a upside here, as the
    // values computed here are only used locally in theta_pi_pool_denominator() and
    // theta_watterson_pool_denominator(), which already do their own caching. Due to that,
    // I think that each value computed here was only requested once exactly by each of those
    // two functions, so at maximum re-used once, which kind of defies the purpose of caching.
    // So, deactivated. Way less memory, and a slight improvement in runtim in our tests at least.

    // static genesis::utils::FunctionCache<double, size_t, size_t, size_t> amnm_cache_{ [](
    //     size_t poolsize, size_t nucleotide_count, size_t allele_frequency
    // ) {
    //     // ...
    // }};
    // return amnm_cache_( poolsize, nucleotide_count, allele_frequency );

    // Edge case check.
    if( allele_frequency == 0 ) {
        throw std::invalid_argument(
            "In computing amnm_(), allele_frequency == 0 is not allowed. "
            "This is likely caused by using DiversityPoolSettings.min_count == 0."
        );
    }

    // We need a binomial distribution in the loop below for which the coefficient stays
    // constant. So we pre-compute that here, and split the computation into its parts.
    // Quick test on some real data reduced the runtime by 30% using this.
    // Also, we are staying in log-space until the very end to allow large n and small p;
    // see log_binomial_distribution() for the underlying implementation.
    auto const k = allele_frequency;
    auto const n = nucleotide_count;
    auto const log_coeff = utils::log_binomial_coefficient( n, k );
    assert( k <= n );

    // #pragma omp parallel for
    double result = 0.0;
    for( size_t r = 1; r <= poolsize - 1; ++r ) {

        // Get the probability that we are looking at in this loop iteration.
        double const p = static_cast<double>( r ) / static_cast<double>( poolsize );
        assert( std::isfinite( p ) && 0.0 < p && p < 1.0 );

        // Compute the remaining parts of the binomial that depend on p.
        // Below, we have the original full function for reference.
        double const log_pow_1 = static_cast<double>(     k ) * std::log(       p );
        double const log_pow_2 = static_cast<double>( n - k ) * std::log( 1.0 - p );
        double const binom = std::exp( log_coeff + log_pow_1 + log_pow_2 );
        // double const binom = utils::binomial_distribution(
        //     allele_frequency, nucleotide_count, p
        // );

        // Sum up the term.
        double const partial = binom / static_cast<double>( r );
        // #pragma omp atomic
        result += partial;

        // Early abort. No need to continue once we reach inf or nan.
        if( ! std::isfinite( result )) {
            break;
        }
    }
    return result;
}

// =================================================================================================
//     Theta Pi
// =================================================================================================

double heterozygosity( SampleCounts const& sample, bool with_bessel )
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
    if( with_bessel ) {
        h *= nt_cnt / ( nt_cnt - 1.0 );
    }
    return h;
}

double theta_pi_pool_denominator(
    DiversityPoolSettings const& settings,
    size_t poolsize, // n
    size_t nucleotide_count  // M
) {
    // PoPoolation variable names:
    // min_count:        b
    // poolsize:         n
    // nucleotide_count: M

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t, size_t, size_t> denom_cache_{ [](
        size_t min_count, size_t poolsize, size_t nucleotide_count
    ){
        // Boundary: if not held, we'd return zero, and that would not be a useful denominator.
        // Update: It's okay, we can return 0 here. The position will just not contribute
        // to the overall diversity sum then, but still considered for the sum of valid positions.
        if( 2 * min_count > nucleotide_count ) {
            return 0.0;
            // throw std::invalid_argument(
            //     "Cannot compute theta_pi_pool_denominator with min_count = " +
            //     std::to_string( min_count ) + " and nucleotide_count = " +
            //     std::to_string( nucleotide_count )
            // );
        }

        // Iterate all allele frequencies in between the min and max-min boundaries.
        double denom = 0.0;

        // #pragma omp parallel for
        for( size_t m_it = min_count; m_it <= ( nucleotide_count - min_count ); ++m_it ) {
            // We iterate from b to M-b (in PoPoolation terminology), inclusively.
            // Use double values however for the computations.
            double const m = static_cast<double>( m_it );
            double const M = static_cast<double>( nucleotide_count );

            // Compute the term. We here use the cache, which also computes results if not yet cached.
            double const term = ( 2.0 * m * ( M - m )) / ( M * ( M - 1.0 ));
            double const partial = term * amnm_( poolsize, nucleotide_count, m_it );

            // #pragma omp atomic
            denom += partial;

            // Early abort. No need to continue once we reach inf or nan.
            if( ! std::isfinite( denom )) {
                break;
            }
        }
        return denom;
    }};

    // Simply return the cached value (which computes them first if not yet cached).
    return denom_cache_( settings.min_count, poolsize, nucleotide_count );
}

// =================================================================================================
//     Theta Watterson
// =================================================================================================

double theta_watterson_pool_denominator(
    DiversityPoolSettings const& settings,
    size_t poolsize,
    size_t nucleotide_count  // M
) {
    // PoPoolation variable names:
    // min_count:        b
    // poolsize:         n
    // nucleotide_count: M

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t, size_t, size_t> denom_cache_{ [](
        size_t min_count, size_t poolsize, size_t nucleotide_count
    ){
        // Boundary: if not held, we'd return zero, and that would not be a useful denominator.
        // Update: It's okay, we can return 0 here. The position will just not contribute
        // to the overall diversity sum then, but still considered for the sum of valid positions.
        if( 2 * min_count > nucleotide_count ) {
            return 0.0;
            // throw std::invalid_argument(
            //     "Cannot compute theta_watterson_pool_denominator with min_count = " +
            //     std::to_string( min_count ) + " and nucleotide_count = " +
            //     std::to_string( nucleotide_count )
            // );
        }

        // Iterate all allele frequencies in between the min and max-min boundaries.
        double denom = 0.0;

        // #pragma omp parallel for
        for( size_t m_it = min_count; m_it <= ( nucleotide_count - min_count ); ++m_it ) {

            // Compute the term. We here use the cache, which also computes results if not yet cached.
            auto const anmn = amnm_( poolsize, nucleotide_count, m_it );

            // #pragma omp atomic
            denom += anmn;

            // Early abort. No need to continue once we reach inf or nan.
            if( ! std::isfinite( denom )) {
                break;
            }
        }
        return denom;
    }};

    // Simply return the cached value (which computes them first if not yet cached).
    return denom_cache_( settings.min_count, poolsize, nucleotide_count );
}

// =================================================================================================
//     Tajima's D Helper Functions
// =================================================================================================

double a_n( double n ) // get_an_buffer
{
    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t> a_n_cache_{ []( size_t n ){
        double sum = 0.0;
        for( size_t i = 1; i < n; ++i ) {
            sum += 1.0 / static_cast<double>( i );
        }
        return sum;
    }};

    // The n value that we get here is a double, because following PoPoolation, we compute it
    // as n_tilde, which is not integer... but we need to use it as an integer here,
    // and the way that PoPoolation computes n_tilde, it is around 1.99,
    // so we want to make sure to round to the nearest number, I think.
    // We do that here, before the cache function call, so that the cache function does not
    // get affected by close but non-identical doubles that round to the same int.
    assert( std::isfinite(n) && n >= 0.0 );
    auto const ni = static_cast<size_t>( std::round( n ));
    return a_n_cache_( ni );
}

double b_n( double n ) // get_bn_buffer
{
    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t> b_n_cache_{ []( size_t n ){
        double sum = 0.0;
        for( size_t i = 1; i < n; ++i ) {
            sum += 1.0 / ( static_cast<double>( i ) * static_cast<double>( i ));
        }
        return sum;
    }};

    // Same logic as in a_n(), see there for details.
    assert( std::isfinite(n) && n >= 0.0 );
    auto const ni = static_cast<size_t>( std::round( n ));
    return b_n_cache_( ni );
}

double f_star( double a_n, double n ) // calculate_fstar
{
    double const nd = static_cast<double>( n );
    return ( nd - 3.0 ) / ( a_n * ( nd - 1.0 ) - nd );
}

double alpha_star( double n ) // get_alphastar_calculator
{
    if( n <= 1 ) {
        throw std::invalid_argument( "Cannot compute alpha star with effective read depth n <= 1" );
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
        throw std::invalid_argument( "Cannot compute beta star with effective read depth n <= 1" );
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
        // The first term t1 has a mistake in PoPoolation, where they use 2 * ( n - 1 )
        // instead of ( 2 * n ) - 1, which we have fixed here.
        auto const t1 = squared( fs ) * ( bn - ((( 2.0 * nd ) - 1.0 ) / squared( nd - 1.0 )));
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
    size_t max_read_depth, size_t poolsize
) {
    // Prepare a matrix with the needed dimensions. PoPoolation only computes this matrix
    // for min( max_read_depth, poolsize ) many columns, but we go all the way and compute
    // all that is needed. Just seems cleaner. Also it avoids a bug that PoPoolation might have there.
    // auto const max_width = max_read_depth < poolsize ? max_read_depth : poolsize;
    auto const max_width = poolsize;
    auto result = genesis::utils::Matrix<double>( max_read_depth + 1, max_width + 1 );

    // Prepare double conversion constants.
    double const poold = static_cast<double>( poolsize );

    // Init first row and column, and top left element.
    result( 0, 0 ) = 1.0;
    for( size_t i = 1; i < max_read_depth + 1; ++i ) {
        result( i, 0 ) = 0.0;
    }
    for( size_t j = 1; j < max_width + 1; ++j ) {
        result( 0, j ) = 0.0;
    }

    // Compute the remaining entries.
    for( size_t i = 1; i < max_read_depth + 1; ++i ) {
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
    size_t max_read_depth, size_t poolsize
) {
    // Here, we need to cache only by poolsize, but additionally make sure that for a given
    // poolsize, the matrix is large enough for max_read_depth.
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
        max_read_depth >= pij_matrix_cache_.at( poolsize ).rows() ||
        poolsize + 1 != pij_matrix_cache_.at( poolsize ).cols()
    ) {
        // Get a bit of leeway to reduce recomputation. Or maybe this is about the approximation
        // that PoPoolation does. Not sure. We just copied their approach here...
        pij_matrix_cache_[ poolsize ] = pij_matrix_( 3 * max_read_depth, poolsize );
    }

    assert( pij_matrix_cache_.count( poolsize ) > 0 );
    assert( max_read_depth <= pij_matrix_cache_.at( poolsize ).rows() );
    assert( poolsize     <= pij_matrix_cache_.at( poolsize ).cols() );
    return pij_matrix_cache_[ poolsize ];
}

double n_base_matrix( size_t read_depth, size_t poolsize ) // get_nbase_buffer
{
    // Shortcut? Boundary check? PoPoolation is not clear on this...
    // Also, might not be needed at all. Who knows. Not me.
    // News: apparently, not needed, as we cover that case below by flipping roles of the variables.
    // Not sure why. But let's do as PoPoolation does! Has been flawless before...
    // if( poolsize <= read_depth ) {
    //     throw std::invalid_argument(
    //         "Cannot compute nbase with poolsize == " + std::to_string( poolsize ) +
    //         " <= read_depth == " + std::to_string( read_depth )
    //     );
    // }

    // Local cache for speed.
    static genesis::utils::FunctionCache<double, size_t, size_t> nbase_cache_{ [](
        size_t read_depth, size_t poolsize
    ) {

        // Get the matrix, cached and by reference, to optimize the access.
        auto const& pij_matrix = pij_matrix_resolver_( read_depth, poolsize );

        double nbase = 0.0;
        auto const minj = ( read_depth < poolsize ) ? read_depth : poolsize;
        for( size_t k = 1; k <= minj; ++k ) {
            assert( read_depth < pij_matrix.rows() );
            assert( k < pij_matrix.cols() );
            nbase += static_cast<double>( k ) * pij_matrix( read_depth, k );
        }
        return nbase;
    }};

    return nbase_cache_( read_depth, poolsize );
}

double n_base( size_t read_depth, size_t poolsize ) // get_nbase_buffer, but better
{
    // The following simple closed form is equivalent to the way more complicated equation given
    // in that hidden PoPoolation auxiliary equations document. See
    // https://math.stackexchange.com/questions/72223/finding-expected-number-of-distinct-values-selected-from-a-set-of-integers
    // for the proof. At the time of writing this, we are however still lacking the proof that
    // the PoPoolation equation and the PoPoolation implementation are equivalent - they never
    // show that, and instead just use their recursive dynamic programming approach (which we
    // re-implemented above) without ever showing (to the best of our knowledge) that this is
    // the same as the given equation.
    double const p = static_cast<double>( read_depth );
    double const n = static_cast<double>( poolsize );
    return n * ( 1.0 - std::pow(( n - 1.0 ) / n, p ));
}

// =================================================================================================
//     Tajima's D
// =================================================================================================

double tajima_d_pool_denominator( // get_ddivisor
    DiversityPoolSettings const& settings,
    double theta,
    size_t poolsize, // n
    double window_avg_denom,
    size_t empirical_min_read_depth
) {
    // PoPoolation variable names:
    // min_count:        b
    // poolsize:         n
    // nucleotide_count: M

    using namespace genesis::utils;

    // Edge cases, only relevant for the Kofler-based correction denomiator variants.
    if(
        settings.tajima_denominator_policy == TajimaDenominatorPolicy::kWithPopoolationBugs ||
        settings.tajima_denominator_policy == TajimaDenominatorPolicy::kProvidedMinReadDepth
    ) {
        if( settings.min_count != 2 ) {
            throw std::invalid_argument(
                "Minimum allele count needs to be set to 2 for calculating pool-corrected Tajima's D "
                "with tajima_d_pool() according to Kofler et al. In case 2 is insufficient, "
                "we recommend to subsample the reads to a smaller read depth."
            );
        }
        if( settings.min_read_depth == 0 ) {
            throw std::invalid_argument(
                "Minimum read depth of 0 is not valid for calculating pool-corrected Tajima's D "
                "with tajima_d_pool()."
            );
        }
        if( 3 * settings.min_read_depth >= poolsize ) {
            throw std::invalid_argument(
                "Invalid minimum read depth >> pool size (as internal aproximation we use: "
                "3 * minimum read depth < pool size) in tajima_d_pool()"
            );
        }
    }

    double alphastar;
    double betastar;
    switch( settings.tajima_denominator_policy ) {
        case TajimaDenominatorPolicy::kEmpiricalMinReadDepth:
        {
            // Use the emprical minimum read depth to get the value.
            auto const avg_n = n_base( empirical_min_read_depth, poolsize );
            alphastar = alpha_star( avg_n );
            betastar  = beta_star( avg_n );
            break;
        }
        case TajimaDenominatorPolicy::kProvidedMinReadDepth:
        {
            // Fix the bugs from above, but still use the user min cov for n_base.
            auto const avg_n = n_base( settings.min_read_depth, poolsize );
            alphastar = alpha_star( avg_n );
            betastar  = beta_star( avg_n );
            break;
        }
        case TajimaDenominatorPolicy::kWithPopoolationBugs:
        {
            // We here re-implement two bugs from PoPoolation that massively change the results.
            // We do this in order to be able to ensure that these are the only differences between
            // our code and PoPoolation. It is weird and freaky though to conciously implement bugs...
            auto const avg_n = n_base( poolsize, poolsize );
            alphastar = beta_star( avg_n );
            betastar  = alphastar;
            break;
        }
        case TajimaDenominatorPolicy::kPoolsize:
        {
            // Use the pool size instead of anything n_base based.
            alphastar = alpha_star( poolsize );
            betastar  = beta_star( poolsize );
            break;
        }
        case TajimaDenominatorPolicy::kUncorrected:
        {
            // No correction at all.
            return 1.0;
        }
        default: {
            throw std::invalid_argument( "Invalid enum value for TajimaDenominatorPolicy" );
        }
    }

    return std::sqrt(
        ( alphastar / window_avg_denom ) * theta + betastar * squared( theta )
    );
}

} // namespace population
} // namespace genesis
