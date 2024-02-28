/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_input_stream.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cstring>

namespace genesis {
namespace population {

// =================================================================================================
//     Private Members
// =================================================================================================

void VcfInputStream::increment_()
{
    // We need this function in a source file instead of the header,
    // solely to have the htslib functions not spam our namespace...
    // We could also work with data directly from our VcfRecord here instead,
    // but this would entail string copies, instead of just pointer copies;
    // sometimes, plain C has its advantages ;-)
    // (or we'd switch to C++17 with stringview instead, which would also solve the issue)

    assert( file_ );
    assert( record_ );

    // If needed, we check the correct order of chromosomes and positions in the input file.
    // To avoid making expensive temporary strings for this,
    // we simply use char pointers to the htslib record data directly here.
    char const* cur_chr = nullptr;
    size_t      cur_pos = 0;
    if( expect_ordered_ ) {
        cur_chr = ::bcf_hdr_id2name( record_->header().data(), record_->data()->rid );
        cur_pos = record_->data()->pos + 1;
    }

    // Read the next record. If this returns false, we are done.
    if( ! record_->read_next( *file_ )) {
        file_ = nullptr;
        return;
    }

    // Do the correct order check if needed.
    if( expect_ordered_ ) {
        assert( cur_chr != nullptr );
        assert( cur_pos > 0 );

        // Get the new chr/pos.
        char const* new_chr = ::bcf_hdr_id2name( record_->header().data(), record_->data()->rid );
        size_t      new_pos = record_->data()->pos + 1;

        // Check!
        if(
            ( strcmp( new_chr, cur_chr )  < 0 ) ||
            ( strcmp( new_chr, cur_chr ) == 0 && new_pos <= cur_pos )
        ) {
            throw std::runtime_error(
                "Malformed VCF file " + filename_ + ": " +
                "unordered chromosomes and positions going from " +
                std::string( cur_chr ) + ":" + std::to_string( cur_pos ) + " to " +
                std::string( new_chr ) + ":" + std::to_string( new_pos )
            );
        }
    }
}

} // namespace population
} // namespace genesis

#endif // htslib guard
