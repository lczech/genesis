#ifndef GENESIS_POPULATION_H_
#define GENESIS_POPULATION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief This header includes all headers in 'population'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/gff_reader.hpp"
#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/simple_pileup_input_iterator.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/formats/sync_input_iterator.hpp"
#include "genesis/population/formats/sync_reader.hpp"
#include "genesis/population/formats/vcf_common.hpp"
#include "genesis/population/formats/vcf_format_iterator.hpp"
#include "genesis/population/formats/vcf_header.hpp"
#include "genesis/population/formats/vcf_input_iterator.hpp"
#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/functions/diversity.hpp"
#include "genesis/population/functions/genome_heatmap.hpp"
#include "genesis/population/functions/genome_region.hpp"
#include "genesis/population/functions/heatmap_colorization.hpp"
#include "genesis/population/functions/structure.hpp"
#include "genesis/population/functions/variant.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/window/af_spectrum.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/sliding_window_iterator.hpp"
#include "genesis/population/window/vcf_window.hpp"
#include "genesis/population/window/window.hpp"

#endif // include guard
