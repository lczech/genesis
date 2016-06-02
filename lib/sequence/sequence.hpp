#ifndef GENESIS_SEQUENCE_SEQUENCE_H_
#define GENESIS_SEQUENCE_SEQUENCE_H_

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
 * @ingroup sequence
 */

#include <string>

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

    Sequence( std::string const& label, std::string const& sites )
        : label_(label)
        , metadata_()
        , sites_(sites)
    {}

    Sequence( std::string const& label, std::string const& metadata, std::string const& sites )
        : label_(label)
        , metadata_(metadata)
        , sites_(sites)
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
        swap( metadata_, other.metadata_ );
        swap( sites_,    other.sites_ );
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    std::string const& label() const;
    void               label( std::string const& value );

    std::string const& metadata() const;
    void               metadata( std::string const& value );

    std::string const& sites() const;
    void               sites( std::string const& value );
    void               sites( std::string &&     value );
    std::string&       sites();

    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t length() const;
    size_t size() const;

    char site_at( size_t index ) const;

    char operator [] (size_t index) const;

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
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string label_;
    std::string metadata_;
    std::string sites_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
