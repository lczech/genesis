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
namespace text {

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
    Table& append ( std::string value );

    // ---------------------------------------------------------------------
    //     Output
    // ---------------------------------------------------------------------

    void write( std::ostream& stream ) const;

    // ---------------------------------------------------------------------
    //     Data members
    // ---------------------------------------------------------------------

private:

    size_t current_col_;

    std::vector<Column> columns_;

};

// =================================================================================================
//     Text Column
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

} // namespace text
} // namespace genesis

#endif // include guard
