#pragma once

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech

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

// We use this header as input to Binder, to pull in additonal headers
// that instantiate templates as needed.
// The headers here could be part of the all_includes header as well,
// but keeping it separate seems a bit cleaner.
// For even more order and modularity, we here simply pull in per-namespace
// headers, so that we can keep the scope of each of them relatively small.

#include <genesis/util/container/bitpacked_vector_instances.hpp>
// #include <genesis/util/container/matrix_instances.hpp>
#include <genesis/util/container/mru_cache_instances.hpp>
#include <genesis/util/math/compensated_sum_instances.hpp>
