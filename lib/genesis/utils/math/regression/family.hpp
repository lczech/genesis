#ifndef GENESIS_UTILS_MATH_REGRESSION_FAMILY_H_
#define GENESIS_UTILS_MATH_REGRESSION_FAMILY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @ingroup utils
 */

#include "genesis/utils/math/regression/link.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>

namespace genesis {
namespace utils {

// =================================================================================================
//     Distribution Families
// =================================================================================================

struct GlmFamily
{
    /**
     * @brief List of common GLM Families.
     */
    enum Family{
        kUnknown,
        kBinomial,
        kPoisson,
        kGaussian,
        kGamma
    };

    /**
     * @brief Internal ID of the GlmFamily, used to check for specific families where needed.
     */
    Family id = kUnknown;

    /**
     * @brief Internal ID of the GlmLink, used to check if the link is the canonical one for a
     * given distribution family.
     */
    GlmLink::Link canonical_link_id = GlmLink::kUnknown;

    /**
     * @brief Variance function for the distribution family.
     */
    std::function<double( double mu )> variance;

    /**
     * @brief Log-Likelihood contribution of a value. To be multiplied by prior weight.
     */
    std::function<double( double y, double mu )> log_likelihood;

    /**
     * @brief Rectify to a valid value, for the fitted mean, to avoid extreme predictions.
     */
    std::function<double( double mu )> rectify;

    /**
     * @brief Unit deviance for the distribution family.
     */
    std::function<double( double y, double mu )> unit_deviance;

    /**
     * @brief Get the canonical link function.
     */
    std::function<GlmLink()> canonical_link;
};

/**
 * @brief Check whether all necessary values and functors of a GlmFamily are set.
 */
inline bool is_defined( GlmFamily const& family )
{
    return family.variance && family.log_likelihood && family.rectify && family.unit_deviance;
}

inline bool is_canonical_link( GlmFamily const& family, GlmLink const& link )
{
    return ( family.canonical_link_id == link.id );
}

// =================================================================================================
//     Binomial Distribution
// =================================================================================================

/**
 * @brief Binomial family functions.
 *
 * The canonical link for binomial is logit, see glm_link_logit().
 */
inline GlmFamily glm_family_binomial()
{
    GlmFamily family;
    family.id = GlmFamily::kBinomial;
    family.canonical_link_id = GlmLink::kLogit;
    family.variance = []( double mu )
    {
        return mu * (1.0 - mu);
    };
    family.log_likelihood = []( double y, double mu )
    {
        // Calculate both parts.
        double l = y * std::log(mu);
        double r = (1.0 - y) * std::log(1.0 - mu);

        // Because of floating point nan propagation, we need to reset if needed.
        l = ( std::isfinite(l) ? l : 0.0 );
        r = ( std::isfinite(r) ? r : 0.0 );

        return l + r;
    };
    family.rectify = []( double mu )
    {
        const double zero = 1.e-10;
        const double one  = 1.0 - 1.e-10;

        if( mu < zero ) {
            return zero;
        } else if( mu > one ) {
            return one;
        }
        return mu;
    };
    family.unit_deviance = []( double y, double mu )
    {
        // Calculate both parts.
        double l = y * std::log( y / mu );
        double r = ( 1.0 - y ) * std::log(( 1.0 - y ) / ( 1.0 - mu ));

        // Because of floating point nan propagation, we need to reset if needed.
        l = ( std::isfinite(l) ? l : 0.0 );
        r = ( std::isfinite(r) ? r : 0.0 );

        return 2.0 * ( l + r );
    };
    family.canonical_link = []()
    {
        return glm_link_logit();
    };
    return family;
}

// =================================================================================================
//     Poison Distribution
// =================================================================================================

/**
 * @brief Poisson family functions.
 *
 * The canonical link for poisson is log, see glm_link_log().
 */
inline GlmFamily glm_family_poisson()
{
    GlmFamily family;
    family.id = GlmFamily::kPoisson;
    family.canonical_link_id = GlmLink::kLog;
    family.variance = []( double mu )
    {
        return mu;
    };
    family.log_likelihood = []( double y, double mu )
    {
        assert( mu > 0.0 );
        return y * std::log(mu) - mu;
    };
    family.rectify = []( double mu )
    {
        const double zero = 1.e-10;

        if( mu < zero ) {
            return zero;
        }
        return mu;
    };
    family.unit_deviance = []( double y, double mu )
    {
        assert( y > 0.0 );
        assert( mu > 0.0 );
        return 2.0 * ( y * std::log( y / mu ) - ( y - mu ));
    };
    family.canonical_link = []()
    {
        return glm_link_log();
    };
    return family;
}

// =================================================================================================
//     Gaussian Distribution
// =================================================================================================

/**
 * @brief Gaussian/normal family functions.
 *
 * The canonical link for gaussian/normal is identity, see glm_link_identity().
 */
inline GlmFamily glm_family_gaussian()
{
    GlmFamily family;
    family.id = GlmFamily::kGaussian;
    family.canonical_link_id = GlmLink::kIdentity;
    family.variance = []( double mu )
    {
        (void) mu;
        return 1.0;
    };
    family.log_likelihood = []( double y, double mu )
    {
        auto const x = y - mu;
        return x * x;
    };
    family.rectify = []( double mu )
    {
        return mu;
    };
    family.unit_deviance = []( double y, double mu )
    {
        double const d = y - mu;
        return d * d;
    };
    family.canonical_link = []()
    {
        return glm_link_identity();
    };
    return family;
}

// =================================================================================================
//     Gamma Distribution
// =================================================================================================

/**
 * @brief Gamma family functions.
 *
 * The canonical link for gamma is identity, see glm_link_gamma().
 */
inline GlmFamily glm_family_gamma()
{
    GlmFamily family;
    family.id = GlmFamily::kGamma;
    family.canonical_link_id = GlmLink::kInverse;
    family.variance = []( double mu )
    {
        return mu * mu;
    };
    family.log_likelihood = []( double y, double mu )
    {
        auto const x = y / mu;
        assert( x > 0.0 );
        return std::log(x) - x;
    };
    family.rectify = []( double mu )
    {
        return mu;
    };
    family.unit_deviance = []( double y, double mu )
    {
        double const f = y / mu;
        assert( f > 0.0 );
        return 2.0 * ( f - std::log( f ) - 1.0 );
    };
    family.canonical_link = []()
    {
        return glm_link_inverse();
    };
    return family;
}

} // namespace utils
} // namespace genesis

#endif // include guard
