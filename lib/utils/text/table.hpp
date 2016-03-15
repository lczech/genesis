#ifndef GENESIS_UTILS_TEXT_TABLE_H_
#define GENESIS_UTILS_TEXT_TABLE_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

struct Layout;

class Style;

// =================================================================================================
//     Text Table
// =================================================================================================

/**
 * @brief
 */
class Table
{
    // ---------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------

public:

    class Column;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

public:

    Table() = default;
    ~Table() = default;

    Table( Table const& ) = default;
    Table( Table&& )      = default;

    Table& operator= ( Table const& ) = default;
    Table& operator= ( Table&& )      = default;

    // void swap( Table& other )
    // {
    //     using std::swap;
    // }

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t length() const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    void clear();
    void clear_content();

    Column& add_column( std::string label = "" );

    Table& operator << ( std::string value );
    // Table& operator << ( Style const& value );

    Table& append ( std::string value );
    Table& append ( Style const& style, std::string value );

    // ---------------------------------------------------------------------
    //     Output
    // ---------------------------------------------------------------------

    void write( std::ostream& out ) const;
    void write( std::ostream& out, Layout const& layout ) const;

    std::string to_string() const;
    std::string to_string( Layout const& layout ) const;

    friend std::ostream& operator << (std::ostream& out, Table const& table);

    // ---------------------------------------------------------------------
    //     Data members
    // ---------------------------------------------------------------------

private:

    size_t current_col_ = 0;
    std::vector<Column> columns_;

};

// =================================================================================================
//     Table Column
// =================================================================================================

class Table::Column
{
    // ---------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------

public:

    enum class Justification
    {
        kLeft,
        kRight,
        kCentered
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

public:

    Column() = default;

    explicit Column( std::string label, Justification justify = Justification::kLeft )
        : label_( label )
        , just_(  justify )
        , width_( label.size() )
    {}

    ~Column() = default;

    Column( Column const& ) = default;
    Column( Column&& )      = default;

    Column& operator= ( Column const& ) = default;
    Column& operator= ( Column&& )      = default;

    // void swap( Column& other )
    // {
    //     using std::swap;
    // }

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    void        label( std::string value );
    std::string label() const;

    void          justify( Justification value );
    Justification justify() const;

    void   width( size_t value );
    size_t width() const;

    void shrink_width();

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    size_t length() const;

    std::string row( size_t i ) const;

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    void clear_content();

    void append( std::string value );
    void append( Style const& style, std::string value );

    // ---------------------------------------------------------------------
    //     Output
    // ---------------------------------------------------------------------

public:

    void write_row( std::ostream& stream, size_t row ) const;
    void write_label( std::ostream& stream ) const;

protected:

    void write( std::ostream& stream, std::string text ) const;

    // ---------------------------------------------------------------------
    //     Data members
    // ---------------------------------------------------------------------

private:

    std::string   label_ = "";
    Justification just_  = Justification::kLeft;
    size_t        width_ = 0;

    std::vector<std::string> data_;

};

// =================================================================================================
//     Table Layout
// =================================================================================================

struct Layout
{
    // ---------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------

    /**
     * @brief One line of the Layout.
     *
     * This struct has different roles depending on the kind of line:
     *
     *   * For lines separating parts of the Table (above the header, between header and content,
     *     and below the content), the attribute `enabled` is used when writing to determine
     *     whether to write this line at all. Also, the `filler` is used to fill the width of the
     *     columns.
     *   * For the header line and all content lines, the `enabled` attribute is ignored (as those
     *     lines always need to be written). The `filler` is also ignored, because instead of it
     *     the actual text is written.
     * %
     */
    struct Line
    {
        bool        enabled      = false;

        std::string left_border  = "";
        std::string filler       = "";
        std::string separator    = " ";
        std::string right_border = "";
    };

    /**
     * @brief Helper struct to bind a layout to a table.
     *
     * An std::stream can only take one argument at a time. This Binder servers as a single
     * argument that internally provides both a layout and a table. The `operator<<` of Binder is
     * overloaded so that the table is outputted to the stream with the layout.
     */
    struct Binder
    {
        Binder(Layout const& l, Table const& t)
            : layout(l)
            , table(t)
        {}

        Layout const& layout;
        Table const& table;

        friend std::ostream& operator << (std::ostream& out, Binder const& binder);
    };

    // ---------------------------------------------------------------------
    //     Operators
    // ---------------------------------------------------------------------

    Binder operator () ( Table const& table );

    // ---------------------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------------------

    Line top;
    Line header;
    Line separator;
    Line row;
    Line bottom;

};

// ---------------------------------------------------------------------
//     Default Table Layouts
// ---------------------------------------------------------------------

Layout minimal_layout();

Layout simple_layout(  bool condensed = false );
Layout simple_grid(    bool condensed = false );
Layout simple_frame(   bool condensed = false );

Layout extended_grid(  bool condensed = false );
Layout extended_frame( bool condensed = false );

Layout double_grid(    bool condensed = false );
Layout double_frame(   bool condensed = false );

} // namespace utils
} // namespace genesis

#endif // include guard
