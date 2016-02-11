#ifndef GENESIS_UTILS_IO_COUNTING_ISTREAM_ITERATOR_H_
#define GENESIS_UTILS_IO_COUNTING_ISTREAM_ITERATOR_H_

/**
 * @brief Header of CountingIstreamIterator class.
 *
 * @file
 * @ingroup utils
 */

#include <iterator>
#include <stdexcept>
#include <streambuf>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Istream Iterator
// =================================================================================================

/**
 * @brief Wrapper class to iterate over an std::istream while keeping track of lines and columns.
 *
 * It provides most functionality of std::istreambuf_iterator, thus it is (almost) a drop-in
 * replacement. Differences are:
 *
 *   * The handling of '\r' characters (part of the CR+LF new lines as used in Windows): they are
 *     completely skipped in this iterator. This ensures that all new lines are simply represented
 *     as '\n' independent of the file format.
 *   * It has no copy constructor/assignment. This is to ensure that the line and column counting
 *     works properly. Therefore, also only pre-increment is allowed. The iterator must thus also
 *     be passed by reference.
 *
 * It has two member functions line() and column() that return the corresponding values for the
 * current iterator position and a member function get() that provides a checked version of the
 * dereference operator.
 */
class CountingIstreamIterator
{
public:

    // -------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------

    using self_type         = CountingIstreamIterator;
    using iterator_category = std::input_iterator_tag;
    using value_type        = char;

    using difference_type   = std::istreambuf_iterator<char>::difference_type;
    using pointer           = std::istreambuf_iterator<char>::pointer;
    using reference         = std::istreambuf_iterator<char>::reference;

    /*
     * important
     * http://www.drdobbs.com/the-standard-librarian-streambufs-and-st/184401357
     *
     * istream iterator and buffer -> istreambuf_iterator
     * http://www.cplusplus.com/reference/iterator/istreambuf_iterator/
     * http://insanecoding.blogspot.de/2011/11/how-to-read-in-file-in-c.html
     *
     * usage
     * http://www.drdobbs.com/cpp/iterators-and-iostreams/232600539
     */

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    CountingIstreamIterator()
        : it_()
        , line_(0)
        , column_(0)
    {}

    CountingIstreamIterator(std::istream& in)
        : it_(in)
        , line_(1)
        , column_(1)
    {}

    CountingIstreamIterator(std::streambuf* in_buf)
        : it_(in_buf)
        , line_(1)
        , column_(1)
    {}

    ~CountingIstreamIterator() = default;

    // Copy construction deleted to ensure correct line/column counting!
    CountingIstreamIterator(self_type const&)     = delete;
    CountingIstreamIterator(self_type&&) noexcept = default;

    // Copy construction deleted to ensure correct line/column counting!
    self_type& operator= (self_type const&)     = delete;
    self_type& operator= (self_type&&) noexcept = default;

    void swap (self_type& other) noexcept
    {
        using std::swap;
        swap(it_,     other.it_);
        swap(line_,   other.line_);
        swap(column_, other.column_);
    }

    // -------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------

    /**
     * @brief Dereference operator. Return the current char.
     *
     * This is the "raw" version for extracting chars from the stream. See `get()` for a checked
     * version.
     */
    char operator * () const
    {
        return *it_;
    }

    /**
     * @brief Return the current char, with range check.
     *
     * This function is similar to the dereference operator, but additionally performs a check of
     * the current char: This iterator is meant for ASCII (or similar) text format encodings, and
     * its output should be usable for lookup tables etc. Thus, this function ensures that the char
     * is in the range [0, 127]. If not, an `std::domain_error` is thrown.
     */
    char get() const
    {
        if( *it_ < 0 ) {
            throw std::domain_error( "Invalid input char at " + at() + "." );
        }
        return *it_;
    }

    /**
     * @brief Return the current line of the input stream.
     *
     * The counter starts with line 1 for input stream. The value for default constructed objects
     * (no stream provided) is 0, to be able to distinguish it from actual streams.
     */
    size_t line() const
    {
        return line_;
    }


    /**
    * @brief Return the current column of the input stream.
    *
    * The counter starts with column 1 for each line of the input stream. The value for default
    * constructed objects (no stream provided) is 0, to be able to distinguish it from actual
    * streams. New line characters `\n` are included in counting and count as the last character
    * of a line.
    */
    size_t column() const
    {
        return column_;
    }

    /**
     * @brief Return a textual representation of the current input position in the form
     * "line:column".
     */
    std::string at() const
    {
        return std::to_string( line_ ) + ":" + std::to_string( column_ );
    }

    // -------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------

    self_type& operator ++ ()
    {
        ++column_;

        // Treat new lines always as a simple '\n' internally.
        // This works for both unix and Win formatted files. It does not work for Mac OS <= 9,
        // as this uses a single \r for delimiting lines, thus the line and column counter will
        // never be increased in this case and the whitespace not outputted.
        // Maybe in the future we could provide some compile time handling of this issue, but for
        // now, this solution should work in most cases.

        char c = *it_;
        if (c == '\r') {
            ++it_;
        }
        if (c == '\n') {
            ++line_;
            column_ = 1;
        }

        ++it_;
        return *this;
    }

    /*
    // disabled post-increment operator: this is taking a copy of the object, which is not allowed!
    self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }
    */

    bool operator == (self_type const& other) const
    {
        return it_ == other.it_;
    }

    bool operator != (self_type const& other) const
    {
        return !(*this == other);
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    std::istreambuf_iterator<char> it_;

    size_t line_;
    size_t column_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
