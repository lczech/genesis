#ifndef GENESIS_TAXONOMY_TAXOPATH_H_
#define GENESIS_TAXONOMY_TAXOPATH_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include <string>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Taxopath
// =================================================================================================

/**
 * @brief Helper class to store a taxonomic path.
 *
 * We call a string of the form
 *
 *     Animalia;Vertebrata;Mammalia;Carnivora
 *
 * a taxonomic path. Those strings are often used in taxonomic databases, and usually
 * use semicola to separate their parts.
 *
 * This class stores such a string, splitted into its single elements. Each such element can be
 * seen as a Taxon; thus, a Taxopath can be added to a Taxonomy.
 *
 * For example, the above taxonomic path would be stored as
 *
 *     [ "Animalia", "Vertebrata", "Mammalia", "Carnivora" ]
 *
 * Each of those strings is then called a taxonomic path element.
 *
 * This class is mainly an intermediate broker between a taxonomic path string and a Taxon.
 * It is useful to transition between a Taxonomy and its string representation, for storing it
 * in a text file or some other database format. It is also useful for looking up certain Taxa
 * in a Taxonomy by using their taxonomic path.
 *
 * See the TaxopathParser and TaxopathGenerator classes for populating a Taxopath and for
 * turning it back into a string, respectively.
 *
 * Internally, it is a thin wrapper for a vector of strings. This might be a bit overengineered for
 * now, but offers expansion that might become necessary in the future.
 */
class Taxopath
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::vector< std::string >::iterator               iterator;
    typedef std::vector< std::string >::const_iterator         const_iterator;
    typedef std::vector< std::string >::reverse_iterator       reverse_iterator;
    typedef std::vector< std::string >::const_reverse_iterator const_reverse_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Taxopath()  = default;
    ~Taxopath() = default;

    explicit Taxopath( std::vector< std::string > const& elements );
    explicit Taxopath( std::vector< std::string > &&     elements );

    Taxopath( Taxopath const& ) = default;
    Taxopath( Taxopath&& )      = default;

    Taxopath& operator= ( Taxopath const& ) = default;
    Taxopath& operator= ( Taxopath&& )      = default;

    void swap( Taxopath& other );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    bool empty() const;
    size_t size() const;

    std::string const& at ( size_t index ) const;
    std::string&       at ( size_t index );

    std::string const& operator [] ( size_t index ) const;
    std::string&       operator [] ( size_t index );

    std::vector< std::string > const& elements() const;
    std::vector< std::string > &      elements();

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void assign( std::vector< std::string > const& from );

    void push_back( std::string const& value );
    void push_back( std::string&& value );

    std::string pop_back();

    void clear();

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin() noexcept;
    iterator end() noexcept;

    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;

    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector< std::string > elements_;
};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
