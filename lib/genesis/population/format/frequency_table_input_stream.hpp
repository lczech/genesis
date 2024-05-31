#ifndef GENESIS_POPULATION_FORMAT_FREQUENCY_TABLE_INPUT_STREAM_H_
#define GENESIS_POPULATION_FORMAT_FREQUENCY_TABLE_INPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/variant.hpp"

#include "genesis/sequence/reference_genome.hpp"
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
//     Frequency Table Input Stream
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as a table of allele frequencies or counts.
 *
 * The expected table has to be in what R calls the "wide" format, that is, samples are in separate
 * columns. This is because otherwise, the amount of data duplication for the fixed columns
 * such as chromosome name and position would just be too much waste.
 *
 * The parser will automatically try to determine which samples contain which types of data
 * (reference and alternative counts, frequencies, read depth), and compute whatever needed from that.
 *
 * Some formats do not contain information on the reference and/or alternative base, such as the
 * HAF-pipe frequency tables. For these cases, a reference_genome() can be provided, which will
 * at least set the reference base of the Variant correctly. The alternative base will then be
 * set to the transition base of the reference (A <-> G and C <-> T), which might be wrong, but is
 * the most likely that we can do in the absence of further information. We might add using a
 * reference panel VCF in the future to solve this problem, but as most of our downstream algorithms
 * do not really care about which base is ref and alt, we don't support this as of now.
 *
 * If there is no ref base column (or if it is `N`) or ref genome given, we cannot know to which
 * bases the counts correspond to. In that case, we assign the ref count to `A`, and the alt count
 * to `G`, respectively. If only the ref base is given, but no alt base, we again use the transition
 * base, as explained above.
 */
class FrequencyTableInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FrequencyTableInputStream;
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
     * our Variant and SampleCounts samples. Use the dereference `operator*()`
     * and `operator->()` to get the Variant at the current locus of the iteration.
     */
    class Iterator
    {
        // -------------------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------------------

    public:

        using self_type         = FrequencyTableInputStream::Iterator;
        using value_type        = Variant;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

    private:

        // When parsing the header line, we need to keep track of what information is present.
        // For this, we use information per sample, as well as about the constant fields.

        // Per sample information, to make sure that we can actually process a sample.
        // Do we have ref and alt counts? Do we have frequency and/or read depth?
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
            // indices is the order in which the SampleCounts in the final Variant are produced.
            // Shall be accessed through get_sample_info_() to ensure correct setup of the index.
            std::unordered_map<std::string, SampleInfo> sample_infos;
        };

        // Store the data that is parsed per sample, before processing it into our final Variant.
        struct SampleData
        {
            bool   is_missing = false;
            size_t ref_cnt    = 0;
            size_t alt_cnt    = 0;
            size_t cov        = 0;
            double frq        = 0.0;
        };

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

    private:

        Iterator() = default;
        Iterator( FrequencyTableInputStream const* parent )
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

        friend FrequencyTableInputStream;

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
         * FrequencyTableInputStream instance will compare equal, as long as neither of them is
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

        int evaluate_if_field_is_chr_( std::string const& field );
        int evaluate_if_field_is_pos_( std::string const& field );
        int evaluate_if_field_is_ref_( std::string const& field );
        int evaluate_if_field_is_alt_( std::string const& field );

        int evaluate_if_field_is_sample_ref_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );
        int evaluate_if_field_is_sample_alt_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );
        int evaluate_if_field_is_sample_frq_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );
        int evaluate_if_field_is_sample_cov_(
            std::string const& field,
            std::unordered_set<std::string>& all_samplenames
        );

        // ---------------------------------------------
        //     Sample Helpers
        // ---------------------------------------------

        SampleInfo& get_sample_info_( std::string const& samplename );
        bool is_ignored_sample_( std::string const& samplename ) const;
        static bool parse_if_missing_(
            FrequencyTableInputStream const* parent,
            genesis::utils::InputStream& input_stream
        );

        // ---------------------------------------------
        //     String Matching Helpers
        // ---------------------------------------------

        bool match_header_field_(
            std::string const& field,
            std::string const& user_string,
            std::vector<std::string> const& predefined_list
        ) const;

        bool match_header_sample_(
            std::string const& field,
            std::string const& user_substring,
            std::vector<std::string> const& predefined_list,
            std::string& samplename
        ) const;

        bool match_header_sample_(
            std::string const& field,
            std::string const& user_substring,
            std::vector<std::string> const& predefined_list1,
            std::vector<std::string> const& predefined_list2,
            std::string& samplename
        ) const;

        bool match_header_sample_user_partial_(
            std::string const& field,
            std::string const& substring,
            std::string& samplename
        ) const;

        bool match_header_sample_predefined_partial_(
            std::string const& field,
            std::string const& substring,
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
        FrequencyTableInputStream const* parent_ = nullptr;

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
    FrequencyTableInputStream() = default;

    /**
     * @brief Create an instance that reads from an @p input_source.
     */
    explicit FrequencyTableInputStream(
        std::shared_ptr<utils::BaseInputSource> input_source
    )
        // Call the other constuctor, to avoid code duplication.
        : FrequencyTableInputStream( input_source, std::unordered_set<std::string>{}, false )
    {}

    /**
     * @brief Create an instance that reads from an @p input_source.
     *
     * Additionally, this constructor takes a list of @p sample_names which are used as filter so
     * that only those samples are evaluated and accessible - or,
     * if @p inverse_sample_names is set to `true` - instead all <i>but</i> those samples.
     */
    FrequencyTableInputStream(
        std::shared_ptr<utils::BaseInputSource> input_source,
        std::unordered_set<std::string> const& sample_names_filter,
        bool inverse_sample_names_filter = false
    )
        : input_source_( input_source )
        , sample_names_filter_( sample_names_filter )
        , inverse_sample_names_filter_( inverse_sample_names_filter )
    {}

    ~FrequencyTableInputStream() = default;

    FrequencyTableInputStream( self_type const& ) = default;
    FrequencyTableInputStream( self_type&& )      = default;

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
    //     Header Parsing Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Specify a string that marks the chromosome column in the header.
     *
     * By default, this string is empty, and instead we search for the chromosome column
     * in the header by matching with a list of commonly used strings, such as
     * `chromosome`, `chr`, or `contig`.
     *
     * However, if set to a non-empty string, this string is searched instead in the header, and
     * the respective column is used for the chromosome information when parsing the table.
     */
    self_type& header_chromosome_string( std::string const& str )
    {
        usr_chr_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set string that marks the chromosome columnn in the header.
     *
     * See the setter header_chromosome_string( std::string const& ) for details.
     */
    std::string const& header_chromosome_string() const
    {
        return usr_chr_name_;
    }

    /**
     * @brief Specify a string that marks the position column in the header.
     *
     * See the setter header_chromosome_string( std::string const& ) for details;
     * this setter here however specifies the column for the position within a given chromosome.
     */
    self_type& header_position_string( std::string const& str )
    {
        usr_pos_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set string that marks the position columnn in the header.
     *
     * See the setter header_position_string( std::string const& ) for details.
     */
    std::string const& header_position_string() const
    {
        return usr_pos_name_;
    }

    /**
     * @brief Specify a string that marks the reference base column in the header.
     *
     * See the setter header_chromosome_string( std::string const& ) for details;
     * this setter here however specifies the column for the reference base.
     */
    self_type& header_reference_base_string( std::string const& str )
    {
        usr_ref_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set string that marks the reference base columnn in the header.
     *
     * See the setter header_reference_base_string( std::string const& ) for details.
     */
    std::string const& header_reference_base_string() const
    {
        return usr_ref_name_;
    }

    /**
     * @brief Specify a string that marks the alternative base column in the header.
     *
     * See the setter header_chromosome_string( std::string const& ) for details;
     * this setter here however specifies the column for the alternative base.
     */
    self_type& header_alternative_base_string( std::string const& str )
    {
        usr_alt_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set string that marks the alternative base columnn in the header.
     *
     * See the setter header_alternative_base_string( std::string const& ) for details.
     */
    std::string const& header_alternative_base_string() const
    {
        return usr_alt_name_;
    }

    /**
     * @brief Specify a (sub)string that is the prefix or suffix
     * for header columns containing the reference base count of a sample.
     *
     * By default, this string is empty, and instead we search for the reference base count columns
     * of samples in the header by matching with a list of commonly used prefixes and suffixes,
     * such as `ref_cnt` or `reference-base-count`.
     *
     * However, if set to a non-empty string, this string is searched instead in the header
     * as a prefix or suffix, and for every match, the respective column is used as the reference
     * base count information of a sample when parsing the table. The sample name is then the
     * remainder of the column name that is left without the prefix or suffix.
     */
    self_type& header_sample_reference_count_substring( std::string const& str )
    {
        usr_smp_ref_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set (sub)string that is the prefix or suffix
     * for header columns containing the reference base count of a sample.
     *
     * See the setter header_sample_reference_count_substring( std::string const& ) for details.
     */
    std::string const& header_sample_reference_count_substring() const
    {
        return usr_smp_ref_name_;
    }

    /**
     * @brief Specify a (sub)string that is the prefix or suffix
     * for header columns containing the alternative base count of a sample.
     *
     * See the setter header_sample_reference_count_substring( std::string const& ) for details;
     * this setter here however specifies the prefix or suffix for columns containing
     * the alternative base count of samples.
     */
    self_type& header_sample_alternative_count_substring( std::string const& str )
    {
        usr_smp_alt_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set (sub)string that is the prefix or suffix
     * for header columns containing the alternative base count of a sample.
     *
     * See the setter header_sample_alternative_count_substring( std::string const& ) for details.
     */
    std::string const& header_sample_alternative_count_substring() const
    {
        return usr_smp_alt_name_;
    }

    /**
     * @brief Specify a (sub)string that is the prefix or suffix
     * for header columns containing the frequency of a sample.
     *
     * See the setter header_sample_reference_count_substring( std::string const& ) for details;
     * this setter here however specifies the prefix or suffix for columns containing
     * the frequency of samples.
     */
    self_type& header_sample_frequency_substring( std::string const& str )
    {
        usr_smp_frq_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set (sub)string that is the prefix or suffix
     * for header columns containing the frequency of a sample.
     *
     * See the setter header_sample_frequency_substring( std::string const& ) for details.
     */
    std::string const& header_sample_frequency_substring() const
    {
        return usr_smp_frq_name_;
    }

    /**
     * @brief Specify a (sub)string that is the prefix or suffix
     * for header columns containing the read depth of a sample (that is,
     * the sum of reference and alternative base counts).
     *
     * See the setter header_sample_reference_count_substring( std::string const& ) for details;
     * this setter here however specifies the prefix or suffix for columns containing
     * the read depth of samples.
     */
    self_type& header_sample_read_depth_substring( std::string const& str )
    {
        usr_smp_cov_name_ = str;
        return *this;
    }

    /**
     * @brief Return the currently set (sub)string that is the prefix or suffix
     * for header columns containing the read depth of a sample.
     *
     * See the setter header_sample_read_depth_substring( std::string const& ) for details.
     */
    std::string const& header_sample_read_depth_substring() const
    {
        return usr_smp_cov_name_;
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    std::shared_ptr<::genesis::sequence::ReferenceGenome> reference_genome() const
    {
        return ref_genome_;
    }

    /**
     * @brief Reference genome used to phase input data without reference bases.
     *
     * Some frequency table formats, such as the ones coming from HAF-pipe, do not contain
     * information on the reference or alternative bases. In these cases, we could just assign
     * the frequencies to random bases. However, when given the proper reference genome here
     * that was used to infer the frequencies in the first place, we can at least assign
     * the correct reference base.
     *
     * Note: While supplying the reference genome will correctly set the referen base,
     * we might still not be able to obtain the alternative base that the frequency represents,
     * if that information is simply not present in the input file. For instance, with the HAF-pipe
     * output format, that is unknowable. In the future, we might add reading in a founder panel VCF,
     * or something alike that would give that information.
     * However, most of our downstream algorithms do not really need to know the exact alternative
     * base anyway, so instead, in these cases, we simply assign the transition base of the
     * reference base (A <-> G and C <-> T) instead, to keep it simple. That is the most likely
     * we can do without further information.
     */
    self_type& reference_genome( std::shared_ptr<::genesis::sequence::ReferenceGenome> value )
    {
        ref_genome_ = value;
        return *this;
    }

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

    std::string const& missing_value() const
    {
        return usr_missing_;
    }

    /**
     * @brief Set the string that indicates missing data.
     *
     * By default, we use `.`, `na`, and `nan` as indicators of missing data, in which case the
     * SampleCounts will be set to missing when any of these values is involved in the parsing.
     * With this setting, instead, the given @p value will be used to indicate missing data.
     */
    self_type& missing_value( std::string const& value )
    {
        usr_missing_ = value;
        return *this;
    }

    double int_factor() const
    {
        return int_factor_;
    }

    /**
     * @brief Set the factor by which frequencies are multiplied if no read depth information is
     * present for a sample.
     *
     * We allow parsing information on allele counts (ref and alt counts), or frequencies and read
     * depth. Howver, there are methods such as HAF-pipe that only output a final frequency, and
     * (by default) do not offer any information on the (effective) read depth that a sample has.
     *
     * However, our internal data representation uses counts instead of frequencies, as we based
     * our equations on existing pool-sequencing population genetic statistics, such as those
     * developed by PoPoolation. Hence, we need to convert from frequencies to counts somehow.
     * In the absence of any read depth information, we hence use a trick, by multiplying the
     * frequency with a large number to obtain counts. In subsequent analyses, using a large number
     * here will basically inactivate the Bessel's correction for read depth (or at least minimize
     * its influence).
     *
     * By default, we use a factor that is the largest integer value that can be represented in
     * double precision floating point numbers (i.e., 9007199254740992.0), which minimizes the above
     * mentioned Bessel's correction influence. However, with this setting, a different factor can
     * be used instead, which is useful when actual (effective) read depth information is available.
     *
     * We currently only allow to set this for the whole input, instead of on a per-sample basis.
     * If needed, we might re-work this feature in the future to allow per-sample effctive read
     * depth.
     */
    self_type& int_factor( double value )
    {
        if(
            value > max_int_factor_ ||
            static_cast<double>( static_cast<SampleCounts::size_type>( value )) != value
        ) {
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
     * also their read depth), as well as their frequency, we do a double check to make sure that
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
    std::shared_ptr<::genesis::sequence::ReferenceGenome> ref_genome_;
    char separator_char_ = '\t';

    // Missing data indicators. We have a default set that we use, and a user-provided one.
    // Both are used case-insensitive. Need to be sorted so that `nan` comes before `na`,
    // as otherwise, `na` would match already when its actually `nan`, leading to an error.
    std::vector<std::string> missing_ = { ".", "nan", "na" };
    std::string usr_missing_;

    // When reading frequencies, for now, we want to turn them into counts, as this is what
    // our data infrastructure of Variant expects. To lose as little precision as possible, we
    // multiply the frequency [ 0.0, 1.0 ] by the largest integer for which itself and all
    // smaller integers can be stored in a double exactly. This guarantees that all frequencies
    // are mapped into the largest double range that is as exact as we can manage here.
    // Using these fake large counts also basically eliminates Bessel's correction for read depth
    // that we have in some of our pop gen equations, as we do not want that when working with
    // frequencies anyway, e.g., when obtaining them from HAF-pipe.
    // See https://stackoverflow.com/q/1848700/4184258 for the exact double value used here.
    static constexpr double max_int_factor_ = 9007199254740992.0;

    // We use a smaller factor by default, to make sure that we can add numbers without reaching
    // the max int precesion point of double, just in case. We are internally using size_t for
    // counts, so that should not happen, but prevents accidents. Also, using a power of ten here
    // it makes numbers converted from data nicer for users and potentially easier to understand,
    // as they more clearly correspond to the frequencies actually specified in the input data.
    static constexpr double default_int_factor_ = 1000000.0;

    // Make sure that this actually fits into the SampleCounts values.
    static_assert(
        static_cast<double>( static_cast<SampleCounts::size_type>( max_int_factor_ )) == max_int_factor_,
        "Numeric type for SampleCounts does not fit for FrequencyTableInputStream::max_int_factor_"
    );

    // The above is the max that we can use, but we allow users to set the used int factor that is
    // used for frequency-based computations, so that they can for example use the expected
    // read depth information from HAF-pipe, or similar values instead.
    // double int_factor_ = max_int_factor_;
    double int_factor_ = default_int_factor_;

    // When we have multiple pieces of information for a sample, we do cross checks, to make
    // sure that everything is in order. This relative value here is used for frequencies.
    double allowed_rel_freq_error_ = 0.001;

    // What does the frequency mean? We use: true = ref, false = alt frequency.
    // Probably would be more neat with an enum, but it's only used internally, so that's okay.
    bool frequency_is_ref_ = true;

    // Default names for header fields in a csv file that typically describe
    // the columns and quantities we are interested in. All used case-insensitively.
    std::vector<std::string> chr_names_ = { "chromosome", "chrom", "chr", "contig" };
    std::vector<std::string> pos_names_ = { "position", "pos" };
    std::vector<std::string> ref_names_ = { "reference", "referencebase", "ref", "refbase" };
    std::vector<std::string> alt_names_ = { "alternative", "alternativebase", "alt", "altbase" };
    std::vector<std::string> cnt_names_ = { "counts", "count", "cnt", "ct" };
    std::vector<std::string> frq_names_ = { "frequency", "freq", "maf", "af", "allelefrequency" };
    std::vector<std::string> cov_names_ = { "coverage", "cov", "readdepth", "depth", "ad" };

    // User supplied overwrites for the above automatic terms.
    // If either of them is given, we use those instead of the generic word lists.
    std::string usr_chr_name_;
    std::string usr_pos_name_;
    std::string usr_ref_name_;
    std::string usr_alt_name_;
    std::string usr_smp_ref_name_;
    std::string usr_smp_alt_name_;
    std::string usr_smp_frq_name_;
    std::string usr_smp_cov_name_;

};

} // namespace population
} // namespace genesis

#endif // include guard
