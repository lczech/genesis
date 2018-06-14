#ifndef GENESIS_TAXONOMY_TAXON_H_
#define GENESIS_TAXONOMY_TAXON_H_

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
#include "genesis/taxonomy/taxon_data.hpp"

#include <memory>
#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Taxon;
void swap( Taxon& lhs, Taxon& rhs );

// =================================================================================================
//     Taxon
// =================================================================================================

/**
 * @brief Store a Taxon, i.e., an element in a Taxonomy, with its name, rank, ID and sub-taxa.
 *
 * This class models a taxon within the hierarchy of a Taxonomy. Each such taxon can have a
 * @link name() name@endlink assigned to it, a @link rank() taxonomic rank@endlink,
 * an @link id() ID@endlink, and an arbitrarily nested set of sub-taxa.
 * The name, rank, and ID are free-form strings which can have any value.
 *
 * The name is one part of a taxonomic path string. For example, given the string
 *
 *     Animalia;Vertebrata;Mammalia;Carnivora
 *
 * a Taxon could have the name "Mammalia", and contain a Taxon with the name "Carnivora" as child.
 * See Taxonomy for details, see Taxopath for the taxonomic path string format.
 *
 * Each taxon is itself also a Taxonomy, in terms of class inheritance. This also makes some
 * biological sense, as a taxon can be seen as the taxonomy of its sub-taxa. We however only
 * consider the Taxonomy as the top level of the hierarchy. For more information, see there.
 * The class inheritance is mainly due to software design descisions, in order to make working
 * with @link Taxonomy Taxonomies@endlink and @link Taxon Taxa@endlink as easy and straight forward
 * as possible.
 */
class Taxon : public Taxonomy
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Taxonomy is a friend, as it needs access to parent_ in order to ensure consistency.
     */
    friend Taxonomy;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor. Does nothing. Yields an empty Taxon.
     */
    Taxon();

    /**
     * @brief Constructor that uses the given name for the Taxon.
     */
    explicit Taxon( std::string const& name, std::string const& rank = "", std::string const& id = "" );

    virtual ~Taxon() = default;

    /**
     * @brief Copy constructor.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly when copying and to copy the data.
     *
     * Copying first sets the parent() pointer to `nullptr`. This is because we might want to get a
     * 'blank' copy, i.e., a Taxon that is not attached to a Taxonomy. This way, the functions
     * expecting a parent pointer don't get a potentially invalid pointer.
     *
     * If however we copy a Taxon into a Taxonomy (or into some other Taxon), reset_parent_pointers_()
     * is called later anyway, which then sets the parent pointer to the correct value.
     */
    Taxon( Taxon const& );

    /**
     * @brief Move constructor.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly, and to treat the data correctlty when copying.
     */
    Taxon( Taxon&& );

    /**
     * @brief Copy assignment operator.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly, and to treat the data correctlty when copying.
     *
     * See the @link Taxon( Taxon const& other ) move constructor@endlink for details.
     */
    Taxon& operator= ( Taxon const& );

    /**
     * @brief Move assignment operator.
     *
     * We need a custom version of this in order to set the Taxon::parent() pointers of all children
     * correctly, and to treat the data correctlty when copying.
     */
    Taxon& operator= ( Taxon&& );

    /**
     * @brief Swapperator for Taxon.
     */
    friend void swap( Taxon& lhs, Taxon& rhs );

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Return the name of this taxon.
     */
    std::string const& name() const;

    /**
     * @brief Set the name of this taxon.
     */
    void               name( std::string const& value );

    /**
     * @brief Return the rank of this taxon.
     */
    std::string const& rank() const;
    void               rank( std::string const& value );

    /**
     * @brief Set the ID of this taxon.
     */
    std::string const& id() const;
    void               id( std::string const& value );

    /**
     * @brief Return a pointer to the parent of this taxon, or a `nullptr` if this is the top level taxon.
     */
    Taxon const* parent () const;

    /**
     * @brief Return a pointer to the parent of this taxon, or a `nullptr` if this is the top level taxon.
     */
    Taxon*       parent ();

    // -------------------------------------------------------------------------
    //     Data
    // -------------------------------------------------------------------------

    /**
     * @brief Return `true` if the Taxon has a data object assigned to it.
     */
    bool has_data() const;

    template< class TaxonDataType >
    TaxonDataType& data()
    {
        return dynamic_cast< TaxonDataType& >( *data_ );
    }

    template< class TaxonDataType >
    TaxonDataType const& data() const
    {
        return dynamic_cast< TaxonDataType const& >( *data_ );
    }

    template< class TaxonDataType >
    TaxonDataType* data_cast()
    {
        return dynamic_cast< TaxonDataType* >( data_.get() );
    }

    template< class TaxonDataType >
    TaxonDataType const* data_cast() const
    {
        return dynamic_cast< TaxonDataType const* >( data_.get() );
    }

    /**
     * @brief Return a pointer to the data.
     *
     * In most cases, using data<>() is more convenient. However, in some cases, this function
     * might be necessary.
     */
    BaseTaxonData*       data_ptr();

    /**
     * @brief Return a const pointer to the data.
     *
     * In most cases, using data<>() is more convenient. However, in some cases, this function
     * might be necessary.
     */
    BaseTaxonData const* data_ptr() const;

    /**
     * @brief Reset the data pointer of this Taxon.
     *
     * Using this function, a Taxon can be assigend new data. It is also possible to change the
     * data type completely (as long as it derives from BaseTaxonData). Be however aware that many
     * functions that work with a Taxonomy expect a certain data type. Thus, changing it might break
     * those functions and lead to exceptions and other errors.
     */
    Taxon& reset_data( std::unique_ptr< BaseTaxonData > data );

    // -------------------------------------------------------------------------
    //     Protected Implementation Details
    // -------------------------------------------------------------------------

protected:

    /**
     * @brief Virtual implementation function for adding a child taxon.
     *
     * See Taxonomy::add_child_() for details. In addition to the base class implementation, this
     * function also sets the parent pointer of the Taxon.
     */
    Taxon& add_child_( Taxon const& child, bool merge_duplicates ) override;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string name_;
    std::string rank_;
    std::string id_;

    Taxon*      parent_;

    std::unique_ptr< BaseTaxonData > data_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
