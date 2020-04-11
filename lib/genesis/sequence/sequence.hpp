#ifndef GENESIS_SEQUENCE_SEQUENCE_H_
#define GENESIS_SEQUENCE_SEQUENCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @ingroup sequence
 */

#include <string>
#include <vector>

namespace genesis {
namespace sequence {

class Sequence
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::string::iterator       iterator;
    typedef std::string::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Sequence() = default;

    Sequence( std::string const& label, std::string const& sites, size_t abundance = 1 )
        : label_(label)
        , sites_(sites)
        , abundance_(abundance)
    {}

    ~Sequence() = default;

    Sequence( Sequence const& ) = default;
    Sequence( Sequence&& )      = default;

    Sequence& operator= ( Sequence const& ) = default;
    Sequence& operator= ( Sequence&& )      = default;

    void swap( Sequence& other )
    {
        using std::swap;
        swap( label_,    other.label_ );
        swap( sites_,    other.sites_ );
    }

    void clear()
    {
        label_.clear();
        sites_.clear();
        phred_scores_.clear();
        abundance_ = 1;
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    std::string& label()
    {
        return label_;
    }

    std::string const& label() const
    {
        return label_;
    }

    void label( std::string const& value )
    {
        label_ = value;
    }

    void label( std::string&& value )
    {
        label_ = std::move( value );
    }

    std::string& sites()
    {
        return sites_;
    }

    std::string const& sites() const
    {
        return sites_;
    }

    void sites( std::string const& value )
    {
        sites_ = value;
    }

    void sites( std::string && value )
    {
        sites_ = std::move( value );
    }

    std::vector<unsigned char>& phred_scores()
    {
        return phred_scores_;
    }

    std::vector<unsigned char> const& phred_scores() const
    {
        return phred_scores_;
    }

    void phred_scores( std::vector<unsigned char> const& value )
    {
        phred_scores_ = value;
    }

    void phred_scores( std::vector<unsigned char>&& value )
    {
        phred_scores_ = std::move( value );
    }

    size_t abundance() const
    {
        return abundance_;
    }

    void abundance( size_t value )
    {
        abundance_ = value;
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
    * @brief Return the length (number of sites) of this sequence.
    */
    size_t length() const
    {
        return sites_.size();
    }

    /**
     * @brief Alias for length().
     */
    size_t size() const
    {
        return sites_.size();
    }

    char& site_at( size_t index )
    {
        return sites_.at(index);
    }

    char site_at( size_t index ) const
    {
        return sites_.at(index);
    }

    char& operator [] (size_t index)
    {
        return sites_[index];
    }

    char  operator [] (size_t index) const
    {
        return sites_[index];
    }

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin()
    {
        return sites_.begin();
    }

    iterator end()
    {
        return sites_.end();
    }

    const_iterator begin() const
    {
        return sites_.cbegin();
    }

    const_iterator end() const
    {
        return sites_.cend();
    }

    const_iterator cbegin() const
    {
        return sites_.cbegin();
    }

    const_iterator cend() const
    {
        return sites_.cend();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string label_;
    std::string sites_;

    std::vector<unsigned char> phred_scores_;
    size_t      abundance_ = 1;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
