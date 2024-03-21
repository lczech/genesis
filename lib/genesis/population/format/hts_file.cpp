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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include "genesis/population/format/hts_file.hpp"

extern "C" {
    #include <htslib/hts.h>
}

#include <cstdlib>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

HtsFile::HtsFile(
    std::string const& file_name,
    std::string const& mode
)
    : file_name_( file_name )
    , hts_file_( ::hts_open( file_name.c_str(), mode.c_str() ))
{
    // Check if file opening worked.
    if( ! hts_file_ ) {
        throw std::runtime_error( "Failed to open htslib file " + file_name );
    }
}

HtsFile::~HtsFile()
{
    if( hts_file_ ) {
        ::hts_close( hts_file_ );
    }
}

HtsFile::HtsFile( HtsFile&& other )
{
    // We need a custom move, as the explicitly defaulted one "moves" the hts_file_ pointer, which
    // effectively copies it, and then upon destruction of the moved-from object frees the hts_file_.
    // So, instead we swap, so that once `other` gets destroyed (as it is moved from, it will go
    // out of scope soon), our current data of `this` gets also destroyed with it.
    std::swap( file_name_, other.file_name_ );
    std::swap( hts_file_, other.hts_file_ );
}

HtsFile& HtsFile::operator= ( HtsFile&& other )
{
    // Same reasoning as above. Need additional check for self assignment,
    // as otherwise we'd destroy the header as well.
    if( this == &other ) {
        return *this;
    }
    std::swap( file_name_, other.file_name_ );
    std::swap( hts_file_, other.hts_file_ );
    return *this;
}

// =================================================================================================
//     Accessors
// =================================================================================================

std::string HtsFile::format_description() const
{
    ::htsFormat const* fmt_ptr = ::hts_get_format( hts_file_ );
    char* fmt_str = ::hts_format_description( fmt_ptr );
    auto const ret = std::string( fmt_str );
    free( fmt_str );
    return ret;
}

std::string HtsFile::format_extension() const
{
    ::htsFormat const* fmt_ptr = ::hts_get_format( hts_file_ );
    return std::string( ::hts_format_file_extension( fmt_ptr ));
}

} // namespace population
} // namespace genesis

#endif // htslib guard
