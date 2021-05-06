#ifndef GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

#include <functional>
#include <memory>

namespace genesis {
namespace utils {

// =================================================================================================
//      Lambda Iterator
// =================================================================================================

/**
 * @brief Type erasure for iterators, using std::function to get rid of the underlying input type.
 *
 * This class offers some abstraction to get a uniform iterator type over some underlying iterator.
 * Expects a function (most likely, you want to use a lambda) that converts the underlying data
 * into the desired type T, which is where the type erasure happens. The data that is iterated over
 * is kept in a shared pointer, which at the end of the iteration (when the underlying iterator
 * is done) simply has to be returned as nullptr by the lambda to indicate the end of the iteration.
 *
 * Example:
 *
 *     // Convert from an iterator over VcfRecord to Variant.
 *     auto beg = vcf_range.begin();
 *     auto end = vcf_range.end();
 *
 *     // Create the conversion with type erasure via the lambda function.
 *     generator = LambdaIteratorGenerator<Variant>(
 *         [beg, end]() mutable -> std::shared_ptr<Variant>{
 *             if( beg != end ) {
 *                 auto res = std::make_shared<Variant>( convert_to_variant(*beg) );
 *                 ++beg;
 *                 return res;
 *             } else {
 *                 return nullptr;
 *             }
 *         }
 *     );
 *
 *     // Iterate over generator.begin() and generator.end()
 *     for( auto it : generator ) ...
 *
 * For other types of iterators, instead of `beg` and `end`, other input can be used:
 *
 *     // Use a pileup iterator, which does not offer begin and end.
 *     auto it = SimplePileupInputIterator( utils::from_file( pileup_file_.value ), reader );
 *     generator = LambdaIteratorGenerator<Variant>(
 *         [it]() mutable -> std::shared_ptr<Variant>{
 *             if( it ) {
 *                 auto res = std::make_shared<Variant>( convert_to_variant(*it) );
 *                 ++it;
 *                 return res;
 *             } else {
 *                 return nullptr;
 *             }
 *         }
 *     );
 *
 * And accordinly for other underlying iterator types.
 */
template<class T>
class LambdaIteratorGenerator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = LambdaIteratorGenerator;
    using value_type        = T;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Iterator
    // -------------------------------------------------------------------------

    class LambdaIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = LambdaIteratorGenerator<T>::LambdaIterator;

        // LambdaIterator() = default;

        LambdaIterator(
            LambdaIteratorGenerator* generator,
            std::shared_ptr<T> current_element
        )
            : generator_(generator)
            , current_element_(current_element)
        {}

        ~LambdaIterator() = default;

        LambdaIterator( self_type const& ) = default;
        LambdaIterator( self_type&& )      = default;

        LambdaIterator& operator= ( self_type const& ) = default;
        LambdaIterator& operator= ( self_type&& )      = default;

        // friend LambdaIteratorGenerator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        T const& operator*() const
        {
            return *current_element_;
        }

        // T operator*()
        // {
        //     return *current_element_;
        // }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
        {
            current_element_ = generator_->get_element_();
            return *this;
        }

        self_type& operator ++(int)
        {
            auto cpy = *this;
            current_element_ = generator_->get_element_();
            return cpy;
        }

        bool operator==( self_type const& it ) const
        {
            return current_element_ == it.current_element_;
        }

        bool operator!=( self_type const& it ) const
        {
            return !(*this == it);
        }

    private:

        LambdaIteratorGenerator* generator_;
        std::shared_ptr<T> current_element_;

    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    LambdaIteratorGenerator() = default;

    LambdaIteratorGenerator(
        std::function<std::shared_ptr<value_type>()> get_element
    )
        : get_element_(get_element)
    {}

    ~LambdaIteratorGenerator() = default;

    LambdaIteratorGenerator( self_type const& ) = default;
    LambdaIteratorGenerator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    friend LambdaIterator;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    LambdaIterator begin()
    {
        return LambdaIterator( this, get_element_() );
    }

    LambdaIterator end()
    {
        return LambdaIterator( this, nullptr );
    }

    /**
     * @brief Return whether a function was assigend to this generator, that is, whether it is
     * default constructed (`false`) or not (`true`).
     */
    operator bool() const
    {
        return static_cast<bool>( get_element_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::function<std::shared_ptr<value_type>()> get_element_;

};

/**
 * @brief Alias for the internal iterator of a LambdaIteratorGenerator for easier usage.
 */
template<class T>
using LambdaIterator = typename LambdaIteratorGenerator<T>::LambdaIterator;

} // namespace utils
} // namespace genesis

#endif // include guard
