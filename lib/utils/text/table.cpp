/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/text/table.hpp"

#include "utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace text {

// =================================================================================================
//     Text Table
// =================================================================================================

// ---------------------------------------------------------------------
//     Accessors
// ---------------------------------------------------------------------

size_t Table::length() const
{
    if( columns_.size() == 0 ) {
        return 0;
    }

    size_t len = columns_[0].length();
    for( auto const& c : columns_ ) {
        if( len != c.length() ) {
            throw std::length_error("Table columns are unevenly filled.");
        }
    }

    // If we are here, no exception was thrown. Thus, all columns are evenly filled. The current
    // marker for inserting new values therefore needs to be at the first column, too. If not,
    // we failed to set it correctly somewhere.
    assert( current_col_ == 0 );

    return len;
}

// ---------------------------------------------------------------------
//     Modifiers
// ---------------------------------------------------------------------

/**
 * @brief Clears all columns and their data from the table.
 */
void Table::clear()
{
    current_col_ = 0;
    columns_.clear();
}

/**
 * @brief Clears the data contents of all columns. Their labels etc stay unchanged.
 */
void Table::clear_content()
{
    current_col_ = 0;
    for( auto& c : columns_ ) {
        c.clear_content();
    }
}

/**
 * @brief Add a column to the table.
 *
 * If the table already contains columns with data, the new one is initialized with empty strings
 * for the whole length of the table.
 */
Table::Column& Table::add_column( std::string label )
{
    auto const len = length();
    columns_.push_back( Column(label) );
    for( size_t i = 0; i < len; ++i ) {
        columns_.back().append("");
    }
    return columns_.back();
}

Table& Table::operator << ( std::string value )
{
    return append( value );
}

Table& Table::append ( std::string value )
{
    columns_[ current_col_ ].append( value );

    ++current_col_;
    if( current_col_ >= columns_.size() ) {
        current_col_ = 0;
    }

    return *this;
}

// ---------------------------------------------------------------------
//     Output
// ---------------------------------------------------------------------

void Table::write( std::ostream& stream ) const
{
    // Write labels.
    for( auto const& c : columns_ ) {
        c.write_label(stream);
        stream << " ";
    }
    stream << "\n";

    // Write data.
    for( size_t i = 0; i < length(); ++i ) {
        for( auto const& c : columns_ ) {
            c.write_row(stream, i);
            stream << " ";
        }
        stream << "\n";
    }
}

// =================================================================================================
//     Text Column
// =================================================================================================

// ---------------------------------------------------------------------
//     Properties
// ---------------------------------------------------------------------

void Table::Column::label( std::string value )
{
    value = trim(value);
    width_ = std::max( width_, value.size() );
    label_ = value;
}

std::string Table::Column::label() const
{
    return label_;
}

void Table::Column::justify( Justification value )
{
    just_ = value;
}

Table::Column::Justification Table::Column::justify() const
{
    return just_;
}

/**
 * @brief Set the width of this column.
 *
 * If the new value is smaller than the current one, nothing happens. The width can only grow, or be
 * set to the minimal possible value using shrink_width(). This ensures that all text fits within
 * the column.
 */
void Table::Column::width( size_t value )
{
    if( value > width_ ) {
        width_ = value;
    }
}

size_t Table::Column::width() const
{
    return width_;
}

/**
 * @brief Shrinks the column width to the minmal width that suffices to fit in all values of the
 * column (i.e., both data and label).
 *
 * The column keeps track of its needed minimal width when just adding data. Thus, this function is
 * only neccessary to call once the width was changed manually or if the label or a data value
 * have been changed after being added.
 */
void Table::Column::shrink_width()
{
    size_t mx = label_.size();
    for( auto const& v : data_ ) {
        mx = std::max( mx, v.size() );
    }
    width_ = mx;
}

// ---------------------------------------------------------------------
//     Accessors
// ---------------------------------------------------------------------

size_t Table::Column::length() const
{
    return data_.size();
}

std::string Table::Column::row( size_t i ) const
{
    // Throws out of range if neccessary.
    return data_.at( i );
}

// ---------------------------------------------------------------------
//     Modifiers
// ---------------------------------------------------------------------

void Table::Column::clear_content()
{
    width_ = label_.size();
    data_.clear();
}

void Table::Column::append( std::string value )
{
    value = trim(value);
    width_ = std::max( width_, value.size() );
    data_.push_back(value);
}

// ---------------------------------------------------------------------
//     Output
// ---------------------------------------------------------------------

void Table::Column::write_row( std::ostream& stream, size_t row ) const
{
    // Throws out_of_range if neccessary.
    write( stream, data_.at(row) );
}

void Table::Column::write_label( std::ostream& stream ) const
{
    write( stream, label_ );
}

void Table::Column::write( std::ostream& stream, std::string text ) const
{
    assert( text.size() <= width_ );

    // Use a separate stream in order to not mess with the original attributes.
    // Not optimal in terms of speed, but for now it's good enough.
    std::stringstream ss;
    ss << std::fixed;
    ss.fill(' ');
    ss.width(width_);

    if( just_ == Justification::kCentered ) {
        // Justify text by prepending spaces (and later left aligning it).
        const size_t pad = (width_ - text.size()) / 2;
        text = std::string(pad, ' ') + text;
    }

    ss << ( just_ == Justification::kRight ? std::right : std::left );
    ss << text;
    stream << ss.str();
}

} // namespace text
} // namespace genesis
