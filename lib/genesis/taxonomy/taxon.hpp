#ifndef GENESIS_TAXONOMY_TAXON_H_
#define GENESIS_TAXONOMY_TAXON_H_

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
 * @brief Store a Taxon, i.e., an element in a Taxonomy, with its name, rank and sub-taxa.
 *
 * This class models a taxon within the hierarchy of a Taxonomy. Each such taxon can have a
 * @link name() name@endlink assigned to it, a @link rank() taxonomic rank@endlink, and an
 * arbitrarily nested set of sub-taxa.
 * The name and the rank are both free-form strings which can have any value.
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

    Taxon();
    Taxon( std::string const& name );

    virtual ~Taxon() = default;

    Taxon( Taxon const& );
    Taxon( Taxon&& );

    Taxon& operator= ( Taxon const& );
    Taxon& operator= ( Taxon&& );

    friend void swap( Taxon& lhs, Taxon& rhs );

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    std::string const& name() const;
    void               name( std::string const& value );

    std::string const& rank() const;
    void               rank( std::string const& value );

    Taxon const* parent () const;
    Taxon*       parent ();

    // -------------------------------------------------------------------------
    //     Data
    // -------------------------------------------------------------------------

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

    BaseTaxonData*       data_ptr();
    BaseTaxonData const* data_ptr() const;

    Taxon& reset_data( std::unique_ptr< BaseTaxonData > data );

    // -------------------------------------------------------------------------
    //     Protected Implementation Details
    // -------------------------------------------------------------------------

protected:

    Taxon& add_child_( Taxon const& child ) override;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string name_;
    std::string rank_;

    Taxon*      parent_;

    std::unique_ptr< BaseTaxonData > data_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
