#ifndef GENESIS_SEQUENCE_H_
#define GENESIS_SEQUENCE_H_

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
 * @brief This header includes all headers in 'sequence'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include "genesis/sequence/counts.hpp"
#include "genesis/sequence/formats/fai_input_stream.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/formats/fasta_writer.hpp"
#include "genesis/sequence/formats/fastq_reader.hpp"
#include "genesis/sequence/formats/fastq_writer.hpp"
#include "genesis/sequence/formats/fastx_input_stream.hpp"
#include "genesis/sequence/formats/fastx_input_view_stream.hpp"
#include "genesis/sequence/formats/fastx_output_stream.hpp"
#include "genesis/sequence/formats/phylip_reader.hpp"
#include "genesis/sequence/formats/phylip_writer.hpp"
#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/functions/consensus.hpp"
#include "genesis/sequence/functions/dict.hpp"
#include "genesis/sequence/functions/entropy.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/functions/labels.hpp"
#include "genesis/sequence/functions/quality.hpp"
#include "genesis/sequence/functions/signatures.hpp"
#include "genesis/sequence/functions/signature_specifications.hpp"
#include "genesis/sequence/functions/stats.hpp"
#include "genesis/sequence/kmer/alphabet.hpp"
#include "genesis/sequence/kmer/bitfield.hpp"
#include "genesis/sequence/kmer/canonical_encoding.hpp"
#include "genesis/sequence/kmer/extractor.hpp"
#include "genesis/sequence/kmer/function.hpp"
#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/sequence/kmer/microvariant_scanner.hpp"
#include "genesis/sequence/printers/bitmap.hpp"
#include "genesis/sequence/printers/simple.hpp"
#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/sequence/sequence_set.hpp"

#endif // include guard
