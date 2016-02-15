#ifndef GENESIS_SEQUENCE_SEQUENCE_H_
#define GENESIS_SEQUENCE_SEQUENCE_H_

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

    // -----------------------------------------------------
    //     Properties
    // -----------------------------------------------------

    std::string const& label() const;
    std::string&       label();
    void               label( std::string const& value );

    std::string const& metadata() const;
    std::string&       metadata();
    void               metadata( std::string const& value );

    std::string const& sites() const;
    std::string&       sites();
    void               sites( std::string const& value );
    void               sites( std::string &&     value );

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

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

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    std::string label_;
    std::string metadata_;
    std::string sites_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
