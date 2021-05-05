#ifndef GENESIS_POPULATION_FORMATS_HTS_FILE_H_
#define GENESIS_POPULATION_FORMATS_HTS_FILE_H_

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
 * @brief
 *
 * @file
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include <string>

extern "C" {
    // #include <htslib/hts.h>

    struct htsFile;
}

// =================================================================================================
//     HTS File
// =================================================================================================

namespace genesis {
namespace population {

/**
 * @brief Wrap an <code>\::htsFile</code> struct.
 *
 * This thin wrapper simply applies RAII to the htslib struct.
 */
class HtsFile
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create an empty instance, with no file attached.
     */
    HtsFile() = default;

    /**
     * @brief Create an instance that opens a file.
     *
     * For the @p mode param, see the <code>\::hts_open()</code> documentation of htslib.
     * By default, we open for read-only access.
     */
    explicit HtsFile(
        std::string const& file_name,
        std::string const& mode = "r"
    );

    ~HtsFile();

    HtsFile( HtsFile const& ) = delete;
    HtsFile( HtsFile&& other );

    HtsFile& operator= ( HtsFile const& ) = delete;
    HtsFile& operator= ( HtsFile&& other );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    std::string const& file_name() const
    {
        return file_name_;
    }

    ::htsFile* data()
    {
        return hts_file_;
    }

    /**
     * @brief Return a human-readable description of the file format.
     *
     * See the <code>\::hts_format_description()</code> documentation of htslib for details.
     */
    std::string format_description() const;

    /**
     * @brief Return the file format extension.
     *
     * See the <code>\::hts_format_file_extension()</code> documentation of htslib for details.
     */
    std::string format_extension() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string file_name_ = "";
    ::htsFile*  hts_file_ = nullptr;
};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
