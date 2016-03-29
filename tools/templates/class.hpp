#ifndef GENESIS_###CAPMODULNAME###_###CAPFILENAME###_H_
#define GENESIS_###CAPMODULNAME###_###CAPFILENAME###_H_

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
 * @brief
 *
 * @file
 * @ingroup ###MODULNAME###
 */

namespace genesis {

// =================================================================================================
//     ###CLASSNAME###
// =================================================================================================

/**
 * @brief
 */
###TEMPLATE_LINE###template <###TEMPLATE_PARAM###>
class ###CLASSNAME###
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    ###CLASSNAME###()  = default;
    ~###CLASSNAME###() = default;

    ###CLASSNAME###( ###CLASSNAME### const& ) = default;
    ###CLASSNAME###( ###CLASSNAME###&& )      = default;

    ###CLASSNAME###& operator= ( ###CLASSNAME### const& ) = default;
    ###CLASSNAME###& operator= ( ###CLASSNAME###&& )      = default;

    void swap( ###CLASSNAME###& other )
    {
        using std::swap;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

};

} // namespace genesis
###TEMPLATE_START###
// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "###MODULNAME###/###FILENAME###.tpp"###TEMPLATE_END###

#endif // include guard
