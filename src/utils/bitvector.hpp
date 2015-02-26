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
    static const size_t IntSize = sizeof(IntType) * 8;

    friend Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs);
    friend Bitvector operator - (Bitvector const& lhs, Bitvector const& rhs);

    friend std::ostream& operator << (std::ostream& out, Bitvector const& rhs);

    /**
     * @brief Constructor that takes a size and an optional bool value to initialize the Bitvector,
     * false by default.
     */
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
            UnsetBuffer();
        }
    }

    /**
     * @brief Constructor that takes a size and a list of values (positions) to be set to true.
     */
    Bitvector (const size_t size, const std::initializer_list<int> list) : Bitvector(size, false)
    {
        for (int e : list) {
            Set(e);
        }
    }

    /**
     * @brief Returns the size (number of bits) of this Bitvector.
     */
    inline size_t size() const
    {
        return size_;
    }

    // ---------------------------------------------------------
    //     Single Bit Functions
    // ---------------------------------------------------------

    /**
     * @brief Return the value of a single bit, without boundary check.
     */
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
        if (index >= size_) {
            return;
        }
        data_[index / IntSize] |= bit_mask_[index % IntSize];
    }

    inline void Unset (size_t index)
    {
        if (index >= size_) {
            return;
        }
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
        if (index >= size_) {
            return;
        }
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

    /**
     * @brief Strict subset.
     */
    inline bool operator <  (Bitvector const& rhs) const
    {
        return ((*this & rhs) == *this) && (Count() < rhs.Count());
    }

    /**
     * @brief Strict superset.
     */
    inline bool operator >  (Bitvector const& rhs) const
    {
        return rhs < *this;
    }

    /**
     * @brief Subset or equal.
     */
    inline bool operator <= (Bitvector const& rhs) const
    {
        return (*this == rhs) || (*this < rhs);
    }

    /**
     * @brief Superset or equal.
     */
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
    std::string DumpInt(IntType x) const;

    // ---------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------

protected:

    void UnsetBuffer();

    static const IntType all_0_;
    static const IntType all_1_;

    static const IntType bit_mask_[IntSize];
    static const IntType ones_mask_[IntSize];

    static const IntType count_mask_[4];

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

    size_t               size_;
    std::vector<IntType> data_;
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
