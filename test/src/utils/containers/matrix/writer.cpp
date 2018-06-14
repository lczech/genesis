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
 * @brief Testing Matrix class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <sstream>

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/reader.hpp"
#include "genesis/utils/containers/matrix/writer.hpp"

using namespace genesis;
using namespace utils;

// ================================================================================================
//     Reader
// ================================================================================================

TEST( Matrix, Writer )
{
    NEEDS_TEST_DATA;

    // Read simple matrix.
    // auto reader = MatrixReader<double>( ' ' );
    // auto const simple = reader.from_file( environment->data_dir + "utils/matrix/simple.mat" );
    //
    // auto writer = MatrixWriter<double>();

    // writer.to_file( simple, "/home/lucas/blank.mat" );
    // writer.to_file( simple, "/home/lucas/rows.mat",
    //     { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j" }
    // );
    // writer.to_file( simple, "/home/lucas/both.mat",
    //     { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j" },
    //     { "A", "B", "C" }
    // );
    // writer.to_file( simple, "/home/lucas/full.mat",
    //     { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j" },
    //     { "A", "B", "C" },
    //     "x"
    // );
}
