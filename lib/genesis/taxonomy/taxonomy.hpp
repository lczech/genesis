#ifndef GENESIS_TAXONOMY_TAXONOMY_H_
#define GENESIS_TAXONOMY_TAXONOMY_H_

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
void swap( Taxonomy& lhs, Taxonomy& rhs );

// =================================================================================================
//     Taxonomy
// =================================================================================================

/**
 * @brief Store a Taxonomy, i.e., a nested hierarchy of @link Taxon Taxa@endlink.
 *
 * We call a string of the form
 *
 *     Animalia;Vertebrata;Mammalia;Carnivora
 *
 * a taxonomic path string. Those strings are often found in taxonomic
 * databases, and usually use semicola to separete their parts. Each part of such a string is
 * called a Taxon, and can have a rank associated with it. See Taxopath for details on the format.
 *
 * In the example above, the rank associations could be
 *
 *     Kingdom: Animalia
 *     Phylum:  Vertebrata
 *     Class:   Mammalia
 *     Order:   Carnivora
 *
 * We use the term "taxon" to refer to one element in such a string, and model this in the
 * Taxon class. Both the Taxonomy and Taxon classes work with just those parts of the string.
 * Each Taxon can itself contain further lower level Taxa, resulting in a hierarchy.
 *
 * The above taxonomic path string for example would give a hierarchy of Taxa like this
 *
 *     Animalia
 *         Vertebrata
 *             Mammalia
 *                 Carnivora
 *
 * where each line is one Taxon, stored within their parent Taxa. A Taxonomy is the uppermost
 * parent of the Taxa in the hierarchy.
 *
 * There are functions to work with taxonomic path strings and Taxopath%s directly,
 * for example to "translate" them into a Taxonomy or to find a Taxon given a Taxopath.
 * See the namespace taxonomy for a list of those functions.
 *
 * In a sense, each Taxon is itself a Taxonomy, because of their hierarchical relationship.
 * However, we use the distinction between the two in order to separate concerns.
 * That means, only the Taxonomy should be seen as the top level of the hierarchy.
 *
 * This class serves as a container for storing a list of @link Taxon Taxa@endlink. It allows to
 * @link add_child( std::string const& ) add@endlink, @link remove_child() remove @endlink and
 * @link get_child() get @endlink Taxa by their name, as well as iterating over them.
 */
class Taxonomy
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::vector<Taxon>::iterator             iterator;
    typedef std::vector<Taxon>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Taxonomy()          = default;
    virtual ~Taxonomy() = default;

    Taxonomy( Taxonomy const& );
    Taxonomy( Taxonomy&& );

    Taxonomy& operator= ( Taxonomy const& );
    Taxonomy& operator= ( Taxonomy&& );

    friend void swap( Taxonomy& lhs, Taxonomy& rhs );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const;

    bool has_child ( std::string name ) const;

    Taxon const& get_child ( std::string name ) const;
    Taxon&       get_child ( std::string name );

    Taxon const& operator [] ( std::string name ) const;
    Taxon&       operator [] ( std::string name );

    Taxon const& at ( size_t index  ) const;
    Taxon&       at ( size_t index  );

    Taxon const& operator [] ( size_t index ) const;
    Taxon&       operator [] ( size_t index );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    Taxon& add_child( Taxon const&       child );
    Taxon& add_child( std::string const& name );

    void remove_child( std::string const& name );
    void clear_children();

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
    //     Internal Implementation Details
    // -------------------------------------------------------------------------

protected:

    virtual Taxon& add_child_( Taxon const& child );

    void reset_parent_pointers_( Taxon* parent );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<Taxon> children_;
};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
