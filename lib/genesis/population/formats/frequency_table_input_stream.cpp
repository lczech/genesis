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

#include "genesis/population/formats/frequency_table_input_stream.hpp"

#include "genesis/population/functions/functions.hpp"
#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Init and Header Parsing
// =================================================================================================

// -------------------------------------------------------------------------
//     sample_names
// -------------------------------------------------------------------------

std::vector<std::string> FrequencyTableInputStream::Iterator::sample_names() const
{
    // We only need this rarely, so we don't need an efficient algorithm for this.
    // Could be done by storing a vector in the first place, but that would mean that we need to
    // keep that in sync with the other data objects... So instead we just create the vector here.
    std::vector<std::string> result;
    result.resize( header_info_.sample_infos.size() );
    for( auto const& sample_info : header_info_.sample_infos ) {
        assert( sample_info.second.index < result.size() );
        result[ sample_info.second.index ] = sample_info.first;
    }
    return result;
}

// -------------------------------------------------------------------------
//     parse_header_
// -------------------------------------------------------------------------

void FrequencyTableInputStream::Iterator::parse_header_()
{
    // Only called when we have a parent.
    assert( parent_ );

    // Get a vector of each field in the header line.
    auto const header_fields = utils::split(
        input_stream_->get_line(), parent_->separator_char_, false
    );

    // We keep a list of all sample names that we found, including the ignored ones,
    // in order to check that this fits with the given fitler sample name list, just in case.
    std::unordered_set<std::string> all_samplenames;

    // Parse all the headers once, in a dry run, to collect information on the samples.
    // We do this two-pass process in order to be able to correctly size the resulting vectors,
    // and use pointers to the actual data elements directly, instead of indices, which is likely
    // to be going to be slower.
    for( auto const& field : header_fields ) {
        // Safety.
        if( field.empty() || ! std::all_of( field.begin(), field.end(), utils::is_graph )) {
            throw std::runtime_error(
                "Invalid frequency table with non-graph characters or empty field in header."
            );
        }

        // Do something with the field, depending on what text it contains.
        parse_header_field_( field, all_samplenames );
    }

    // Important checks that we could do as an assertion, but it's kind of what this whole class
    // relies on, so let's check it all the time.
    if( column_processors_.size() != header_fields.size() ) {
        throw std::domain_error(
            "Internal error: Number of column processors does not match number of columns"
        );
    }

    // Check that all samples have at least two of the fields,
    // so that we can compute actual counts from them.
    check_header_fields_( all_samplenames );

    // If ref and/or alt base columns are not present, we want to use 'N' instead.
    // As there are no column processors for those in this case, we can just leave the Variant
    // at it's default, which is 'N' for both, and it will never change. Assert here that we have 'N'
    assert( current_variant_->reference_base == 'N' );
    assert( current_variant_->alternative_base == 'N' );

    // Now resize the intermediate and result data to the number of samples that we are expecting.
    // The processor lambdas will directly write to these, using indices into them.
    sample_data_->resize( header_info_.sample_infos.size() );
    current_variant_->samples.resize( header_info_.sample_infos.size() );
}

// -------------------------------------------------------------------------
//     check_header_fields_
// -------------------------------------------------------------------------

void FrequencyTableInputStream::Iterator::check_header_fields_(
    std::unordered_set<std::string> const& all_samplenames
) const {
    // Check that chr and pos are there.
    // The ref and alt base are not needed, we will just use 'N' instead.
    if( ! header_info_.has_chr ) {
        throw std::runtime_error(
            "Invalid frequency table that does not contain a chromosome column"
        );
    }
    if( ! header_info_.has_pos ) {
        throw std::runtime_error(
            "Invalid frequency table that does not contain a position column"
        );
    }

    // Check that we have enough information for each sample to be processable.
    // We can work with just ref and alt counts, or just the frequency. If either of those is
    // present, we are good.
    for( auto const& sample_info_entry : header_info_.sample_infos ) {
        auto const& sample_info = sample_info_entry.second;

        // List all valid combinations of data fields that we can handle.
        // If either of them occurs, we can parse that sample and extract its counts.
        bool good = sample_info.has_frq;
        good |= ( sample_info.has_ref && sample_info.has_alt );
        good |= ( sample_info.has_cov && ( sample_info.has_ref || sample_info.has_alt ));
        if( ! good ) {
            throw std::runtime_error(
                "Frequency table sample \"" + sample_info_entry.first + "\" does not contain enough "
                "information to compute allele counts."
            );
        }
    }

    // We also want to warn if not all sample types are the same. That might indicate that
    // something is not quite right. That is, if one sample has frequencies, but another one
    // has counts, that seems rather fishy. We can work with this, but might want to tell the user.
    // To this end, we turn the bools into binary indicator flags, and check for equality.
    int sample_flags = 0;
    for( auto const& sample_info_entry : header_info_.sample_infos ) {
        auto const& sample_info = sample_info_entry.second;

        // Create the flag. We already checked before that it never is 0, as otherwise, the
        // previous check in the loop above would not have succeeded.
        int flag = 0;
        flag += sample_info.has_ref ? 1 : 0;
        flag += sample_info.has_alt ? 2 : 0;
        flag += sample_info.has_frq ? 4 : 0;
        flag += sample_info.has_cov ? 8 : 0;
        assert( flag > 0 );
        if( sample_flags == 0 ) {
            sample_flags = flag;
        }

        // If the flags differ, issue a warning once. We then don't need to further check.
        if( sample_flags != flag ) {
            LOG_WARN << "Frequency table samples contain different types of data "
                     << "(reference or alternative counts, frequencies, or coverage). "
                     << "We can handle this, but it might indicate that something went wrong "
                     << "when parsing and interpreting the header fields to obtain sample names.";
            break;
        }
    }

    // Check that all given sample names for filtering are actually valid names.
    assert( parent_ );
    for( auto const& sn : parent_->sample_names_filter_ ) {
        if( all_samplenames.count( sn ) == 0 ) {
            throw std::invalid_argument(
                "Frequency table header does not contain given sample name filter \"" + sn + "\"."
            );
        }
    }
}

// =================================================================================================
//     Parse Header Fields
// =================================================================================================

// -------------------------------------------------------------------------
//     parse_header_field_
// -------------------------------------------------------------------------

void FrequencyTableInputStream::Iterator::parse_header_field_(
    std::string const& field,
    std::unordered_set<std::string>& all_samplenames
) {
    // We already checked before, so just assert here.
    assert( ! field.empty() );

    // We here have some field given from the table header, and want to figure out what it is.
    // We try all our types of fields that are supported by this reader: is it
    //  - the chromosome name or the position in the chromosome
    //  - the ref or alt base
    //  - one of numbers of ref/alt counts, coverage, or frequency for a sample
    // We try to evaluate the field as all types, and see if any of them matches the patterns we
    // are looking for, and keep track of how many matched (the functions return 1 on success of
    // matching, and 0 otherwise), to make sure that we only have at most one valid match.
    // If more than that, the field is ambiguous as far as our pattern matching is concerned,
    // and in that case, we cannot continue safely.
    // While matching, the functions also set up the respective parsers for the columns,
    // so that after this, we have a parser for all columns set up.
    int matches = 0;
    matches += evaluate_field_as_chr_( field );
    matches += evaluate_field_as_pos_( field );
    matches += evaluate_field_as_ref_( field );
    matches += evaluate_field_as_alt_( field );
    matches += evaluate_field_as_sample_ref_( field, all_samplenames );
    matches += evaluate_field_as_sample_alt_( field, all_samplenames );
    matches += evaluate_field_as_sample_frq_( field, all_samplenames );
    matches += evaluate_field_as_sample_cov_( field, all_samplenames );

    if( matches == 0 ) {
        // Field that we could not make sense of. Ignored for now; might add warning later.
        // In order to ignore it, we need to use an empty processor function that does nothing.
        auto cur_var = current_variant_;
        auto const sep_char = parent_->separator_char_;
        column_processors_.push_back( [cur_var, sep_char]( genesis::utils::InputStream& it ){
            utils::skip_until(
                it, [sep_char]( char c ){
                    return c == '\n' || c == sep_char;
                }
            );
        });
    }
    if( matches > 1 ) {
        // If multiple match, we cannot distinguish between them, and rather throw than cause
        // unexpected behaviour for the user.
        throw std::runtime_error(
            "Cannot read frequency table header, as it contains ambiguous headers. "
            "Header field name \"" + field + "\" matches multiple types of data columns."
        );
    }
}

// -------------------------------------------------------------------------
//     evaluate_field_as_chr_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_chr_(
    std::string const& field
) {
    assert( parent_ );
    assert( ! field.empty() );

    // Check if we find a matching header name. Either it matches the user provided header column
    // name, if that is provided, or it matches any of our predefined names for that column.
    // We don't actually need to check if the usr_chr_name_ is non-empty, as it only matches the
    // non-empty field if it also is non-empty. If it doesn't match either, we are done here.
    if( ! match_header_field_( field, parent_->usr_chr_name_, parent_->chr_names_ )) {
        return 0;
    }

    // Checks that the type of column was not already given via some other column header.
    if( header_info_.has_chr ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, "
            "as it contains multiple columns for the chromosome."
        );
    }
    header_info_.has_chr = true;

    // We need a copy of the ptr, so that we can capture this with a lambda.
    // In C++11, lambda cannot yet directly capture class members...
    auto cur_var = current_variant_;
    auto const sep_char = parent_->separator_char_;

    // Add processing function that will be used for parsing values of this column.
    column_processors_.push_back( [cur_var, sep_char]( genesis::utils::InputStream& it ){
        // Read the chromosome name, and check its validity.
        cur_var->chromosome = utils::read_until(
            it, [sep_char]( char c ){
                return ! utils::is_graph( c ) || c == '\n' || c == sep_char;
            }
        );
        if( cur_var->chromosome.empty() ) {
            throw std::runtime_error(
                "Malformed frequency table with empty chromosome name in line " + it.at()
            );
        }
    });

    // Indicate that we found a matching header name.
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_pos_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_pos_(
    std::string const& field
) {
    // Same setup as in evaluate_field_as_chr_(). See there for comments.
    // Only difference is the parsing function in the processor lambda.

    assert( parent_ );
    assert( ! field.empty() );
    if( ! match_header_field_( field, parent_->usr_pos_name_, parent_->pos_names_ )) {
        return 0;
    }
    if( header_info_.has_pos ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, "
            "as it contains multiple columns for the position."
        );
    }
    header_info_.has_pos = true;
    auto cur_var = current_variant_;
    column_processors_.push_back( [cur_var]( genesis::utils::InputStream& it ){
        cur_var->position = utils::parse_unsigned_integer<size_t>( it );
        if( cur_var->position == 0 ) {
            throw std::runtime_error(
                "Malformed frequency table with position == 0 in line " + it.at()
            );
        }
    });
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_ref_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_ref_(
    std::string const& field
) {
    // Same setup as in evaluate_field_as_chr_(). See there for comments.
    // Only difference is the parsing function in the processor lambda.

    assert( parent_ );
    assert( ! field.empty() );
    if( ! match_header_field_( field, parent_->usr_ref_name_, parent_->ref_names_ )) {
        return 0;
    }
    if( header_info_.has_ref ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, "
            "as it contains multiple columns for the reference base."
        );
    }
    header_info_.has_ref = true;
    auto cur_var = current_variant_;
    column_processors_.push_back( [cur_var]( genesis::utils::InputStream& it ){
        // Read the single char base, and check it.
        // This even works when we are at the end of the data already.
        auto const b = utils::to_upper( *it );
        if( ! is_valid_base_or_n( b )) {
            throw std::runtime_error(
                "Malformed frequency table with reference base not in [ACGTN] in line " + it.at()
            );
        }
        cur_var->reference_base = b;
        ++it;
    });
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_alt_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_alt_(
    std::string const& field
) {
    // Same setup as in evaluate_field_as_chr_(). See there for comments.
    // Only difference is the parsing function in the processor lambda.

    assert( parent_ );
    assert( ! field.empty() );
    if( ! match_header_field_( field, parent_->usr_alt_name_, parent_->alt_names_ )) {
        return 0;
    }
    if( header_info_.has_alt ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, "
            "as it contains multiple columns for the alternative base."
        );
    }
    header_info_.has_alt = true;
    auto cur_var = current_variant_;
    column_processors_.push_back( [cur_var]( genesis::utils::InputStream& it ){
        // Same as above for the ref base.
        auto const b = utils::to_upper( *it );
        if( ! is_valid_base_or_n( b )) {
            throw std::runtime_error(
                "Malformed frequency table with alternative base not in [ACGTN] in line " + it.at()
            );
        }
        cur_var->alternative_base = b;
        ++it;
    });
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_sample_ref_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_sample_ref_(
    std::string const& field,
    std::unordered_set<std::string>& all_samplenames
) {
    // Here, we use a different matching algorithm than for the fixed fields:
    // We try to find a match that also contains a prefix or suffix that is the sample name.
    // Furthermore, we then need to take care of setting up the sample information
    // and the data accordingly.

    // Check if any combination of names fits the fields, and store the remaining sample name.
    assert( parent_ );
    assert( ! field.empty() );
    std::string samplename;
    if( ! match_header_sample_(
        field, parent_->usr_smp_ref_name_, parent_->ref_names_, parent_->cnt_names_, samplename
    )) {
        return 0;
    }

    // Insert the sample name into the list, for later checking.
    all_samplenames.insert( samplename );

    // Check if we ignore the sample. We do this here before it has been added to the sample info.
    // We still read the value, to make sure that it's good data, but don't do anything with it.
    // We return 1 here to indicate that we matched with a pattern successfully.
    if( is_ignored_sample_( samplename )) {
        column_processors_.push_back( []( genesis::utils::InputStream& it ){
            utils::parse_unsigned_integer<size_t>( it );
        });
        return 1;
    }

    // First check if we have the sample already, or need to create a new one in the info.
    // Then, check that a ref base count was not already found for this sample name.
    auto& sample_info = get_sample_info_( samplename );
    if( sample_info.has_ref ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, as it contains multiple columns "
            "for the reference count of sample \"" + samplename + "\"."
        );
    }
    sample_info.has_ref = true;

    // Now we have a match, and need to set up the column for the sample.
    // We again use a copy of the shared ptr here, so that the lambda can capture it,
    // and we capture the index by value. We cannot capture the adress of the sample data object
    // itself here, as the vector might get resized if more columns are added, so the address
    // is not stable. Hence, we are using the index. There might be other ways to implement that
    // (using a vector of pointers, or only added the lambdas later once we know how many samples
    // there are), but that does neither seem faster nor simpler, so we stick with this approach.
    assert( sample_info.index < std::numeric_limits<size_t>::max() );
    auto sample_data = sample_data_;
    auto index = sample_info.index;
    column_processors_.push_back( [sample_data, index]( genesis::utils::InputStream& it ){
        assert( index < sample_data->size() );
        sample_data->at(index).ref_cnt = utils::parse_unsigned_integer<size_t>( it );
    });

    // Indicate that we found a matching header name.
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_sample_alt_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_sample_alt_(
    std::string const& field,
    std::unordered_set<std::string>& all_samplenames
) {
    // Same as above in evaluate_field_as_sample_ref_(), but without comments here
    // to keep it shorter. See there for explanations.
    // This is quite some code duplication, but we have this intermixed with variable access
    // of the data that we are writing to, etc, and would be quite the cumbersone template
    // function to use instead... not quite sure if this is better.

    assert( parent_ );
    assert( ! field.empty() );
    std::string samplename;
    if( ! match_header_sample_(
        field, parent_->usr_smp_alt_name_, parent_->alt_names_, parent_->cnt_names_, samplename
    )) {
        return 0;
    }
    all_samplenames.insert( samplename );
    if( is_ignored_sample_( samplename )) {
        column_processors_.push_back( []( genesis::utils::InputStream& it ){
            utils::parse_unsigned_integer<size_t>( it );
        });
        return 1;
    }
    auto& sample_info = get_sample_info_( samplename );
    if( sample_info.has_alt ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, as it contains multiple columns "
            "for the alternative count of sample \"" + samplename + "\"."
        );
    }
    sample_info.has_alt = true;
    assert( sample_info.index < std::numeric_limits<size_t>::max() );
    auto sample_data = sample_data_;
    auto index = sample_info.index;
    column_processors_.push_back( [sample_data, index]( genesis::utils::InputStream& it ){
        assert( index < sample_data->size() );
        sample_data->at(index).alt_cnt = utils::parse_unsigned_integer<size_t>( it );
    });
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_sample_frq_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_sample_frq_(
    std::string const& field,
    std::unordered_set<std::string>& all_samplenames
) {
    // Same as above in evaluate_field_as_sample_ref_(), but without comments here
    // to keep it shorter. See there for explanations.

    assert( parent_ );
    assert( ! field.empty() );
    std::string samplename;
    if( ! match_header_sample_(
        field, parent_->usr_smp_frq_name_, parent_->frq_names_, samplename
    )) {
        return 0;
    }
    all_samplenames.insert( samplename );
    if( is_ignored_sample_( samplename )) {
        column_processors_.push_back( []( genesis::utils::InputStream& it ){
            utils::parse_float<double>( it );
        });
        return 1;
    }
    auto& sample_info = get_sample_info_( samplename );
    if( sample_info.has_frq ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, as it contains multiple columns "
            "for the frequency of sample \"" + samplename + "\"."
        );
    }
    sample_info.has_frq = true;
    assert( sample_info.index < std::numeric_limits<size_t>::max() );
    auto sample_data = sample_data_;
    auto index = sample_info.index;
    column_processors_.push_back( [sample_data, index]( genesis::utils::InputStream& it ){
        assert( index < sample_data->size() );
        sample_data->at(index).frq = utils::parse_float<double>( it );
    });
    return 1;
}

// -------------------------------------------------------------------------
//     evaluate_field_as_sample_cov_
// -------------------------------------------------------------------------

int FrequencyTableInputStream::Iterator::evaluate_field_as_sample_cov_(
    std::string const& field,
    std::unordered_set<std::string>& all_samplenames
) {
    // Same as above in evaluate_field_as_sample_ref_(), but without comments here
    // to keep it shorter. See there for explanations.

    assert( parent_ );
    assert( ! field.empty() );
    std::string samplename;
    if( ! match_header_sample_(
        field, parent_->usr_smp_cov_name_, parent_->cov_names_, samplename
    )) {
        return 0;
    }
    all_samplenames.insert( samplename );
    if( is_ignored_sample_( samplename )) {
        column_processors_.push_back( []( genesis::utils::InputStream& it ){
            utils::parse_unsigned_integer<size_t>( it );
        });
        return 1;
    }
    auto& sample_info = get_sample_info_( samplename );
    if( sample_info.has_cov ) {
        throw std::runtime_error(
            "Cannot unambiguously parse frequency table header, as it contains multiple columns "
            "for the coverage of sample \"" + samplename + "\"."
        );
    }
    sample_info.has_cov = true;
    assert( sample_info.index < std::numeric_limits<size_t>::max() );
    auto sample_data = sample_data_;
    auto index = sample_info.index;
    column_processors_.push_back( [sample_data, index]( genesis::utils::InputStream& it ){
        assert( index < sample_data->size() );
        sample_data->at(index).cov = utils::parse_unsigned_integer<size_t>( it );
    });
    return 1;
}

// -------------------------------------------------------------------------
//     Sample Helpers
// -------------------------------------------------------------------------

FrequencyTableInputStream::Iterator::SampleInfo&
FrequencyTableInputStream::Iterator::get_sample_info_(
    std::string const& samplename
) {
    // Get the sample type object for a given sample name from the header info.
    // If the name is not yet present, create it and set its index.
    if( header_info_.sample_infos.count( samplename ) == 0 ) {
        // We use the next available index and create a new entry in the map.
        // Has to be two separate lines for this to work, I think.
        auto const next_index = header_info_.sample_infos.size();
        header_info_.sample_infos[samplename].index = next_index;
    }

    // Now return the element, either an existing one, or the one just created.
    // We assert that the element is there, and all indices in order in the vec and map.
    assert( header_info_.sample_infos.count( samplename ) > 0 );
    assert( header_info_.sample_infos[ samplename ].index < std::numeric_limits<size_t>::max() );
    return header_info_.sample_infos[ samplename ];
}

bool FrequencyTableInputStream::Iterator::is_ignored_sample_(
    std::string const& samplename
) const {
    // We only use the filtering if it has been set to an actual list of names.
    assert( parent_ );
    if( parent_->sample_names_filter_.empty() ) {
        return false;
    }

    // Return whether that sample shall be ignored.
    auto const found = ( parent_->sample_names_filter_.count( samplename ) > 0 );
    return !( found ^ parent_->inverse_sample_names_filter_ );
}

// -------------------------------------------------------------------------
//     String Matching Helpers
// -------------------------------------------------------------------------

bool FrequencyTableInputStream::Iterator::match_header_field_(
    std::string const& field,
    std::string const& user_string,
    std::vector<std::string> const& predefined_list
) const {
    // If the user string is not empty, we want to match against it.
    // Only if it is empty, we want to try to match against the predefined list.
    // Otherwise, if the user string is provided but does not match, we would still compare
    // against our predifined strings, hence defying the purpose of a user provided string,
    // as then the user would not have control over what the exact match should be like any more.
    assert( ! field.empty() );
    if( ! user_string.empty() ) {
        return field == user_string;
    }
    return utils::contains_ci_alnum( predefined_list, field );
}

bool FrequencyTableInputStream::Iterator::match_header_sample_(
    std::string const& field,
    std::string const& user_substring,
    std::vector<std::string> const& predefined_list,
    std::string& samplename
) const {
    // First try to find an exact match with the prefix or suffix provided by the user.
    // This needs to be exact in terms of case sensitivity and non-alnum chars.
    // Same as above, if a user string is provided, we only match against that,
    // but then don't continue to match against the predifined lists as well.
    assert( ! field.empty() );
    if( ! user_substring.empty() ) {
        return match_header_sample_user_partial_( field, user_substring, samplename );
    }

    // If that did not work, we try the predefined lists instead:
    // Try to find the field as a prefix or a suffix in any of the elements of the list.
    for( auto const& name : predefined_list ) {
        if( match_header_sample_predefined_partial_( field, name, samplename )) {
            return true;
        }
    }
    return false;
}

bool FrequencyTableInputStream::Iterator::match_header_sample_(
    std::string const& field,
    std::string const& user_substring,
    std::vector<std::string> const& predefined_list1,
    std::vector<std::string> const& predefined_list2,
    std::string& samplename
) const {
    // First try to find an exact match with the prefix or suffix provided by the user.
    // This needs to be exact in terms of case sensitivity and non-alnum chars.
    // Same as above, if a user string is provided, we only match against that,
    // but then don't continue to match against the predifined lists as well.
    assert( ! field.empty() );
    if( ! user_substring.empty() ) {
        return match_header_sample_user_partial_( field, user_substring, samplename );
    }

    // If that did not work, we try the predefined lists instead:
    // Try to find the field as a prefix or a suffix in any of the elements of the combination
    // of both lists, in any order. This is rather cumbersome, but we only do that for the header
    // line, so it's okay.
    // For example, we might get list1 == ref_names_, and list2 == cnt_names_,
    // and then try to find all their pairwise combinations, such as "refcnt" or "countreference"
    // as a prefix or a suffix of the field. If any matches, we take the rest of the field that
    // was not machted as the sample name.
    for( auto const& name1 : predefined_list1 ) {
        for( auto const& name2 : predefined_list2 ) {
            auto name = name1 + name2;
            if( match_header_sample_predefined_partial_( field, name, samplename )) {
                return true;
            }
            name = name2 + name1;
            if( match_header_sample_predefined_partial_( field, name, samplename )) {
                return true;
            }
        }
    }
    return false;
}

bool FrequencyTableInputStream::Iterator::match_header_sample_user_partial_(
    std::string const& field,
    std::string const& substring,
    std::string& samplename
) const {
    // Check for exact prefix or suffix matches, and also require that there needs to be
    // a remainder to be used as sample name, i.e., that not the whole substring matches.
    if( utils::starts_with( field, substring, samplename ) && ! samplename.empty() ) {
        return true;
    }
    if( utils::ends_with( field, substring, samplename ) && ! samplename.empty() ) {
        return true;
    }
    return false;
}

bool FrequencyTableInputStream::Iterator::match_header_sample_predefined_partial_(
    std::string const& field,
    std::string const& substring,
    std::string& samplename
) const {
    // Check for case insensitive, and only alnum char, prefix or suffix matches, and also require
    // that there needs to be a remainder to be used as sample name, i.e., that not the whole
    // substring matches.
    if( utils::starts_with_ci_alnum( field, substring, samplename, true ) && ! samplename.empty() ) {
        return true;
    }
    if( utils::ends_with_ci_alnum( field, substring, samplename, true ) && ! samplename.empty() ) {
        return true;
    }
    return false;
}

// =================================================================================================
//     Increment and Processing Samples
// =================================================================================================

// -------------------------------------------------------------------------
//     increment_
// -------------------------------------------------------------------------

void FrequencyTableInputStream::Iterator::increment_()
{
    using namespace genesis::utils;
    assert( input_stream_ );
    assert( parent_ );
    auto& it = *input_stream_;

    // The previous iteration reached the end. We only set the iterator to end now, so that the
    // previous line is actually being processed by the user.
    if( ! it ) {
        parent_ = nullptr;
        return;
    }

    // Process all columns, using the processor lambda functions one after another
    // in the order that we expect the columns to be in.
    size_t processor_index = 0;
    while( it && *it != '\n' ) {
        if( processor_index >= column_processors_.size() ) {
            throw std::runtime_error(
                "Error while processing frequency table: More columns in line " +
                std::to_string( it.line() ) + " than in the file header."
            );
        }

        // Process the column. That reads stuff into the Variant or into the SampleData,
        // and leaves the stream at the next char after reading, i.e., the separator or new line.
        column_processors_[processor_index]( it );

        // Check that this is actually the case, and we are left where we expect to be.
        if( it && ( *it != '\n' && *it != parent_->separator_char_ )) {
            throw std::runtime_error(
                "Error while processing frequency table: Unexpected char " +
                utils::char_to_hex( *it ) + " at " + it.at()
            );
        }

        // We are at the end of the field or line. Go to the next field, if there is one.
        assert( !it || ( *it == '\n' || *it == parent_->separator_char_ ));
        if( it && *it == parent_->separator_char_ ) {
            ++it;
        }

        // We are done with this column. The next one needs to be processed differently.
        ++processor_index;
    }
    assert( !it || *it == '\n');
    ++it;

    // Fewer columns than were given in the header.
    // If it's not the correct size, it's wrong. We check for too many columns above already,
    // so here, if its off, it's too few; we still check both, just to be sure.
    if( processor_index != column_processors_.size() ) {
        assert( processor_index < column_processors_.size() );
        throw std::runtime_error(
            "Error while processing frequency table: Fewer columns in line " +
            std::to_string( it.line() - 1 ) + " than in the file header."
        );
    }

    // Process the ref and alt bases, with and without a given ref genome.
    if( parent_->ref_genome_ ) {
        // Get the current ref genome base.
        assert( current_variant_->chromosome.size() > 0 );
        assert( current_variant_->position > 0 );
        auto const ref_gen_base = parent_->ref_genome_->get_base(
            current_variant_->chromosome, current_variant_->position
        );

        // Both ref genome and ref column are given and have a usable value. Try to match them.
        if( header_info_.has_ref && current_variant_->reference_base != 'N' ) {
            // Get a shorthand, and check the bases that the processor allows.
            auto const ref_base = current_variant_->reference_base;
            assert( is_valid_base( ref_base ));

            // Both are given and the base from the file is not 'N', so let's see if they agree.
            // If not, that indicates some issue, so better be careful.
            // We allow the ref genome to use ambiguity bases though.
            if( ! sequence::nucleic_acid_code_containment( ref_gen_base, ref_base )) {
                throw std::runtime_error(
                    "At chromosome \"" + current_variant_->chromosome + "\" position " +
                    std::to_string( current_variant_->position ) +
                    ", the provided reference genome has base '" +
                    std::string( 1, ref_gen_base ) +
                    "', while the reference base column in the frequency file is '" +
                    std::string( 1, ref_base ) +
                    "', which is not contained in the referenge genome, " +
                    "and hence likely indicates an issue with the data"
                );
            }
        } else {
            assert( header_info_.has_ref || current_variant_->reference_base == 'N' );

            // Here, we have the case where either there is no ref base from the input file,
            // or it's 'N', so that we might want to replace it by the ref genome. Both cases are
            // treated the same here: Check that we can use the genome base, or use N if not.
            if( is_valid_base( ref_gen_base )) {
                current_variant_->reference_base = ref_gen_base;
            } else {
                current_variant_->reference_base = 'N';
            }
        }
    } else {
        // If we do not have columns for ref and/or alt base, and no reference genome,
        // they should have been left at 'N'.
        // We use short-circuit or here: If it has ref/alt, the second part is not evaluated.
        assert( header_info_.has_ref || current_variant_->reference_base == 'N' );
        assert( header_info_.has_alt || current_variant_->alternative_base == 'N' );
    }

    // Make sure all sizes of the involved data are in sync.
    assert( sample_data_ );
    assert( current_variant_ );
    assert( header_info_.sample_infos.size() == sample_data_->size() );
    assert( header_info_.sample_infos.size() == current_variant_->samples.size() );

    // Now turn all intermediate data into base counts.
    // We go in random order here, following the content of header_info_.sample_infos
    // Not sure if that matters much speed-wise, but it's the easiest to implement for now.
    for( auto const& sample_info : header_info_.sample_infos ) {
        auto const index = sample_info.second.index;
        assert( index < sample_data_->size() );
        assert( index < current_variant_->samples.size() );
        process_sample_data_( sample_info.second, sample_data_->at(index), *current_variant_, index );
    }
}

// -------------------------------------------------------------------------
//     process_sample_data_
// -------------------------------------------------------------------------

void FrequencyTableInputStream::Iterator::process_sample_data_(
    FrequencyTableInputStream::Iterator::SampleInfo const& sample_info,
    FrequencyTableInputStream::Iterator::SampleData const& sample_data,
    Variant& variant,
    size_t sample_index
) {
    // Store the counts that we get first here,
    // and then use one routine to assign them to the sample later.
    BaseCounts::size_type ref_cnt = 0;
    BaseCounts::size_type alt_cnt = 0;
    bool do_frq_check = false;

    if( sample_info.has_ref && sample_info.has_alt ) {

        // Simple case, just use the counts.
        ref_cnt = sample_data.ref_cnt;
        alt_cnt = sample_data.alt_cnt;
        do_frq_check = true;

        // Check that the coverage fits.
        // We are dealing with integers here, so this needs to be exact.
        if( sample_info.has_cov && sample_data.cov != sample_data.ref_cnt + sample_data.alt_cnt ) {
            throw std::runtime_error(
                "Invalid coverage that is not the sum of the reference and alternative base counts."
            );
        }

    } else if( sample_info.has_ref && sample_info.has_cov ) {

        // Already have checked that case above.
        assert( ! sample_info.has_alt );

        // Check that the values are valid.
        if( sample_data.cov < sample_data.ref_cnt ) {
            throw std::runtime_error(
                "Invalid coverage that is smaller than the reference base count."
            );
        }

        // Set them to the respective counters.
        ref_cnt = sample_data.ref_cnt;
        alt_cnt = sample_data.cov - sample_data.ref_cnt;
        do_frq_check = true;

    } else if( sample_info.has_alt && sample_info.has_cov ) {

        // Same as above, but the other way round.
        // Highly unlikely case, but hey, let's implement it.
        assert( ! sample_info.has_ref );
        throw std::runtime_error(
            "Invalid coverage that is smaller than the alternative base count."
        );
        ref_cnt = sample_data.cov - sample_data.alt_cnt;
        alt_cnt = sample_data.ref_cnt;
        do_frq_check = true;

    } else if( sample_info.has_frq ) {

        // We only have at max one count variable.
        assert(
            static_cast<int>( sample_info.has_ref ) +
            static_cast<int>( sample_info.has_alt ) +
            static_cast<int>( sample_info.has_cov )
            <= 1
        );

        // Get the frequency, and check if it is within tolerance,
        // and and process it to be within the unit interval.
        auto frq = sample_data.frq;
        if( frq < 0.0 ) {
            if( ! utils::almost_equal_relative( frq, 0.0, parent_->allowed_rel_freq_error_ )) {
                throw std::runtime_error( "Invalid frequency < 0.0 in frequency table." );
            }
            frq = 0.0;
        }
        if( frq > 1.0 ) {
            if( ! utils::almost_equal_relative( frq, 1.0, parent_->allowed_rel_freq_error_ )) {
                throw std::runtime_error( "Invalid frequency > 1.0 in frequency table." );
            }
            frq = 1.0;
        }
        assert(( !std::isfinite( frq )) ^ ( 0.0 <= frq && frq <= 1.0 ));

        // Go through different ways of computing counts from the frequency.
        // We can only ever have one of the counts be set, as otherwise, one of the previous
        // ways of computing the counts (see above) would already have kicked in, and we
        // would not have ended up here.
        if( ! std::isfinite( frq )) {
            // Nothing to do, we keep the counts at 0.
        } else if( sample_info.has_cov ) {
            // Avoid rounding errors by doing the second number directly on integers.
            ref_cnt = static_cast<BaseCounts::size_type>( sample_data.cov * frq );
            alt_cnt = sample_data.cov - ref_cnt;
        } else if( sample_info.has_ref ) {
            // We have a frequency f, and a ref count r.
            // Resolve f = r/(r+a) accordingly to get the alt count a.
            // Here, we need to introduce some rounding. No way around this.
            // Let's hope for the best - in particular, that all numbers behave nicely.
            ref_cnt = sample_data.ref_cnt;
            auto const ref_dbl = static_cast<double>( ref_cnt );
            alt_cnt = static_cast<BaseCounts::size_type>(( ref_dbl / frq ) - ref_dbl );
        } else if( sample_info.has_alt ) {
            // Same idea as above, but the other way round.
            alt_cnt = sample_data.alt_cnt;
            auto const alt_dbl = static_cast<double>( alt_cnt );
            ref_cnt = static_cast<BaseCounts::size_type>( alt_dbl / (( 1.0 / frq ) - 1.0 ));
        } else {
            // If no count is given at all, we use a different strategy instead.
            // Multiply by our large number, to get an int that can be prepresented in double.
            auto const int_fac = parent_->int_factor_;
            ref_cnt = static_cast<BaseCounts::size_type>( int_fac * frq );
            alt_cnt = static_cast<BaseCounts::size_type>( int_fac ) - ref_cnt;

            // Also, make sure that the large number actually fits into the size type.
            // We check that when setting the value, but let's be safe.
            assert(
                static_cast<double>( static_cast<BaseCounts::size_type>( int_fac )) == int_fac
            );
        }

        // Lastly, assign to where the counts belong.
        // If the freq does not correspond to the ref but to the alt, we need to flip them.
        if( ! parent_->frequency_is_ref_ ) {
            std::swap( ref_cnt, alt_cnt );
        }

    } else {
        // This case should not happen, as we would have thrown an exception when parsing
        // the header already, as we check there that we have at least one of the above cases.
        throw std::domain_error( "Internal error: No valid data type to parse frequency table." );
    }

    // Now that we have processed the data, we can use the final counts to check the frequency.
    if( do_frq_check && sample_info.has_frq ) {
        auto const ref = static_cast<double>( ref_cnt );
        auto const alt = static_cast<double>( alt_cnt );
        auto const frq = ( parent_->frequency_is_ref_ ? ref : alt ) / ( ref + alt );
        if( ! utils::almost_equal_relative( frq, sample_data.frq, parent_->allowed_rel_freq_error_ )) {
            throw std::runtime_error(
                "Mismatching frequency value ~" + std::to_string( sample_data.frq ) +
                " that has a difference greater than the allowed relative error (" +
                std::to_string( parent_->allowed_rel_freq_error_ ) + ") to the frequency " +
                std::to_string( frq ) + " determined by the reference count " +
                std::to_string( ref_cnt ) + " and alternative count " + std::to_string( alt_cnt )
            );
        }
    }

    // Now store the counts in the sample, using the ref/alt base info if available,
    // or fixed bases if ref and/or alt are not available.
    char ref_base = variant.reference_base;
    char alt_base = variant.alternative_base;
    assert( is_valid_base_or_n( ref_base ));
    assert( is_valid_base_or_n( alt_base ));
    if( utils::char_match_ci( ref_base, 'N' )) {
        ref_base = 'A';
        alt_base = 'G';
    } else if( utils::char_match_ci( alt_base, 'N' )) {
        // Only ref base is given. Use its transition base as the most likely alternative.
        assert( is_valid_base( ref_base ));
        alt_base = ::genesis::sequence::nucleic_acid_transition( ref_base );
    }
    assert( sample_index < variant.samples.size() );
    assert( ref_base != 'N' && ref_base != 'n' );
    assert( alt_base != 'N' && alt_base != 'n' );
    if( ref_base == alt_base ) {
        throw std::runtime_error(
            "At chromosome \"" + variant.chromosome + "\" position " +
            std::to_string( variant.position ) +
            ": Invalid reference and alternative base that are both '" +
            std::string( 1, ref_base ) + "' in frequency table."
        );
    }

    // Set the base counts
    variant.samples[sample_index].clear();
    set_base_count( variant.samples[sample_index], ref_base, ref_cnt );
    set_base_count( variant.samples[sample_index], alt_base, alt_cnt );
}

} // namespace population
} // namespace genesis
