#ifndef GENESIS_POPULATION_FORMAT_VCF_RECORD_H_
#define GENESIS_POPULATION_FORMAT_VCF_RECORD_H_

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

#ifdef GENESIS_HTSLIB

#include "genesis/population/format/vcf_common.hpp"
#include "genesis/population/format/vcf_format_iterator.hpp"
#include "genesis/population/format/vcf_header.hpp"
#include "genesis/utils/containers/range.hpp"

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
    // #include <htslib/hts.h>
    // #include <htslib/vcf.h>

    struct bcf1_t;
}

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace genesis {
namespace population {

class HtsFile;
class VcfHeader;

// =================================================================================================
//     VCF/BCF Record
// =================================================================================================

/**
 * @brief Capture the information of a single SNP/variant line in a VCF/BCF file.
 *
 * The basic usage to iterate the records/lines of a VCF/BCF file is:
 *
 *     // Prepare and read all input data and instanticate our classes.
 *     auto file = HtsFile( infile );
 *     auto header = VcfHeader( file );
 *     auto record = VcfRecord( header );
 *
 *     // Iterate the file line by line.
 *     while( record.read_next( file )) {
 *         // Work with the record by calling record.*() functions.
 *     }
 *
 * Within the loop, the respective values of the record can be accessed, for example the simple
 * first columns via get_chromosome(), get_position(), etc.
 * For the reference and alternative alleles, as well as their combination (which we here call the
 * "variants"), we offer access functions. Filters can be tested via has_filter(), or their absence
 * (that is: `PASS`) via has_pass().
 *
 * Testing whether certain INFO fileds are set can be done via has_info() or assert_info().
 * Accessing the values of the INFO column is a bit more involved, as one needs to know the data
 * type in advance, and call the respective `get_info_*()` for the data type (string, float, int,
 * flag). This is a bit unfortunate, and we here simply mimic the behaviour of htslib.
 * A more complex `any` wrapper migth be possible, but as one would need to cast in the end anyway,
 * we leave it at that solution for now.
 *
 * The most involved part is the per-sample access to the values as indicated by the FORMAT column.
 * Testing that a particular FORMAT key/id/field is present in the record can be done via has_format()
 * and assert_format(). Accessing the values per sample is then done with an iterator that has
 * to be obtained for each FORMAT id separately via the `begin_format_*()` / `end_format_*()`
 * function pair, or for range-based for loops via the `get_format_*()` function.
 * See there, and in particular, see the VcfFormatIterator class documentation for details
 * on usage.
 *
 * Note that VCF for some reason does not seem to support flags in the FORMAT/sample data (the VCF
 * 4.2 Specification does not state this explicitly, but seems to implicitly assume it; they
 * probably do not allow flags because that would lead to variable length per-sample columns).
 * Hence, we also cannot support per-sample flags.
 */
class VcfRecord
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Types of variants of alleles that can occur in a record.
     *
     * Corresponds to the `VCF_*` macro constants defined by htslib. We statically assert
     * that these have the same values.
     */
    enum class VariantType : int {
        kRef      =  0,
        kSnp      =  1,
        kMnp      =  2,
        kIndel    =  4,
        kOther    =  8,
        kBreakend = 16, // Breakend
        kOverlap  = 32, // Overlapping deletion, ALT=*
    };

    /**
     * @brief And-operator for VariantType%s.
     *
     * The function get_variant_types() returns the or'ed (union) values of all variant types that
     * appear in the alternative alleles of the record. Hence, this and-operator can be used
     * to disentangle them and test whether a particular variant occurs in the record:
     *
     *     if( record.get_variant_types() & Record::VariantType::kSnp ) {
     *         ...
     *     }
     *
     * See get_variant_types() for details.
     *
     * It's a bit ugly to return a bool from such a comparison, but for now, it works.
     * Should more complex use cases arise in the future, this might change.
     */
    friend bool operator& ( VcfRecord::VariantType a, VcfRecord::VariantType b )
    {
        return ( static_cast<int>(a) & static_cast<int>(b) );
    }

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default (empty) instance.
     */
    VcfRecord();

    /**
     * @brief Create an instance based on a VCF/BCF header.
     *
     * This is the most common use case, where we create a record instance for a given VCF/BCF file,
     * using its header information for access to details laters.
     */
    explicit VcfRecord( VcfHeader& header );

    /**
     * @brief Create an instance by copy.
     *
     * This calls <code>\::bcf_dup()</code> from htslib to create a copy of the given record.
     * The provided @p bcf1 record hence has to be freed elsewhere.
     */
    VcfRecord( VcfHeader& header, ::bcf1_t* bcf1 );

    ~VcfRecord();

    VcfRecord( VcfRecord const& ) = delete;
    VcfRecord( VcfRecord&& );

    VcfRecord& operator= ( VcfRecord const& ) = delete;
    VcfRecord& operator= ( VcfRecord&& );

    void swap( VcfRecord& other );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the internal htslib `bcf1_t` record data struct pointer.
     */
    ::bcf1_t* data()
    {
        return record_;
    }

    /**
     * @brief Return the internal htslib `bcf1_t` record data struct pointer.
     */
    ::bcf1_t const* data() const
    {
        return record_;
    }

    /**
     * @brief Return the VcfHeader instance associated with this record.
     *
     * This can also be used to obtain internal htslib `bcf_hdr_t` header data struct pointer,
     * if needed for any functionality that is not offered by our wrapper.
     */
    VcfHeader& header()
    {
        return *header_;
    }

    /**
     * @brief Return the VcfHeader instance associated with this record.
     *
     * This can also be used to obtain internal htslib `bcf_hdr_t` header data struct pointer,
     * if needed for any functionality that is not offered by our wrapper.
     */
    VcfHeader const& header() const
    {
        return *header_;
    }

    /**
     * @brief Unpack the htslib `bcf1_t` record data.
     *
     * Some external functions might want to work with the `bcf1_t` record data struct pointer
     * directly, but need to unpack the data first. This is not possible when the VcfRecord is
     * passed by const reference for example, so we offer this function that unpacks a const
     * VcfRecord, so that its internal htslib data is available.
     */
    void unpack() const;

    // -------------------------------------------------------------------------
    //     Simple Fixed Columns
    // -------------------------------------------------------------------------

    /**
     * @brief Get the name of a chromosome/contig/sequence (`CHROM`, first column of the line).
     */
    std::string get_chromosome() const;

    /**
     * @brief Get the position within the chromosome/contig (`POS`, second column of the line).
     *
     * We report the position as given in the VCF/BCF file, that is, 1-based!
     */
    size_t get_position() const;

    /**
     * @brief Get the ID string of the variant (`ID`, third column of the line).
     *
     * Another instance where the overloaded term "ID" is used in VCF and in htslib. Here,
     * it stands for the identifier list of the line for SNP databases.
     */
    std::string get_id() const;

    /**
     * @brief Return a textual representation of the current record chromosome position.
     *
     * This is either `CHROM:POS` or `CHROM:POS (ID)`, depending on whether the ID of the record
     * is set (that is, not equal to `'.'`).
     * See get_chromosome(), get_position(), and get_id() for details on the individual parts.
     */
    std::string at() const;

    /**
     * @brief Get the reference allele/sequence of the variant (`REF`, fourth column of the line).
     */
    std::string get_reference() const;

    /**
     * @brief Get the alternative alleles/sequences of the variant (`ALT`, fifth column of the line).
     */
    std::vector<std::string> get_alternatives() const;

    /**
     * @brief Get a particular alternative allele (`ALT`, fifth column of the line).
     *
     * This is equivalent to calling get_alternatives() to obtain a particular alternative at an
     * @p index, but faster if only a particular entry is needed or when iterating them.
     * Valid indices are in the range `[ 0, get_alternatives_count() )`.
     */
    std::string get_alternative( size_t index ) const;

    /**
     * @brief Get the number of alternative alleles/sequences of the variant
     * (`ALT`, fifth column of the line).
     *
     * This simply gives their count, which is identical to `get_alternatives().size()`.
     * See also get_variant_count(), which gives the number of total variants (reference and
     * alternative alleles), and which hence is `get_alternatives_count() + 1`. Although these
     * functions only differ in the extra counting of the reference allele, we offer both
     * so that using them reflects the intention (are we interested in just the alternative,
     * or all variants?).
     */
    size_t get_alternatives_count() const;

    /**
     * @brief Shortcut to get both the reference (`REF`, fourth column of the line) and the alternative
     * (`ALT`, fifth column of the line) alleles/sequences of the line.
     *
     * This simply combines get_reference() and get_alternatives(). Note that hence the indices
     * of the alternatives are shifted as compared to get_alternatives(), as the reference allele
     * is at index `0`.
     */
    std::vector<std::string> get_variants() const;

    /**
     * @brief Get a particular variant (`REF` or `ALT` allele).
     *
     * This is equivalent to calling get_variants() to obtain a particular alternative at an
     * @p index, but faster if only a particular entry is needed or when iterating them.
     * Valid indices are in the range `[ 0, get_variant_count() )`.
     *
     * Note that compared to get_alternative() and get_alternatives(), the indices of the alternative
     * alleles are shifted, as the reference allele is at index `0`.
     */
    std::string get_variant( size_t index ) const;

    /**
     * @brief Get the total number of variants (`REF` and `ALT` alleles) in the record/line.
     *
     * This is equivalent to `get_variants().size()`, and to `get_alternatives_count() + 1`
     * (taking the `REF` into account).
     */
    size_t get_variant_count() const;

    /**
     * @brief Get the or'ed (union) value of all variant types of the alternative alleles/sequences
     * of the record.
     *
     * This can be used to simply test whether a particular type of variant appears at all in
     * a given record:
     *
     *     if( record.get_variant_types() & VcfRecord::VariantType::kSnp ) {
     *         ...
     *     }
     *
     * See operator& ( VariantType a, VariantType b ) for details.
     *
     * This is a simple wrapper for <code>\::bcf_get_variant_types()</code> from htslib,
     * which however is not ideally named, as "variants" seems to mean REF+ALT in VCF terminology,
     * but the flag for REF has value 0 and hence is (in a sense) always set in the result.
     * Of course, this makes sense, as we always have a reference variant. But technically,
     * we cannot test for this, so this function only is useful for alternative alleles,
     * and not all variants. Still, we follow their terminology here.
     */
    VariantType get_variant_types() const;

    /**
     * @brief Get the variant type of a particular alternative allele/sequence.
     *
     * The @p alt_index is the 0-based index of the alternative allele/sequence for which we want
     * the variant type. This corresponds to the indices in the get_alternatives() result vector,
     * which are hence in the range `[ 0, get_alternatives_count() )`.
     */
    VariantType get_variant_type( size_t alt_index ) const;

    /**
     * @brief Return whether this variant is a SNP.
     *
     * This is simply a wrapper for the htslib function <code>\::bcf_is_snp()</code>.
     * It returns `true` iff the reference and all alternative alleles/sequence are single
     * characters (and none of them is a `'*'` missing allele character).
     */
    bool is_snp() const;

    /**
     * @brief Return whether this variant is a SNP, or a deletion in the alternative.
     *
     * This is similar to is_snp(), but also allows the alternatives to have a deletion.
     */
    bool is_snp_or_alt_del() const;

    /**
     * @brief Get the quality score (`QUAL`, sixth column of the line).
     */
    double get_quality() const;

    // -------------------------------------------------------------------------
    //     Filter Column
    // -------------------------------------------------------------------------

    /**
     * @brief Get the list of all filter values (`PASS` or the names of the non-passing filters)
     * that are applied to the record.
     *
     * For example, the lines
     *
     *     #CHROM POS      ID         REF   ALT    QUAL  FILTER  [...]
     *     20     14370    rs6054257  G     A      29    PASS    [...]
     *     20     17330    .          T     A      3     q10     [...]
     *
     * would return `{"PASS"}` and `{"q10"}`, respectively.
     */
    std::vector<std::string> get_filter_ids() const;

    /**
     * @brief Return whether the record has a given @p filter set.
     *
     * For example, provided with `filter == "q10"`, the function returns whether the `q10` filter
     * is set for the record (indicating that the record failed that filter test).
     */
    bool has_filter( std::string const& filter ) const;

    /**
     * @brief Return whether the record passes the filters, that is, whether `PASS` is set,
     * or no filters were applied yet (`FILTER == '.'`).
     *
     * This is identical to calling has_filter() with the argument `"PASS"`, and is the preferred
     * method to test whether a record line has passed all filters (return value `true`), or not
     * (return value `false`).
     */
    bool pass_filter() const;

    // -------------------------------------------------------------------------
    //     Info Column
    // -------------------------------------------------------------------------

    /**
     * @brief Get the list of all info IDs (`INFO` column) that the record contains.
     *
     * For example, the line
     *
     *     #CHROM POS      ID         REF   ALT    QUAL  FILTER  INFO                     [...]
     *     20     14370    rs6054257  G     A      29    PASS    NS=3;DP=14;AF=0.5;DB;H2  [...]
     *
     * would return a list containing `{"NS", "DP", "AF", "DB", "H2"}`.
     */
    std::vector<std::string> get_info_ids() const;

    /**
     * @brief Return whether the record has a given INFO @p id present.
     */
    bool has_info( std::string const& id ) const;

    /**
     * @copydoc genesis::population::VcfRecord::has_info( std::string const& ) const
     */
    bool has_info( char const* id ) const;

    /**
     * @brief Assert that an INFO entry with a given @p id is present in the record.
     *
     * This is the same as has_info(), but throws an exception in case that the INFO ID is not
     * present.
     */
    void assert_info( std::string const& id ) const;

    /**
     * @copydoc genesis::population::VcfRecord::assert_info( std::string const& ) const
     */
    void assert_info( char const* ) const;

    /**
     * @brief Return the info value for the given key @p id as a string.
     *
     * The function throws an exception if the requested @p id is not present (in the header or
     * the record) or if the value behind that @p id is of a different type.
     */
    std::string get_info_string( std::string const& id ) const;

    /**
     * @brief Write the info value for the given key @p id to a given @p destination string.
     *
     * If the @p destination string is re-used between calles for different records,
     * this is the faster variant that saves on memory allocations.
     *
     * @copydetails genesis::population::VcfRecord::get_info_string( std::string const& ) const
     */
    void get_info_string( std::string const& id, std::string& destination ) const;

    /**
     * @brief Return the info value for the given key @p id as a vector of float/double.
     *
     * While htslib uses `float`, we use `double` throughout genesis, and hence return this here.
     *
     * @copydetails genesis::population::VcfRecord::get_info_string( std::string const& ) const
     */
    std::vector<double> get_info_float( std::string const& id ) const;

    /**
     * @brief Write the info value for the given key @p id to a given
     * @p destination vector of float/double.
     *
     * If the @p destination vector is re-used between calles for different records,
     * this is the faster variant that saves on memory allocations.
     *
     * @copydetails genesis::population::VcfRecord::get_info_float( std::string const& ) const
     */
    void get_info_float( std::string const& id, std::vector<double>& destination ) const;

    /**
     * @brief Return the info value for the given key @p id as a vector of int.
     *
     * @copydetails genesis::population::VcfRecord::get_info_string( std::string const& ) const
     */
    std::vector<int32_t> get_info_int( std::string const& id ) const;

    /**
     * @brief Write the info value for the given key @p id to a given @p destination vector of int.
     *
     * If the @p destination vector is re-used between calles for different records,
     * this is the faster variant that saves on memory allocations.
     *
     * @copydetails genesis::population::VcfRecord::get_info_string( std::string const& ) const
     */
    void get_info_int( std::string const& id, std::vector<int32_t>& destination ) const;

    /**
     * @brief Return whehter an INFO flag is set, that is, whether the info value for a given
     * key @p id is present in the INFO fields.
     *
     * This is meant for flags and returns whether the flag has been set or not in the record.
     */
    bool get_info_flag( std::string const& id ) const;

    // -------------------------------------------------------------------------
    //     Format Column
    // -------------------------------------------------------------------------

    /**
     * @brief Get the list of all format IDs (`FORMAT` column) that the record contains.
     *
     * For example, the line
     *
     *     #CHROM POS      ID        REF ALT  QUAL FILTER INFO       FORMAT      [...]
     *     20     14370    rs6054257 G   A,CG 29   PASS   NS=3;DP=14 GT:GQ:DP:HQ
     *
     * would return a list containing `{"GT", "GQ", "DP", "HQ"}`.
     */
    std::vector<std::string> get_format_ids() const;

    /**
     * @brief Return whether the record has a given FORMAT @p id present.
     */
    bool has_format( std::string const& id ) const;

    /**
     * @copydoc genesis::population::VcfRecord::has_format( std::string const& ) const
     */
    bool has_format( char const* id ) const;

    /**
     * @brief Assert that an FORMAT entry with a given @p id is present in the record.
     *
     * This is the same as has_format(), but throws an exception in case that the FORMAT ID is not
     * present.
     */
    void assert_format( std::string const& id ) const;

    /**
     * @copydoc genesis::population::VcfRecord::assert_format( std::string const& ) const
     */
    void assert_format( char const* id ) const;

    // -------------------------------------------------------------------------
    //     Sample Columns
    // -------------------------------------------------------------------------

    /**
     * @brief Get the begin iterator over the samples that accesses the FORMAT genotype
     * (`GT` field/key/id) as a set of VcfGenotype values.
     *
     * The `GT` genotype field is a special case of htslib, that we hence also have to treat
     * specially here. Instead of strings or ints, we model each call as a VcfGenotype instance;
     * see there for details.
     *
     * The simplest use case is via the get_format_genotype() function; see there for some
     * convenience examples as well.
     *
     * @copydetails begin_format_string()
     */
    VcfFormatIteratorGenotype begin_format_genotype() const;

    /**
     * @brief Get the end iterator over the samples that accesses the FORMAT genotype
     * (`GT` field/key/id) as a set of VcfGenotype values.
     *
     * See begin_format_genotype() for details.
     */
    VcfFormatIteratorGenotype end_format_genotype() const;

    /**
     * @brief Get an iterator pair over the samples that accesses the FORMAT genotype
     * (`GT` field/key/id) as a set of VcfGenotype values.
     *
     * See begin_format_genotype() for why we offer a special function to get the genotype field.
     *
     * In order to simply print the VCF-like genotype information for the samples of a record,
     * the vcf_genotype_string() convenience function can be used:
     *
     *     for( auto gt_iterator : record_iterator->get_format_genotype() ) {
     *         std::cout << vcf_genotype_string( gt_iterator.get_values() ) << "\n";
     *     }
     *
     * @copydetails get_format_string()
     */
    genesis::utils::Range<VcfFormatIteratorGenotype> get_format_genotype() const;

    /**
     * @brief Get the begin iterator over the samples that accesses a certain FORMAT @p id as a
     * string value.
     *
     * This can be used in a for loop over the samples, using the `end_format_*()` iterator
     * for the end condition.
     * See the VcfRecord and the VcfFormatIterator class descriptions for details on usage.
     * Also, see `get_format_*()` for a combined range iterator pair to be used in range-based for
     * loops, and see the other `begin_format_*()`, `end_format_*()`, and `get_format_*()`
     * functions for the equivalents for other FORMAT data types.
     */
    VcfFormatIteratorString begin_format_string( std::string const& id ) const;

    /**
     * @brief Get the end iterator over the samples that accesses a certain FORMAT @p id as a
     * string value.
     *
     * See begin_format_string() for details.
     */
    VcfFormatIteratorString end_format_string() const;

    /**
     * @brief Get an iterator pair over the samples that accesses a certain FORMAT @p id as a
     * string value.
     *
     * This can directly be used in a range-based for loop over the samples.
     * See the VcfRecord and the VcfFormatIterator class descriptions for details on usage.
     * Also, see `begin_format_*()` and `end_format_*()` for the respective begin and end iterators,
     * and see the other `begin_format_*()`, `end_format_*()`, and `get_format_*()`
     * functions for the equivalents for other FORMAT data types.
     */
    genesis::utils::Range<VcfFormatIteratorString> get_format_string( std::string const& id ) const;

    /**
     * @brief Get the begin iterator over the samples that accesses a certain FORMAT @p id as an
     * int value.
     *
     * @copydetails begin_format_string()
     */
    VcfFormatIteratorInt begin_format_int( std::string const& id ) const;

    /**
     * @brief Get the end iterator over the samples that accesses a certain FORMAT @p id as an
     * int value.
     *
     * See begin_format_int() for details.
     */
    VcfFormatIteratorInt end_format_int() const;

    /**
     * @brief Get an iterator pair over the samples that accesses a certain FORMAT @p id as an
     * int value.
     *
     * @copydetails get_format_string()
     */
    genesis::utils::Range<VcfFormatIteratorInt> get_format_int( std::string const& id ) const;

    /**
     * @brief Get the begin iterator over the samples that accesses a certain FORMAT @p id as a
     * float value.
     *
     * @copydetails begin_format_string()
     */
    VcfFormatIteratorFloat begin_format_float( std::string const& id ) const;

    /**
     * @brief Get the end iterator over the samples that accesses a certain FORMAT @p id as a
     * float value.
     *
     * See begin_format_float() for details.
     */
    VcfFormatIteratorFloat end_format_float() const;

    /**
     * @brief Get an iterator pair over the samples that accesses a certain FORMAT @p id as an
     * float value.
     *
     * @copydetails get_format_string()
     */
    genesis::utils::Range<VcfFormatIteratorFloat> get_format_float( std::string const& id ) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Read the next record/line from the given @p source, and replace the content
     * of this VcfRecord instance.
     *
     * This is the function used to iterate a VCF/BCF file. It returns `true` on success, and
     * can hence be used as follows:
     *
     *     while( record.read_next( file )) {
     *         // work with the current record line
     *     }
     *
     * Within that loop, all functions of the record instance can be used.
     */
    bool read_next( HtsFile& source );

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Local helper function that does the repetetive part of the work of loading the info
     * data from a record.
     */
    int get_info_ptr_( std::string const& id, int ht_type, void** dest, int* ndest ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Here, we only manage the record_ pointer instance. The header takes care of itself,
    // and is only pointed to from here, but not managed.
    VcfHeader* header_ = nullptr;
    mutable ::bcf1_t* record_ = nullptr;

    // htslib wants to copy values all the time, so we reserve buffers to avoid reallocations.
    mutable char*    info_dest_string_  = nullptr;
    mutable float*   info_dest_float_   = nullptr;
    mutable int32_t* info_dest_int_     = nullptr;
    mutable int      info_ndest_string_ = 0;
    mutable int      info_ndest_float_  = 0;
    mutable int      info_ndest_int_    = 0;
};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
