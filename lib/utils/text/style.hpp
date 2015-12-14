#ifndef GENESIS_UTILS_TEXT_STYLE_H_
#define GENESIS_UTILS_TEXT_STYLE_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <array>
#include <utility>
#include <string>

namespace genesis {
namespace text {

// =================================================================================================
//     Text Style
// =================================================================================================

class Style
{
    // -------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------

public:

    explicit Style( std::string text )
        : text_(text)
    {}

    // explicit Style( std::string text, bool bold )
    //     : text_(text)
    //     , bold_(bold)
    // {}

    explicit Style( std::string text, std::string foreground_color )
        : text_(text)
        , foreground_(foreground_color)
    {
        get_foreground_color( foreground_color );
    }

    // explicit Style( std::string text, std::string foreground_color, bool bold )
    //     : text_(text)
    //     , foreground_(foreground_color)
    //     , bold_(bold)
    // {
    //     get_foreground_color( foreground_color );
    // }

    explicit Style( std::string text, std::string foreground_color, std::string background_color )
        : text_(text)
        , foreground_(foreground_color)
        , background_(background_color)
    {
        get_foreground_color( foreground_color );
        get_background_color( background_color );
    }

    // explicit Style( std::string text, std::string foreground_color, std::string background_color, bool bold )
    //     : text_(text)
    //     , foreground_(foreground_color)
    //     , background_(background_color)
    //     , bold_(bold)
    // {
    //     get_foreground_color( foreground_color );
    //     get_background_color( background_color );
    // }

    ~Style() = default;

    Style(Style const&) = default;
    Style(Style&&)      = default;

    Style& operator= (Style const&) = default;
    Style& operator= (Style&&)      = default;

    void swap (Style& other) noexcept
    {
        using std::swap;

        swap(text_, other.text_);
        swap(foreground_, other.foreground_);
        swap(background_, other.background_);
        swap(bold_, other.bold_);
    }

    // -------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------

    size_t size() const
    {
        return text_.size();
    }

    std::string text() const
    {
        return text_;
    }

    void text( std::string const& value )
    {
        text_ = value;
    }

    bool bold() const
    {
        return bold_;
    }

    void bold( bool value )
    {
        bold_ = value;
    }

    std::string foreground_color() const
    {
        return foreground_;
    }

    void foreground_color( std::string const& color )
    {
        get_foreground_color( color );
        foreground_ = color;
    }

    std::string background_color() const
    {
        return background_;
    }

    void background_color( std::string const& color )
    {
        get_background_color( color );
        background_ = color;
    }

    // -------------------------------------------------------------------
    //     Output
    // -------------------------------------------------------------------

    std::string to_bash_string() const;
    std::string to_python_string() const;

    // -------------------------------------------------------------------
    //     Style Data
    // -------------------------------------------------------------------

    static bool is_foreground_color( std::string name );
    static bool is_background_color( std::string name );

    static std::string get_foreground_color( std::string name );
    static std::string get_background_color( std::string name );

    static const std::array<std::pair<std::string, std::string>, 17> foreground_colors;
    static const std::array<std::pair<std::string, std::string>, 17> background_colors;

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

private:

    std::string text_;

    std::string foreground_;
    std::string background_;

    bool bold_ = false;
};

// =================================================================================================
//     Basic Operators
// =================================================================================================

inline void swap (Style& lhs, Style& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace text
} // namespace genesis

#endif // include guard
