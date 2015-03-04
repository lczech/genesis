#ifndef GNS_ALIGNMENT_SEQUENCE_H_
#define GNS_ALIGNMENT_SEQUENCE_H_

/**
 * @brief
 *
 * @file
 * @ingroup alignment
 */

#include <string>

namespace genesis {

class Sequence
{
public:

    typedef char SymbolType;

    Sequence () {}
    Sequence (std::string l) : label_(l) {}
    Sequence (std::string l, SymbolType* s, size_t n) : label_(l), sites_(s), length_(n) {}

    inline std::string Label()
    {
        return label_;
    }

    inline size_t Length()
    {
        return length_;
    }

    inline std::string Sites()
    {
        return std::string(sites_, length_);
    }

protected:
    std::string label_;
    SymbolType* sites_;
    size_t      length_;
};

} // namespace genesis

#endif // include guard
