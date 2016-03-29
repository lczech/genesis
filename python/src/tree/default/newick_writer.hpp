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
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_DefaultTreeNewickWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNewickWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreeNewickWriterMixinType = DefaultTreeNewickWriterMixin<typename Base>;

    boost::python::class_< DefaultTreeNewickWriterMixinType > ( name.c_str() )

        // Public Member Functions

        .def(
            "enable_branch_lengths",
            ( bool ( DefaultTreeNewickWriterMixinType::* )(  ))( &DefaultTreeNewickWriterMixinType::enable_branch_lengths )
        )
        .def(
            "enable_branch_lengths",
            ( void ( DefaultTreeNewickWriterMixinType::* )( bool ))( &DefaultTreeNewickWriterMixinType::enable_branch_lengths ),
            ( boost::python::arg("value") )
        )
        .def(
            "enable_names",
            ( bool ( DefaultTreeNewickWriterMixinType::* )(  ))( &DefaultTreeNewickWriterMixinType::enable_names )
        )
        .def(
            "enable_names",
            ( void ( DefaultTreeNewickWriterMixinType::* )( bool ))( &DefaultTreeNewickWriterMixinType::enable_names ),
            ( boost::python::arg("value") )
        )
    ;
}
