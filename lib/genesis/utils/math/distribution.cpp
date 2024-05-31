/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/*
    Below, we include several functions with implementations derived from other GPL code:

    From the random package of Agner Fog:
    multivariate_hypergeometric_distribution(), hypergeometric_distribution(),
    hypergeometric_distribution_inversion_mode_(), hypergeometric_distribution_ratio_of_unifoms_()

    From the GSL (GNU Scientific Library):
    multinomial_distribution() and hypergeometric_distribution_gsl()

    We have adapted their code for our needs. Below, we include the original copyright boilderplates
    for all of the used functions, which all have been published under the GPL.
 */

/*
    *************************** stoc1.cpp **********************************
    * Author:        Agner Fog
    * Date created:  2002-01-04
    * Last modified: 2008-11-30
    * Project:       stocc.zip
    * Source URL:    www.agner.org/random
    *
    * Description:
    * Non-uniform random number generator functions.
    *
    * This file contains source code for the class StochasticLib1 defined in stocc.h.
    *
    * Documentation:
    * ==============
    * The file stocc.h contains class definitions.
    * The file stocc.htm contains further instructions.
    * The file distrib.pdf contains definitions of the statistic distributions.
    * The file sampmet.pdf contains theoretical descriptions of the methods used
    * for sampling from these distributions.
    * The file ran-instructions.pdf contains general instructions.
    *
    * Copyright 2002-2008 by Agner Fog.
    * GNU General Public License http://www.gnu.org/licenses/gpl.html
    ****************************************************************************
 */

/*
 * randist/multinomial.c
 * Copyright (C) 2002 Gavin E. Crooks <gec@compbio.berkeley.edu>
 *
 * randist/hyperg.c
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/distribution.hpp"

#include "genesis/utils/core/options.hpp"
#include "genesis/utils/math/binomial.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Multinomial Distribution
// =================================================================================================

template<class T>
std::vector<size_t> multinomial_distribution_( std::vector<T> const& p, size_t n )
{
    // The implementation follows the GSL function gsl_ran_multinomial(), under GPL 3
    // https://www.gnu.org/software/gsl/doc/html/randist.html#the-multinomial-distribution
    // See file gsl-2.7.1/randist/multinomial.c

    // Get the sum of all weights.
    T norm = 0;
    for( auto e : p ) {
        norm += e;
    }

    // For now, we use a global random engine (thread safe).
    auto& engine = Options::get().random_engine();

    // Do the drawing, filling a result vector x.
    auto x = std::vector<size_t>( p.size() );
    T sum_p = 0;
    size_t sum_n = 0;
    for( size_t k = 0; k < p.size(); ++k ) {
        if( p[k] > 0.0 ) {
            assert( n >= sum_n );
            assert( norm >= sum_p );

            // Need to cast to double here, so that this works with p of type size_t.
            auto const binom_t = n - sum_n;
            auto const binom_p = static_cast<double>( p[k] ) / static_cast<double>(norm - sum_p);
            std::binomial_distribution<size_t> distrib( binom_t, binom_p );
            x[k] = distrib( engine );
        } else {
            x[k] = 0;
        }
        sum_p += p[k];
        sum_n += x[k];
    }

    return x;
}

std::vector<size_t> multinomial_distribution( std::vector<size_t> const& p, size_t n )
{
    return multinomial_distribution_( p, n );
}

std::vector<size_t> multinomial_distribution( std::vector<double> const& p, size_t n )
{
    // Check the weights for validity.
    for( auto e : p ) {
        if( !std::isfinite(e) || e < 0.0 ) {
            throw std::invalid_argument(
                "Cannot compute multinomial distribution "
                "if weights are not non-negative numbers: " + std::to_string( e )
            );
        }
    }
    return multinomial_distribution_( p, n );
}

// =================================================================================================
//     Hypergeometric Distribution
// =================================================================================================

size_t hypergeometric_distribution_ratio_of_unifoms_( size_t n, size_t m, size_t N )
{
    // Implementaiton based on Agner Fog, see https://www.agner.org/random/ published under GPL.
    // Accessible at https://git.wur.nl/Biometris/articles/CRISPR_ABM/-/blob/master/stoc1.cpp

    // Subfunction for Hypergeometric distribution using the ratio-of-uniforms rejection method.
    //
    // The computation time hardly depends on the parameters, except that it matters a lot
    // whether parameters are within the range where the log_factorial function is tabulated.
    //
    // Reference: E. Stadlober: "The ratio of uniforms approach for generating
    // discrete random variates". Journal of Computational and Applied Mathematics,
    // vol. 31, no. 1, 1990, pp. 181-189.

    // This code is valid for 0 < n <= m <= N/2. Assert this.
    assert( 0 <  n );
    assert( n <= m );
    assert( m <= N/2 );
    assert( m + n <= N );

    // Helper function
    auto fc_lnpk_ = []( size_t k, size_t L, size_t m, size_t n )
    {
        assert( m >= k );
        assert( n >= k );
        return log_factorial(k) + log_factorial(m - k) + log_factorial(n - k) + log_factorial(L + k);
    };

    // rNN = 1/(N*(N+2)); mean = n*m/N
    auto const L = N - m - n;
    auto const rNN = 1.0 / (static_cast<double>(N) * static_cast<double>(N + 2));
    auto const mean = (double)n * m * rNN * (N + 2);

    // mode = floor((n+1)*(m+1)/(N+2))
    auto const mode = static_cast<size_t>(
        static_cast<double>(n + 1) * static_cast<double>(m + 1) * rNN * N
    );

    // variance
    auto const var =
        ( static_cast<double>(n) * m * (N - m) * (N - n) ) /
        ( static_cast<double>(N) * N * (N - 1) )
    ;

    // hat width h; hat center a; value at mode fm (maximum)
    double const SHAT1 = 2.943035529371538573;    // 8/e
    double const SHAT2 = 0.8989161620588987408;   // 3-sqrt(12/e)
    auto const hyp_h = std::sqrt(SHAT1 * (var + 0.5)) + SHAT2;
    auto const hyp_a = mean + 0.5;
    auto const hyp_fm = fc_lnpk_(mode, L, m, n);

    // Safety upper bound
    auto hyp_bound = static_cast<size_t>( hyp_a + 4.0 * hyp_h );
    if( hyp_bound > n ) {
        hyp_bound = n;
    }

    // loop until accepted
    auto& engine = Options::get().random_engine();
    std::uniform_real_distribution<double> distrib( 0.0, 1.0 );
    size_t k = 0;
    while( true ) {
        // uniform random number
        double const u = distrib( engine );

        // avoid division by 0
        if( u == 0 ) {
            continue;
        }

        // generate hat distribution, real sample
        auto const x = hyp_a + hyp_h * (distrib( engine ) - 0.5) / u;

        if( x < 0. || x > 2E9 ) {
            // reject, avoid overflow
            continue;
        }

        // integer sample
        k = static_cast<size_t>(x);
        if( k > hyp_bound ) {
            // reject if outside range
            continue;
        }

        // ln(f(k))
        auto const lf = hyp_fm - fc_lnpk_(k, L, m, n);
        if( u * (4.0 - u) - 3.0 <= lf ) {
            // lower squeeze accept
            break;
        }
        if( u * (u - lf) > 1.0 ) {
            // upper squeeze reject
            continue;
        }
        if( 2.0 * log(u) <= lf ) {
            // final acceptance
            break;
        }
    }
    return k;
}

size_t hypergeometric_distribution_inversion_mode_( size_t n, size_t m, size_t N )
{
    // Implementaiton based on Agner Fog, see https://www.agner.org/random/ published under GPL.
    // Accessible at https://git.wur.nl/Biometris/articles/CRISPR_ABM/-/blob/master/stoc1.cpp

    // Subfunction for Hypergeometric distribution.
    // Overflow protection is needed when N > 680 or n > 75.
    //
    // Hypergeometric distribution by inversion method, using down-up
    // search starting at the mode using the chop-down technique.
    //
    // This method is faster than the rejection method when the variance is low.

    // Assumes 0 <= n <= m <= N/2.
    assert( n <= m );
    assert( m <= N/2 );
    assert( N <= 680 );
    assert( n <= 75 );

    // Helper constants
    auto const Mp = static_cast<double>(m + 1);
    auto const np = static_cast<double>(n + 1);
    auto const p  = Mp / (N + 2.0);
    assert( N >= m + n );
    auto const L = N - m - n;
    double const L1 = static_cast<double>(L);

    // mode (real), mode (int), mode+1
    auto const modef = np * p;
    size_t hyp_mode = static_cast<size_t>( modef );
    size_t hyp_mp = 0;
    if( hyp_mode == modef && p == 0.5) {
        hyp_mp = hyp_mode--;
    } else {
        hyp_mp = hyp_mode + 1;
    }

    // mode probability, using log factorial function
    assert( N >= n );
    assert( N >= m );
    assert( n >= hyp_mode );
    assert( m >= hyp_mode );
    auto const hyp_fm = std::exp(
        log_factorial(N - m) - log_factorial(L + hyp_mode) - log_factorial(n - hyp_mode) +
        log_factorial(m) - log_factorial(m - hyp_mode) - log_factorial(hyp_mode) -
        log_factorial(N) + log_factorial(N - n) + log_factorial(n)
    );

    // safety bound - guarantees at least 17 significant decimal digits
    // bound = min(n, (int32_t)(modef + k*c'))
    auto hyp_bound = static_cast<size_t>(
        modef + 11.0 * std::sqrt( modef * (1.0 - p) * (1.0 - n / static_cast<double>(N)) + 1.0 )
    );
    if( hyp_bound > n) {
        hyp_bound = n;
    }

    // loop until accepted
    auto& engine = Options::get().random_engine();
    std::uniform_real_distribution<double> distrib( 0.0, 1.0 );
    while( true ) {
        // uniform random number to be converted
        double U = distrib( engine );

        // start chop-down search at mode
        if( (U -= hyp_fm) <= 0.0 ) {
            return (hyp_mode);
        }

        // factors in iteration
        double c = hyp_fm;
        double d = hyp_fm;

        // Loop counter, and float versions of it
        size_t I;
        double k1 = hyp_mp - 1;
        double k2 = hyp_mode + 1;

        // alternating down- and upward search from the mode
        for( I = 1; I <= hyp_mode; I++, k1--, k2++ ) {
            // Downward search from k1 = hyp_mp - 1
            // divisor, eliminated by scaling
            auto divisor = (np - k1) * (Mp - k1);

            // Instead of dividing c with divisor, we multiply U and d because
            // multiplication is faster. This will give overflow if N > 800
            U *= divisor;
            d *= divisor;
            c *= k1 * (L1 + k1);
            if( (U -= c) <= 0.0 ) {
                return (hyp_mp - I - 1); // = k1 - 1
            }

            // Upward search from k2 = hyp_mode + 1
            divisor = k2 * (L1 + k2);

            // re-scale parameters to avoid time-consuming division
            U *= divisor;
            c *= divisor;
            d *= (np - k2) * (Mp - k2);
            if( (U -= d) <= 0.0 ) {
                return (hyp_mode + I); // = k2
            }
            // Values of n > 75 or N > 680 may give overflow if you leave out this..
            // overflow protection
            // if( U > 1.E100) {U *= 1.E-100; c *= 1.E-100; d *= 1.E-100;}
        }

        // Upward search from k2 = 2*mode + 1 to bound
        for( k2 = I = hyp_mp + hyp_mode; I <= hyp_bound; I++, k2++ ) {
            auto divisor = k2 * (L1 + k2);
            U *= divisor;
            d *= (np - k2) * (Mp - k2);
            if( (U -= d) <= 0.0 ) {
                return (I);
            }
            // more overflow protection
            // if( U > 1.E100) {U *= 1.E-100; d *= 1.E-100;}
        }
    }
}

size_t hypergeometric_distribution( size_t n1, size_t n2, size_t t )
{
    // Implementaiton based on Agner Fog, see https://www.agner.org/random/ published under GPL.
    // Accessible at https://git.wur.nl/Biometris/articles/CRISPR_ABM/-/blob/master/stoc1.cpp

    // We use the same arguments and order as the GSL function, for consistency,
    // but here interally convert to the format used by the Agner Fog implementation.
    // n = number of balls you take; m = number of red balls; N = total number of balls
    size_t n = t;
    size_t m = n1;
    size_t N = n1 + n2;

    // Validity check
    if( n > N ) {
        throw std::invalid_argument(
            "Invalid arguments for hypergeometric_distribution(), called with t == " +
			std::to_string(t) + " > n1 + n2 == " + std::to_string( N ) +
			", as we cannot draw more values without replacement than there are values."
        );
    }

    // Symmetry transformations.
    // We keep the values, so that we can undo the transformations later.
    int fak = 1;
    int64_t addd = 0;
    if( m > N / 2 ) {
        // invert m
        m = N - m;
        fak = -1;
        addd = n;
    }
    if( n > N / 2 ) {
        // invert n
        n = N - n;
        addd += fak * m;
        fak = -fak;
    }
    if( n > m ) {
        std::swap( n, m );
    }
    if( n == 0 ) {
        // Cases with only one possible result end here
        return addd;
    }

    // Choose the method.
    // This function uses inversion by chop-down search from the mode when
    // parameters are small, and the ratio-of-uniforms method when the former
    // method would be too slow or would give overflow.

    size_t x; // result
    if( N > 680 || n > 70 ) {
        // use ratio-of-uniforms method
        x = hypergeometric_distribution_ratio_of_unifoms_( n, m, N );
    } else {
        // inversion method, using chop-down search from mode
        x = hypergeometric_distribution_inversion_mode_( n, m, N );
    }
    // undo symmetry transformations
    return x * fak + addd;
}

size_t hypergeometric_distribution_gsl( size_t n1, size_t n2, size_t t )
{
    // Unused at the moment, but kept here for reference.

    // The implementation follows the GSL function gsl_ran_hypergeometric(), under GPL 3
    // https://www.gnu.org/software/gsl/doc/html/randist.html#the-hypergeometric-distribution
    // See file gsl-2.7.1/randist/hyperg.c

    // An alternative more complex implementation is available by Agner Fog, see agner.org/random
    // Accessible at https://git.wur.nl/Biometris/articles/CRISPR_ABM/-/blob/master/stoc1.cpp
    // and published under GPL, which we implemented above, as it is faster for larger draws.

    // Boundary check
    size_t const n = n1 + n2;
    if( t > n ) {
        t = n;
    }

	// For now, we use a global random engine (thread safe).
    auto& engine = Options::get().random_engine();
    std::uniform_real_distribution<double> distrib( 0.0, 1.0 );

    size_t a = n1;
    size_t b = n1 + n2;
    size_t k = 0;
    if( t < n / 2 ) {
        for( size_t i = 0; i < t; i++ ) {
            double const u = distrib( engine );
            if( b * u < a ) {
                k++;
                if( k == n1 ) {
                    return k;
                }
                --a;
            }
            --b;
        }
        return k;
    } else {
        for( size_t i = 0; i < n - t; i++ ) {
            double const u = distrib( engine );
            if( b * u < a ) {
                k++;
                if( k == n1 ) {
                    return n1 - k;
                }
                --a;
            }
            --b;
        }
        return n1 - k;
    }
}

std::vector<size_t> multivariate_hypergeometric_distribution( std::vector<size_t> const& p, size_t n )
{
    // Implementaiton based on Agner Fog, see https://www.agner.org/random/ published under GPL.
    // Accessible at https://git.wur.nl/Biometris/articles/CRISPR_ABM/-/blob/master/stoc1.cpp

    // Prepare and check boundary conditions
    auto x = std::vector<size_t>( p.size() );
    if( p.empty() ) {
        return x;
    }

    // Compute total number of balls
    size_t sum = 0;
    for( auto e : p ) {
        if( !std::isfinite(e) || e < 0.0 ) {
            throw std::invalid_argument(
                "Cannot compute multivariate hypergeometric distribution "
                "if weights are not non-negative numbers: " + std::to_string( e )
            );
        }
        sum += e;
    }
    if( n > sum ) {
        throw std::invalid_argument(
            "Cannot compute multivariate hypergeometric distribution "
            "with n==" + std::to_string( n ) + " > sum(p)==" + std::to_string( sum ) +
			", as we cannot draw more values without replacement than there are values to draw."
        );
    }

    // Generate output by repeatedly drawing from the hypergeometric distribution
    size_t i;
    for( i = 0; i < p.size() - 1; ++i ) {
        if( p[i] > 0.0 ) {
            assert( sum >= p[i] );
            x[i] = hypergeometric_distribution( p[i], sum - p[i], n );
            n -= x[i];
            sum -= p[i];
        } else {
            x[i] = 0;
        }
    }

    // Get the last value as the remainder
    x[i] = n;
    return x;
}

} // namespace utils
} // namespace genesis
