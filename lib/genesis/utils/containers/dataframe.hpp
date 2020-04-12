#ifndef GENESIS_UTILS_CONTAINERS_DATAFRAME_H_
#define GENESIS_UTILS_CONTAINERS_DATAFRAME_H_

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
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/containers/deref_iterator.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Dataframe
// =================================================================================================

/**
 * @brief
 */
class Dataframe
{
public:

    // ---------------------------------------------------------------------------------------------
    //     Forward Declarations
    // ---------------------------------------------------------------------------------------------

    template <typename T>
    class Column;

    // ---------------------------------------------------------------------------------------------
    //     Column Base
    // ---------------------------------------------------------------------------------------------

    class ColumnBase
    {
    public:

        // -------------------------------------------------------------------------
        //     Member Types
        // -------------------------------------------------------------------------

        using size_type = size_t;

        friend class Dataframe;
        friend void swap( Dataframe& lhs, Dataframe& rhs );

        // -------------------------------------------------------------------------
        //     Constructor and Rule of Five
        // -------------------------------------------------------------------------

    protected:

        ColumnBase() = delete;

        ColumnBase( Dataframe& df, size_type index )
            : df_( &df )
            , index_( index )
        {}

        ColumnBase( ColumnBase const& ) = delete;
        ColumnBase( ColumnBase&& )      = delete;

        ColumnBase& operator= ( ColumnBase const& ) = delete;
        ColumnBase& operator= ( ColumnBase&& )      = delete;

    public:

        virtual ~ColumnBase() = default;

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

        size_type index() const
        {
            assert( df_ && &(df_->at(index_)) == this );
            return index_;
        }

        std::string const& name() const
        {
            return df_->col_name( index_ );
        }

        size_type size() const
        {
            // Non-virtual interface
            return size_();
        }

        bool empty() const
        {
            // Non-virtual interface
            return empty_();
        }

        // -------------------------------------------------------------------------
        //     Casting and Types
        // -------------------------------------------------------------------------

        template<typename T>
        Column<T>& as()
        {
            return dynamic_cast<Column<T>&>( *this );
        }

        template<typename T>
        Column<T> const& as() const
        {
            return dynamic_cast<Column<T> const&>( *this );
        }

        template<typename T>
        bool is() const
        {
            auto const c = dynamic_cast<Column<T> const*>( this );
            return ( c != nullptr );
        }

        template<typename T>
        T& get( size_type index )
        {
            return as<T>()[index];
        }

        template<typename T>
        T const& get( size_type index ) const
        {
            return as<T>()[index];
        }

        template<typename T>
        T& get( std::string const& row_name )
        {
            return as<T>()[ dataframe().row_index( row_name ) ];
        }

        template<typename T>
        T const& get( std::string const& row_name ) const
        {
            return as<T>()[ dataframe().row_index( row_name ) ];
        }

        // -------------------------------------------------------------------------
        //     Purely Virtual Functions
        // -------------------------------------------------------------------------

    protected:

        // Non-virtual interface for public functions.
        virtual size_type size_() const = 0;
        virtual bool empty_() const = 0;

        // Private functions for internal use only.
        virtual void clear_() = 0;
        virtual void resize_( size_type ) = 0;
        virtual void add_row_() = 0;
        virtual void remove_row_( size_type row_index ) = 0;
        virtual std::unique_ptr<ColumnBase> clone_() const = 0;

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        Dataframe* df_;
        size_type   index_;

    };

    // ---------------------------------------------------------------------------------------------
    //     Column Class
    // ---------------------------------------------------------------------------------------------

    /**
     * @brief
     */
    template <typename T>
    class Column : public ColumnBase
    {
    public:

        // -------------------------------------------------------------------------
        //     Member Types
        // -------------------------------------------------------------------------

        using self_type       = Column<T>;
        using value_type      = T;
        using container_type  = std::vector< value_type >;

        using       reference = value_type&;
        using const_reference = value_type const&;
        using       pointer   = value_type*;
        using const_pointer   = value_type const*;

        using       iterator  = typename container_type::iterator;
        using const_iterator  = typename container_type::const_iterator;

        using size_type       = size_t;

        friend class Dataframe;

        static_assert(
            ! std::is_same<value_type, bool>::value,
            "Cannot instanciate Dataframe Column with type bool, "
            "because std::vector<bool> does not offer proper references."
        );

        // -------------------------------------------------------------------------
        //     Constructor and Rule of Five
        // -------------------------------------------------------------------------

    private:

        Column() = delete;

        Column( Dataframe& df, size_type index )
            : ColumnBase( df, index )
        {}

        Column( Column const& ) = delete;
        Column( Column&& )      = delete;

        Column& operator= ( Column const& other ) = delete;
        Column& operator= ( Column&& other )      = delete;

    public:

        virtual ~Column() override = default;

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
            return content_[ dataframe().row_index( row_name ) ];
        }

        const_reference operator[] ( std::string const& row_name ) const
        {
            return content_[ dataframe().row_index( row_name ) ];
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
            return content_[ dataframe().row_index( row_name ) ];
        }

        const_reference at( std::string const& row_name ) const
        {
            return content_[ dataframe().row_index( row_name ) ];
        }

        // -------------------------------------------------------------------------
        //     Modifiers
        // -------------------------------------------------------------------------

        /**
         * @brief Overwrite a column by the elements of a `std::vector`.
         *
         * The size of the vector needs to match the number of rows of the Dataframe.
         * The vector is copied.
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

        /**
         * @brief Overwrite a column by the elements of a `std::vector`.
         *
         * The size of the vector needs to match the number of rows of the Dataframe.
         * The vector is moved.
         */
        self_type& operator = ( std::vector<value_type>&& vec )
        {
            if( vec.size() != content_.size() ) {
                throw std::runtime_error(
                    "Cannot assign vector with different size to Dataframe column."
                );
            }

            content_ = std::move( vec );
            return *this;
        }

        // -------------------------------------------------------------------------
        //     Interaction Operators
        // -------------------------------------------------------------------------

        /**
         * @brief Explicit conversion to `std::vector`.
         *
         * We can only offer const ref (or copy), but no non-const ref, as the latter would allow
         * users to change the vector size, breaking our invariants of equal-sized columns in the
         * dataframe. In order to modify all elements as if they were a vector, use the iterators
         * obtained from begin() and end() instead.
         */
        std::vector<value_type> const& to_vector() const
        {
            return content_;
        }

        /**
         * @brief Implicit conversion to `std::vector`.
         *
         * @copydoc to_vector()
         */
        operator std::vector<value_type> const&() const
        {
            return content_;
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        size_type size_() const override
        {
            return content_.size();
        }

        bool empty_() const override
        {
            return content_.empty();
        }

        void clear_() override
        {
            content_.clear();
        }

        void resize_( size_type size ) override
        {
            content_.resize( size );
        }

        void add_row_() override
        {
            content_.emplace_back();
        }

        void remove_row_( size_type row_index ) override
        {
            content_.erase( content_.begin() + row_index );
        }

        std::unique_ptr<ColumnBase> clone_() const override
        {
            // Nicer version, doesn't work in gcc 4.9 though...
            // auto ret = std::unique_ptr< self_type >( new self_type( *df_, index_ ));
            // ret->content_ = content_;
            // return ret;

            // Seem to need this for older compilers...
            auto ret = std::unique_ptr< ColumnBase >( new self_type( *df_, index_ ));
            dynamic_cast<self_type&>( *ret ).content_ = content_;
            return ret;
        }

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        std::vector< value_type > content_;
    };

    // ---------------------------------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------------------------------

public:

    using self_type       = Dataframe;
    using value_type      = ColumnBase;
    using container_type  = std::vector< std::unique_ptr< value_type >>;

    using       reference = value_type&;
    using const_reference = value_type const&;
    using       pointer   = value_type*;
    using const_pointer   = value_type const*;

    using       iterator  = utils::DereferenceIterator< container_type::iterator >;
    using const_iterator  = utils::DereferenceIterator< container_type::const_iterator >;

    using size_type       = size_t;

    // ---------------------------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------------------------------

    Dataframe()  = default;
    ~Dataframe() = default;

    Dataframe( Dataframe const& other )
        : row_names_ ( other.row_names_ )
        , col_names_ ( other.col_names_ )
        , row_lookup_( other.row_lookup_ )
        , col_lookup_( other.col_lookup_ )
    {
        columns_.clear();
        for( auto const& col : other.columns_ ) {
            columns_.emplace_back( col->clone_() );
            columns_.back()->df_ = this;
            columns_.back()->index_ = columns_.size() - 1;

            assert( columns_.size() > 0 );
            assert( columns_.back()->size() == row_names_.size() );
        }
    }

    Dataframe& operator= ( Dataframe const& other )
    {
        using std::swap;

        // Check for self assignment. Just in case.
        if( &other == this ) {
            return *this;
        }

        // Copy-swap-idiom.
        self_type temp( other );
        swap( *this, temp );
        return *this;
    }

    Dataframe( Dataframe&& )             = default;
    Dataframe& operator= ( Dataframe&& ) = default;

    friend void swap( self_type& lhs, self_type& rhs )
    {
        using std::swap;
        swap( lhs.columns_,    rhs.columns_ );
        swap( lhs.row_names_,  rhs.row_names_ );
        swap( lhs.col_names_,  rhs.col_names_ );
        swap( lhs.row_lookup_, rhs.row_lookup_ );
        swap( lhs.col_lookup_, rhs.col_lookup_ );

        // Need to swap dataframe pointers of the columns as well!
        for( size_t i = 0; i < lhs.columns_.size(); ++i ) {
            lhs.columns_[i]->df_ = &lhs;
        }
        for( size_t i = 0; i < rhs.columns_.size(); ++i ) {
            rhs.columns_[i]->df_ = &rhs;
        }
    }

    /**
     * @brief Helper function to validate internal invariants.
     *
     * Located in genesis/utils/containers/dataframe/operators.hpp
     */
    friend bool validate( Dataframe const& );

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
        assert( columns_.size() == col_names_.size() );
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
        return *columns_.at( col_index );
    }

    const_reference operator[] ( size_type col_index ) const
    {
        return *columns_.at( col_index );
    }

    reference operator[] ( std::string const& col_name )
    {
        return *columns_[ col_index( col_name ) ];
    }

    const_reference operator[] ( std::string const& col_name ) const
    {
        return *columns_[ col_index( col_name ) ];
    }

    reference at( size_type col_index )
    {
        return *columns_.at( col_index );
    }

    const_reference at( size_type col_index ) const
    {
        return *columns_.at( col_index );
    }

    reference at( std::string const& col_name )
    {
        return *columns_[ col_index( col_name ) ];
    }

    const_reference at( std::string const& col_name ) const
    {
        return *columns_[ col_index( col_name ) ];
    }

    // ---------------------------------------------------------------------------------------------
    //     Indexing and Naming
    // ---------------------------------------------------------------------------------------------

    bool has_row_name( std::string const& row_name ) const
    {
        if( row_name.empty() ) {
            throw std::runtime_error( "Cannot use empty row name." );
        }
        return ( row_lookup_.count( row_name ) > 0 );
    }

    bool has_unnamed_rows() const
    {
        return row_lookup_.size() < row_names_.size();
    }

    size_t row_index( std::string const& row_name ) const
    {
        if( row_lookup_.count( row_name ) == 0 ) {
            throw std::out_of_range( "Dataframe has no row with name '" + row_name + "'." );
        }
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

    bool has_col_name( std::string const& col_name ) const
    {
        if( col_name.empty() ) {
            throw std::runtime_error( "Cannot use empty column name." );
        }
        return ( col_lookup_.count( col_name ) > 0 );
    }

    bool has_unnamed_cols() const
    {
        return col_lookup_.size() < col_names_.size();
    }

    size_t col_index( std::string const& col_name ) const
    {
        if( col_lookup_.count( col_name ) == 0 ) {
            throw std::out_of_range( "Dataframe has no column with name '" + col_name + "'." );
        }
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

    std::vector<std::string> const& col_names() const
    {
        return col_names_;
    }

    // ---------------------------------------------------------------------------------------------
    //     Adding rows and cols
    // ---------------------------------------------------------------------------------------------

    template<class T>
    Column<T>& add_unnamed_col()
    {
        auto const index = columns_.size();
        columns_.emplace_back( std::unique_ptr<Column<T>>( new Column<T>( *this, index )));
        columns_.back()->resize_( row_names_.size() );
        col_names_.emplace_back();

        return columns_.back()->as<T>();
    }

    template<class T>
    Column<T>& add_unnamed_col( T const& init )
    {
        auto& col = add_unnamed_col<T>();
        for( auto& e : col.content_ ) {
            e = init;
        }
        return col;
    }

    template<class T>
    Column<T>& add_unnamed_col( std::vector<T> const& init )
    {
        auto copy = init;
        return add_unnamed_col( std::move( copy ));
    }

    template<class T>
    Column<T>& add_unnamed_col( std::vector<T>&& init )
    {
        if( init.size() != row_names_.size() ) {
            throw std::invalid_argument(
                "Cannot add column to Dataframe if initial values vector is of different size."
            );
        }
        auto& col = add_unnamed_col<T>();
        assert( col.size() == row_names_.size() );

        // for( size_t i = 0; i < init.size(); ++i ) {
        //     col.content_[i] = init[i];
        // }
        col.content_ = std::move( init );
        return col;
    }

    template<class T>
    Column<T>& add_col( std::string const& name )
    {
        if( name.empty() ) {
            throw std::runtime_error(
                "Cannot add a column with an empty name. Use add_unnamed_col() instead."
            );
        }
        if( col_lookup_.count( name ) > 0 ) {
            throw std::runtime_error( "Column with name " + name + " already exists in Dataframe." );
        }

        auto const index = columns_.size();
        // columns_.emplace_back( *this, index );
        columns_.emplace_back( std::unique_ptr<Column<T>>( new Column<T>( *this, index )));
        columns_.back()->resize_( row_names_.size() );
        col_names_.emplace_back( name );
        col_lookup_[ name ] = index;

        return columns_.back()->as<T>();
    }

    template<class T>
    Column<T>& add_col( std::string const& name, T const& init )
    {
        auto& col = add_col<T>( name );
        for( auto& e : col.content_ ) {
            e = init;
        }
        return col;
    }

    template<class T>
    Column<T>& add_col( std::string const& name, std::vector<T> const& init )
    {
        auto copy = init;
        return add_col( name, std::move( copy ));
    }

    template<class T>
    Column<T>& add_col( std::string const& name, std::vector<T>&& init )
    {
        if( init.size() != row_names_.size() ) {
            throw std::invalid_argument(
                "Cannot add column to Dataframe if initial values vector is of different size: "
                + std::to_string( init.size() ) + " != " + std::to_string( row_names_.size() )
            );
        }
        auto& col = add_col<T>( name );
        assert( col.size() == row_names_.size() );

        // for( size_t i = 0; i < init.size(); ++i ) {
        //     col.content_[i] = init[i];
        // }
        col.content_ = std::move( init );
        return col;
    }

    self_type& add_unnamed_row()
    {
        row_names_.emplace_back();

        for( auto& col : columns_ ) {
            assert( row_names_.size() == col->size() + 1 );
            col->add_row_();
        }

        return *this;
    }

    self_type& add_row( std::string const& name )
    {
        if( name.empty() ) {
            throw std::runtime_error(
                "Cannot add a row with an empty name. Use add_unnamed_row() instead."
            );
        }
        if( row_lookup_.count( name ) > 0 ) {
            throw std::runtime_error( "Row with name " + name + " already exists in Dataframe." );
        }

        // Add name.
        row_names_.emplace_back( name );
        row_lookup_[ name ] = row_names_.size() - 1;

        // Add content.
        for( auto& col : columns_ ) {
            assert( row_names_.size() == col->size() + 1 );
            col->add_row_();
        }

        return *this;
    }

    // ---------------------------------------------------------------------------------------------
    //     Replacing cols
    // ---------------------------------------------------------------------------------------------

    template<class T>
    Column<T>& replace_col( size_type at_index )
    {
        assert( columns_.size() == col_names_.size() );
        if( at_index >= columns_.size() ) {
            throw std::runtime_error(
                "Invalid column index greater than or equal to number of columns."
            );
        }

        columns_[ at_index ] = std::unique_ptr<Column<T>>( new Column<T>( *this, at_index ));
        columns_[ at_index ]->resize_( row_names_.size() );
        return columns_[ at_index ]->as<T>();
    }

    template<class T>
    Column<T>& replace_col( size_type at_index, T const& init )
    {
        auto& col = replace_col<T>( at_index );
        for( auto& e : col.content_ ) {
            e = init;
        }
        return col;
    }

    template<class T>
    Column<T>& replace_col( size_type at_index, std::vector<T> const& init )
    {
        auto copy = init;
        return replace_col( at_index, std::move( copy ));
    }

    template<class T>
    Column<T>& replace_col( size_type at_index, std::vector<T>&& init )
    {
        if( init.size() != row_names_.size() ) {
            throw std::invalid_argument(
                "Cannot replace column in Dataframe if initial values vector is of different size."
            );
        }
        auto& col = replace_col<T>( at_index );
        assert( col.size() == row_names_.size() );

        // for( size_t i = 0; i < init.size(); ++i ) {
        //     col.content_[i] = init[i];
        // }
        col.content_ = std::move( init );
        return col;
    }

    template<class T>
    Column<T>& replace_col( std::string const& at_name )
    {
        if( at_name.empty() ) {
            throw std::runtime_error( "Cannot replace column with empty name given." );
        }
        if( col_lookup_.count( at_name ) == 0 ) {
            throw std::runtime_error(
                "Column with name " + at_name + " does not exist in Dataframe."
            );
        }

        auto const index = col_index( at_name );
        assert( col_names_[ index ] == at_name );
        return replace_col<T>( index );
    }

    template<class T>
    Column<T>& replace_col( std::string const& at_name, T const& init )
    {
        auto& col = replace_col<T>( at_name );
        for( auto& e : col.content_ ) {
            e = init;
        }
        return col;
    }

    template<class T>
    Column<T>& replace_col( std::string const& at_name, std::vector<T> const& init )
    {
        auto copy = init;
        return replace_col( at_name, std::move( copy ));
    }

    template<class T>
    Column<T>& replace_col( std::string const& at_name, std::vector<T>&& init )
    {
        if( init.size() != row_names_.size() ) {
            throw std::invalid_argument(
                "Cannot replace column in Dataframe if initial values vector is of different size."
            );
        }
        auto& col = replace_col<T>( at_name );
        assert( col.size() == row_names_.size() );

        // for( size_t i = 0; i < init.size(); ++i ) {
        //     col.content_[i] = init[i];
        // }
        col.content_ = std::move( init );
        return col;
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
            col->clear_();
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
            throw std::runtime_error(
                "Invalid column index greater than or equal to number of columns."
            );
        }

        // Remove elements.
        auto const name = col_names_[ col_index ];
        columns_.erase( columns_.begin() + col_index );
        col_names_.erase( col_names_.begin() + col_index );
        col_lookup_.erase( name );

        // Adjust remaining indices.
        for( size_t i = col_index; i < columns_.size(); ++i ) {
            --columns_[i]->index_;
            assert( columns_[i]->index() == i );
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
            throw std::runtime_error( "Invalid row index greater than or equal to number of rows." );
        }

        // Remove elements.
        for( auto& col : columns_ ) {
            assert( col->size() == row_names_.size() );
            // col.content_.erase( col.content_.begin() + row_index );
            col->remove_row_( row_index );
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

    container_type columns_;

    std::vector< std::string > row_names_;
    std::vector< std::string > col_names_;

    std::unordered_map< std::string, size_t > row_lookup_;
    std::unordered_map< std::string, size_t > col_lookup_;

};

// =================================================================================================
//     Dataframe Assertions
// =================================================================================================

static_assert( std::is_move_constructible<Dataframe>::value, "Dataframe is not move constructible." );
static_assert( std::is_move_assignable<Dataframe>::value, "Dataframe is not move assignable." );

} // namespace utils
} // namespace genesis

#endif // include guard
