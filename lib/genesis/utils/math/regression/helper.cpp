/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.        See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.        If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

/*
    =================================================================
            License
    =================================================================

    The implementation is based on the snp.matrix and X.snp.matrix classes by
    David Clayton <david.clayton@cimr.cam.ac.uk> and Hin-Tak Leung <htl10@users.sourceforge.net>.
    The source is in C, but is originally intended for usage in R.
    This file in particular is based on snpStats_1.32.0/src/mla.c

    The original code is published under the GNU General Public Licence version 3 (GPLv3).
    We use the same license, hence see above for details.

    The snp.matrix and X.snp.matrix classes.
    Copyright (C) 2008 David Clayton and Hin-Tak Leung

    The package does not seem to be maintained any more, and does not seem to
    have a proper repository. For more information, try these sites:
    https://bioconductor.org/packages/release/bioc/html/snpStats.html
    https://www.rdocumentation.org/packages/snpStats/
    http://www-gene.cimr.cam.ac.uk/clayton/software/
*/

#include "genesis/utils/math/regression/helper.hpp"

#include <cassert>
#include <cstdlib>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =============================================================================
//     Linear Algebra Helper Functions
// =============================================================================

/* If resid==0, return a vector containing the appropriate strata (weighted)
     means. Otherwise, center the input    vector around these. i.e    calculate
     either the "fitted value" or the residual from a model in which only
     strata are fitted. In this and following functions, y_new can coincide with
     y. Matrices are stored in Fortran order
     Returns number of empty strata */

GlmFreedom weighted_centering(
    std::vector<double> const& y,
    std::vector<double> const& weights,
    std::vector<size_t> const& strata,
    bool                       with_intercept,
    bool                       centering,
    std::vector<double>&       y_new
) {
    // Prepare return value. Has reasonable defaults already.
    GlmFreedom result;

    // Prepare result vector.
    if( &y_new != &y ) {
        y_new.resize( y.size() );
    }
    assert( y_new.size() == y.size() );

    // Check
    if( ! weights.empty() && weights.size() != y.size() ) {
        throw std::invalid_argument(
            "weighted_residuals: y and weights need to have same length."
        );
    }

    if( strata.empty() ) {
        if( ! with_intercept ) {
            // Nothing to do ... if necessary copy input to output
            if( &y_new != &y ) {
                y_new = y;
            }
            return result;
        }

        double swt = 0.0;
        double swy = 0.0;

        if( weights.empty() ) {
            for( size_t i = 0; i < y.size(); ++i ) {
                swy += y[i];
            }
            swt = static_cast<double>( y.size() );
        } else {
            for( size_t i = 0; i < y.size(); ++i ) {
                double wi = weights[i];
                swt += wi;
                swy += wi * y[i];
            }
        }
        swy /= swt;

        if( swt > 0.0 ) {
            for( size_t i = 0; i < y.size(); ++i ) {
                y_new[i] = centering ? y[i] - swy : swy;
            }
        } else {
            result.empty_strata = 1;
        }

    } else {
        if( strata.size() != y.size() ) {
            throw std::invalid_argument(
                "weighted_centering: y and strata need to have same length."
            );
        }

        auto swy = std::vector<double>( strata.size(), 0.0 );
        auto swt = std::vector<double>( strata.size(), 0.0 );

        // Error check.
        for( size_t s = 0; s < strata.size(); ++s ) {
            if( strata[s] < 1 || strata[s] > strata.size() ) {
                throw std::invalid_argument(
                    "weighted_residuals: invalid stratum value outside of [1,N] found."
                );
            }
            if( strata[s] > result.max_stratum ) {
                result.max_stratum = strata[s];
            }
        }

        if( weights.empty() ) {
            for( size_t i = 0; i < y.size(); ++i ) {
                int s = strata[i] - 1;
                swt[s] += 1.0;
                swy[s] += y[i];
            }
        } else {
            for( size_t i = 0; i < y.size(); ++i ) {
                double wi = weights[i];
                int s = strata[i] - 1;
                swt[s] += wi;
                swy[s] += wi * y[i];
            }
        }

        for( size_t s = 0; s < strata.size(); ++s ) {
            double sws = swt[s];
            if( sws > 0.0 ) {
                swy[s] /= sws;
            } else {
                ++result.empty_strata;
            }
        }
        for( size_t i = 0; i < y.size(); ++i ) {
            int s = strata[i] - 1;
            if( swt[s] ) {
                y_new[i] = centering ? y[i] - swy[s] : swy[s];
            }
        }
    }

    return result;
}

double weighted_residuals(
    std::vector<double> const& x,
    std::vector<double> const& y,
    std::vector<double> const& weights,
    std::vector<double>&       y_new
) {
    if( x.size() != y.size() ) {
        throw std::invalid_argument(
            "weighted_residuals: x and y need to have same length."
        );
    }

    double swxx = 0.0;
    double swxy = 0.0;

    if( weights.empty() ) {
        for( size_t i = 0; i < x.size(); ++i ) {
            double const xi = x[i];
            swxy += xi * y[i];
            swxx += xi * xi;
        }
    } else {
        if( weights.size() != x.size() ) {
            throw std::invalid_argument(
                "weighted_residuals: x and weights need to have same length."
            );
        }

        for( size_t i = 0; i < x.size(); ++i ) {
            double const xi = x[i];
            double const wi = weights[i];
            double const wx = wi * xi;
            swxy += wx * y[i];
            swxx += wx * xi;
        }
    }

    if( &y_new != &y ) {
        y_new.resize( y.size() );
    }
    assert( y_new.size() == y.size() );
    if( swxx > 0 ) {
        swxy /= swxx;
        for( size_t i = 0; i < x.size(); ++i ) {
            y_new[i] = y[i] - swxy * x[i];
        }

        return swxy;
    } else {
        if( &y_new != &y ) {
            for( size_t i = 0; i < x.size(); ++i ) {
                y_new[i] = y[i];
            }
        }
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double weighted_sum_of_squares( std::vector<double> const& x, std::vector<double> const& weights )
{
    double res = 0.0;
    if( weights.empty() ) {
        for( size_t i = 0; i < x.size(); ++i ) {
            res += x[i] * x[i];
        }
    } else {
        if( weights.size() != x.size() ) {
            throw std::invalid_argument(
                "weighted_sum: x and weights need to have same length."
            );
        }
        for( size_t i = 0; i < x.size(); ++i ) {
            res += weights[i] * x[i] * x[i];
        }
    }
    return res;
}

double weighted_inner_product(
    std::vector<double> const& x,
    std::vector<double> const& y,
    std::vector<double> const& weights
) {
    if( x.size() != y.size() ) {
        throw std::invalid_argument(
            "weighted_inner_product: x and y need to have same length."
        );
    }

    double res = 0.0;
    if( weights.empty() ) {
        for( size_t i = 0; i < y.size(); ++i ) {
            res += y[i] * x[i];
        }
    } else {
        if( weights.size() != y.size() ) {
            throw std::invalid_argument(
                "weighted_inner_product: x and weights need to have same length."
            );
        }
        for( size_t i = 0; i < y.size(); ++i ) {
            res += weights[i] * y[i] * x[i];
        }
    }
    return res;
}

double weighted_sum(
    std::vector<double> const& x,
    std::vector<double> const& weights
) {
    double res = 0.0;
    if( weights.empty() ) {
        for( size_t i = 0; i < x.size(); ++i ) {
            res += x[i];
        }
    } else {
        if( weights.size() != x.size() ) {
            throw std::invalid_argument(
                "weighted_sum: x and weights need to have same length."
            );
        }
        for( size_t i = 0; i < x.size(); ++i ) {
            res += weights[i] * x[i];
        }
    }
    return res;
}

} // namespace utils
} // namespace genesis
