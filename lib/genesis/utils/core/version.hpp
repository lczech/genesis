#ifndef GENESIS_UTILS_CORE_VERSION_H_
#define GENESIS_UTILS_CORE_VERSION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @brief Some stuff that is totally not imporatant, but nice.
 *
 * @file
 * @ingroup utils
 */

#include <cstdint>
#include <string>

/**
 * @namespace genesis
 *
 * @brief Container namespace for all symbols of genesis in order to keep them
 * separate when used as a library.
 */
namespace genesis {

// =================================================================================================
//     Genesis
// =================================================================================================

/**
 * @brief Return the current genesis version.
 *
 * We use [semantic versioning](http://semver.org/) 2.0.0 for genesis.
 *
 * Given a version number MAJOR.MINOR.PATCH, increment the:
 *
 *  *  MAJOR version when you make incompatible API changes,
 *  *  MINOR version when you add functionality in a backwards-compatible manner, and
 *  *  PATCH version when you make backwards-compatible bug fixes.
 *
 * Additional labels for pre-release and build metadata are available as extensions to the
 * MAJOR.MINOR.PATCH format.
 */
inline std::string genesis_version()
{
    // The following line is automatically replaced by the deploy scripts. Do not change manually.
    return "v0.33.0"; // #GENESIS_VERSION#
}

/**
 * @brief Return the current genenis version name.
 *
 * Just for fun.
 */
inline std::string genesis_version_name()
{
    // The following line is automatically replaced by the deploy scripts. Do not change manually.
    return "Phase Change"; // #GENESIS_VERSION_NAME#
}

/**
 * @brief Return the URL of the genesis home page.
 */
inline std::string genesis_url()
{
    return "http://genesis-lib.org";
}

/**
 * @brief Return the header for genesis.
 *
 * This is simply a text version of the logo, including the current version. It can for example be
 * displayed at the start of a program to indicate that this program uses genesis.
 */
inline std::string genesis_header()
{
    return "\
                                     ,     \n\
        __    __    __    __   __     __   \n\
      /   ) /___) /   ) /___) (_ ` / (_ `  \n\
     (___/ (___  /   / (___  (__) / (__)   \n\
      __/______________________________    \n\
    (__/                                   \n\
               2014-2025 by Lucas Czech    \n\
               " + genesis_url()     +    "\n\
               " + genesis_version() +    "\n";
}

/**
 * @brief Return the genesis license boilerplate information.
 *
 * This function is useful for programs with terminal interaction. In such cases, you should include
 * a command to show this license information.
 */
inline std::string genesis_license()
{
    return "\
    Genesis - A toolkit for working with phylogenetic data.\n\
    Copyright (C) 2014-2025 Lucas Czech\n\
    \n\
    This program is free software: you can redistribute it and/or modify\n\
    it under the terms of the GNU General Public License as published by\n\
    the Free Software Foundation, either version 3 of the License, or\n\
    (at your option) any later version.\n\
    \n\
    This program is distributed in the hope that it will be useful,\n\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
    GNU General Public License for more details.\n\
    \n\
    You should have received a copy of the GNU General Public License\n\
    along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\
    \n\
    Contact:\n\
    Lucas Czech <lucas.czech@sund.ku.dk>\n\
    University of Copenhagen, Globe Institute, Section for GeoGenetics\n\
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark\n";
}

inline std::string ee(int r)
{
    uint64_t x[4] = {1198840465960072866,1198609267608314688,1376216421886990656,1545107134173456};
    std::string s; for(int i=0;i<(2*r)/3;++i) { s += (((x[(i/7)%4]/r)>>((i%7)*8))%256); } return s;
}

} // namespace genesis

#endif // include guard
