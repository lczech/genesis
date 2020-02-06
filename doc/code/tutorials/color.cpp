/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/genesis.hpp"

int main()
{
    using namespace genesis::utils;

    // Some ways to obtian a color.
    Color c1 = Color( 1.0, 0.0, 1.0 );
    Color c2 = Color::from_bytes( 255, 0, 255 );
    Color c3 = Color::from_hex( "#FF00FF" );

    // Basic usage of a Color Map.
    auto color_map = ColorMap( color_list_viridis() );
    auto some_color = color_map( 0.3 );

    // Some exemplary values for log scaling.
    auto values = std::vector<double>{ 2.3, 43.5, 195.8, 566.0, 846.2 };
    auto color_norm = ColorNormalizationLogarithmic( 1.0, 1000.0);
    auto colors = color_map( color_norm, values );

    // Let's avoid ugly compiler warnings about unused variables.
    // Not part of the tutorial. Just part of being clean.
    (void) c1;
    (void) c2;
    (void) c3;
    (void) some_color;
}
