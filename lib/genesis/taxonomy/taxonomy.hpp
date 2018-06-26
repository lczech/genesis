#ifndef GENESIS_TAXONOMY_TAXONOMY_H_
#define GENESIS_TAXONOMY_TAXONOMY_H_

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

#include <string>
#include <list>

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

    typedef std::list<Taxon>::iterator             iterator;
    typedef std::list<Taxon>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Taxonomy()          = default;
    virtual ~Taxonomy() = default;

    /**
     * @brief Copy constructor.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly when copying.
     */
    Taxonomy( Taxonomy const& );

    /**
     * @brief Move constructor.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly when copying.
     */
    Taxonomy( Taxonomy&& );

    /**
     * @brief Copy assigment operator.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly when copying.
     */
    Taxonomy& operator= ( Taxonomy const& );

    /**
     * @brief Move assignment operator.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly when copying.
     */
    Taxonomy& operator= ( Taxonomy&& );

    friend void swap( Taxonomy& lhs, Taxonomy& rhs );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the number of immediate child @link Taxon Taxa@endlink.
     *
     * See total_taxa_count() for counting all Taxa, including all nested ones.
     */
    size_t size() const;

    /**
     * @brief Return whether an immediate child Taxon with the given name exists.
     */
    bool has_child ( std::string name ) const;

    /**
     * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
     */
    Taxon const& get_child ( std::string name ) const;

    /**
     * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
     */
    Taxon&       get_child ( std::string name );

    /**
     * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
     */
    Taxon const& operator [] ( std::string name ) const;

    /**
     * @brief Return the child Taxon with a given name if it exists, or throw otherwise.
     */
    Taxon&       operator [] ( std::string name );

    /**
     * @brief Return the child Taxon at the given index.
     *
     * The function throws an exception if the index in invalid, i.e., `>=` size().
     */
    Taxon const& at ( size_t index  ) const;

    /**
     * @brief Return the child Taxon at the given index.
     *
     * The function throws an exception if the index in invalid, i.e., `>=` size().
     */
    Taxon&       at ( size_t index  );

    /**
     * @brief Return the child Taxon at the given index.
     *
     * The function does not check whether the provided index is within the valid range of size().
     */
    Taxon const& operator [] ( size_t index ) const;

    /**
     * @brief Return the child Taxon at the given index.
     *
     * The function does not check whether the provided index is within the valid range of size().
     */
    Taxon&       operator [] ( size_t index );

    size_t index_of( std::string const& name ) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Add a child Taxon as a copy of a given Taxon and return it.
     *
     * If @p merge_duplicates is `true` (default), the Taxonomy is checked for a child Taxon with
     * the same name, and if one exists, it is recursively merged with the given Taxon.
     * Otherwise (@p merge_duplicates is `false`), the Taxon is added, even if this creates
     * another child Taxon with the same name.
     */
    Taxon& add_child( Taxon const&       child, bool merge_duplicates = true );

    /**
     * @brief Add a child Taxon by creating a new one with the given name and return it.
     *
     * If @p merge_duplicates is `true` (default), the Taxonomy is checked for a child Taxon with
     * the same name, and if one exists, nothing is added.
     * Otherwise (@p merge_duplicates is `false`), a new Taxon is added, even if this creates
     * another child Taxon with the same name.
     */
    Taxon& add_child( std::string const& name, bool merge_duplicates = true );

    /**
     * @brief Remove a child Taxon with a certain name.
     *
     * The Taxon (and all its sub-taxa) are destroyed.
     * Throws an `std::runtime_error` if there is no Taxon with the given name.
     */
    void remove_child( std::string const& name );

    /**
     * @brief Remove a child Taxon at a certain index.
     *
     * The Taxon (and all its sub-taxa) are destroyed.
     * Throws an `std::runtime_error` if the index is out of bounds.
     */
    void remove_at( size_t index );

    /**
     * @brief Remove all children.
     */
    void clear_children();

    /**
     * @brief Sort the taxonomy according to some compare criterion.
     */
    template <class Compare>
    void sort( Compare comp )
    {
        children_.sort( comp );
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    /**
     * @brief Return an iterator to the beginning of the child taxa.
     */
    iterator begin();

    /**
     * @brief Return an iterator to the end of the child taxa.
     */
    iterator end();

    /**
     * @brief Return a const iterator to the beginning of the child taxa.
     */
    const_iterator begin() const;

    /**
     * @brief Return a const iterator to the end of the child taxa.
     */
    const_iterator end() const;

    /**
     * @brief Return a const iterator to the beginning of the child taxa.
     */
    const_iterator cbegin() const;

    /**
     * @brief Return a const iterator to the end of the child taxa.
     */
    const_iterator cend() const;

    // -------------------------------------------------------------------------
    //     Internal Implementation Details
    // -------------------------------------------------------------------------

protected:

    /**
     * @brief Virtual implementation function for adding a child Taxon.
     *
     * This function is invoked by all add_child() functions in order to implement the non-virtual
     * interface pattern.
     *
     * It needs to be virtual because adding a child Taxon differs for Taxonomy and Taxon.
     * In the latter case, the additional @link Taxon::parent() parent @endlink property has to be set.
     * Thus, this function is overridden by Taxon, see Taxon::add_child_().
     *
     * If a child Taxon with the same name already exists, it is recursively merged with the given Taxon.
     * The function returns the child.
     */
    virtual Taxon& add_child_( Taxon const& child, bool merge_duplicates );

    /**
     * @brief Internal helper function that resets the parent pointer of all stored Taxa.
     *
     * This function is used whenever the `children_` container is changed (copy, move, add elements)
     * in order to make sure that all parent pointers are correct.
     *
     * As all constructors and assignment operators use this function, we also make sure that
     * adding or removing elements or changing their order (sort etc) does not break the correctness of
     * the parent pointers. This might be slightly inefficient if many such moves (and thus, reset
     * operations) are done in sequence. However, this is acceptable in order to keep the object
     * stable at any time.
     */
    void reset_parent_pointers_( Taxon* parent );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::list<Taxon> children_;
};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
