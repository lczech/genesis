#ifndef GENESIS_TAXONOMY_FORMATS_TAXOPATH_GENERATOR_H_
#define GENESIS_TAXONOMY_FORMATS_TAXOPATH_GENERATOR_H_

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
//     Taxopath Generator
// =================================================================================================

/**
 * @brief Helper class to generate a taxonomic path string from a Taxopath object
 * or a Taxon.
 *
 * This class bundles the parameters used for generating a taxonomic path string.
 * This is needed in order to allow customization of the generation process.
 * The result of the process is a string representation of the taxonomic path, with all
 * names of the super-taxa of the given taxon (and the taxon itself), concatenated using the
 * @link delimiter( std::string const& value ) delimiter()@endlink.
 *
 * For example, the Taxon
 *
 *     Tax_1
 *         Tax_2
 *             Tax_2
 *                 Tax_4
 *
 * might result in a string representation
 *
 *     Tax_1;Tax_2;;Tax_4;
 *
 * The same result is also obtained from a Taxopath `[ "Tax_1", "Tax_2", "Tax_2", "Tax_4" ]`.
 */
class TaxopathGenerator
{
public:

    enum class TaxonField
    {
        kName,
        kRank,
        kId
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    TaxopathGenerator()  = default;
    ~TaxopathGenerator() = default;

    TaxopathGenerator( TaxopathGenerator const& ) = default;
    TaxopathGenerator( TaxopathGenerator&& )      = default;

    TaxopathGenerator& operator= ( TaxopathGenerator const& ) = default;
    TaxopathGenerator& operator= ( TaxopathGenerator&& )      = default;

    // -------------------------------------------------------------------------
    //     Generating
    // -------------------------------------------------------------------------

    /**
     * @brief Return a string representation of a Taxopath.
     *
     * This generator function uses the settings of this class to generate the string.
     */
    std::string to_string(   Taxopath const& taxopath ) const;

    /**
     * @brief Shortcut function alias for
     * @link to_string( Taxopath const& taxopath ) const to_string( Taxopath )@endlink.
     *
     * This shortcut enables to use a TaxopathGenerator object as functor.
     */
    std::string operator() ( Taxopath const& taxopath ) const;

    /**
     * @brief Return a string representation of a Taxon.
     *
     * This generator function uses the settings of this class to generate the string.
     */
    std::string to_string(   Taxon const& taxon ) const;

    /**
     * @brief Shortcut function alias for
     * @link to_string( Taxon const& taxon ) const to_string( Taxon )@endlink.
     *
     * This shortcut enables to use a TaxopathGenerator object as functor.
     */
    std::string operator() ( Taxon const& taxon ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Set the string field to use for Taxon input.
     *
     * The Taxon class stores three strings that describe a taxon: its name, its rank, and an ID.
     * Using this setting, one can chose which of the fields to use when generating a taxopath.
     */
    TaxopathGenerator& field( TaxonField value );

    /**
     * @brief Return the currelty set field to use for describing a Taxon.
     *
     * See @link field( TaxonField ) the setter@endlink for details.
     */
    TaxonField         field() const;

    /**
     * @brief Set the string used to join the taxonomic path string elements.
     *
     * This value is used in between the elements of the taxonomic path string.
     * Default is ';', as this is the usual value in many databases. See Taxopath for details.
     */
    TaxopathGenerator& delimiter( std::string const& value );

    /**
     * @brief Return the currelty set value used to join the taxonomic path string elements.
     *
     * See @link delimiter( std::string const& value ) the setter@endlink for details.
     */
    std::string        delimiter() const;

    /**
     * @brief Set whether lower level taxa are omitted if they are the same as the next higher level
     * one.
     *
     * If set to `true`, lower level names are set to empty if they are the same as higher level names.
     * Default is `false`, that is, nothing is trimmed.
     *
     * Example: For a Taxopath
     *
     *     [ "Tax_1", "Tax_1", "Tax_2" ]
     *
     * @link to_string( Taxopath const& taxopath ) const the generator function@endlink returns
     * `Tax_1;Tax_1;Tax_2`, and respectively `Tax_1;;Tax_2` with trimming nested duplicates.
     */
    TaxopathGenerator& trim_nested_duplicates( bool value );

    /**
     * @brief Return the currently set value whether to trim nested duplicates of taxa names.
     *
     * See @link trim_nested_duplicates( bool value ) the setter@endlink for details.
     */
    bool               trim_nested_duplicates() const;

    /**
     * @brief Set whether to append the delimiter string to the generated string.
     *
     * In many taxonomic databases, the string representation ends with the delimiter char, for example
     *
     *     Animalia;Vertebrata;Mammalia;Carnivora;
     *
     * This setting determines whether this last delimiter is appended or not.
     */
    TaxopathGenerator& append_delimiter( bool value );

    /**
     * @brief Return whether currently a delimiter is appended to the taxonomic path string.
     *
     * See @link append_delimiter( bool value ) the setter@endlink for details.
     */
    bool               append_delimiter() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    TaxonField  field_                  = TaxonField::kName;
    std::string delimiter_              = ";";
    bool        trim_nested_duplicates_ = false;
    bool        append_delimiter_       = false;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
