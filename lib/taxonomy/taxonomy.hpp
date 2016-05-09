#ifndef GENESIS_TAXONOMY_TAXONOMY_H_
#define GENESIS_TAXONOMY_TAXONOMY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

// ================================================================================================
//     Forward Declarations
// ================================================================================================

class Rank;

// ================================================================================================
//     Taxonomy
// ================================================================================================

class Taxonomy
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::vector<Rank>::iterator             iterator;
    typedef std::vector<Rank>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Taxonomy()          = default;
    virtual ~Taxonomy() = default;

    Taxonomy( Taxonomy const& ) = default;
    Taxonomy( Taxonomy&& )      = default;

    Taxonomy& operator= ( Taxonomy const& ) = default;
    Taxonomy& operator= ( Taxonomy&& )      = default;

    void swap( Taxonomy& other )
    {
        using std::swap;
        swap( children_, other.children_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const;

    bool has_child ( std::string name ) const;

    Rank const& get_child ( std::string name ) const;
    Rank&       get_child ( std::string name );

    Rank const& operator [] ( std::string name ) const;
    Rank&       operator [] ( std::string name );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    Rank& add_child( Rank const&        child );
    Rank& add_child( Rank&&             child );
    Rank& add_child( std::string const& name );

    void remove_child( std::string const& name );

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    // -------------------------------------------------------------------------
    //     Protected Implementation Details
    // -------------------------------------------------------------------------

protected:

    virtual Rank& add_child_( Rank&& child );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<Rank> children_;
};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
