#ifndef GENESIS_TAXONOMY_TAXOPATH_H_
#define GENESIS_TAXONOMY_TAXOPATH_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include <functional>
#include <stdexcept>
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
 * This class serves as an intermediate broker between a taxonomic path string and a Taxon.
 * It is useful to transition between a Taxonomy and its string representation, for storing it
 * in a text file or some other database format. It is also useful for looking up certain Taxa
 * in a Taxonomy by using their taxonomic path.
 *
 * It furthermore offers all comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`),
 * which behave naturally for sorting a set of Taxopath%s (akin to `std::lexicographical_compare`).
 *
 * See the TaxopathParser and TaxopathGenerator classes for populating a Taxopath and for
 * turning it back into a string, respectively.
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

    /**
     * @brief Fill constructor that uses the given strings to initialize the taxonomic elements.
     *
     * The strings are copied and used as Taxopath elements in the order in which they are stored
     * in the vector.
     */
    explicit Taxopath( std::vector< std::string > const& elements )
        : elements_( elements )
    {}

    /**
     * @brief Fill constructor that uses the given strings to initialize the taxonomic elements.
     *
     * The strings are moved and used as Taxopath elements in the order in which they are stored
     * in the vector.
     */
    explicit Taxopath( std::vector< std::string > &&     elements )
        : elements_( std::move( elements ))
    {}


    Taxopath( Taxopath const& ) = default;
    Taxopath( Taxopath&& )      = default;

    Taxopath& operator= ( Taxopath const& ) = default;
    Taxopath& operator= ( Taxopath&& )      = default;

    /**
     * @brief Swap the elements of two Taxopath%s.
     */
    void swap( Taxopath& other )
    {
        using std::swap;
        swap( elements_, other.elements_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the Taxopath is empty, i.e., does not contain any elements.
     */
    bool empty() const
    {
        return elements_.empty();
    }

    /**
     * @brief Return the number of elements of this Taxopath.
     */
    size_t size() const
    {
        return elements_.size();
    }

    /**
     * @brief Return the taxonomic element at a certain position.
     *
     * The function throws if the given index is not within the size of the taxonomic elements.
     */
    std::string const& at ( size_t index ) const
    {
        return elements_.at( index );
    }

    /**
     * @brief Return the taxonomic element at a certain position.
     *
     * The function throws if the given index is not within the size of the taxonomic elements.
     */
    std::string& at ( size_t index )
    {
        return elements_.at( index );
    }

    /**
     * @brief Return the taxonomic element at a certain position.
     *
     * The function does not check whether the given index is valid.
     */
    std::string const& operator [] ( size_t index ) const
    {
        return elements_[ index ];
    }

    /**
     * @brief Return the taxonomic element at a certain position.
     *
     * The function does not check whether the given index is valid.
     */
    std::string& operator [] ( size_t index )
    {
        return elements_[ index ];
    }

    /**
     * @brief Return the elements of the Taxopath as a vector of strings.
     */
    std::vector< std::string > const& elements() const
    {
        return elements_;
    }

    /**
     * @brief Return the elements of the Taxopath as a vector of strings.
     */
    std::vector< std::string > & elements()
    {
        return elements_;
    }


    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Replace the current elements of the Taxopath by a list of strings.
     *
     * The strings are copied and used as Taxopath elements in the order in which they are stored
     * in the vector.
     */
    void assign( std::vector< std::string > const& from )
    {
        elements_ = from;
    }

    /**
     * @brief Add an element to the end of the Taxopath by copying it.
     */
    void push_back( std::string const& value )
    {
        elements_.push_back( value );
    }

    /**
     * @brief Add an element to the end of the Taxopath by moving it.
     */
    void push_back( std::string&& value )
    {
        elements_.push_back( std::move( value ));
    }

    /**
     * @brief Remove the last element of the Taxopath and return its value.
     *
     * The returned value is obtained as a copy of the last element before it is removed.
     */
    std::string pop_back()
    {
        if( elements_.empty() ) {
            throw std::out_of_range( "Cannot pop last element of empty Taxopath." );
        }
        auto last = elements_.back();
        elements_.pop_back();
        return last;
    }

    /**
     * @brief Clear all taxonomic elements. This results in an empty Taxopath.
     */
    void clear()
    {
        elements_.clear();
    }

    // -------------------------------------------------------------------------
    //     Comparison
    // -------------------------------------------------------------------------

    friend bool operator == ( Taxopath const & lhs, Taxopath const & rhs )
    {
        return lhs.elements_ == rhs.elements_;
    }

    friend bool operator != ( Taxopath const & lhs, Taxopath const & rhs )
    {
        return lhs.elements_ != rhs.elements_;
    }

    friend bool operator < ( Taxopath const & lhs, Taxopath const & rhs )
    {
        return lhs.elements_ < rhs.elements_;
    }

    friend bool operator <= ( Taxopath const & lhs, Taxopath const & rhs )
    {
        return lhs.elements_ <= rhs.elements_;
    }

    friend bool operator > ( Taxopath const & lhs, Taxopath const & rhs )
    {
        return lhs.elements_ > rhs.elements_;
    }

    friend bool operator >= ( Taxopath const & lhs, Taxopath const & rhs )
    {
        return lhs.elements_ >= rhs.elements_;
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    /**
     * @brief Return an iterator to the beginning of the taxonomic elements.
     */
    iterator begin()
    {
        return elements_.begin();
    }

    /**
     * @brief Return an iterator to the end of the taxonomic elements.
     */
    iterator end()
    {
        return elements_.end();
    }

    /**
     * @brief Return a const iterator to the beginning of the taxonomic elements.
     */
    const_iterator begin() const
    {
        return elements_.cbegin();
    }

    /**
     * @brief Return a const iterator to the end of the taxonomic elements.
     */
    const_iterator end() const
    {
        return elements_.cend();
    }

    /**
     * @brief Return a reverse iterator to the reverse beginning of the taxonomic elements.
     */
    reverse_iterator rbegin()
    {
        return elements_.rbegin();
    }

    /**
     * @brief Return a reverse iterator to the reverse end of the taxonomic elements.
     */
    reverse_iterator rend()
    {
        return elements_.rend();
    }

    /**
     * @brief Return a const reverse iterator to the reverse beginning of the taxonomic elements.
     */
    const_reverse_iterator rbegin() const
    {
        return elements_.crbegin();
    }

    /**
     * @brief Return a const reverse iterator to the reverse end of the taxonomic elements.
     */
    const_reverse_iterator rend() const
    {
        return elements_.crend();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector< std::string > elements_;
};

} // namespace taxonomy
} // namespace genesis

// ================================================================================================
//     Standard Hash Function
// ================================================================================================

namespace std
{
    /**
     * @brief Hash function for Taxopath.
     *
     * We just forward to the hash of the internal data of a Taxopath, that is, we hash its strings.
     */
    template<>
    struct hash<genesis::taxonomy::Taxopath>
    {
        using argument_type = genesis::taxonomy::Taxopath;
        using result_type   = std::size_t;

        result_type operator()( argument_type const& t ) const {
            result_type result = 0;
            for( auto const& e : t ) {
                result ^= std::hash<std::string>{}( e );
            }
            return result;
        }
    };
}

#endif // include guard
