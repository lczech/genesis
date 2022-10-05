#ifndef GENESIS_POPULATION_FORMATS_FREQUENCY_TABLE_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_FREQUENCY_TABLE_INPUT_ITERATOR_H_

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

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/variant.hpp"

#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Frequency Table Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as a table of allele frequencies or counts.
 *
 * The expected table has to be in what R calls the "wide" format, that is, samples are in separate
 * columns. This is because otherwise, the amount of data duplication for the fixed columns
 * such as chromosome name and position would just be too much waste.
 *
 * If there is no ref base given (if it is `N`), we cannot know to which bases the counts
 * correspond to. In that case, we assign the ref count to `A`, and the alt count to `C`,
 * respectively. If only the ref base is given, but no alt base, we again use `C` for the alt base,
 * unless the ref is already `C`, in which case we use `A` for the alt base.
 */
class FrequencyTableInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FrequencyTableInputIterator;
    using value_type        = Variant;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

public:

    /**
     * @brief %Iterator over loci of the input sources.
     *
     * This is the class that does the actual work of turning the underlying file data into
     * our Variant and BaseCounts samples. Use the dereference `operator*()`
     * and `operator->()` to get the Variant at the current locus of the iteration.
     */
    class Iterator
    {
        // -------------------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------------------

    public:

        using self_type         = FrequencyTableInputIterator::Iterator;
        using value_type        = Variant;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

    private:

        // When parsing the header line, we need to keep track of what information is present.
        // For this, we use information per sample, as well as about the constant fields.

        // Per sample information, to make sure that we can actually process a sample.
        // Do we have ref and alt counts? Do we have frequency and/or coverage?
        struct SampleInfo
        {
            size_t index = std::numeric_limits<size_t>::max();
            bool has_ref = false;
            bool has_alt = false;
            bool has_frq = false;
            bool has_cov = false;
        };

        // All column info, to make sure that we have all the data needed before processing.
        // Do we have the chromosome and position column? Do we have the ref and alt base column?
        // Which samples are present, and which data do they contain?
        struct HeaderInfo
        {
            // We need to make sure that we only see each data type once.
            bool has_chr = false;
            bool has_pos = false;
            bool has_ref = false;
            bool has_alt = false;

            // We make a list of all sample names that we have seen, noting which type
            // of field we found, to make sure that we have all relevant ones. The order of the
            // indices is the order in which the BaseCounts in the final Variant are produced.
            // Shall be accessed through get_sample_info_() to ensure correct setup of the index.
            std::unordered_map<std::string, SampleInfo> sample_infos;
        };

        // Store the data that is parsed per sample, before processing it into our final Variant.
        struct SampleData
        {
            size_t ref_cnt;
            size_t alt_cnt;
            size_t cov;
            double frq;
        };

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

    private:

        Iterator() = default;
        Iterator( FrequencyTableInputIterator const* parent )
            : parent_( parent )
            , input_stream_(    std::make_shared<utils::InputStream>( parent_->input_source_ ))
            , sample_data_(     std::make_shared<std::vector<SampleData>>() )
            , current_variant_( std::make_shared<Variant>() )
        {
            // Safeguard
            if( ! parent_ ) {
                return;
            }

            // We need a manually constructored shared ptr, as this is private,
            // and make_shared does not have access to this. Hm nope, seems to work?!
            // sample_data_ = std::shared_ptr<std::vector<SampleData>>( new std::vector<SampleData>() );

            // Start streaming the data
            parse_header_();
            increment_();
        }

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend FrequencyTableInputIterator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const* operator->() const
        {
            assert( current_variant_ );
            return &*current_variant_;
        }

        // value_type* operator->()
        // {
        //     return &current_variant_;
        // }

        value_type const& operator*() const
        {
            assert( current_variant_ );
            return *current_variant_;
        }

        // value_type& operator*()
        // {
        //     return current_variant_;
        // }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
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
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same
         * FrequencyTableInputIterator instance will compare equal, as long as neither of them is
         * past-the-end. A valid (not past-the-end) iterator and an end() iterator will not compare
         * equal; all past-the-end iterators compare equal, independently from which parent they
         * were created.
         */
        bool operator==( self_type const& it ) const
        {
            return parent_ == it.parent_;
        }

        bool operator!=( self_type const& it ) const
        {
            return !(*this == it);
        }

        // -------------------------------------------------------------------------
        //     Data Access
        // -------------------------------------------------------------------------

        /**
         * @brief Return the sample names found in the header,
         * in the order in which they are in the Variant of each iteration.
         */
        std::vector<std::string> sample_names() const;

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        // ---------------------------------------------
        //     Init and Header Parsing
        // ---------------------------------------------

        void parse_header_();

        void check_header_fields_(
            std::unordered_set<std::string> const& all_samplenames
        ) const;

        // ---------------------------------------------
        //     Parse Header Fields
        // ---------------------------------------------

        void parse_header_field_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );

        int evaluate_field_as_chr_( std::string const& field );
        int evaluate_field_as_pos_( std::string const& field );
        int evaluate_field_as_ref_( std::string const& field );
        int evaluate_field_as_alt_( std::string const& field );

        int evaluate_field_as_sample_ref_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );
        int evaluate_field_as_sample_alt_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );
        int evaluate_field_as_sample_frq_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );
        int evaluate_field_as_sample_cov_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );

        // ---------------------------------------------
        //     Sample Helpers
        // ---------------------------------------------

        SampleInfo& get_sample_info_( std::string const& samplename );
        bool is_ignored_sample_( std::string const& samplename ) const;

        // ---------------------------------------------
        //     String Matching Helpers
        // ---------------------------------------------

        bool match_header_sample_(
            std::string const& field,
            std::vector<std::string> const& list1,
            std::vector<std::string> const& list2,
            std::string& samplename
        ) const;

        bool match_header_sample_(
            std::string const& field,
            std::vector<std::string> const& list,
            std::string& samplename
        ) const;

        bool match_header_sample_partial_(
            std::string const& field,
            std::string const& prefix,
            std::string& samplename
        ) const;

        // ---------------------------------------------
        //     Increment and Processing Samples
        // ---------------------------------------------

        void increment_();

        void process_sample_data_(
            SampleInfo const& sample_info,
            SampleData const& sample_data,
            Variant& variant,
            size_t sample_index
        );

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        // Parent. If null, this indicates the end of the input and that we are done iterating.
        FrequencyTableInputIterator const* parent_ = nullptr;

        // Data stream to read from.
        std::shared_ptr<utils::InputStream> input_stream_;

        // We keep information about the header, and which samples there are.
        // This also stores which sample has which index, so that we can access them in the Variant.
        HeaderInfo header_info_;

        // At the beginning, we guess which columns contain which information, based on their
        // headers. In order to keep the actual parsing as fast as possible, we use a lambda
        // for each column that is set up in the beginning. This avoids checking types
        // of columns for each row over and over again - we simply need to call all processors
        // in order, across the input file line.
        std::vector<std::function<void(genesis::utils::InputStream&)>> column_processors_;

        // We use a set of sample data objects to buffer values in, using a pointer to keep its
        // address stable (as iterators might be copied or moved implicitly while setting up
        // a for loop for instance). This is accessed via lambdas that capture the shared pointer.
        // This data is used to process them into the actual variant later.
        std::shared_ptr<std::vector<SampleData>> sample_data_;

        // Same reasoning as above: Using a pointer to keep the address of the Variant stable.
        // This is the final object that is exposed to the user.
        std::shared_ptr<Variant> current_variant_;
    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input.
     *
     * Use input_source() to assign an input afterwards.
     */
    FrequencyTableInputIterator() = default;

    /**
     * @brief Create an instance that reads from an @p input_source.
     */
    explicit FrequencyTableInputIterator(
        std::shared_ptr<utils::BaseInputSource> input_source
    )
        // Call the other constuctor, to avoid code duplication.
        : FrequencyTableInputIterator( input_source, std::unordered_set<std::string>{}, false )
    {}

    /**
     * @brief Create an instance that reads from an @p input_source.
     *
     * Additionally, this constructor takes a list of @p sample_names which are used as filter so
     * that only those samples are evaluated and accessible - or,
     * if @p inverse_sample_names is set to `true` - instead all <i>but</i> those samples.
     */
    FrequencyTableInputIterator(
        std::shared_ptr<utils::BaseInputSource> input_source,
        std::unordered_set<std::string> const& sample_names_filter,
        bool inverse_sample_names_filter = false
    )
        : input_source_( input_source )
        , sample_names_filter_( sample_names_filter )
        , inverse_sample_names_filter_( inverse_sample_names_filter )
    {}

    ~FrequencyTableInputIterator() = default;

    FrequencyTableInputIterator( self_type const& ) = default;
    FrequencyTableInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        return Iterator( this );
    }

    Iterator end() const
    {
        return Iterator();
    }

    // -------------------------------------------------------------------------
    //     Basic Input Settings
    // -------------------------------------------------------------------------

    std::shared_ptr<utils::BaseInputSource> input_source() const
    {
        return input_source_;
    }

    /**
     * @brief Set the input source.
     *
     * This overwrites the source if it was already given in the constructor.
     * Shall not be called after iteration has been started.
     */
    self_type& input_source( std::shared_ptr<utils::BaseInputSource> value )
    {
        input_source_ = value;
        return *this;
    }

    std::unordered_set<std::string> const& sample_names_filter() const
    {
        return sample_names_filter_;
    }

    /**
     * @brief Set the sample names to filter for.
     *
     * This overwrites the sample names if the were already given in the constructor.
     * We internally sort them, for faster access.
     * Shall not be called after iteration has been started.
     */
    self_type& sample_names_filter( std::unordered_set<std::string> const& value )
    {
        sample_names_filter_ = value;
        return *this;
    }

    bool inverse_sample_names_filter() const
    {
        return inverse_sample_names_filter_;
    }

    /**
     * @brief Set whether to reverse the sample names to filter for.
     *
     * This overwrites the sample names reverse setting if the were already given in the constructor.
     * Shall not be called after iteration has been started.
     */
    self_type& inverse_sample_names_filter( bool value )
    {
        inverse_sample_names_filter_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    char separator_char() const
    {
        return separator_char_;
    }

    /**
     * @brief Set the separator char used for parsing the tabluar input data.
     *
     * By default, we use a tab `\\t`, but any other character, such as comma, can be used here.
     */
    self_type& separator_char( char value )
    {
        separator_char_ = value;
        return *this;
    }

    double int_factor() const
    {
        return int_factor_;
    }

    /**
     * @brief Set the factor by which frequencies are multiplied if no coverage information is
     * present for a sample.
     *
     * We allow parsing information on allele counts (ref and alt counts), or frequencies and
     * coverage. Howver, there are methods such as HAF-pipe that only output a final frequency,
     * and (by default) do not offer any information on the (effective) coverage that a sample has.
     *
     * However, our internal data representation uses counts instead of frequencies, as we based
     * our equations on existing pool-sequencing population genetic statistics, such as those
     * developed by PoPoolation. Hence, we need to convert from frequencies to counts somehow.
     * In the absence of any coverage information, we hence use a trick, by multiplying the
     * frequency with a large number to obtain counts. In subsequent analyses, using a large number
     * here will basically inactivate the Bessel's correction for coverage (or at least minimize its
     * influence).
     *
     * By default, we use a factor that is the largest integer value that can be represented in
     * double precision floating point numbers (i.e., 9007199254740992.0), which minimizes the above
     * mentioned Bessel's correction influence. However, with this setting, a different factor can
     * be used instead, which is useful when actual (effective) coverage information is available.
     *
     * We currently only allow to set this for the whole input, instead of on a per-sample basis.
     * If needed, we might re-work this feature in the future to allow per-sample effctive coverage.
     */
    self_type& int_factor( double value )
    {
        if( static_cast<double>( static_cast<BaseCounts::size_type>( value )) != value ) {
            throw std::runtime_error(
                "Cannot set int_factor to " + std::to_string( value ) + " as this is out of range "
                "of the int type used for storing base counts."
            );
        }
        int_factor_ = value;
        return *this;
    }

    double allowed_relative_frequency_error() const
    {
        return allowed_rel_freq_error_;
    }

    /**
     * @brief Allowed error margin for frequencies.
     *
     * If an input table contains information on both the ref/alt counts (or only of of them, but
     * also their coverage), as well as their frequency, we do a double check to make sure that
     * everything is in order. This should be the case if the table was computed correctly.
     *
     * This setting here allows to set the threshold for what is considered correct. It is a
     * relative measure, defaulting to 0.1%. That is, the default value is 0.001 of allowed
     * relative error between the count-based frequency that we compute, and the frequency given
     * in the table.
     *
     * Furthermore, we also use this threshold to check that frequencies as given in the input
     * data fall within the range [0.0, 1.0]. Everything outside of that range that is not also
     * within the allowed relative error (as provided here) will lead to an exception.
     * Values thare are within that error, but still slightly outside of the range, will be set
     * to be within range, to get proper frequencies.
     */
    self_type& allowed_relative_frequency_error( double value )
    {
        allowed_rel_freq_error_ = value;
        return *this;
    }

    bool frequency_is_ref() const
    {
        return frequency_is_ref_;
    }

    /**
     * @brief Set whether frequencies are ref or alt frequencies.
     *
     * When the data table contains frequencies, it needs to be decided whether this frequency
     * corresponds to the reference base (use `true` here, default), or to the alternative base
     * (use `false` here).
     */
    self_type& frequency_is_ref( bool value )
    {
        frequency_is_ref_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Input data.
    std::shared_ptr<utils::BaseInputSource> input_source_;
    std::unordered_set<std::string> sample_names_filter_;
    bool inverse_sample_names_filter_ = false;

    // Input settings.
    char separator_char_ = '\t';

    // When reading frequencies, for now, we want to turn them into counts, as this is what
    // our data infrastructure of Variant expects. To lose as little precision as possible, we
    // multiply the frequency [ 0.0, 1.0 ] by the largest integer for which itself and all
    // smaller integers can be stored in a double exactly. This guarantees that all frequencies
    // are mapped into the largest double range that is as exact as we can manage here.
    // Using these fake large counts also basically eliminates Bessel's correction for coverage
    // that we have in some of our pop gen equations, as we do not want that when working with
    // frequencies anyway, e.g., when obtaining them from HAF-pipe.
    // See https://stackoverflow.com/q/1848700/4184258 for the exact double value used here.
    static constexpr double max_int_factor_ = 9007199254740992.0;

    // Make sure that this actually fits into the BaseCounts values.
    static_assert(
        static_cast<BaseCounts::size_type>( max_int_factor_ ) == max_int_factor_,
        "Numeric type for BaseCounts does not fit for FrequencyTableInputIterator::max_int_factor_"
    );

    // The above is the max that we can use, but we allow users to set the used int factor that is
    // used for frequency-based computations, so that they can for example use the expected
    // coverage information from HAF-pipe, or similar values instead.
    double int_factor_ = max_int_factor_;

    // When we have multiple pieces of information for a sample, we do cross checks, to make
    // sure that everything is in order. This relative value here is used for frequencies.
    double allowed_rel_freq_error_ = 0.001;

    // What does the frequency mean? We use: true = ref, false = alt frequency.
    bool frequency_is_ref_ = true;

    // Default names for header fields in a csv file that typically describe
    // the columns and quantities we are interested in. All used case-insensitively.
    std::vector<std::string> chr_names_ = { "chromosome", "chrom", "chr", "contig" };
    std::vector<std::string> pos_names_ = { "position", "pos" };
    std::vector<std::string> ref_names_ = { "reference", "referencebase", "ref", "refbase" };
    std::vector<std::string> alt_names_ = { "alternative", "alternativebase", "alt", "altbase" };
    std::vector<std::string> cnt_names_ = { "count", "cnt" };
    std::vector<std::string> frq_names_ = { "frequency", "freq", "maf", "af" };
    std::vector<std::string> cov_names_ = { "coverage", "cov", "depth", "ad" };

};

} // namespace population
} // namespace genesis

#endif // include guard
