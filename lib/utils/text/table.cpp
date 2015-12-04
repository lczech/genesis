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

void Table::write( std::ostream& out ) const
{
    // Write labels.
    for( auto const& c : columns_ ) {
        c.write_label(out);
        out << " ";
    }
    out << "\n";

    // Write data.
    for( size_t i = 0; i < length(); ++i ) {
        for( auto const& c : columns_ ) {
            c.write_row(out, i);
            out << " ";
        }
        out << "\n";
    }
}

void Table::write( std::ostream& out, Frame const& frame ) const
{
    // Take a Frame Line and print it according to the table data.
    auto write_line = [&] (Frame::Line const& line) {
        if( line.enabled ) {
            out << line.left_border;
            for( size_t ci = 0; ci < columns_.size(); ++ci ) {
                // out << std::string( columns_[ci].width(), line.filler );
                for( size_t i = 0; i < columns_[ci].width(); ++i ) {
                    out << line.filler;
                }
                if( ci < columns_.size() - 1 ) {
                    out << line.separator;
                }
            }
            out << line.right_border << "\n";
        }
    };

    // Write line above header.
    write_line(frame.top);

    // Write labels.
    out << frame.header.left_border;
    for( size_t ci = 0; ci < columns_.size(); ++ci ) {
        columns_[ci].write_label(out);
        if( ci < columns_.size() - 1 ) {
            out << frame.header.separator;
        }
    }
    out << frame.header.right_border << "\n";

    // Write line between header and content.
    write_line(frame.separator);

    // Write data.
    for( size_t i = 0; i < length(); ++i ) {
        out << frame.row.left_border;
        for( size_t ci = 0; ci < columns_.size(); ++ci ) {
            columns_[ci].write_row(out, i);
            if( ci < columns_.size() - 1 ) {
                out << frame.row.separator;
            }
        }
        out << frame.row.right_border << "\n";
    }

    // Write line below content.
    write_line(frame.bottom);
}

std::string Table::to_string() const
{
    std::stringstream ss;
    write(ss);
    return ss.str();
}

std::string Table::to_string( Frame const& frame ) const
{
    std::stringstream ss;
    write(ss, frame);
    return ss.str();
}

std::ostream& operator << (std::ostream& out, Table const& table)
{
    table.write(out);
    return out;
}

// =================================================================================================
//     Table Column
// =================================================================================================

// ---------------------------------------------------------------------
//     Properties
// ---------------------------------------------------------------------

void Table::Column::label( std::string value )
{
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
 * If the new value is smaller than the current one, nothing happens. The width can only grow, or
 * be set to the minimal possible value using shrink_width(). This ensures that all text fits
 * within the column.
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

// =================================================================================================
//     Table Frame
// =================================================================================================

// ---------------------------------------------------------------------
//     Binding
// ---------------------------------------------------------------------

std::ostream& operator << (std::ostream& out, Frame::Binder const& binder)
{
    binder.table.write(out, binder.frame);
    return out;
}

/**
 * @brief Functional operator that allows to bind a Frame to a Table so that they can be used
 * in one ostream command.
 *
 * Using this function makes outputting a Table to some stream easier when using frames:
 *
 *     Table t;
 *     Frame f;
 *     // Fill t and set f.
 *     std::cout << f(t);
 *
 * or even simpler, create a Frame from one of the predefined settings on the fly:
 *
 *     Table t;
 *     // Fill t.
 *     std::cout << simple_frame(true)(t);
 *
 * This function is thus a handy shortcut to avoid using the Table write functions directly.
 */
Frame::Binder Frame::operator () ( Table const& table )
{
    return Binder(*this, table);
}

// ---------------------------------------------------------------------
//     Default Frames
// ---------------------------------------------------------------------

Frame minimal_frame()
{
    // Frame already has minimal settings (just a space as separator, nothing else).
    return Frame();
}

Frame simple_frame( bool wide )
{
    auto f = Frame();

    f.header.left_border  = (wide ? " " : "");
    f.header.separator    = (wide ? "   " : " ");
    f.header.right_border = (wide ? " " : "");

    f.separator.enabled      = true;
    f.separator.left_border  = (wide ? "-" : "");
    f.separator.filler       = "-";
    f.separator.separator    = (wide ? "- -" : " ");
    f.separator.right_border = (wide ? "-" : "");

    f.row = f.header;

    return f;
}

Frame full_frame( bool wide )
{
    auto f = Frame();

    f.top.enabled      = true;
    f.top.left_border  = (wide ? "+-" : "+");
    f.top.filler       = "-";
    f.top.separator    = (wide ? "-+-" : "+");
    f.top.right_border = (wide ? "-+" : "+");

    f.header.left_border  = (wide ? "| " : "|");
    f.header.separator    = (wide ? " | " : "|");
    f.header.right_border = (wide ? " |" : "|");

    f.separator = f.top;
    f.row       = f.header;
    f.bottom    = f.top;

    return f;
}

Frame extended_frame( bool wide )
{
    auto f = Frame();

    f.top.enabled      = true;
    f.top.left_border  = (wide ? "┌─" : "┌");
    f.top.filler       = "─";
    f.top.separator    = (wide ? "─┬─" : "┬");
    f.top.right_border = (wide ? "─┐" : "┐");

    f.header.left_border  = (wide ? "│ " : "│");
    f.header.separator    = (wide ? " │ " : "│");
    f.header.right_border = (wide ? " │" : "│");

    f.separator.enabled      = true;
    f.separator.left_border  = (wide ? "├─" : "├");
    f.separator.filler       = "─";
    f.separator.separator    = (wide ? "─┼─" : "┼");
    f.separator.right_border = (wide ? "─┤" : "┤");

    f.row = f.header;

    f.bottom.enabled      = true;
    f.bottom.left_border  = (wide ? "└─" : "└");
    f.bottom.filler       = "─";
    f.bottom.separator    = (wide ? "─┴─" : "┴");
    f.bottom.right_border = (wide ? "─┘" : "┘");

    return f;
}

Frame double_frame( bool wide )
{
    auto f = Frame();

    f.top.enabled      = true;
    f.top.left_border  = (wide ? "╔═" : "╔");
    f.top.filler       = "═";
    f.top.separator    = (wide ? "═╦═" : "╦");
    f.top.right_border = (wide ? "═╗" : "╗");

    f.header.left_border  = (wide ? "║ " : "║");
    f.header.separator    = (wide ? " ║ " : "║");
    f.header.right_border = (wide ? " ║" : "║");

    f.separator.enabled      = true;
    f.separator.left_border  = (wide ? "╠═" : "╠");
    f.separator.filler       = "═";
    f.separator.separator    = (wide ? "═╬═" : "╬");
    f.separator.right_border = (wide ? "═╣" : "╣");

    f.row = f.header;

    f.bottom.enabled      = true;
    f.bottom.left_border  = (wide ? "╚═" : "╚");
    f.bottom.filler       = "═";
    f.bottom.separator    = (wide ? "═╩═" : "╩");
    f.bottom.right_border = (wide ? "═╝" : "╝");

    return f;
}

} // namespace text
} // namespace genesis
