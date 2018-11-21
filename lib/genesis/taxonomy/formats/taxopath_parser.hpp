#ifndef GENESIS_TAXONOMY_FORMATS_TAXOPATH_PARSER_H_
#define GENESIS_TAXONOMY_FORMATS_TAXOPATH_PARSER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include <string>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Taxon;
class Taxopath;

// =================================================================================================
//     Taxopath Parser
// =================================================================================================

/**
 * @brief Helper class to parse a string containing a taxonomic path string into a Taxopath object.
 *
 * This class bundles the parameters used for parsing a taxonomic path strings and offers functions
 * for the actual parsing. This is needed in order to allow customization of the parsing process,
 * for example in TaxonomyReader. Furthermore, this prevents code duplication in places where the
 * input is a taxonomic path string.
 * The result of the parsing process is a Taxopath object. See there for details.
 *
 * The elements are expected to be char separated, using the value of
 * @link delimiters( std::string const& value ) delimiters()@endlink to separate them.
 * Default is ';'.
 *
 * For example: The input string
 *
 *     Tax_1; Tax_2 ;;Tax_4;
 *
 * is parsed into the Taxopath
 *
 *     [ "Tax_1", "Tax_2", "Tax_2", "Tax_4" ]
 *
 * That is, missing elements are filled up with the preceeding ones - this is a common technique
 * in taxonomic databases, which is useful for unspecified taxa in deeper taxonomies.
 *
 * Furthermore, if the string ends with the delimiter char, this is removed by default. See above
 * for an example of this;
 * see @link remove_trailing_delimiter( bool value ) remove_trailing_delimiter()@endlink
 * to change that behaviour and instead keep this last element.
 * Also, the first taxon in the string cannot be empty. Otherwise an `std::runtime_error` is thrown.
 */
class TaxopathParser
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    TaxopathParser()  = default;
    ~TaxopathParser() = default;

    TaxopathParser( TaxopathParser const& ) = default;
    TaxopathParser( TaxopathParser&& )      = default;

    TaxopathParser& operator= ( TaxopathParser const& ) = default;
    TaxopathParser& operator= ( TaxopathParser&& )      = default;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    /**
     * @brief Parse a taxonomic path string into a Taxopath object and return it.
     *
     * See @link TaxopathParser the class description@endlink for details on what this parser does.
     */
    Taxopath parse( std::string const& taxopath ) const;

    /**
     * @brief Helper function to turn a Taxon into a Taxopath.
     *
     * This function is probably not need often, as the Taxopath is a helper object from
     * a taxonomic path string towards a Taxon object, but not the other way round.
     * In order to get the string from a Taxon, see the TaxopathGenerator class instead.
     *
     * However, this function might still be useful in some cases. You never know.
     */
    Taxopath parse(  Taxon const& taxon ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Set the chars used to split the taxonomic path string.
     *
     * Those chars are used to split the taxon name into its hierarchical parts.
     * Default is ';', as this is the usual value in many databases. See Taxopath for details.
     *
     * If this value is set to multiple chars (string longer than 1), any of them is used for splitting.
     *
     * Example: The taxonomic path string
     *
     *     Archaea;Euryarchaeota;Halobacteria;
     *
     * is split into "Archaea", "Euryarchaeota" and "Halobacteria".
     */
    TaxopathParser& delimiters( std::string const& value )
    {
        delimiters_ = value;
        return *this;
    }

    /**
     * @brief Return the currelty set delimiter chars used to split the taxonomic path string.
     *
     * See @link delimiters( std::string const& value ) the setter@endlink for details.
     */
    std::string     delimiters() const
    {
        return delimiters_;
    }

    /**
     * @brief Set whether to trim whitespaces around the taxonomic elements after splitting them.
     *
     * Default is `true`. If set to true, the taxa given are trimmed off white spaces after splitting
     * them. This is helpful if the input string is copied from some spreadsheet application or
     * CSV file, where spaces between cells might be added.
     *
     * If set to `false`, all elements are left as they are.
     *
     * Example: The line
     *
     *     Archaea; Aigarchaeota; Aigarchaeota Incertae Sedis;	11091	class	123
     *
     * contains spaces both between the taxa names (separated by `;`), as well as within the names.
     * Only the former ones will be trimmed, while latter ones are left as they are.
     */
    TaxopathParser& trim_whitespaces( bool value )
    {
        trim_whitespaces_ = value;
        return *this;
    }

    /**
     * @brief Return the currently set value whether whitespaces are trimmed off the taxonomic elements.
     *
     * See @link trim_whitespaces( bool value ) the setter@endlink for details.
     */
    bool            trim_whitespaces() const
    {
        return trim_whitespaces_;
    }

    /**
     * @brief Set whether to remove an empty taxonomic element at the end, if it occurs.
     *
     * In many taxonomic databases, the taxonomic string representation end with a ';' by default.
     * When splitting such a string, this results in an empty last element. If this option is set to
     * `true` (default), this element is removed from the Taxopath.
     *
     * If set to `false`, the element is not removed, but instead treated as a normal "empty" element,
     * which means, it is replaced by the value of the preceeding element. See
     * @link TaxopathParser the class description@endlink for details on that.
     */
    TaxopathParser& remove_trailing_delimiter( bool value )
    {
        remove_trailing_delimiter_ = value;
        return *this;
    }

    /**
     * @brief Return whether currently trailing delimiters are removed from the taxonomic path
     * string.
     *
     * See @link remove_trailing_delimiter( bool value ) the setter@endlink for details.
     */
    bool            remove_trailing_delimiter() const
    {
        return remove_trailing_delimiter_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string delimiters_                = ";";
    bool        trim_whitespaces_          = true;
    bool        remove_trailing_delimiter_ = true;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
