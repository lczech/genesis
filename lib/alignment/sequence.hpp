#ifndef GENESIS_ALIGNMENT_SEQUENCE_H_
#define GENESIS_ALIGNMENT_SEQUENCE_H_

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

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    typedef char SymbolType;

    Sequence (std::string label, std::string sites) : label_(label), sites_(sites) {}
    ~Sequence();

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    inline std::string label() const
    {
        return label_;
    }

    inline size_t length() const
    {
        return sites_.size();
    }

    inline SymbolType site(size_t index) const
    {
        return sites_[index];
    }

    inline const std::string& sites() const
    {
        return sites_;
    }

    // -----------------------------------------------------
    //     Mutators
    // -----------------------------------------------------

    void remove_gaps();
    void replace(char search, char replace);

    // -----------------------------------------------------
    //     Dump and Debug
    // -----------------------------------------------------

    std::string dump() const;

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    SymbolType gap_char = '-';

protected:
    std::string label_;
    std::string sites_;
};

} // namespace genesis

#endif // include guard
