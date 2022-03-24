#ifndef GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/utils/containers/optional.hpp"
#include "genesis/utils/core/thread_pool.hpp"

#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//      Helpers
// =================================================================================================

/**
 * @brief Empty helper data struct to serve as a dummy for LambdaIterator.
 *
 * By default, the LambdaIterator::Data type does not do anything.
 * This class here serves as the empty dummy class, so that the user does not have to
 * provide one when using LambdaIterator without an extra data type.
 */
struct EmptyLambdaIteratorData
{};

// =================================================================================================
//      Lambda Iterator
// =================================================================================================

/**
 * @brief Type erasure for iterators, using std::function to eliminate the underlying input type.
 *
 * This class offers an abstraction to get a uniform iterator type over a set of underlying iterators
 * of different type. It expects a function (most likely, you want to use a lambda) that converts
 * the underlying data into the desired type T, which is where the type erasure happens.
 * The data that is iterated over is kept in an @link utils::Optional Optional@endlink,
 * which at the end of the iteration (when the underlying iterator is done) simply has to be
 * returned as @link genesis::utils::nullopt nullopt@endlink by the lambda to indicate the end
 * of the iteration.
 *
 * Example:
 *
 * ~~~{.cpp}
 *     // Convert from an iterator over VcfRecord to Variant.
 *     auto beg = vcf_range.begin();
 *     auto end = vcf_range.end();
 *
 *     // Create the conversion with type erasure via the lambda function.
 *     auto generator = LambdaIterator<Variant>(
 *         [beg, end]() mutable -> genesis::utils::Optional<Variant>{
 *             if( beg != end ) {
 *                 auto res = utils::make_optional<Variant>( convert_to_variant(*beg) );
 *                 ++beg;
 *                 return res;
 *             } else {
 *                 return genesis::utils::nullopt;
 *             }
 *         }
 *     );
 *
 *     // Iterate over generator.begin() and generator.end()
 *     for( auto const& it : generator ) ...
 * ~~~
 *
 * For other types of iterators, instead of `beg` and `end`, other input can be used:
 *
 * ~~~{.cpp}
 *     // Use a pileup iterator, which does not offer begin and end.
 *     auto it = SimplePileupInputIterator( utils::from_file( pileup_file_.value ), reader );
 *     auto generator = LambdaIterator<Variant>(
 *         [it]() mutable -> genesis::utils::Optional<Variant>{
 *             if( it ) {
 *                 auto res = utils::make_optional<Variant>( convert_to_variant(*it) );
 *                 ++it;
 *                 return res;
 *             } else {
 *                 return genesis::utils::nullopt;
 *             }
 *         }
 *     );
 * ~~~
 *
 * And accordingly for other underlying iterator types.
 *
 * In addition to the type `T` that we iterate over, for user convenience, we also offer to use
 * a data storage variable of the template type `D` (typedef'd as LambdaIterator::Data).
 * This data is provided at construction of the LambdaIterator, and can be accessed via the
 * @link LambdaIterator::data() data()@endlink functions.
 * it is a generic extra variable to store iterator-specific information.
 * As the LambdaIterator is intended to be initializable with just a lambda function that yields
 * the elements to traverse over, there is otherwise no convenient way to access related information
 * of the underlying iterator. For example, when iterating over a file, one might want to store
 * the file name or other characteristics of the input in the data().
 *
 * The class furthermore offers filters and transformations of the underlying iterator data,
 * using the functions add_filter(), add_transform(), and add_transform_filter(), which can all be
 * mixed and are executed as a combined list in the order in which they were added using these
 * three functions (that is, it can be first a filter, then a transformation, then a filter again).
 * This allows to easily skip elements of the underlying iterator without the need to add an
 * additional layer of abstraction.
 *
 * Lastly, the class offers block buffering in a separate thread, for speed up. This capability
 * takes care of the underlying iterator processing (including potential file parsing etc),
 * and buffers blocks of elements, so that the actual user function of this class has faster access
 * to it. For example, when processing data along a genome with lots of computations per position,
 * it makes sense to run the file reading in a separate thread and buffer positions as needed,
 * which this class does automatically. This can be deactivated by setting the block size to 0.
 *
 * We are aware that with all this extra functionality, the class is slighly overloaded, and that
 * the filters and the block buffering would typically go in separate classes for modularity.
 * However, we are taking user convenience and speed into account here: Instead of having to add
 * filters or a block buffer wrapper around each input iterator that is then wrapped in a
 * LambdaIterator anyway, we rather take care of this in one place; this also reduces levels of
 * abstraction, and hence (hopefully) increases processing speed.
 *
 * @see VariantInputIterator for a use case of this iterator that allows to traverse different
 * input file types that all are convertible to @link genesis::population::Variant Variant@endlink.
 */
template<class T, class D = EmptyLambdaIteratorData>
class LambdaIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = LambdaIterator;
    using value_type        = T;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    using Data              = D;

    /**
     * @brief Default size for block buffering.
     *
     * The class by default buffers blocks of elements of this size, with the buffer loaded in a
     * separate thread, in order to speed up iterating over elements that need some processing,
     * such as input files, which is the typical use case of this class.
     */
    static size_t const DEFAULT_BLOCK_SIZE = 1024;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator over the data.
     */
    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type         = LambdaIterator<T, D>::Iterator;
        using value_type        = T;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        using Data              = D;

    private:

        Iterator() = default;

        Iterator(
            LambdaIterator const* generator
        )
            : generator_(     generator )
            , current_block_( std::make_shared<std::vector<T>>() )
            , buffer_block_(  std::make_shared<std::vector<T>>() )
            , thread_pool_(   std::make_shared<utils::ThreadPool>( 1 ))
            , future_(        std::make_shared<std::future<size_t>>() )
        {
            // We use the generator as a check if this Iterator is intended to be a begin()
            // or end() iterator. If its the former, init and get the first element block.
            // We could also just use the default constructor to create end() iterators,
            // this would have the same effect.
            // After we are done iterating the input, we then set the generator_ to nullptr,
            // as a sign that we are done. This allows us also to know if we reached end() without
            // having to store the end() iterator when using this class.
            if( generator_ ) {
                if( ! generator_->get_element_ ) {
                    throw std::invalid_argument(
                        "Cannot use LambdaIterator without a function to get elements."
                    );
                }

                // Initialize the current_block_ and buffer_block_,
                // and read the first block(s) of the file.
                init_();
            }
        }

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend LambdaIterator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const * operator->() const
        {
            assert( current_block_ );
            assert( current_pos_ < end_pos_ );
            assert( current_pos_ < current_block_->size() );
            return &((*current_block_)[current_pos_]);
        }

        value_type * operator->()
        {
            assert( current_block_ );
            assert( current_pos_ < end_pos_ );
            assert( current_pos_ < current_block_->size() );
            return &((*current_block_)[current_pos_]);
        }

        value_type const & operator*() const
        {
            assert( current_block_ );
            assert( current_pos_ < end_pos_ );
            assert( current_pos_ < current_block_->size() );
            return (*current_block_)[current_pos_];
        }

        value_type & operator*()
        {
            assert( current_block_ );
            assert( current_pos_ < end_pos_ );
            assert( current_pos_ < current_block_->size() );
            return (*current_block_)[current_pos_];
        }

        /**
         * @brief Access the data stored in the iterator.
         */
        Data const& data() const
        {
            if( ! generator_ ) {
                throw std::runtime_error(
                    "Cannot access default constructed or past-the-end LambdaIterator content."
                );
            }
            assert( generator_ );
            return generator_->data_;
        }

        // We do not offer non-const access at the moment. If we did,
        // the generator pointer stored in this iterator would need to be non-const as well.

        // /* *
        //  * @brief Access the data stored in the iterator.
        //  */
        // Data& data()
        // {
        //     if( ! generator_ ) {
        //         throw std::runtime_error(
        //             "Cannot access default constructed or past-the-end LambdaIterator content."
        //         );
        //     }
        //     assert( generator_ );
        //     return generator_->data_;
        // }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++()
        {
            increment_();
            return *this;
        }

        // self_type operator ++(int)
        // {
        //     auto cpy = *this;
        //     increment_();
        //     return cpy;
        // }

        /**
         * @brief Return whether the iterator is at a valid position,
         * that is, if its input has yielded an element.
         */
        operator bool() const
        {
            // We here rely on the fact that we set the generator_ to nullptr if this is either
            // the end() iterator or if the iteration has reached its end after the data is done.
            return generator_ != nullptr;
        }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are copies of each other without having moved will compare equal,
         * as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal,
         * no matter from which LambdaIterator they were created.
         * Two past-the-end iterators compare equal.
         */
        bool operator==( self_type const& other ) const
        {
            // We compare the generators as a baseline - two past-the-end iterator shall
            // always compare equal. If only one of them is past-the-end, they will compare false.
            // Only if both are valid (not past-the-end) iterators, we compare their current
            // position in the block - two iterators to the same position in the same block
            // (using pointer comparison for the block - not actual data comparison) are equal.
            if( ! generator_ || ! other.generator_ ) {
                // generator_ is used as the indicator whether this is a past-the-end iterator
                // (in which case it is a nullptr), or not.
                return generator_ == other.generator_;
            }
            assert( generator_ && other.generator_ );
            return current_block_ == other.current_block_ && current_pos_ == other.current_pos_;
        }

        bool operator!=( self_type const& other ) const
        {
            return !(*this == other);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        void init_()
        {
            // Check that they are set up from the constructor.
            assert( generator_ );
            assert( current_block_ );
            assert( buffer_block_ );

            // Edge case: no buffering.
            // Block size zero indicates to use no buffering, so we just use a single element.
            if( generator_->block_size_ == 0 ) {
                // Make space for it, read the first element, and then we are done here.
                current_block_->resize( 1 );
                end_pos_ = 1;
                increment_();
                return;
            }

            // Init the records and create empty VcfRecord to read into.
            // The blocks have been initialized in the contructor already; assert this.
            current_block_->resize( generator_->block_size_ );
            buffer_block_->resize(  generator_->block_size_ );
            assert( current_block_->size() == generator_->block_size_ );
            assert( buffer_block_->size()  == generator_->block_size_ );

            // Read the first block synchronously,
            // so that there is data initialized to be dereferenced.
            end_pos_ = read_block_( generator_, current_block_, generator_->block_size_ );
            assert( current_pos_ == 0 );

            // If there is less data than the block size, the file is already done.
            // No need to start the async buffering, we can just get out of here.
            if( end_pos_ < generator_->block_size_ ) {

                // Edge case: zero elements read. We are already done then.
                if( end_pos_ == 0 ) {
                    generator_ = nullptr;
                }
                return;
            }

            // If we are here, the first block was fully read,
            // so we start the async worker thread to fill the buffer.
            assert( end_pos_ == generator_->block_size_ );
            fill_buffer_block_();
        }

        void increment_()
        {
            assert( generator_ );
            assert( current_block_ && current_block_->size() > 0 );

            // Edge case: no buffering.
            // Read the next element. If there is none, we are done.
            if( generator_->block_size_ == 0 ) {
                assert( current_block_->size() == 1 );
                if( ! get_next_element_( generator_, (*current_block_)[0] )) {
                    generator_ = nullptr;
                }
                return;
            }

            // Finish the reading (potentially waiting if not yet finished in the worker thread).
            // The future returns how much data there was to be read, which we use as our status.
            // After that, swap the buffer and start a new reading operation in the worker thread.

            // Move to the next element in the vector.
            ++current_pos_;
            assert( current_pos_ <= end_pos_ );

            // If we are at the end of the data, we are either done iterating,
            // or reached the end of the current buffer block.
            if( current_pos_ == end_pos_ ) {

                // If we did not get a full block size when reading, we are done iterating.
                // Indicate this by unsetting the generator_ pointer.
                if( end_pos_ < generator_->block_size_ ) {
                    generator_ = nullptr;
                    return;
                }

                // If we are at the end of the record vector, and if that vector was full
                // (complete block size), there is more data, so start reading.

                assert( end_pos_ == generator_->block_size_ );
                assert( future_ );
                assert( future_->valid() );

                // Get how many records were read into the buffer, which also waits for the reading
                // if necessary. If we did not read any now, that means that the number of total
                // elements is a multiple of the block size, which we handle as a special case here.
                // There is probably some better way to restructure the code to avoid this edge
                // case... but good enough for now.
                end_pos_ = future_->get();
                if( end_pos_ == 0 ) {
                    generator_ = nullptr;
                    return;
                }

                // Here, we know that there is more data, so we can swap the buffer,
                // start reading again, and set or internal current location
                // to the first element of the vector again.
                assert( end_pos_ > 0 && end_pos_ <= generator_->block_size_ );
                std::swap( buffer_block_, current_block_ );
                fill_buffer_block_();
                current_pos_ = 0;
            }
        }

        void fill_buffer_block_()
        {
            // Those shared pointers have been initialized in the constructor; let's assert this.
            assert( generator_ );
            assert( thread_pool_ );
            assert( future_ );

            // This function is only every called after we finished any previous operations,
            // so let's assert that the thread pool and future are in the states that we expect.
            assert( thread_pool_->load() == 0 );
            assert( ! future_->valid() );

            // In order to use lambda captures by copy for class member variables in C++11, we first
            // have to make local copies, and then capture those. Capturing the class members direclty
            // was only introduced later. Bit cumbersome, but gets the job done.
            auto generator    = generator_;
            auto buffer_block = buffer_block_;
            auto block_size   = generator_->block_size_;

            // The lambda returns the result of read_block_ call, that is, the number of records
            // that have been read, and which we later (in the future_) use to see how much data we got.
            *future_ = thread_pool_->enqueue(
                [ generator, buffer_block, block_size ](){
                    return read_block_( generator, buffer_block, block_size );
                }
            );
        }

        /**
         * @brief Read a block of data into a buffer, and return the number of elements read.
         */
        static size_t read_block_(
            LambdaIterator const* generator,
            std::shared_ptr<std::vector<T>> buffer_block,
            size_t block_size
        ) {
            // This is a static function that does not depend on the class member data, so that
            // we can use it from the future lambda in the thread pool above without having to worry
            // about lambda captures of `this` going extinct... which was an absolutely nasty bug to
            // find! Such a rookie mistake! For that reason, we also take all arguments as shared
            // pointers, so that they are kept alive while the thread pool is working.
            // However, once its done with its work, the function (the one that we give to the thread
            // pool with a lambda) is popped from the thread queue, so that the shared pointer can
            // be freed again - that is, we do not need to worry about the lambda keeping the shared
            // pointer from freeing its memory indefinitely.

            assert( generator );
            assert( buffer_block->size() == block_size );

            // Read as long as there is data. Return number of read records.
            size_t i = 0;
            for( ; i < block_size; ++i ) {
                if( ! get_next_element_( generator, (*buffer_block)[i] )) {
                    break;
                }
            }
            return i;
        }

        /**
         * @brief Read an element from the underlying iterator, store it in a @p target,
         * and return whether an element was found.
         *
         * This also applies the filtering and transformations as needed. The function is static,
         * so that it can be called asynchronously without the iterator going out of scope.
         */
        static bool get_next_element_(
            LambdaIterator const* generator,
            T& target
        ) {
            assert( generator );
            bool got_element = false;
            while( true ) {
                // Get the next element from the input source, stored in an Optional<T>.
                auto new_elem = generator->get_element_();

                if( new_elem ) {
                    // If this is an element (not yet at the end of the data),
                    // apply all transforms and filters, and get out of here if all of them
                    // return `true`, that is, if none of them wants to filter out the element.
                    // If however one of them returns `false`, we need to find another element.
                    got_element = true;
                    for( auto const& tra_fil : generator->transforms_and_filters_ ) {
                        got_element = tra_fil( *new_elem );
                        if( ! got_element ) {
                            // If one of the transforms/filters does not want us to continue,
                            // we do not call the others. Break out of the inner loop.
                            break;
                        }
                    }

                    // If we got out of the above loop without any failing filters,
                    // we have fonud an element, so let's move it to our target.
                    if( got_element ) {
                        // We already checked that new_elem holds data, so no need to repeat,
                        // just assert it. Move, then break out of the while loop.
                        assert( new_elem );
                        target = std::move( *new_elem );
                        break;
                    }
                    // Else (got_element == false): Loop again, try the next element.

                } else {
                    // If this is not a valid element, we reached the end of the input.
                    // We note this in the return value, then break out of the while loop.
                    assert( new_elem == nullopt );
                    got_element = false;
                    break;
                }
            }
            return got_element;
        }

    private:

        // Parent.
        LambdaIterator const* generator_ = nullptr;

        // Buffer buffering data, and positions in it.
        std::shared_ptr<std::vector<T>> current_block_;
        std::shared_ptr<std::vector<T>> buffer_block_;
        size_t current_pos_ = 0;
        size_t end_pos_ = 0;

        // Thread pool to run the buffering in the background.
        // Also, store the future_ used to keep track of the background task. It returns the number of
        // elements that have been read into the buffer (block_size_, or less at the end of the file).
        std::shared_ptr<utils::ThreadPool> thread_pool_;
        std::shared_ptr<std::future<size_t>> future_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    LambdaIterator() = default;

    /**
     * @brief Create an iterator over some underlying content.
     *
     * The constructor expects the function that returns elements as long as there is underlying
     * data, and returns and empty optional (@link genesis::utils::nullopt nullopt@endlink)
     * once the end is reached.
     */
    LambdaIterator(
        std::function<utils::Optional<value_type>()> get_element,
        size_t block_size = DEFAULT_BLOCK_SIZE
    )
        : get_element_(get_element)
        , block_size_( block_size )
    {}

    /**
     * @copydoc LambdaIterator( std::function<utils::Optional<value_type>()>, size_t )
     *
     * Additionally, @p data can be given here, which we simply store and make accessible
     * via data(). This is a convenience so that iterators generated via a `make` function
     * can for example forward their input source name for user output.
     */
    LambdaIterator(
        std::function<utils::Optional<value_type>()> get_element,
        Data const& data,
        size_t block_size = DEFAULT_BLOCK_SIZE
    )
        : get_element_(get_element)
        , data_(data)
        , block_size_( block_size )
    {}

    /**
     * @copydoc LambdaIterator( std::function<utils::Optional<value_type>()>, Data const&, size_t )
     *
     * This version of the constructor takes the data by r-value reference, for moving it.
     */
    LambdaIterator(
        std::function<utils::Optional<value_type>()> get_element,
        Data&& data,
        size_t block_size = DEFAULT_BLOCK_SIZE
    )
        : get_element_(get_element)
        , data_( std::move( data ))
        , block_size_( block_size )
    {}

    ~LambdaIterator() = default;

    LambdaIterator( self_type const& ) = default;
    LambdaIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin()
    {
        return Iterator( this );
    }

    Iterator end()
    {
        return Iterator( nullptr );
    }

    /**
     * @brief Return whether a function to get elemetns was assigend to this generator,
     * that is, whether it is default constructed (`false`) or not (`true`).
     */
    operator bool() const
    {
        return static_cast<bool>( get_element_ );
    }

    /**
     * @brief Access the data stored in the iterator.
     */
    Data const& data() const
    {
        return data_;
    }

    /**
     * @brief Access the data stored in the iterator.
     */
    Data& data()
    {
        return data_;
    }

    // -------------------------------------------------------------------------
    //     Filters and Transformations
    // -------------------------------------------------------------------------

    /**
     * @brief Add a transformation function that is applied to each element of the iteration.
     *
     * Note that all of add_transform(), add_filter(), and add_transform_filter() are chained
     * in the order in which they are added - meaning that they can be mixed as needed.
     * For example, it makes sense to first filter by some property, and then apply transformations
     * only on those elements that passed the filter to avoid unneeded work.
     */
    self_type& add_transform( std::function<void(T&)> transform )
    {
        transforms_and_filters_.push_back(
            [transform]( T& element ){
                transform( element );
                return true;
            }
        );
        return *this;
    }

    /**
     * @brief Add a filter function that is applied to each element of the iteration.
     *
     * If the function returns `false`, the element is skipped in the iteration.
     *
     * @copydetails add_transform()
     */
    self_type& add_filter( std::function<bool(T const&)> filter )
    {
        transforms_and_filters_.push_back(
            [filter]( T& element ){
                return filter( element );
            }
        );
        return *this;
    }

    /**
     * @brief Add a transformation and filter function that is applied to each element
     * of the iteration.
     *
     * This can be used to transform and filter an alement at the same time,
     * as a shortcut where several steps might be needed at once.
     * If the function returns `false`, the element is skipped in the iteration.
     *
     * @copydetails add_transform()
     */
    self_type& add_transform_filter( std::function<bool(T&)> filter )
    {
        transforms_and_filters_.push_back(
            [filter]( T& element ){
                return filter( element );
            }
        );
        return *this;
    }

    /**
     * @brief Clear all filters and transformations.
     */
    self_type& clear_filters_and_transformations()
    {
        transforms_and_filters_.clear();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // std::vector<std::function<void(T&)>> transforms_;
    // std::vector<std::function<bool(T const&)>> filters_;
    std::vector<std::function<bool(T&)>> transforms_and_filters_;

    // Underlying iterator and associated data.
    std::function<utils::Optional<value_type>()> get_element_;
    Data data_;

    // Block buffering settings.
    size_t block_size_ = DEFAULT_BLOCK_SIZE;

};

} // namespace utils
} // namespace genesis

#endif // include guard
