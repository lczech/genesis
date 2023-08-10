#ifndef GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_
#define GENESIS_UTILS_CONTAINERS_LAMBDA_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * The data that is iterated over is stored here, and the end of the iterator is indicated by the
 * lambda by returning `false`.
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
 *         [beg, end]( Variant& var ) mutable {
 *             if( beg != end ) {
 *                 var =  convert_to_variant(*beg);
 *                 ++beg;
 *                 return true;
 *             } else {
 *                 return false;
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
 *         [it]( Variant& var ) mutable {
 *             if( it ) {
 *                 var = *it;
 *                 ++it;
 *                 return true;
 *             } else {
 *                 return false;
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
 * As an additional layer of convenience, the class offers visitors for each element, as well as
 * callbacks for the beginning and end of the iteration. Those are meant as simplifications to
 * reduce code duplcation in user code, and can be used in combination with each other. For example,
 * a visitor can be added that (via a lambda capture) counts statistics of the elements beging
 * processed, and those can then be reported at the end of the iteration. This could of course also
 * be achieved by adding this functionatlity in the loop body and at the loop end when running
 * this iterator. However, for example in our tool https://github.com/lczech/grenedalf, we have
 * setup functions for a LambdaIterator (of type VariantInputIterator) that are re-used across
 * commands. Then, instead of having to have code duplication or repeated function calls in those
 * commands, we only need to add the callbacks when creating the iterator, and they are used in
 * every command automatically.
 *
 * Lastly, the class offers block buffering in a separate thread, for speed up. This capability
 * takes care of the underlying iterator processing (including potential file parsing etc),
 * and buffers blocks of elements, so that the user of this class has faster access to it.
 * For example, when processing data along a genome with lots of computations per position,
 * it makes sense to run the file reading in a separate thread and buffer positions as needed,
 * which this class does automatically. This can be activated by setting the block_size() to the
 * indended number of elements to be buffered. By default, this is set to 0, meaning that no
 * buffering is done. Note that small buffer sizes can induce overhead for the thread
 * synchronisation; we hence recommend to use block sizes of 1000 or greater, as needed.
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
     * The class can buffers blocks of elements of this size, with the buffer loaded in a
     * separate thread, in order to speed up iterating over elements that need some processing,
     * such as input files, which is the typical use case of this class.
     */
    static size_t const DEFAULT_BLOCK_SIZE = 0;

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

        /**
         * @brief Default constructor for empty (past-the-end) data.
         *
         * Public, so that an empty default instance can be used in a (placeholder) variable
         * before assining it a value (e.g., when default-constructing an object that holds a
         * LambdaIterator instance).
         */
        Iterator() = default;

    private:

        /**
         * @brief Constructor for data iteration.
         *
         * Private, as this needs to be created by the parent class (with an instance of itself)
         * to have the data generator functor available.
         */
        Iterator(
            LambdaIterator const* generator
        )
            : generator_(     generator )
            , current_block_( std::make_shared<std::vector<T>>() )
            , buffer_block_(  std::make_shared<std::vector<T>>() )
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
                begin_iteration_();
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

        void begin_iteration_()
        {
            // Check that they are set up from the constructor.
            assert( generator_ );
            assert( current_block_ );
            assert( buffer_block_ );

            // Before starting to init anything, call the callbacks.
            execute_begin_callbacks_();

            // Run the correct init function depending on buffering.
            if( generator_->block_size_ == 0 ) {
                begin_iteration_without_buffer_();
            } else {
                begin_iteration_with_buffer_();
            }
        }

        void begin_iteration_without_buffer_()
        {
            // Edge case: no buffering.
            // Block size zero indicates to use no buffering, so we just use a single element.
            // Make space for it, read the first element, and then we are done here.
            current_block_->resize( 1 );
            end_pos_ = 1;
            increment_();
        }

        void begin_iteration_with_buffer_()
        {
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

            // Check how much data we got, and whether we want to start the background worker.
            // If there is less data than the block size, the file is already done.
            // However, if the first block was fully read, we start the async worker thread
            // to fill the buffer with the next block of data.
            if( end_pos_ == generator_->block_size_ ) {
                fill_buffer_block_();
            } else if( end_pos_ == 0 ) {
                // Edge case: zero elements read. We are already done then.
                end_iteration_();
                return;
            }
            assert( end_pos_ > 0 );
            assert( end_pos_ <= generator_->block_size_ );

            // Now we have an element, which will be the first one of the iteration,
            // and so we execute the visitors for it.
            assert( current_pos_ == 0 );
            execute_visitors_( (*current_block_)[current_pos_] );
        }

        void increment_()
        {
            // Make sure that all things are still in place.
            assert( generator_ );
            assert( current_block_ && current_block_->size() > 0 );

            // Make sure that sizes have not been changed in the parent class.
            assert(
                current_block_->size() == generator_->block_size_ ||
                (
                    generator_->block_size_ == 0 &&
                    current_block_->size()  == 1
                )
            );
            assert( buffer_block_->size()  == generator_->block_size_ );

            // Run the actual increment implementation.
            if( generator_->block_size_ == 0 ) {
                increment_without_buffer_();
            } else {
                increment_with_buffer_();
            }
        }

        void increment_without_buffer_()
        {
            // Edge case: no buffering.
            // Read the next element. If there is none, we are done.
            assert( current_pos_ == 0 );
            assert( current_block_->size() == 1 );
            if( get_next_element_( generator_, (*current_block_)[0] )) {
                execute_visitors_( (*current_block_)[current_pos_] );
            } else {
                end_iteration_();
            }
        }

        void increment_with_buffer_()
        {
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
                    end_iteration_();
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
                    end_iteration_();
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

            // Now we have moved to the next element, and potentially the next block,
            // so we are ready to call the visitors for that element.
            execute_visitors_( (*current_block_)[current_pos_] );
        }

        void fill_buffer_block_()
        {
            // Those shared pointers have been initialized in the constructor; let's assert this.
            assert( generator_ );
            assert( generator_->thread_pool_ );
            assert( future_ );

            // This function is only every called after we finished any previous operations,
            // so let's assert that the thread pool and future are in the states that we expect.
            // assert( generator_->thread_pool_->load() == 0 );
            assert( ! future_->valid() );

            // Make sure that sizes have not been changed in the parent class.
            assert(
                current_block_->size() == generator_->block_size_ ||
                (
                    generator_->block_size_ == 0 &&
                    current_block_->size()  == 1
                )
            );
            assert( buffer_block_->size()  == generator_->block_size_ );

            // In order to use lambda captures by copy for class member variables in C++11, we first
            // have to make local copies, and then capture those. Capturing the class members direclty
            // was only introduced later. Bit cumbersome, but gets the job done.
            auto generator    = generator_;
            auto buffer_block = buffer_block_;
            auto block_size   = generator_->block_size_;

            // The lambda returns the result of read_block_ call, that is, the number of records that
            // have been read, and which we later (in the future_) use to see how much data we got.
            *future_ = generator_->thread_pool_->enqueue(
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
            bool usable_element = false;
            while( true ) {
                // Get the next element from the input source.
                bool const got_element = generator->get_element_( target );

                if( got_element ) {
                    // If this is an element (not yet at the end of the data),
                    // apply all transforms and filters, and get out of here if all of them
                    // return `true`, that is, if none of them wants to filter out the element.
                    // If however one of them returns `false`, we need to find another element.
                    usable_element = true;
                    for( auto const& tra_fil : generator->transforms_and_filters_ ) {
                        usable_element = tra_fil( target );
                        if( ! usable_element ) {
                            // If one of the transforms/filters does not want us to continue,
                            // we do not call the others. Break out of the inner loop.
                            break;
                        }
                    }

                    // If we got out of the above loop without any failing filters,
                    // we have fonud an element, so let's move it to our target.
                    if( usable_element ) {
                        // We already checked that got_element holds data, so no need to repeat,
                        // just assert it. Done here, break out of the while loop.
                        assert( got_element );
                        break;
                    }
                    // Else (usable_element == false): Loop again, try the next element.

                } else {
                    // If this is not a valid element, we reached the end of the input.
                    // We note this in the return value, then break out of the while loop.
                    assert( ! got_element );
                    usable_element = false;
                    break;
                }
            }
            return usable_element;
        }

        void end_iteration_()
        {
            // This function is called at any point when we reach the end of the input source.
            // We first need to call all end callbacks, before unsetting the generator pointer.
            // Doing so is our signal that we have reached the end, and is used for example
            // in the equality comparison of this iterator to test whether an iterator is valid
            // or not (or equal to the past-the-end iterator).
            assert( generator_ );
            execute_end_callbacks_();
            generator_ = nullptr;
        }

        void execute_visitors_( T const& element ) const
        {
            assert( generator_ );
            for( auto const& visitor : generator_->visitors_ ) {
                visitor( element );
            }
        }

        void execute_begin_callbacks_() const
        {
            assert( generator_ );
            for( auto const& cb : generator_->begin_callbacks_ ) {
                cb( *generator_ );
            }
        }

        void execute_end_callbacks_() const
        {
            assert( generator_ );
            for( auto const& cb : generator_->end_callbacks_ ) {
                cb( *generator_ );
            }
        }

    private:

        // Parent.
        LambdaIterator const* generator_ = nullptr;

        // Buffer buffering data, and positions in it.
        std::shared_ptr<std::vector<T>> current_block_;
        std::shared_ptr<std::vector<T>> buffer_block_;
        size_t current_pos_ = 0;
        size_t end_pos_ = 0;

        // Store the future_ used to keep track of the background task. It returns the number of
        // elements that have been read into the buffer (block_size_, or less at the end of the file).
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
     * The constructor expects the function that takes an element by reference to assign it its
     * new value at each iteration, and returns `true` if there was an element (iteration still
     * ongoing), or `false` once the end of the underlying iterator is reached.
     */
    LambdaIterator(
        std::function<bool(value_type&)> get_element,
        std::shared_ptr<utils::ThreadPool> thread_pool = {},
        size_t block_size = DEFAULT_BLOCK_SIZE
    )
        : get_element_(get_element)
        , thread_pool_( thread_pool )
        , block_size_( block_size )
    {
        if( ! thread_pool ) {
            thread_pool = std::make_shared<utils::ThreadPool>( 1 );
        }
        thread_pool_ = thread_pool;
    }

    /**
     * @copydoc LambdaIterator( std::function<bool(value_type&)>, std::shared_ptr<utils::ThreadPool>, size_t )
     *
     * Additionally, @p data can be given here, which we simply store and make accessible
     * via data(). This is a convenience so that iterators generated via a `make` function
     * can for example forward their input source name for user output.
     */
    LambdaIterator(
        std::function<bool(value_type&)> get_element,
        Data const& data,
        std::shared_ptr<utils::ThreadPool> thread_pool = {},
        size_t block_size = DEFAULT_BLOCK_SIZE
    )
        : LambdaIterator( get_element, thread_pool, block_size )
    {
        data_ = data;
    }

    /**
     * @copydoc LambdaIterator( std::function<bool(value_type&)>, Data const&, std::shared_ptr<utils::ThreadPool>, size_t )
     *
     * This version of the constructor takes the data by r-value reference, for moving it.
     */
    LambdaIterator(
        std::function<bool(value_type&)> get_element,
        Data&& data,
        std::shared_ptr<utils::ThreadPool> thread_pool = {},
        size_t block_size = DEFAULT_BLOCK_SIZE
    )
        : LambdaIterator( get_element, thread_pool, block_size )
    {
        data_ = std::move( data );
    }

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
        if( has_started_ ) {
            throw std::runtime_error( "LambdaIterator: Cannot call begin() multiple times." );
        }
        has_started_ = true;
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
    self_type& add_transform( std::function<void(T&)> const& transform )
    {
        return add_transform_filter(
            [transform]( T& element ){
                transform( element );
                return true;
            }
        );
    }

    /**
     * @brief Add a filter function that is applied to each element of the iteration.
     *
     * If the function returns `false`, the element is skipped in the iteration.
     *
     * @copydetails add_transform()
     */
    self_type& add_filter( std::function<bool(T const&)> const& filter )
    {
        return add_transform_filter(
            [filter]( T& element ){
                return filter( element );
            }
        );
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
    self_type& add_transform_filter( std::function<bool(T&)> const& filter )
    {
        // Sanity check.
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change filters/transformations after iteration has started."
            );
        }
        transforms_and_filters_.push_back( filter );
        return *this;
    }

    /**
     * @brief Clear all filters and transformations.
     */
    self_type& clear_filters_and_transformations()
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change filters/transformations after iteration has started."
            );
        }
        transforms_and_filters_.clear();
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Visitors and Callbacks
    // -------------------------------------------------------------------------

    /**
     * @brief Add a visitor function that is executed when the iterator moves to a new element
     * during the iteration.
     *
     * These functions are executed when starting and incrementing the iterator, once for each
     * element, in the order in which they are added here. They take the element that the iterator
     * just moved to as their argument, so that user code can react to the new element.
     *
     * They are a way of adding behaviour to the iteration loop that could also simply be placed
     * in the beginning of the loop body of the user code. Still, offering this here can reduce
     * redundant code, such as logging elements during the iteration.
     */
    self_type& add_visitor( std::function<void(T const&)> const& visitor )
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change visitors after iteration has started."
            );
        }
        visitors_.push_back( visitor );
        return *this;
    }

    /**
     * @brief Clear all functions that are executed on incrementing to the next element.
     */
    self_type& clear_visitors()
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change visitors after iteration has started."
            );
        }
        visitors_.clear();
        return *this;
    }

    /**
     * @brief Add a callback function that is executed when the begin() function is called.
     *
     * The callback needs to accept the LambdaIterator object itself, as a means to, for example,
     * access its data(), and is meant as a reporting mechanism. For example, callbacks can be added
     * that write properties of the underlying data sources to log. They are executed in the order
     * added.
     *
     * Similar to the functionality offered by add_visitor(), this could also be achieved by
     * executing these functions direclty where needed, but having it as a callback here might help
     * to reduce code duplication.
     *
     * See also add_end_callback().
     */
    self_type& add_begin_callback( std::function<void(LambdaIterator const&)> const& callback )
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change callbacks after iteration has started."
            );
        }
        begin_callbacks_.push_back( callback );
        return *this;
    }

    /**
     * @brief Add a callback function that is executed when the end() of the iteration is reached.
     *
     * This is similar to the add_begin_callback() functionality, but instead of executing the
     * callback when starting the iteration, it is called when ending it. Again, this is meant
     * as a means to reduce user code duplication, for example for logging needs.
     */
    self_type& add_end_callback( std::function<void(LambdaIterator const&)> const& callback )
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change callbacks after iteration has started."
            );
        }
        end_callbacks_.push_back( callback );
        return *this;
    }

    /**
     * @brief Clear all functions that have been added via add_begin_callback() and
     * add_end_callback().
     */
    self_type& clear_callbacks()
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change callbacks after iteration has started."
            );
        }
        begin_callbacks_.clear();
        end_callbacks_.clear();
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Thread Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Get the thread pool used for buffering of elements in this iterator.
     */
    std::shared_ptr<utils::ThreadPool> thread_pool() const
    {
        return thread_pool_;
    }

    /**
     * @brief Set the thread pool used for buffering of elements in this iterator.
     *
     * Shall not be changed after iteration has started, that is, after calling begin().
     */
    self_type& thread_pool( std::shared_ptr<utils::ThreadPool> value )
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change thread pool after iteration has started."
            );
        }
        thread_pool_ = value;
        return *this;
    }

    /**
     * @brief Get the currenlty set block size used for buffering the input data.
     */
    size_t block_size() const
    {
        return block_size_;
    }

    /**
     * @brief Set the block size used for buffering the input data.
     *
     * Shall not be changed after iteration has started, that is, after calling begin().
     *
     * By default, this is set to 0, meaning that no buffering is done. Note that small buffer
     * sizes can induce overhead for the thread synchronisation; we hence recommend to use block
     * sizes of 1000 or greater, as needed.
     */
    self_type& block_size( size_t value )
    {
        if( has_started_ ) {
            throw std::runtime_error(
                "LambdaIterator: Cannot change thread pool block size after iteration has started."
            );
        }
        block_size_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // We have two different types of functions that we accept to operate on the data:
    // the transforms and filters are executed when filling the buffers,
    // while the visits are executed once the iteration reaches the respective element.
    std::vector<std::function<bool(T&)>> transforms_and_filters_;
    std::vector<std::function<void(T const&)>> visitors_;

    // We furthermore allow callbacks for the beginning and and of the iteration.
    std::vector<std::function<void(self_type const&)>> begin_callbacks_;
    std::vector<std::function<void(self_type const&)>> end_callbacks_;

    // Underlying iterator and associated data.
    std::function<bool( value_type& )> get_element_;
    Data data_;

    // Thread pool to run the buffering in the background.
    std::shared_ptr<utils::ThreadPool> thread_pool_;

    // Block buffering settings.
    size_t block_size_ = DEFAULT_BLOCK_SIZE;

    // Flag to avoid accidentally calling begin() twice, or calling threading settings
    // after begin() has already been called.
    bool has_started_ = false;

};

} // namespace utils
} // namespace genesis

#endif // include guard
