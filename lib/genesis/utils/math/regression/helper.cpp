/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
    We massively refactored the original code, for example by using vectors and matrices instead
    of pointers, and by using proper data structures instead of lists of in/out function parameters.

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
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =============================================================================
//     Linear Algebra Helper Functions
// =============================================================================

GlmFreedom weighted_mean_centering(
    std::vector<double> const& y_input,
    std::vector<double> const& weights,
    std::vector<size_t> const& strata,
    bool                       with_intercept,
    bool                       centering,
    std::vector<double>&       y_output
) {
    // Prepare return value. Has reasonable defaults already.
    GlmFreedom result;

    // Prepare result vector.
    if( &y_output != &y_input ) {
        y_output.resize( y_input.size() );
    }
    assert( y_output.size() == y_input.size() );

    // Check
    if( ! weights.empty() && weights.size() != y_input.size() ) {
        throw std::invalid_argument(
            "weighted_residuals: y and weights need to have same length."
        );
    }

    if( strata.empty() ) {
        if( ! with_intercept ) {
            // Nothing to do ... if necessary copy input to output
            if( &y_output != &y_input ) {
                if( centering ) {
                    y_output = y_input;
                } else {
                    y_output = std::vector<double>( y_input.size(), 0.0 );
                }
            }
            return result;
        }

        // Calcualte the (weighted) mean of the y values.
        double swt = 0.0;
        double swy = 0.0;
        if( weights.empty() ) {
            size_t swt_cnt = 0;
            for( size_t i = 0; i < y_input.size(); ++i ) {
                if( std::isfinite( y_input[i] )) {
                    swy += y_input[i];
                    ++swt_cnt;
                }
            }
            swt = static_cast<double>( swt_cnt );
        } else {
            for( size_t i = 0; i < y_input.size(); ++i ) {
                if( weights[i] < 0.0 ) {
                    throw std::runtime_error(
                        "weighted_mean_centering: weights have to be non-negative."
                    );
                }
                if( std::isfinite( weights[i] ) && std::isfinite( y_input[i] )) {
                    swy += weights[i] * y_input[i];
                    swt += weights[i];
                }
            }
        }
        assert( std::isfinite( swy ) );
        assert( std::isfinite( swt ) );
        assert( swt >= 0.0 );

        // Calculate the centring (or set to mean).
        // Non-finite y values will simply stay non finite here - no need for extra checks.
        if( swt > 0.0 ) {
            swy /= swt;
            assert( std::isfinite( swy ) );

            for( size_t i = 0; i < y_input.size(); ++i ) {
                y_output[i] = ( centering ? y_input[i] - swy : swy );
            }
        } else {
            result.empty_strata = 1;
        }

    } else {
        if( strata.size() != y_input.size() ) {
            throw std::invalid_argument(
                "weighted_centering: y and strata need to have same length."
            );
        }

        auto swy = std::vector<double>( strata.size(), 0.0 );
        auto swt = std::vector<double>( strata.size(), 0.0 );

        // Error checking, and finding max stratum.
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

        // Calcualte the (weighted) mean of the y values per stratum.
        if( weights.empty() ) {
            for( size_t i = 0; i < y_input.size(); ++i ) {
                if( std::isfinite( y_input[i] )) {
                    size_t const s = strata[i] - 1;
                    swy[s] += y_input[i];
                    swt[s] += 1.0;
                }
            }
        } else {
            for( size_t i = 0; i < y_input.size(); ++i ) {
                if( weights[i] < 0.0 ) {
                    throw std::runtime_error(
                        "weighted_mean_centering: weights have to be non-negative."
                    );
                }
                if( std::isfinite( weights[i] ) && std::isfinite( y_input[i] )) {
                    size_t const s = strata[i] - 1;
                    swy[s] += weights[i] * y_input[i];
                    swt[s] += weights[i];
                }
            }
        }
        for( size_t s = 0; s < strata.size(); ++s ) {
            assert( std::isfinite( swy[s] ) );
            assert( std::isfinite( swt[s] ) );
            assert( swt[s] >= 0.0 );

            if( swt[s] > 0.0 ) {
                swy[s] /= swt[s];
            } else {
                ++result.empty_strata;
            }
            assert( std::isfinite( swy[s] ) );
        }

        // Calculate the centring (or set to mean).
        // Again, non-finite y values will simply stay non finite here - no need for extra checks.
        for( size_t i = 0; i < y_input.size(); ++i ) {
            size_t const s = strata[i] - 1;
            if( swt[s] > 0.0 ) {
                y_output[i] = ( centering ? y_input[i] - swy[s] : swy[s] );
            }
        }
    }

    return result;
}

double weighted_residuals(
    std::vector<double> const& x_input,
    std::vector<double> const& y_input,
    std::vector<double> const& weights,
    std::vector<double>&       y_output
) {
    if( x_input.size() != y_input.size() ) {
        throw std::invalid_argument(
            "weighted_residuals: x and y need to have same length."
        );
    }

    double swxx = 0.0;
    double swxy = 0.0;

    if( weights.empty() ) {
        for( size_t i = 0; i < x_input.size(); ++i ) {
            if( std::isfinite( x_input[i] ) && std::isfinite( y_input[i] )) {
                swxy += x_input[i] * y_input[i];
                swxx += x_input[i] * x_input[i];
            }
        }
    } else {
        if( weights.size() != x_input.size() ) {
            throw std::invalid_argument(
                "weighted_residuals: x and weights need to have same length."
            );
        }

        for( size_t i = 0; i < x_input.size(); ++i ) {
            if( weights[i] < 0.0 ) {
                throw std::runtime_error(
                    "weighted_residuals: weights have to be non-negative."
                );
            }
            if( std::isfinite( x_input[i] )
                && std::isfinite( y_input[i] )
                && std::isfinite( weights[i] )
            ) {
                double const wx = weights[i] * x_input[i];
                swxy += wx * y_input[i];
                swxx += wx * x_input[i];
            }
        }
    }
    assert( std::isfinite( swxx ));
    assert( std::isfinite( swxy ));

    if( &y_output != &y_input ) {
        y_output.resize( y_input.size() );
    }
    assert( y_output.size() == y_input.size() );
    if( swxx > 0.0 ) {
        swxy /= swxx;
        for( size_t i = 0; i < x_input.size(); ++i ) {
            y_output[i] = y_input[i] - swxy * x_input[i];
        }

        return swxy;
    } else {
        if( &y_output != &y_input ) {
            for( size_t i = 0; i < x_input.size(); ++i ) {
                y_output[i] = y_input[i];
            }
        }
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double weighted_sum_of_squares(
    std::vector<double> const& x_input,
    std::vector<double> const& weights
) {
    double res = 0.0;
    if( weights.empty() ) {
        for( size_t i = 0; i < x_input.size(); ++i ) {
            if( std::isfinite( x_input[i] )) {
                res += x_input[i] * x_input[i];
            }
        }
    } else {
        if( weights.size() != x_input.size() ) {
            throw std::invalid_argument(
                "weighted_sum: x and weights need to have same length."
            );
        }
        for( size_t i = 0; i < x_input.size(); ++i ) {
            if( weights[i] < 0.0 ) {
                throw std::runtime_error(
                    "weighted_sum_of_squares: weights have to be non-negative."
                );
            }
            if( std::isfinite( x_input[i] ) && std::isfinite( weights[i] )) {
                res += weights[i] * x_input[i] * x_input[i];
            }
        }
    }
    assert( std::isfinite( res ));
    return res;
}

double weighted_inner_product(
    std::vector<double> const& x_input,
    std::vector<double> const& y_input,
    std::vector<double> const& weights
) {
    if( x_input.size() != y_input.size() ) {
        throw std::invalid_argument(
            "weighted_inner_product: x and y need to have same length."
        );
    }

    double res = 0.0;
    if( weights.empty() ) {
        for( size_t i = 0; i < y_input.size(); ++i ) {
            if( std::isfinite( y_input[i] ) && std::isfinite( x_input[i] )) {
                res += y_input[i] * x_input[i];
            }
        }
    } else {
        if( weights.size() != y_input.size() ) {
            throw std::invalid_argument(
                "weighted_inner_product: x and weights need to have same length."
            );
        }
        for( size_t i = 0; i < y_input.size(); ++i ) {
            if( weights[i] < 0.0 ) {
                throw std::runtime_error(
                    "weighted_inner_product: weights have to be non-negative."
                );
            }
            if( std::isfinite( weights[i] )
                && std::isfinite( y_input[i] )
                && std::isfinite( x_input[i] )
            ) {
                res += weights[i] * y_input[i] * x_input[i];
            }
        }
    }
    assert( std::isfinite( res ));
    return res;
}

double weighted_sum(
    std::vector<double> const& x_input,
    std::vector<double> const& weights
) {
    double res = 0.0;
    if( weights.empty() ) {
        for( size_t i = 0; i < x_input.size(); ++i ) {
            if( std::isfinite( x_input[i] )) {
                res += x_input[i];
            }
        }
    } else {
        if( weights.size() != x_input.size() ) {
            throw std::invalid_argument(
                "weighted_sum: x and weights need to have same length."
            );
        }
        for( size_t i = 0; i < x_input.size(); ++i ) {
            if( weights[i] < 0.0 ) {
                throw std::runtime_error(
                    "weighted_sum: weights have to be non-negative."
                );
            }
            if( std::isfinite( weights[i] ) && std::isfinite( x_input[i] )) {
                res += weights[i] * x_input[i];
            }
        }
    }
    assert( std::isfinite( res ));
    return res;
}

} // namespace utils
} // namespace genesis
