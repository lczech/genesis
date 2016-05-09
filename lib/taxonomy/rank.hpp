#ifndef GENESIS_TAXONOMY_RANK_H_
#define GENESIS_TAXONOMY_RANK_H_

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

#include "taxonomy/taxonomy.hpp"

namespace genesis {
namespace taxonomy {

class Rank : public Taxonomy
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
     * @brief Default constructor. Does nothing.
     */
    Rank()
        : parent_(nullptr)
    {}

    /**
     * @brief Constructor that uses the given name for the Rank.
     */
    Rank( std::string name )
        : name_(name)
        , parent_(nullptr)
    {}

    /**
     * @brief Default destructor.
     */
    ~Rank() = default;

    Rank( Rank const& ) = default;
    Rank( Rank&& )      = default;

    Rank& operator= ( Rank const& ) = default;
    Rank& operator= ( Rank&& )      = default;

    void swap( Rank& other )
    {
        using std::swap;
        Taxonomy::swap( other );
        swap( name_,   other.name_ );
        swap( parent_, other.parent_ );
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    std::string const& name() const;
    void               name( std::string const& value );

    Rank const* parent () const;
    Rank*       parent ();

    // -------------------------------------------------------------------------
    //     Protected Implementation Details
    // -------------------------------------------------------------------------

protected:

    Rank& add_child_( Rank&& child ) override;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string name_;
    Rank*       parent_;
};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
