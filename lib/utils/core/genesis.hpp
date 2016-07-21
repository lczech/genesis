#ifndef GENESIS_UTILS_CORE_GENESIS_H_
#define GENESIS_UTILS_CORE_GENESIS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Some stuff that is totally not imporatant, but nice.
 *
 * @file
 * @ingroup utils
 */

#include <stdint.h>
#include <string>

/**
 * @namespace genesis
 *
 * @brief Container namespace for all symbols of genesis in order to keep them
 * separate when used as a library.
 */
namespace genesis {

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
    return "v0.8.0"; // #GENESIS_VERSION#
}

/**
 * @brief Return the URL of the genesis home page.
 */
inline std::string genesis_url()
{
    return "http://genesis-lib.org/";
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
               2014-2016 by Lucas Czech    \n\
               " + genesis_version() +    "\n\
               " + genesis_url()     +    "\n";
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
    Copyright (C) 2014-2016 Lucas Czech\n\
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
    Lucas Czech <lucas.czech@h-its.org>\n\
    Exelixis Lab, Heidelberg Institute for Theoretical Studies\n\
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany\n";
}

} // namespace genesis

#endif // include guard
