#ifndef GENESIS_UTILS_CONTAINERS_DATAFRAME_H_
#define GENESIS_UTILS_CONTAINERS_DATAFRAME_H_

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
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Data Frame
// =================================================================================================

/**
 * @brief
 */
template <typename T>
class Dataframe
{
public:

    // ---------------------------------------------------------------------------------------------
    //     Column Class
    // ---------------------------------------------------------------------------------------------

    /**
     * @brief
     */
    class Column
    {
    public:

        // -------------------------------------------------------------------------
        //     Member Types
        // -------------------------------------------------------------------------

        using self_type       = Column;
        using value_type      = T;

        using       reference =       value_type&;
        using const_reference = const value_type&;
        using       pointer   =       value_type*;
        using const_pointer   = const value_type*;

        using       iterator  = typename std::vector< value_type >::iterator;
        using const_iterator  = typename std::vector< value_type >::const_iterator;

        using size_type       = size_t;

        // -------------------------------------------------------------------------
        //     Constructor and Rule of Five
        // -------------------------------------------------------------------------

        friend class Dataframe;

        Column( Dataframe& df, size_type index )
            : df_( &df )
            , index_( index )
        {}

        ~Column() = default;

        Column( Column const& ) = default;
        Column( Column&& )      = default;

        Column& operator= ( Column const& other )
        {
            // Either we assign when constructing (thanks, STL), in which case the assigned-to
            // column has size 0, or we do so when moving the vector contents, in which case
            // the size needs to stay the same.
            if( content_.size() > 0 && content_.size() != other.size() ) {
                throw std::runtime_error(
                    "Cannot assign Dataframe column with different size."
                );
            }

            content_ = other.content_;
            return *this;
        }

        Column& operator= ( Column&& other )
        {
            // Either we assign when constructing (thanks, STL), in which case the assigned-to
            // column has size 0, or we do so when moving the vector contents, in which case
            // the size needs to stay the same.
            if( content_.size() > 0 && content_.size() != other.size() ) {
                throw std::runtime_error(
                    "Cannot assign Dataframe column with different size."
                );
            }

            content_ = std::move( other.content_ );
            return *this;
        }

        // -------------------------------------------------------------------------
        //     Iterators
        // -------------------------------------------------------------------------

    public:

        iterator begin()
        {
            return content_.begin();
        }

        const_iterator begin() const
        {
            return content_.cbegin();
        }

        iterator end()
        {
            return content_.end();
        }

        const_iterator end() const
        {
            return content_.cend();
        }

        const_iterator cbegin()
        {
            return content_.cbegin();
        }

        const_iterator cend()
        {
            return content_.cend();
        }

        // -------------------------------------------------------------------------
        //     Properties
        // -------------------------------------------------------------------------

        Dataframe& dataframe()
        {
            return *df_;
        }

        Dataframe const& dataframe() const
        {
            return *df_;
        }

        size_type size() const
        {
            return content_.size();
        }

        bool empty() const
        {
            return content_.empty();
        }

        size_type index() const
        {
            return index_;
        }

        std::string const& name() const
        {
            return df_->col_name( index_ );
        }

        // -------------------------------------------------------------------------
        //     Element Access
        // -------------------------------------------------------------------------

        reference operator[] ( size_type index )
        {
            return content_[ index ];
        }

        const_reference operator[] ( size_type index ) const
        {
            return content_[ index ];
        }

        reference operator[] ( std::string const& row_name )
        {
            return content_[ df_->row_index( row_name ) ];
        }

        const_reference operator[] ( std::string const& row_name ) const
        {
            return content_[ df_->row_index( row_name ) ];
        }

        reference at( size_type index )
        {
            return content_.at( index );
        }

        const_reference at( size_type index ) const
        {
            return content_.at( index );
        }

        reference at( std::string const& row_name )
        {
            return content_[ df_->row_index( row_name ) ];
        }

        const_reference at( std::string const& row_name ) const
        {
            return content_[ df_->row_index( row_name ) ];
        }

        // -------------------------------------------------------------------------
        //     Modifiers
        // -------------------------------------------------------------------------

        /**
         * @brief Overwrite a column by the elements of a `std::vector`.
         *
         * The size of the vector needs to match the number of rows of the Dataframe.
         */
        self_type& operator = ( std::vector<value_type> const& vec )
        {
            if( vec.size() != content_.size() ) {
                throw std::runtime_error(
                    "Cannot assign vector with different size to Dataframe column."
                );
            }

            content_ = vec;
            return *this;
        }

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        Dataframe* df_ = nullptr;
        size_type  index_;

        std::vector< value_type > content_;


    };

    // ---------------------------------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------------------------------

    using self_type       = Dataframe;
    using value_type      = Column;

    using       reference =       value_type&;
    using const_reference = const value_type&;
    using       pointer   =       value_type*;
    using const_pointer   = const value_type*;

    using       iterator  = typename std::vector< value_type >::iterator;
    using const_iterator  = typename std::vector< value_type >::const_iterator;

    using size_type       = size_t;

    // ---------------------------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------------------------------

    friend class Column;

    Dataframe()  = default;
    ~Dataframe() = default;

    Dataframe( Dataframe const& ) = default;
    Dataframe ( Dataframe&& )      = default;

    Dataframe& operator= ( Dataframe const& ) = default;
    Dataframe& operator= ( Dataframe&& )      = default;

    // ---------------------------------------------------------------------------------------------
    //     Iterators
    // ---------------------------------------------------------------------------------------------

    iterator begin()
    {
        return columns_.begin();
    }

    const_iterator begin() const
    {
        return columns_.cbegin();
    }

    iterator end()
    {
        return columns_.end();
    }

    const_iterator end() const
    {
        return columns_.cend();
    }

    const_iterator cbegin()
    {
        return columns_.cbegin();
    }

    const_iterator cend()
    {
        return columns_.cend();
    }

    // ---------------------------------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------------------------------

    size_type rows() const
    {
        return row_names_.size();
    }

    size_type cols() const
    {
        return columns_.size();
    }

    bool empty() const
    {
        return columns_.empty() && row_names_.empty();
    }

    // ---------------------------------------------------------------------------------------------
    //     Column Access
    // ---------------------------------------------------------------------------------------------

    reference operator[] ( size_type col_index )
    {
        return columns_.at( col_index );
    }

    const_reference operator[] ( size_type col_index ) const
    {
        return columns_.at( col_index );
    }

    reference operator[] ( std::string const& col_name )
    {
        return columns_[ col_index( col_name ) ];
    }

    const_reference operator[] ( std::string const& col_name ) const
    {
        return columns_[ col_index( col_name ) ];
    }

    reference at( size_type col_index )
    {
        return columns_.at( col_index );
    }

    const_reference at( size_type col_index ) const
    {
        return columns_.at( col_index );
    }

    reference at( std::string const& col_name )
    {
        return columns_[ col_index( col_name ) ];
    }

    const_reference at( std::string const& col_name ) const
    {
        return columns_[ col_index( col_name ) ];
    }

    // ---------------------------------------------------------------------------------------------
    //     Element Access
    // ---------------------------------------------------------------------------------------------

    typename Column::reference operator () ( size_type row_index, size_type col_index )
    {
        return at( col_index ).at( row_index );
    }

    typename Column::const_reference operator () ( size_type row_index, size_type col_index ) const
    {
        return at( col_index ).at( row_index );
    }

    typename Column::reference operator () ( std::string const& row_name, size_type col_index )
    {
        return at( col_index ).at( row_name );
    }

    typename Column::const_reference operator () ( std::string const& row_name, size_type col_index ) const
    {
        return at( col_index ).at( row_name );
    }

    typename Column::reference operator () ( size_type row_index, std::string const& col_name )
    {
        return at( col_name ).at( row_index );
    }

    typename Column::const_reference operator () ( size_type row_index, std::string const& col_name ) const
    {
        return at( col_name ).at( row_index );
    }

    typename Column::reference operator () ( std::string const& row_name, std::string const& col_name )
    {
        return at( col_name ).at( row_name );
    }

    typename Column::const_reference operator () ( std::string const& row_name, std::string const& col_name ) const
    {
        return at( col_name ).at( row_name );
    }

    // ---------------------------------------------------------------------------------------------
    //     Indexing and Naming
    // ---------------------------------------------------------------------------------------------

    size_t row_index( std::string const& row_name ) const
    {
        return row_lookup_.at( row_name );
    }

    std::string const& row_name( size_type row_index ) const
    {
        return row_names_.at( row_index );
    }

    self_type& row_name( size_type row_index, std::string const& value )
    {
        auto const& old = row_names_.at( row_index );
        row_lookup_.erase( old );
        row_lookup_[ value ] = row_index;
        row_names_.at( row_index ) = value;

        return *this;
    }

    std::vector<std::string> const& row_names() const
    {
        return row_names_;
    }

    size_t col_index( std::string const& col_name ) const
    {
        return col_lookup_.at( col_name );
    }

    std::string const& col_name( size_type col_index ) const
    {
        return col_names_.at( col_index );
    }

    self_type& col_name( size_type col_index, std::string const& value )
    {
        auto const& old = col_names_.at( col_index );
        col_lookup_.erase( old );
        col_lookup_[ value ] = col_index;
        col_names_.at( col_index ) = value;

        return *this;
    }

    std::vector<std::string> col_names() const
    {
        return col_names_;
    }

    // ---------------------------------------------------------------------------------------------
    //     Adding rows and cols
    // ---------------------------------------------------------------------------------------------

    self_type& add_col()
    {
        auto const index = columns_.size();
        columns_.emplace_back( *this, index );
        columns_.back().content_.resize( row_names_.size() );
        col_names_.emplace_back();

        return *this;
    }

    self_type& add_col( std::string const& name )
    {
        if( col_lookup_.count( name ) > 0 ) {
            throw std::runtime_error( "Column with name " + name + " already exists in Dataframe." );
        }

        auto const index = columns_.size();
        columns_.emplace_back( *this, index );
        columns_.back().content_.resize( row_names_.size() );
        col_names_.emplace_back( name );
        col_lookup_[ name ] = index;

        return *this;
    }

    self_type& add_row()
    {
        row_names_.emplace_back();

        for( auto& col : columns_ ) {
            col.content_.emplace_back();
        }

        return *this;
    }

    self_type& add_row( std::string const& name )
    {
        // Add name.
        if( row_lookup_.count( name ) > 0 ) {
            throw std::runtime_error( "Row with name " + name + " already exists in Dataframe." );
        }
        row_names_.emplace_back( name );
        row_lookup_[ name ] = row_names_.size() - 1;

        // Add content.
        for( auto& col : columns_ ) {
            col.content_.emplace_back();
        }

        return *this;
    }

    // ---------------------------------------------------------------------------------------------
    //     Removing rows and cols
    // ---------------------------------------------------------------------------------------------

    self_type& clear()
    {
        columns_.clear();
        row_names_.clear();
        col_names_.clear();
        row_lookup_.clear();
        col_lookup_.clear();
        return *this;
    }

    self_type& clear_rows()
    {
        for( auto& col : columns_ ) {
            col.content_.clear();
        }
        row_names_.clear();
        row_lookup_.clear();
        return *this;
    }

    self_type& clear_cols()
    {
        columns_.clear();
        col_names_.clear();
        col_lookup_.clear();
        return *this;
    }

    self_type& remove_col( size_type col_index )
    {
        assert( columns_.size() == col_names_.size() );
        if( col_index >= columns_.size() ) {
            throw std::runtime_error( "Invalid column index greater than number of columns." );
        }

        // Remove elements.
        auto const name = col_names_[ col_index ];
        columns_.erase( columns_.begin() + col_index );
        col_names_.erase( col_names_.begin() + col_index );
        col_lookup_.erase( name );

        // Adjust remaining indices.
        for( size_t i = 0; i < columns_.size(); ++i ) {
            // --columns_[i].index_;

            // We do not need to adjust the indices, as the erase uses the assignment operators,
            // which already make sure that the indices stay correct (simply by not changing them).
            // So here, we simply control whether the indices of the columns are intact.
            assert( columns_[i].index_ == i );
        }

        // Adjust indices of all lookup table values that are greater than the removed index.
        for( auto& le : col_lookup_ ) {
            assert( le.second != col_index );
            if( le.second > col_index ) {
                --le.second;
            }
        }

        return *this;
    }

    self_type& remove_col( std::string const& col_name )
    {
        auto const index = col_index( col_name );
        assert( col_names_[ index ] == col_name );
        remove_col( index );
        return *this;
    }

    self_type& remove_row( size_type row_index )
    {
        if( row_index >= row_names_.size() ) {
            throw std::runtime_error( "Invalid row index greater than number of rows." );
        }

        // Remove elements.
        for( auto& col : columns_ ) {
            assert( col.content_.size() == row_names_.size() );
            col.content_.erase( col.content_.begin() + row_index );
        }
        auto const name = row_names_[ row_index ];
        row_names_.erase( row_names_.begin() + row_index );
        row_lookup_.erase( name );

        // Adjust remaining indices.
        for( auto& le : row_lookup_ ) {
            assert( le.second != row_index );
            if( le.second > row_index ) {
                --le.second;
            }
        }

        return *this;
    }

    self_type& remove_row( std::string const& row_name )
    {
        auto const index = row_index( row_name );
        assert( row_names_[ index ] == row_name );
        remove_row( index );
        return *this;
    }

    // ---------------------------------------------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------------------------------------------

private:

    std::vector< value_type > columns_;

    std::vector< std::string > row_names_;
    std::vector< std::string > col_names_;

    std::unordered_map< std::string, size_t > row_lookup_;
    std::unordered_map< std::string, size_t > col_lookup_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
