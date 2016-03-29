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

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS(sequence_codes_export, "sequence")
{

    boost::python::def(
        "amino_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::amino_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::amino_acid_text_colors ()")
    );

    boost::python::def(
        "nucleic_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::nucleic_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::nucleic_acid_text_colors ()")
    );

    boost::python::def(
        "amino_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_all ()")
    );

    boost::python::def(
        "amino_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_degenerated ()")
    );

    boost::python::def(
        "amino_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_plain ()")
    );

    boost::python::def(
        "amino_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_undetermined ()")
    );

    boost::python::def(
        "nucleic_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_all ()")
    );

    boost::python::def(
        "nucleic_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_degenerated ()")
    );

    boost::python::def(
        "nucleic_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_plain ()")
    );

    boost::python::def(
        "nucleic_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_undetermined ()")
    );

    boost::python::def(
        "translate_amino_acid",
        ( std::string ( * )( char ))( &::genesis::sequence::translate_amino_acid ),
        ( boost::python::arg("code") ),
        get_docstring("std::string ::genesis::sequence::translate_amino_acid (char code)")
    );

    boost::python::def(
        "translate_nucleic_acid",
        ( std::string ( * )( char ))( &::genesis::sequence::translate_nucleic_acid ),
        ( boost::python::arg("code") ),
        get_docstring("std::string ::genesis::sequence::translate_nucleic_acid (char code)")
    );
}
