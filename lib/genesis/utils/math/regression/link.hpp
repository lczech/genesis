#ifndef GENESIS_UTILS_MATH_REGRESSION_LINK_H_
#define GENESIS_UTILS_MATH_REGRESSION_LINK_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include <cmath>
#include <cstdint>
#include <functional>

namespace genesis {
namespace utils {

// =================================================================================================
//     Link Functions
// =================================================================================================

struct GlmLink
{
    /**
     * @brief List of common GLM Link functions.
     */
    enum Link {
        kUnknown,
        kLogit,
        kLog,
        kIdentity,
        kInverse
    };

    /**
     * @brief Internal ID, used to check if the link is the canonical one for a distribution family.
     */
    Link id = kUnknown;

    /**
     * @brief Link function.
     */
    std::function<double( double mu )> link;

    /**
     * @brief Inverse of the link function.
     */
    std::function<double( double eta )> inverse_link;

    /**
     * @brief Derivative of the link function.
     */
    std::function<double( double mu )> derivative;

};

/**
 * @brief Check whether all necessary values and functors of a GlmLink are set.
 */
inline bool is_defined( GlmLink const& link )
{
    return link.link && link.inverse_link && link.derivative;
}

// =================================================================================================
//     Logit Link
// =================================================================================================

/**
 * @brief Logit link functions.
 *
 * The canonical family for logit is binomial, see glm_family_binomial().
 */
inline GlmLink glm_link_logit()
{
    GlmLink link;
    link.id = GlmLink::kLogit;
    link.link = []( double mu )
    {
        return std::log( mu / ( 1.0 - mu ));
    };
    link.inverse_link = []( double eta )
    {
        return std::exp( eta ) / ( 1.0 + std::exp( eta ));
    };
    link.derivative = []( double mu )
    {
        return 1.0 / ( mu * ( 1.0 - mu ));
    };
    return link;
}

// =================================================================================================
//     Log Link
// =================================================================================================

/**
 * @brief Log link functions.
 *
 * The canonical family for logit is poisson, see glm_family_poisson().
 */
inline GlmLink glm_link_log()
{
    GlmLink link;
    link.id = GlmLink::kLog;
    link.link = []( double mu )
    {
        return std::log( mu );
    };
    link.inverse_link = []( double eta )
    {
        return std::exp( eta );
    };
    link.derivative = []( double mu )
    {
        return 1.0 / mu;
    };
    return link;
}

// =================================================================================================
//     Identity Link
// =================================================================================================

/**
 * @brief Identity link functions.
 *
 * The canonical family for identiy is normal/gaussian, see glm_family_gaussian().
 */
inline GlmLink glm_link_identity()
{
    GlmLink link;
    link.id = GlmLink::kIdentity;
    link.link = []( double mu )
    {
        return mu;
    };
    link.inverse_link = []( double eta )
    {
        return eta;
    };
    link.derivative = []( double mu )
    {
        (void) mu;
        return 1.0;
    };
    return link;
}

// =================================================================================================
//     Inverse Link
// =================================================================================================

/**
 * @brief Inverse link functions.
 *
 * The canonical family for inverse is gamma, see glm_family_gamma().
 */
inline GlmLink glm_link_inverse()
{
    GlmLink link;
    link.id = GlmLink::kInverse;
    link.link = []( double mu )
    {
        return 1.0 / mu;
    };
    link.inverse_link = []( double eta )
    {
        return 1.0 / eta;
    };
    link.derivative = []( double mu )
    {
        return 1.0 / ( mu * mu );
    };
    return link;
}

} // namespace utils
} // namespace genesis

#endif // include guard
