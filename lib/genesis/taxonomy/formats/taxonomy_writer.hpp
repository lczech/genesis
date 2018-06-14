#ifndef GENESIS_TAXONOMY_FORMATS_TAXONOMY_WRITER_H_
#define GENESIS_TAXONOMY_FORMATS_TAXONOMY_WRITER_H_

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
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/formats/taxopath_generator.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxonomy Writer
// =================================================================================================

/**
 * @brief Write a Taxonomy as a list of Taxopaths.
 */
class TaxonomyWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TaxonomyWriter() = default;
    ~TaxonomyWriter() = default;

    TaxonomyWriter( TaxonomyWriter const& ) = default;
    TaxonomyWriter( TaxonomyWriter&& )      = default;

    TaxonomyWriter& operator= ( TaxonomyWriter const& ) = default;
    TaxonomyWriter& operator= ( TaxonomyWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    void        to_stream ( Taxonomy const& tax, std::ostream&      os ) const;
    void        to_file   ( Taxonomy const& tax, std::string const& fn ) const;
    std::string to_string ( Taxonomy const& tax ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

public:

    TaxopathGenerator& taxopath_generator()
    {
        return tax_gen_;
    }

    TaxonomyWriter& write_ids( bool value )
    {
        write_ids_ = value;
        return *this;
    }

    bool write_ids() const
    {
        return write_ids_;
    }

    TaxonomyWriter& write_ranks( bool value )
    {
        write_ranks_ = value;
        return *this;
    }

    bool write_ranks() const
    {
        return write_ranks_;
    }

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    bool write_ids_   = true;
    bool write_ranks_ = true;

    TaxopathGenerator tax_gen_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
