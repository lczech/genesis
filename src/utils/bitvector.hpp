#ifndef GNS_UTILS_BITVECTOR_H_
#define GNS_UTILS_BITVECTOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace genesis {

// =============================================================================
//     Bitvector
// =============================================================================

class Bitvector
{
public:

    // ---------------------------------------------------------
    //     Declarations and Class Functions
    // ---------------------------------------------------------

    typedef uint64_t    IntType;
    static const size_t IntSize = 64;

    friend Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs);
    //~ friend Bitvector operator - (Bitvector const& lhs, Bitvector const& rhs);

    friend std::ostream& operator << (std::ostream& out, Bitvector const& rhs);

    Bitvector (const size_t size, const bool init = false) : size_(size)
    {
        // reserve enough bits/
        data_.resize( (size / IntSize) + (size % IntSize == 0 ? 0 : 1) );

        // init according to flag.
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] = init ? all_1_ : all_0_;
        }

        // if we initialized with true, we need to unset the surplus bits at the end!
        if (init) {
            // we need to the (slow) loop instead of the (fast) approach below it, because the
            // latter does only work for little endian processors... i think.
            for (size_t i = size % IntSize; i < IntSize; ++i) {
                data_.back() ^= bit_mask_[i];
            }
            //~ data_.back() &= bit_mask_[size % IntSize] - 1;
        }
    }

    Bitvector (const size_t size, const std::initializer_list<int> list) : Bitvector(size, false)
    {
        for (int e : list) {
            Set(e);
        }
    }

    inline size_t size() const
    {
        return size_;
    }

    // ---------------------------------------------------------
    //     Single Bit Functions
    // ---------------------------------------------------------

    inline bool operator [] (size_t index) const {
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
    }

    inline bool Get (size_t index) const
    {
        if (index >= size_) {
            return false;
        }
        return static_cast<bool> (data_[index / IntSize] & bit_mask_[index % IntSize]);
    }

    inline void Set (size_t index)
    {
        data_[index / IntSize] |= bit_mask_[index % IntSize];
    }

    inline void Unset (size_t index)
    {
        data_[index / IntSize] &= ~(bit_mask_[index % IntSize]);
    }

    inline void Set (size_t index, bool value)
    {
        if (value) {
            Set(index);
        } else {
            Unset(index);
        }
    }

    inline void Flip (size_t index)
    {
        data_[index / IntSize] ^= bit_mask_[index % IntSize];
    }

    // ---------------------------------------------------------
    //     Operators
    // ---------------------------------------------------------

    Bitvector& operator &= (Bitvector const& rhs);
    Bitvector& operator |= (Bitvector const& rhs);
    Bitvector& operator ^= (Bitvector const& rhs);
    Bitvector  operator ~  () const;

    bool operator == (const Bitvector &other) const;
    bool operator != (const Bitvector &other) const
    {
        return !(*this == other);
    }

    inline bool operator <  (Bitvector const& rhs) const
    {
        return ((*this & rhs) == *this)  && (Count() < rhs.Count());
    }

    inline bool operator >  (Bitvector const& rhs) const
    {
        return rhs < *this;
    }

    inline bool operator <= (Bitvector const& rhs) const
    {
        return (*this == rhs) || (*this < rhs);
    }

    inline bool operator >= (Bitvector const& rhs) const
    {
        return (*this == rhs) || (*this > rhs);
    }

    // ---------------------------------------------------------
    //     Other Functions
    // ---------------------------------------------------------

           Bitvector SymmetricDifference (                      Bitvector const& rhs) const;
    static Bitvector SymmetricDifference (Bitvector const& lhs, Bitvector const& rhs);

    size_t  Count() const;
    size_t  Hash()  const;
    IntType XHash() const;

    void    Invert();
    void    Normalize();

    std::string Dump() const;

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

protected:

    size_t               size_;
    std::vector<IntType> data_;

    static const IntType all_0_;
    static const IntType all_1_;
    static const IntType bit_mask_[IntSize];
};

} // namespace genesis

// =============================================================================
//     Namespace std extension
// =============================================================================

namespace std {

template<>
struct hash<genesis::Bitvector>
{
    size_t operator() (genesis::Bitvector const &rhs) const
    {
      return rhs.Hash();
    }
};

} // namespace std

#endif // include guard
