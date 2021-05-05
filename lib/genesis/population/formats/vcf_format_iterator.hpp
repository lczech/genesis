#ifndef GENESIS_POPULATION_FORMATS_VCF_FORMAT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_VCF_FORMAT_ITERATOR_H_

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

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_common.hpp"
#include "genesis/population/formats/vcf_header.hpp"

#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

extern "C" {
    // #include <htslib/hts.h>
    // #include <htslib/vcf.h>

    struct bcf_hdr_t;
    struct bcf1_t;
}

namespace genesis {
namespace population {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template< typename S, typename T >
class VcfFormatIterator;

// Declare the specializations of the iterator that are used by VcfRecord, for ease of use.
using VcfFormatIteratorString   = VcfFormatIterator<char*, std::string>;
using VcfFormatIteratorInt      = VcfFormatIterator<int32_t, int32_t>;
using VcfFormatIteratorFloat    = VcfFormatIterator<float, double>;
using VcfFormatIteratorGenotype = VcfFormatIterator<int32_t, VcfGenotype>;

// =================================================================================================
//     VCF/BCF Format Helper
// =================================================================================================

/**
 * @brief Provide htslib helper functions.
 *
 * This class only exists so that we do not need to include the htslib headers in our headers.
 * By doing so, we avoid polluting our user's global namespace with htslib symbols, and also do
 * not have them to link against htlib. The downside is that we need to replicate code from htslib
 * here, and then statically assert that constants etc are identical... Ugly, but the price we pay.
 *
 * This class here serves as a non-templated class that can have a compilation unit, which we cannot
 * have for the below class template.
 */
class VcfFormatHelper
{
private:

    // Need constructor only to have static asserts have access to class members
    VcfFormatHelper();

    template< typename S, typename T >
    friend class VcfFormatIterator;

    // Need these in VcfFormatIterator to not include the htslib headers... not sure if this is
    // a good idea after all... maybe we should have just rolled with it and included them...
    static const int8_t  bcf_int8_vector_end_  = (-127);         /* INT8_MIN  + 1 */
    static const int16_t bcf_int16_vector_end_ = (-32767);       /* INT16_MIN + 1 */
    static const int32_t bcf_int32_vector_end_ = (-2147483647);  /* INT32_MIN + 1 */
    static const int64_t bcf_int64_vector_end_ = (-9223372036854775807LL);  /* INT64_MIN + 1 */
    static const char    bcf_str_vector_end_   = 0;
    static const int8_t  bcf_int8_missing_     = (-128);          /* INT8_MIN  */
    static const int16_t bcf_int16_missing_    = (-32767-1);      /* INT16_MIN */
    static const int32_t bcf_int32_missing_    = (-2147483647-1); /* INT32_MIN */
    static const int64_t bcf_int64_missing_    = (-9223372036854775807LL - 1LL);  /* INT64_MIN */
    static const char    bcf_str_missing_      = 0x07;

    /**
     * @brief Wrapper for htslib bcf_hdr_nsamples to be able to use this in our templated header
     * functions and classes, such as VcfFormatIterator, without having to include htslib there.
     * It is a macro there, so we cannot forward declare it...
     */
    static int32_t bcf_hdr_nsamples_( ::bcf_hdr_t const* header );

    /**
     * @brief Wrapper for htslib header->samples[index] to be able to use this in our templated header
     * functions and classes, such as VcfFormatIterator, without having to include htslib there.
     * It is part of a struct, so we cannot forward declare it...
     */
    static std::string hdr_sample_name_( ::bcf_hdr_t const* header, size_t index );

    /**
     * @brief Wrap htslib bcf_get_format_string() so that we can use it in VcfFormatIterator
     * without having to include the htslib header there.
     */
    static int bcf_get_format_string_(
        const bcf_hdr_t *hdr, bcf1_t *line, const char *tag, char ***dst, int *ndst
    );

    /**
     * @brief Wrap htslib bcf_get_format_values() so that we can use it in VcfFormatIterator
     * without having to include the htslib header there.
     */
    static int bcf_get_format_values_(
        const bcf_hdr_t *hdr, bcf1_t *line, const char *tag, void **dst, int *ndst, int type
    );

    /**
     * @brief Wrap htslib bcf_get_genotypes() so that we can use it in VcfFormatIterator
     * without having to include the htslib header there.
     */
    static int bcf_get_genotypes_( const bcf_hdr_t *hdr, bcf1_t *line, void **dst, int *ndst );

    /**
     * @brief Wrap htslib bcf_float_is_vector_end() so that we can use it in VcfFormatIterator
     * without having to include the htslib header there.
     */
    static int bcf_float_is_vector_end_(float f);

    /**
     * @brief Wrap htslib bcf_float_is_missing() so that we can use it in VcfFormatIterator
     * without having to include the htslib header there.
     */
    static int bcf_float_is_missing_(float f);

};

// =================================================================================================
//     VCF/BCF Format/Sample Iterator
// =================================================================================================

/**
 * @brief Iterate the FORMAT information for the samples in a SNP/variant line in a VCF/BCF file.
 *
 * One instance is meant to iterate all values of the samples for one FORMAT field key (give by its
 * `id` tag/key). VCF/BCF supports different data types, for which this class template is instanciated
 * in the `begin_format_*()` and `get_format_*()` functions of VcfRecord: std::string, int, and float.
 *
 * The FORMAT data in VCF is fairly flexible and complex:
 *
 *  - For a given FORMAT ID field (e.g., "AF"), each sample can contain multiple values for that
 *    field, as indicated by the `Number` value in the VCF header line for that FORMAT field.
 *  - However, this seems not to be the case for strings (char* in htslib), which only ever seem
 *    to return one string value per sample in htslib. To our knowledge, this is not properly
 *    documented in htslib, but we implemented this class here as if this was true. So, for string
 *    data, one can only obtain a single string per sample, and has to split it afterwards,
 *    if necessary.
 *  - For the other data types (int and float/double), there can be missing data as well, so that
 *    not all samples might have the same number of values.
 *  - The genotype field (`GT`) is yet another special case that is handled by htslib as either
 *    string or int, depending on the context. We here hence wrap this as VcfGenotype for simplicity.
 *
 * Basically, what that means is that we need an iterator for the values of each sample within
 * this iterator over samples, which again usually is within an iterator over the records/lines
 * if the VCF file. However, we simplify here a bit, and replace the innermost iterator (over
 * values of the current sample): Most FORMAT tags only have one value anyway (see below for that
 * use case), and also we don't want too many classes to confuse us.
 * To accomodate for this, and to make iterating values as easy as possible (given the complexity),
 * we offer functions to automatically skip such missing values.
 *
 * A simple use case for this iterator is hence as follows:
 *
 *     // Load a file and init the data structures.
 *     auto file = HtsFile( "path/to/file.vcf" );
 *     auto header = VcfHeader( file );
 *     auto record = VcfRecord( header );
 *
 *     // Iterate all records/lines of the VCF file.
 *     while( record.read( file )) {
 *         // Skip if the read depth (DP) FORMAT is not available for the current record.
 *         if( !record.has_format("DP") ) {
 *             continue;
 *         }
 *
 *         // Iterate the DP data for all samples of the record, loading them as int.
 *         for( auto& sample_dp : record.get_format_int("DP") ) {
 *             LOG_INFO << "At sample " << sample_dp.sample_name();
 *
 *             // Iterate all individual values for that sample that are given in the data.
 *             while( sample_dp.has_value() ) {
 *                 LOG_INFO << "- " << sample_dp.get_value();
 *                 sample_dp.next_value();
 *             }
 *         }
 *     }
 *
 * See also VcfInputIterator for a wrapper around the basic loop over records/lines.
 *
 * The above example relies on the implicit notion of a "current" value per sample,
 * as we move between values via the next_value() function. Note that the next_value() function
 * automatically skips missing values.
 * If however the exact indices of the values within a sample are important (that is, if missing
 * values shall not be skipped automatically), an alternative approach is to use the `*_at()`
 * functions that this iterator class provides:
 *
 *     // (replacement for the innermost while loop of above)
 *     for( size_t i = 0; i < sample_dp.values_per_sample(); ++i ) {
 *         if( sample_dp.has_value_at(i) ) {
 *             LOG_INFO << "- " << sample_dp.get_value_at(i);
 *         }
 *     }
 *
 * Alternatively, if only a single value is expected per sample anyway (which is probably the case
 * for most kind of FORMAT fields), we do not need to loop and can simplify the access:
 *
 *     // (again, replacement for the innermost while loop of above)
 *     if( sample_dp.has_value() ) {
 *         LOG_INFO << "- " << sample_dp.get_value();
 *     }
 *
 * Furthermore, in the first example, we used a range-based for loop, which automates the
 * increment and comparison to the end iterator. It is instead of course also possible to do this
 * explicitly:
 *
 *    // (replacement for the range-based for loop of the first example)
 *    for( auto sample_dp = record.begin_format_int("DP"); sample_dp != record.end_format_int(); ++sample_dp ) {
 *       // ...
 *    }
 *
 * Here, each call to the pre-increment operator++() moves to the first value of the next sample.
 * Then as before, in order to move between values of the current sample, use has_value(),
 * get_value(), and next_value() within the loop.
 *
 * Note that we also provide `*_at()` functions that take the sample index as input. These are hence
 * fully independent of the current iterator position (sample and value position), and can be used
 * to access values at arbitrary sample and value indices.
 *
 * Furthermore, a vector with all values can be obtaind, which is for example useful for the genotype
 * `GT` field, which can be used like this:
 *
 *     // (replacement for the range-based for loop of the first example)
 *     for( auto sample_gt : record.get_format_genotype() ) {
 *         LOG_INFO << vcf_genotype_string( sample_gt.get_values() );
 *     }
 *
 * This yields a print-out of the genotypes of each sample in VCF style, see vcf_genotype_string()
 * for details.
 *
 * Further implementation details:
 * The class is a template for the source type `S` as used by htslib (char*, int32_t, float),
 * and for the target type `T` as used by us (std::string, int32_t, double, VcfGenotype).
 * Most of the implementation is shared, but some htslib-related functions (check for their
 * hard-coded special values, memory management of allocated arrays, ...) need to be specialized
 * for the different data types. See the very end of this file for the respective implementations.
 */
template< typename S, typename T >
class VcfFormatIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type = VcfFormatIterator<S,T>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default (empty) instance, that is used to indicate the end iterator position.
     *
     * By default, this has `is_end_ == true`, so that we can easily check for default constructed
     * instances, which are used as the past-the-end iterators in the loop when using this class.
     */
    VcfFormatIterator() = default;

    /**
     * @brief Create an instance, given the htslib @p header, @p record line, and the FORMAT @p id
     * tag/key (as well as its data type @p ht_type) that we want to iterate over.
     *
     * Usually, this class does not need to be constructed by the user. Instead, it is obtained
     * from the `begin_format_*()` and `end_format_*()`, or the `get_format_*()` iterator functions
     * of VcfRecord. That way, it can easily be used for iterating all samples of a given VCF record line.
     */
    VcfFormatIterator( ::bcf_hdr_t* header, ::bcf1_t* record, std::string const& id, VcfValueType ht_type )
        : is_end_(false)
        , header_(header)
        , record_(record)
    {
        // TODO Remove all htslib function calls from this headers and put them in a cpp file instead.

        // First, get all values for the given record line and the FORMAT id tag, store them in
        // value_buffer_, store the result of the htslib function call (which gives the total number
        // of values that are written to value_buffer_) in values_total_, and also store the number of
        // reserved values in values_reserved_ (we don't really need that last number as of now,
        // but better keep it). All this needs some specialization for the different data types
        // that we can handle here (char*, int, float), so we use a specialized function tenokate
        // for this that is instanciated for each data type (see end of this file).
        auto const res = construct_values_( header_, record_, id, ht_type );

        // Now, check that the result is valid, that is, res >= 0 (which has been returned
        // by the above function call). If not, check_value_return_code_() throws an exception for us.
        // Once that check finishes without throwing, we assert that we have proper data.
        // In that case, we have res >= 0 indicating the number of total values, so these
        // are the same then.
        VcfHeader::check_value_return_code_(
            // In order to not include the htslib headers here, we have to use a fixed value
            // that we define ourselves, instead of their definition BCF_HL_FMT = 2.
            // We statically assert that they are identical in vcf_common.cpp
            header_, id, static_cast<int>( ht_type ), static_cast<int>( VcfHeaderLine::kFormat ), res
        );
        assert( value_buffer_ );
        assert( res >= 0 );
        assert( values_total_ >= 0 );
        assert( ht_type == VcfValueType::kString || res == values_total_ );
        assert( values_total_ <= values_reserved_ );

        // Now, as there can be multiple values per sample, we need to get the number of values
        // per sample. htslib does not have any helpful comments on that, but their examples
        // suggest that just division by the total number of samples works, meaning that each sample
        // has the same number of values set in the array that we get, and that missing values are
        // padded properly. So, let's do this, and assert that this is evenly divisible, to make
        // sure that this is what we actually want here.
        // Addendum after bug hunt: Apparently, this is different for char*/string data, where
        // the number returned from the htslib call to get the valies bcf_get_format_string()
        // corresponds to something like the longest string (plus some extra), instead of the number
        // of values. That means that strings can never have more than one value, and that we need
        // to set their values_total_ differently (which we do in construct_values_)...
        // We use our wrapper VcfFormatHelper::bcf_hdr_nsamples_ here, which wraps bcf_hdr_nsamples()
        // so that we do not have to include htslib here.
        num_samples_ = VcfFormatHelper::bcf_hdr_nsamples_(header);
        assert( static_cast<size_t>( values_total_ ) % num_samples_ == 0 );
        values_per_sample_ = static_cast<size_t>( values_total_ ) / num_samples_;
        assert( values_per_sample_ * num_samples_ == static_cast<size_t>( values_total_ ));

        // Now, go to the first value of the first sample.
        assert( value_idx_ == -1 );
        go_to_next_value_();
        assert( value_idx_ >= 0 );
    }

    ~VcfFormatIterator() = default;

    VcfFormatIterator( VcfFormatIterator const& ) = default;
    VcfFormatIterator( VcfFormatIterator&& )      = default;

    VcfFormatIterator& operator= ( VcfFormatIterator const& ) = default;
    VcfFormatIterator& operator= ( VcfFormatIterator&& )      = default;

    // -------------------------------------------------------------------------
    //     General Data Access
    // -------------------------------------------------------------------------

    /**
     * @brief Get the raw htslib structure pointer for the header.
     */
    ::bcf_hdr_t* header_data()
    {
        return header_;
    }

    /**
     * @brief Get the raw htslib structure pointer for the record/line.
     */
    ::bcf1_t* record_data()
    {
        return record_;
    }

    /**
     * @brief Return the total number of samples that we are iterating over.
     */
    size_t sample_count() const
    {
        return num_samples_;
    }

    /**
     * @brief Return the number of values that each sample has.
     *
     * Note that VCF allows for unspecified values (missing data) and early ending data if a
     * particular sample does not have that many values. This function here hence returns the
     * maximum number of values per sample, as specified in the header.
     */
    size_t values_per_sample() const
    {
        return values_per_sample_;
    }

    // -------------------------------------------------------------------------
    //     Iterator for Samples
    // -------------------------------------------------------------------------

    /**
     * @brief Dereference, which gives the iterator itself instead of the value,
     * as our values should be accessed via the get_value() or get_value_at() functions.
     */
    self_type& operator*()
    {
        return *this;
    }

    /**
     * @brief Equality comparison, needed to detect the end of the iteration.
     */
    bool operator == ( self_type const& other ) const
    {
        // Two iterators are equal if either both of them are the past-the-end iterator (in which
        // case we do not need to check any of the values, as they are irrelevant then),
        // or both are not past-the-end, but all their (important) values match.
        // We do not check the value buffer here, as this will differ for different instances.
        return ( is_end_ && other.is_end_ ) || (
            !is_end_ && !other.is_end_ &&
            header_ == other.header_ && record_ == other.record_ &&
            values_total_ == other.values_total_ && num_samples_ == other.num_samples_ &&
            values_per_sample_ == other.values_per_sample_ &&
            sample_idx_ == other.sample_idx_ && value_idx_ == other.value_idx_
        );
    }

    /**
     * @brief Inequality comparison, needed to detect the end of the iteration.
     */
    bool operator != ( self_type const& other ) const
    {
        return !( *this == other );
    }

    /**
     * @brief Pre-increment operator to move to the next sample.
     *
     * In particular, move to the first valid value of the next sample, or, if we reached the end
     * of the samples, set the end flag, so that we know we are done.
     */
    self_type& operator ++ ()
    {
        ++sample_idx_;
        if( sample_idx_ < num_samples_ ) {
            // If we are still within the sample boundaries, go to the first valid entry of the
            // new sample that we just moved to.
            value_idx_ = -1;
            go_to_next_value_();
            assert( value_idx_ >= 0 );
        } else {
            // We have reached the end of the samples.
            is_end_ = true;
        }
        return *this;
    }

    /**
     * @brief Post-increment operator to move to the next sample.
     *
     * This does the same as the pre-increment, but returns an iterator to the previous sample/value.
     * Note that this creates a copy, which is additional effort. Hence, we strongly recommend
     * to use the pre-increment operator `++sample` instead whenever possible.
     */
    self_type operator ++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // -------------------------------------------------------------------------
    //     Current element data access within the sample
    // -------------------------------------------------------------------------

    /**
     * @brief Return the index of the column of the current sample.
     *
     * See value_index() to get the index of the current value within the current sample.
     */
    size_t sample_index() const
    {
        return sample_idx_;
    }

    /**
     * @brief Return the index of the current value within the current sample.
     *
     * See sample_index() to get the index of the current sample.
     */
    size_t value_index() const
    {
        assert( value_idx_ >= 0 );
        return value_idx_;
    }

    /**
     * @brief Return the name of the current sample, as given in the <code>\#CHROM ...</code>
     * header line of the VCF file.
     */
    std::string sample_name() const
    {
        // We use our wrapper VcfFormatHelper::bcf_hdr_nsamples_ here, which wraps bcf_hdr_nsamples()
        // so that we do not have to include htslib here.
        assert( static_cast<int32_t>( num_samples_ ) == VcfFormatHelper::bcf_hdr_nsamples_( header_ ));
        assert( sample_idx_ < num_samples_ );

        // We use our wrapper VcfFormatHelper::hdr_sample_name_ here, which wraps
        // header->samples[index]() so that we do not have to include htslib here.
        return VcfFormatHelper::hdr_sample_name_( header_, sample_idx_ );
        // return header_->samples[sample_idx_];
    }

    /**
     * @brief Return whether the iterator currently resides at a valid value of the current sample.
     *
     * The function tests whether the value at value_index() of the current sample at sample_index()
     * is valid, that is, not missing and not the end of the data for that sample.
     *
     * This function is `true` for a maximum of values_per_sample() many values per sample when
     * iterating through them via next_value(). It can be less than that if there are missing
     * values in the VCF data.
     */
    bool has_value() const
    {
        // The current sample has a valid value if we are still within its bounds and
        // if the current value is valid (not end of the vector and not missing).
        // For both, we use the non-throwing versions of the test functions.
        assert( value_idx_ >= 0 );
        return
            test_index_boundaries_( sample_idx_, value_idx_, false ) &&
            test_valid_value_( sample_idx_, value_idx_, false )
        ;
    }

    /**
     * @brief Get the value where the iterator currently resides.
     *
     * That is, get the value at index value_index() for the sample at sample_index().
     * The function assumes that this is a valid value, that is, that has_value() returned `true`.
     */
    T get_value() const
    {
        // Get the value at the current position, and do some checks. We assume here
        // that this function is only ever called when has_next_value() is true - everything else
        // is a user error, but we want to avoid the overhead of runtime checks in release,
        // so, assertions it is.
        assert( value_idx_ >= 0 );
        assert( test_index_boundaries_( sample_idx_, value_idx_, false ));
        assert( test_valid_value_( sample_idx_, value_idx_, false ));
        assert( has_value() );

        // If all is good, instanciate a target type with the source type value, and return.
        // For all our cases (string, int, float), this works without special conversions.
        return T{ *get_value_ptr_( sample_idx_, value_idx_ ) };
    }

    /**
     * @brief Move to the next value within the current sample.
     *
     * This increases the value_index() to the next valid value within the current sample at
     * sample_index(). Invalid values (e.g., missing data) are skipped automatically.
     */
    void next_value()
    {
        // Move to the next value, if there is one.
        go_to_next_value_();
    }

    /**
     * @brief Return the number of valid values for the current sample.
     *
     * That corresponds to how often next_value() will be called when looping over values before
     * has_value() returns `false`.
     */
    size_t valid_value_count() const
    {
        return valid_value_count_at( sample_idx_ );
    }

    // -------------------------------------------------------------------------
    //     Arbitrary element data access within the sample or at arbitrary sample
    // -------------------------------------------------------------------------

    /**
     * @brief Return the sample name at a given index within 0 and sample_count().
     */
    std::string sample_name_at( size_t sample_index ) const
    {
        test_index_boundaries_( sample_index, 0, true );
        // We use our wrapper VcfFormatHelper::bcf_hdr_nsamples_ here, which wraps bcf_hdr_nsamples()
        // so that we do not have to include htslib here.
        assert( static_cast<int32_t>( num_samples_ ) == VcfFormatHelper::bcf_hdr_nsamples_( header_ ));
        assert( sample_index < num_samples_ );

        // We use our wrapper VcfFormatHelper::hdr_sample_name_ here,
        // which wraps header->samples[index]() so that we do not have to include htslib here.
        return VcfFormatHelper::hdr_sample_name_( header_, sample_index );
        // return header_->samples[sample_index];
    }

    /**
     * @brief Return whether the value at a given index within the current sample is valid.
     *
     * Invalid values are either missing or marked as the end of the vector in htslib. If neither
     * is the case (and if the index is within bounds), the value is considered valid.
     */
    bool has_value_at( size_t value_index ) const
    {
        return has_value_at( sample_idx_, value_index );
    }

    /**
     * @brief Return whether the value at a given index within the given sample is valid.
     *
     * Invalid values are either missing or marked as the end of the vector in htslib. If neither
     * is the case (and if the indices are within bounds), the value is considered valid.
     */
    bool has_value_at( size_t sample_index, size_t value_index ) const
    {
        // Basic index boundary checks. Throws if indices are not within bounds of the arrays.
        test_index_boundaries_( sample_index, value_index, true );

        // Get access to the value, and check that it is an actual value.
        S* ptr = get_value_ptr_( sample_index, value_index );
        return (! is_vector_end_( *ptr )) && (! is_missing_value_( *ptr ));
    }

    /**
     * @brief Get the value at a given @p value_index of the current sample.
     */
    T get_value_at( size_t value_index ) const
    {
        return get_value_at( sample_idx_, value_index );
    }

    /**
     * @brief Get the value at a given @p value_index of a given sample at @p sample_index.
     */
    T get_value_at( size_t sample_index, size_t value_index ) const
    {
        // Basic index boundary checks. Throws if indices are not within bounds of the arrays.
        test_index_boundaries_( sample_index, value_index, true );

        // Get access to the value, and check that it is an actual value.
        // If not, the test function throws an exception.
        test_valid_value_( sample_index, value_index, true );
        S* ptr = get_value_ptr_( sample_index, value_index );

        // If all is good, instanciate a target type with the source type value, and return.
        // For all our cases (string, int, float), this works without special conversions.
        return T{ *ptr };
    }

    /**
     * @brief Get a vector of all values for the current sample.
     *
     * If @p include_missing is `true`, the resulting vector has the size of values_per_sample(),
     * and also contains any missing or end-of-vector values as provided by htslib, using their
     * raw constants to indicate these values.
     * If @p include_missing is `false` (default), instead, these values are skipped, so that the
     * resulting vector might be smaller than values_per_sample().
     *
     * This function needs to allocate a vector; hence, the other access methods are preferred for
     * speed reasons.
     */
    std::vector<T> get_values( bool include_missing = false ) const
    {
        return get_values_at( sample_idx_, include_missing );
    }

    /**
     * @brief Get a vector of all values for a given sample.
     *
     * @copydetails get_values()
     */
    std::vector<T> get_values_at( size_t sample_index, bool include_missing = false ) const
    {
        test_index_boundaries_( sample_index, 0, true );
        // We use our wrapper VcfFormatHelper::bcf_hdr_nsamples_ here, which
        // wraps bcf_hdr_nsamples() so that we do not have to include htslib here.
        assert( static_cast<int32_t>( num_samples_ ) == VcfFormatHelper::bcf_hdr_nsamples_( header_ ));
        assert( sample_index < num_samples_ );

        std::vector<T> result;
        if( include_missing ) {
            result.reserve( values_per_sample_ );
            for( size_t i = 0; i < values_per_sample_; ++i ) {
                S* ptr = get_value_ptr_( sample_index, i );
                result.emplace_back( *ptr );
            }
        } else {
            auto const cnt = valid_value_count_at( sample_index );
            result.reserve( cnt );
            for( size_t i = 0; i < values_per_sample_; ++i ) {
                if( test_valid_value_( sample_index, i, false )) {
                    S* ptr = get_value_ptr_( sample_index, i );
                    result.emplace_back( *ptr );
                }
            }
        }
        return result;
    }

    /**
     * @brief Return the number of valid values for a given @p sample_index.
     *
     * This corresponds to the resulting vector size when calling get_values() or get_values_at()
     * with `include_missing == false`.
     */
    size_t valid_value_count_at( size_t sample_index ) const
    {
        size_t cnt = 0;
        for( size_t i = 0; i < values_per_sample_; ++i ) {
            if( test_valid_value_( sample_index, i, false )) {
                ++cnt;
            }
        }
        return cnt;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    S* get_value_ptr_( size_t sample_index, size_t value_index ) const
    {
        // Only called internally, so we can simply assert that the indices are within bounds.
        assert( value_buffer_ );
        assert( values_total_ >= 0 );
        assert( sample_index < num_samples_ );
        assert( value_index < values_per_sample_ );
        assert( sample_index * values_per_sample_ + value_index < static_cast<size_t>( values_total_ ));

        // We want to get an element from the value buffer that corresponds ot a given sample
        // (multiply by values per sample to get the correct part of the array), and a given
        // value index within that sample (add at the end to get to value within the array part).
        return value_buffer_.get() + sample_index * values_per_sample_ + value_index;
    }

    void go_to_next_value_()
    {
        // We potentially start at -1 for the start of a new sample, but then move to a proper index.
        ++value_idx_;
        assert( value_idx_ >= 0 );

        // Now find a valid value within the bounds of the data for this sample.
        while( static_cast<size_t>( value_idx_ ) < values_per_sample_ ) {
            S* current = get_value_ptr_( sample_idx_, value_idx_ );

            // If we reach the vector end marker, set our value index to the proper past-the-end index
            // for the current sample, and get out of here. No valid values in this sample left.
            if( is_vector_end_( *current )) {
                value_idx_ = values_per_sample_;
                break;
            }

            // If the value is missing, move to the next instead.
            if( is_missing_value_( *current )) {
                ++value_idx_;
                continue;
            }

            // If neither, we found a valid value.
            break;
        }
        assert( value_idx_ >= 0 );

        // We either end up at a valid value (within bounds, not end-of-vector, not missing),
        // or we reached the end of the values (either through the iterations by incrementing
        // due to missing values, or because of the hard set in the vecor end condition above).
        // We need to test for whether we reached the end first and then rely on
        // short-circuit-evaluation, so that the valid value test is only conducted
        // if we are at a valid index!
        assert(
            ( static_cast<size_t>( value_idx_ ) >= values_per_sample_ ) ||
            (
                test_index_boundaries_( sample_idx_, value_idx_, false ) &&
                test_valid_value_( sample_idx_, value_idx_, false )
            )
        );
    }

    bool test_index_boundaries_( size_t sample_index, size_t value_index, bool throwing ) const
    {
        // Assert that the values per sample is a non-negative number, so that the omission
        // of the second param of this function for cases where we just want to check the sample
        // index still works.
        // ... always true due to unsigned int...
        // assert( values_per_sample_ >= 0 );

        if( sample_index >= num_samples_ ) {
            if( throwing ) {
                throw std::invalid_argument(
                    "Cannot get value at sample " + std::to_string(sample_index) + " at index " +
                    std::to_string( value_index ) + ", as there are only " +
                    std::to_string( num_samples_ ) + " samples in the VCF/BCF record."
                );
            } else {
                return false;
            }
        }
        if( value_index >= values_per_sample_ ) {
            if( throwing ) {
                throw std::invalid_argument(
                    "Cannot get value at sample " + std::to_string(sample_index) + " at index " +
                    std::to_string( value_index ) + ", as there are only " +
                    std::to_string( values_per_sample_ ) + " values per sample in this VCF/BCF record."
                );
            } else {
                return false;
            }
        }
        return true;
    }

    bool test_valid_value_( size_t sample_index, size_t value_index, bool throwing ) const
    {
        // Here, we only assert that the indices are valid, as this job has to be done
        // by the caller independently beforehand.
        assert( test_index_boundaries_( sample_index, value_index, false ));

        S* ptr = get_value_ptr_( sample_index, value_index );
        assert( ptr );
        if( is_vector_end_( *ptr )) {
            if( throwing ) {
                throw std::invalid_argument(
                    "Cannot get value at sample " + std::to_string(sample_index) + " at index " +
                    std::to_string( value_index ) +
                    ", as this value is marked as the vector end for that sample."
                );
            } else {
                return false;
            }
        }
        if( is_missing_value_( *ptr )) {
            if( throwing ) {
                throw std::invalid_argument(
                    "Cannot get value at sample " + std::to_string(sample_index) + " at index " +
                    std::to_string( value_index ) +
                    ", as this value is marked as missing for that sample."
                );
            } else {
                return false;
            }
        }
        return true;
    }

    // -------------------------------------------------------------------------
    //     Specializations
    // -------------------------------------------------------------------------

    // We need a couple of special functions that are different for each data type (char*, int, float)
    // that we want to process. Their respective specializations are implemented at the end of this
    // file - this also means that other instanciations will not work out of the box.

    /**
     * @brief Construct the value array using htslib functions, filling the value_buffer_.
     *
     * The function needs to fill value_buffer_ with data, set values_total_ to the number
     * of data elements in the buffer, and set values_reserved_ to the number of total reserved
     * elements in the buffer (which might be larger).
     *
     * As htslib uses the return value of their function call to indicate both the number of elements
     * in the data (what we store in values_total_) as well as error codes if the call failed,
     * we have to take this into account. Hence, our function here returns the return code of
     * bcf_get_format_values()/bcf_get_format_string() from htslib, which is either a negative
     * number indicating some error, or the non-negative number that indicates the number of valid
     * values that have been allocated (in which case it is the same as values_total_).
     */
    int construct_values_(
        ::bcf_hdr_t* header, ::bcf1_t* record, std::string const& id, VcfValueType ht_type
    );

    /**
     * @brief Test whether a value is marked as the end-of-vector by htslib.
     */
    bool is_vector_end_( S val ) const;

    /**
     * @brief Test whether a value is marked as missing by htslib.
     */
    bool is_missing_value_( S val ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Let's keep it easy to detect the end iterator. We initialize to true here,
    // so that we can simply use a default constructor for the end iterator.
    bool is_end_ = true;

    // We keep pointers to the header and the record here, but do not manage them.
    ::bcf_hdr_t* header_ = nullptr;
    ::bcf1_t* record_ = nullptr;

    // Data members needed for htslib functions: value_buffer_ and values_reserved_ are where
    // the data from the record gets copied to. We use a shared_ptr to manage the lifetime,
    // in order to not have to bother with this when copies of this iterator are made
    // (which happens automatically in range-based for loops for example...).
    // values_total_ is the total number of values stored in the buffer, which should be <= the
    // reserved size.
    // TODO find a way to re-use these buffers (maybe store them in VcfRecord instead?!, or
    // by re-using the same iterator instance?! how about a static map from format ID to buffer,
    // so that one can iterate multiple formats at the same time?! then, the pointer here would
    // just need to point to the map value, but not manage it?! not sure how that would need to be)
    // --> make that a std::unordered_map<std::string [id], struct{ buff, buff_size }> that is
    // held by VcfRecord and handed over to the iterator on construction!
    std::shared_ptr<S> value_buffer_ = nullptr;
    int values_reserved_ = 0;
    int values_total_ = 0;

    // Store the total number of samples in the record that we want to iterate over.
    size_t num_samples_;

    // Futhermore, values_per_sample_ is the count of values per sample (you don't say?!),
    // which is computed as the total number of values (returned by the htslib function that gives
    // us the data in the first place) divided by the number of samples. This is really ugly,
    // but it seems that's how htslib wants us to do this.
    size_t values_per_sample_;

    // Position (in number of samples) of our iteration, and position (in number of values per
    // sample) within the current sample. We start with -1 for the value index, to make the "go
    // to next value" function easy to implement without special case for the first value.
    // As htslib assumes int indices anyway, and as there won't be 2 billion values for a single
    // sample in vcf, the loss of variable scope should be okay (as we are using int instand of uint).
    // The value -1 is only ever used in between function calls, and no user should ever be able
    // to observe this value, e.g., via the value_index() function (which we of course assert).
    size_t sample_idx_ = 0;
    int32_t value_idx_ = -1;
};

// =================================================================================================
//     Specializations as needed
// =================================================================================================

// -------------------------------------------------------------------------
//     construct_values_
// -------------------------------------------------------------------------

template<>
inline int VcfFormatIterator<int32_t, int32_t>::construct_values_(
    ::bcf_hdr_t* header, ::bcf1_t* record, std::string const& id, VcfValueType ht_type
) {
    // Get all values, and store the result of the function call, which gives the total
    // number of values that are written to value_buffer_. We store that as values_total_,
    // but also return it so that it can be used as an error checking mechanism
    // (htslib misuses that and encodes errors by negative numbers).
    // As we use a shared_ptr for lifetime management of the buffer, but htslib expects a plain pointer,
    // we do this in two steps, transferring ownership to the shared_ptr after the htslib call.
    // We need a custom deleter for the shared_ptr, as C++11 does not support delete[] for this
    // out of the box (see https://stackoverflow.com/a/13062069/4184258).
    assert( static_cast<int>( ht_type ) == static_cast<int>( VcfValueType::kInteger ));
    int32_t* tmp_ptr = nullptr;
    values_total_ = VcfFormatHelper::bcf_get_format_values_(
        header, record, id.c_str(), reinterpret_cast<void**>( &tmp_ptr ),
        &values_reserved_, static_cast<int>( ht_type )
    );
    value_buffer_ = std::shared_ptr<int32_t>( tmp_ptr, std::default_delete<int32_t[]>());
    return values_total_;
}

template<>
inline int VcfFormatIterator<float, double>::construct_values_(
    ::bcf_hdr_t* header, ::bcf1_t* record, std::string const& id, VcfValueType ht_type
) {
    // See above <int32_t, int32_t> for an explanation. Here, it's the same, just for float.
    assert( static_cast<int>( ht_type ) == static_cast<int>( VcfValueType::kFloat ));
    float* tmp_ptr = nullptr;
    values_total_ = VcfFormatHelper::bcf_get_format_values_(
        header, record, id.c_str(), reinterpret_cast<void**>( &tmp_ptr ),
        &values_reserved_, static_cast<int>( ht_type )
    );
    value_buffer_ = std::shared_ptr<float>( tmp_ptr, std::default_delete<float[]>());
    return values_total_;
}

template<>
inline int VcfFormatIterator<char*, std::string>::construct_values_(
    ::bcf_hdr_t* header, ::bcf1_t* record, std::string const& id, VcfValueType ht_type
) {
    // See above <int32_t, int32_t> for the basic explanation of what we do here.
    // For char*/string we however need to call a different bcf_get_format_*()
    // function, and need two deletion steps to free the char* as well as the char** memory,
    // so we use a custom deleter here that does both.
    (void) ht_type;
    assert( static_cast<int>( ht_type ) == static_cast<int>( VcfValueType::kString ));
    char** tmp_ptr = nullptr;
    int const res = VcfFormatHelper::bcf_get_format_string_(
        header, record, id.c_str(), &tmp_ptr, &values_reserved_
    );
    value_buffer_ = std::shared_ptr<char*>( tmp_ptr, [](char** ptr){
        free(ptr[0]);
        free(ptr);
    });

    // The above htslib call returns the number of chars for the longest string in the data
    // (plus some extra), but not the number of total values. That implies that we cannot have
    // mutliple string values for one sample (which seems not to be documented in htslib anywhere,
    // but seems to be just assumed). Also, that means that we have to manually set the value total
    // to a useful value, which for our purposes has to be the number of samples, so that
    // our division to get values_per_sample_ works out properly.

    // We use our wrapper VcfFormatHelper::bcf_hdr_nsamples_ here, which wraps bcf_hdr_nsamples()
    // so that we do not have to include htslib here.
    values_total_ = VcfFormatHelper::bcf_hdr_nsamples_(header);

    // Now let's return our htslib function call result, which is only used to test that it is
    // not negative (which would indicate an error).
    return res;
}

template<>
inline int VcfFormatIterator<int32_t, VcfGenotype>::construct_values_(
    ::bcf_hdr_t* header, ::bcf1_t* record, std::string const& id, VcfValueType ht_type
) {
    // This class is supposed to be only instanciated from the VcfFormatHelper iterator begin/end functions.
    // Hence, we here simply assert that the provided values are correct.
    // if( id != "GT" ) {
    //     throw std::invalid_argument(
    //         "Can only create an instance of VcfFormatIterator that is using VcfGenotype " +
    //         "for the FORMAT key/id \"GT\", but \"" + id + "\" was provided instead."
    //     );
    // }
    // if( ht_type != VcfValueType.kInt ) {
    //     throw std::invalid_argument(
    //         "Can only create an instance of VcfFormatIterator for value type " +
    //         vcf_value_type_to_string( VcfValueType.kInt ) ", but " +
    //         vcf_value_type_to_string( ht_type ) + " was provided instead."
    //     );
    // }
    (void) id;
    (void) ht_type;
    assert( id == "GT" );
    assert( static_cast<int>( ht_type ) == static_cast<int>( VcfValueType::kInteger ));

    // See above <int32_t, int32_t> for an explanation. Here, it's the same, but we later convert
    // to our VcfGenotype instance instead (implicitly, when returning a value).
    int32_t* tmp_ptr = nullptr;
    values_total_ = VcfFormatHelper::bcf_get_genotypes_(
        header, record, reinterpret_cast<void**>( &tmp_ptr ), &values_reserved_
    );
    value_buffer_ = std::shared_ptr<int32_t>( tmp_ptr, std::default_delete<int32_t[]>());
    return values_total_;
}

// -------------------------------------------------------------------------
//     is_vector_end_
// -------------------------------------------------------------------------

template<>
inline bool VcfFormatIterator<int32_t, int32_t>::is_vector_end_( int32_t val ) const
{
    return val == VcfFormatHelper::bcf_int32_vector_end_;
}

template<>
inline bool VcfFormatIterator<float, double>::is_vector_end_( float val ) const
{
    return VcfFormatHelper::bcf_float_is_vector_end_( val );
}

template<>
inline bool VcfFormatIterator<char*, std::string>::is_vector_end_( char* val ) const
{
    assert( val );
    return *val == VcfFormatHelper::bcf_str_vector_end_;
}

template<>
inline bool VcfFormatIterator<int32_t, VcfGenotype>::is_vector_end_( int32_t val ) const
{
    return val == VcfFormatHelper::bcf_int32_vector_end_;
}

// -------------------------------------------------------------------------
//     is_missing_value_
// -------------------------------------------------------------------------

template<>
inline bool VcfFormatIterator<int32_t, int32_t>::is_missing_value_( int32_t val ) const
{
    return val == VcfFormatHelper::bcf_int32_missing_;
}

template<>
inline bool VcfFormatIterator<float, double>::is_missing_value_( float val ) const
{
    return VcfFormatHelper::bcf_float_is_missing_( val );
}

template<>
inline bool VcfFormatIterator<char*, std::string>::is_missing_value_( char* val ) const
{
    assert( val );
    return *val == VcfFormatHelper::bcf_str_missing_;
}

template<>
inline bool VcfFormatIterator<int32_t, VcfGenotype>::is_missing_value_( int32_t val ) const
{
    return val == VcfFormatHelper::bcf_int32_missing_;
}

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
