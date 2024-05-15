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

#include "genesis/population/plotting/cathedral_plot.hpp"

#include "genesis/population/plotting/genome_heatmap.hpp"
#include "genesis/utils/color/heat_map.hpp"
#include "genesis/utils/color/normalization.hpp"
#include "genesis/utils/containers/matrix/reader.hpp"
#include "genesis/utils/containers/matrix/writer.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Cathedral Plot Parameters
// =================================================================================================

void validate_cathedral_plot_record( CathedralPlotRecord const& record )
{
    if( record.value_matrix.empty() ) {
        return;
    }
    auto const cols_good = ( record.value_matrix.cols() == record.parameters.width );
    auto const rows_good = ( record.value_matrix.rows() == record.parameters.height );
    if( !cols_good || !rows_good ) {
        throw std::domain_error(
            "Invalid Cathedral Plot Record, where the parameters specify a width and height of " +
            std::to_string( record.parameters.width ) + "x" +
            std::to_string( record.parameters.height ) +
            " pixels, but the contained value matrix has dimensions " +
            std::to_string( record.value_matrix.cols() ) + "x" +
            std::to_string( record.value_matrix.rows() )
        );
    }
    if( record.value_matrix.rows() != record.window_widths.size() ) {
        throw std::domain_error(
            "Invalid Cathedral Plot Record, where the value matrix has " +
            std::to_string( record.value_matrix.rows() ) +
            " rows, but the window widths list contains " +
            std::to_string( record.window_widths.size() ) + " entries"
        );
    }
}

double cathedral_window_width(
    CathedralPlotRecord const& record, size_t row
) {
    if( record.parameters.width < 2 || record.parameters.height < 2 ) {
        throw std::invalid_argument( "cathedral_window_width invalid: width < 2 || height < 2" );
    }
    if( row >= record.parameters.height ) {
        throw std::invalid_argument( "cathedral_window_width invalid: row >= height" );
    }

    // We need the values as doubles. Do this once here, for conciseness.
    double const chr_len  = static_cast<double>( record.chromosome_length );
    double const width_d  = static_cast<double>( record.parameters.width );
    double const height_d = static_cast<double>( record.parameters.height );
    double const row_d    = static_cast<double>( row );

    double window_width = 0.0;
    switch( record.parameters.window_width_method ) {
        case CathedralWindowWidthMethod::kExponential: {
            auto const decay = - std::log( 1.0 / width_d ) / ( height_d - 1.0 );
            window_width = chr_len * std::exp( -decay * row_d);
            break;
        }
        case CathedralWindowWidthMethod::kGeometric: {
            // We use a scaler based on the image dimensions so that the first window
            // is the whole chromosome, and the last is window width = single pixel.
            auto const scaler = width_d / height_d;
            auto const denom = scaler * row_d + 1.0;
            window_width = chr_len / denom;
            break;
        }
        case CathedralWindowWidthMethod::kLinear: {
            // minimum width of windows, where each window corresponds to one pixel of width,
            // and max width, corresponding to whole genome as one window.
            auto const min_win_width = chr_len / width_d;
            auto const max_win_width = chr_len;

            // How far are we in the rows, as a fraction?
            // Use that to interplate linearly between min and max window width.
            auto const row_frac = row_d / height_d;
            window_width = ( max_win_width - min_win_width ) * ( 1.0 - row_frac ) + min_win_width;
            break;
        }
        default: {
            throw std::invalid_argument( "cathedral_window_width requested with invalid method" );
        }
    }
    return window_width;
}

std::string cathedral_window_width_method_to_string( CathedralWindowWidthMethod method )
{
    switch( method ) {
        case CathedralWindowWidthMethod::kExponential: {
            return "exponential";
        }
        case CathedralWindowWidthMethod::kGeometric: {
            return "geometric";
        }
        case CathedralWindowWidthMethod::kLinear: {
            return "linear";
        }
    }
    throw std::runtime_error( "Invalid CathedralWindowWidthMethod" );
}

CathedralWindowWidthMethod cathedral_window_width_method_from_string( std::string const& method )
{
    auto const lower = utils::to_lower( method );
    if( lower == "exponential" ) {
        return CathedralWindowWidthMethod::kExponential;
    }
    if( lower == "geometric" ) {
        return CathedralWindowWidthMethod::kGeometric;
    }
    if( lower == "linear" ) {
        return CathedralWindowWidthMethod::kLinear;
    }
    throw std::invalid_argument(
        "cathedral_window_width_method_from_string(): Invalid method name \"" + method + "\""
    );
}

// =================================================================================================
//     Storage Functions
// =================================================================================================

genesis::utils::JsonDocument cathedral_plot_parameters_to_json_document(
    CathedralPlotParameters const& parameters
) {
    using namespace genesis::utils;

    // Create a top-level object Json document.
    auto document = JsonDocument::object();
    auto& obj = document.get_object();

    // For simplicity, we just write (or overwrite) the entries that we are interested in here.
    // We could add a check for their existence, but for now we declare that as a user error.
    // Not a use case that we will have within the library for now.
    // We use camelCase for the parameters, as recommended by https://jsonapi.org/recommendations/
    // as well as the Google JSON Style Guide.
    obj["width"]  = JsonDocument::number_unsigned( parameters.width );
    obj["height"] = JsonDocument::number_unsigned( parameters.height );
    obj["windowWidthMethod"] = JsonDocument::string(
        cathedral_window_width_method_to_string( parameters.window_width_method )
    );

    return document;
}

genesis::utils::JsonDocument cathedral_plot_record_to_json_document(
    CathedralPlotRecord const& record
) {
    using namespace genesis::utils;
    validate_cathedral_plot_record( record );

    // First we add the parameters, so that those are also part of the document.
    // This also sets up the document to be a Json object, in case it was null (default constructed).
    auto document = cathedral_plot_parameters_to_json_document( record.parameters );

    // Now fill the object with our data.
    auto& obj = document.get_object();
    obj["title"]            = JsonDocument::string( record.title );
    obj["plotName"]   = JsonDocument::string( record.plot_name );
    obj["chromosomeName"]   = JsonDocument::string( record.chromosome_name );
    obj["chromosomeLength"] = JsonDocument::number_unsigned( record.chromosome_length );
    obj["windowWidths"]     = JsonDocument( record.window_widths );

    // For user convenience, we also store the min and max values,
    // so that downstream plots can be adjusted more easily.
    auto const min_max = finite_minimum_maximum(
        record.value_matrix.begin(), record.value_matrix.end()
    );
    obj["minValue"] = JsonDocument::number_float( min_max.min );
    obj["maxValue"] = JsonDocument::number_float( min_max.max );

    return document;
}

void save_cathedral_plot_record_to_targets(
    genesis::utils::JsonDocument const& record_document,
    genesis::utils::Matrix<double> const& record_value_matrix,
    std::shared_ptr<genesis::utils::BaseOutputTarget> json_target,
    std::shared_ptr<genesis::utils::BaseOutputTarget> csv_target
) {
    using namespace genesis::utils;

    // Error checking, to avoid user error. Would be better to have that also coded into
    // the functions, but well, refactor later...
    if( record_document.empty() ) {
        throw std::invalid_argument(
            "save_cathedral_plot_record_to_files(): Empty JSON document provided. "
            "Call cathedral_plot_record_to_json_document() or related functions first."
        );
    }
    if( record_value_matrix.empty() ) {
        throw std::invalid_argument(
            "save_cathedral_plot_record_to_files(): Empty value matrix provided. "
            "Call compute_cathedral_matrix() or related functions first."
        );
    }

    // Write both files, using their respective readers.
    JsonWriter().write( record_document, json_target );
    MatrixWriter<double>( "," ).write( record_value_matrix, csv_target );
}

void save_cathedral_plot_record_to_files(
    genesis::utils::JsonDocument const& record_document,
    genesis::utils::Matrix<double> const& record_value_matrix,
    std::string const& base_path
) {
    using namespace genesis::utils;
    save_cathedral_plot_record_to_targets(
        record_document,
        record_value_matrix,
        to_file( base_path + ".json" ),
        to_file( base_path + ".csv" )
    );
}

void save_cathedral_plot_record_to_files(
    CathedralPlotRecord const& record,
    std::string const& base_path
) {
    auto const document = cathedral_plot_record_to_json_document( record );
    save_cathedral_plot_record_to_files( document, record.value_matrix, base_path );
}

std::pair<genesis::utils::JsonDocument, genesis::utils::Matrix<double>>
load_cathedral_plot_record_components_from_files(
    std::string const& base_path
) {
    using namespace genesis::utils;

    // We want to be lenient here, and allow to either specify the base path,
    // or either of the two actual files that we want to read.
    // Init with just the base path. If this works, we are good.
    auto json_file = base_path + ".json";
    auto csv_file  = base_path + ".csv";

    // If either file does not exist, we examine further.
    if( !file_exists( json_file ) || !file_exists( csv_file )) {
        if( ends_with( base_path, ".json" )) {
            json_file = base_path;
            csv_file  = file_filename( base_path ) + ".csv";
        } else if( ends_with( base_path, ".csv" )) {
            json_file = file_filename( base_path ) + ".json";
            csv_file  = base_path;
        }
        if( !file_exists( json_file ) || !file_exists( csv_file )) {
            throw std::invalid_argument(
                "load_cathedral_plot_record_components_from_files(): Cannot find json/csv files "
                "for base path \"" + base_path + "\""
            );
        }
    }

    // Now we have two files that exist. Read them, and return their contents.
    return std::make_pair(
        JsonReader().read( from_file( json_file )),
        MatrixReader<double>( "," ).read( from_file( csv_file ))
    );
}

CathedralPlotRecord load_cathedral_plot_record_from_files(
    std::string const& base_path
) {
    // Read the data.
    auto components = load_cathedral_plot_record_components_from_files( base_path );
    auto const& json = components.first;

    // Fill the record. We currently only read the fields that we are actually using downstream.
    // Might need amendment if we make use of other fields as well, such as the window widths vec.
    CathedralPlotRecord result;
    result.title             = json[ "title" ].get_string();
    result.plot_name         = json[ "plotName" ].get_string();
    result.chromosome_name   = json[ "chromosomeName" ].get_string();
    result.chromosome_length = json[ "chromosomeLength" ].get_number_unsigned();
    result.parameters.width  = json[ "width" ].get_number_unsigned();
    result.parameters.height = json[ "height" ].get_number_unsigned();
    result.parameters.window_width_method = cathedral_window_width_method_from_string(
        json[ "windowWidthMethod" ].get_string()
    );
    for( auto const& elem : json[ "windowWidths" ].get_array() ) {
        result.window_widths.push_back( elem.get_number_float() );
    }

    // Also get the value data.
    result.value_matrix = std::move( components.second );

    // Now check internal consistency, and return the result.
    validate_cathedral_plot_record( result );
    return result;
}

// =================================================================================================
//     Plotting Functions
// =================================================================================================

genesis::utils::Matrix<genesis::utils::Color> make_cathedral_plot_heatmap(
    CathedralPlotRecord const& record,
    genesis::utils::HeatmapParameters const& heatmap_parameters
) {
    validate_cathedral_plot_record( record );
    return utils::make_heatmap_matrix( record.value_matrix, heatmap_parameters );
}

genesis::utils::SvgDocument make_cathedral_plot_svg(
    CathedralPlotRecord const& record,
    genesis::utils::HeatmapParameters const& heatmap_parameters,
    genesis::utils::Matrix<genesis::utils::Color> const& image
) {
    using namespace genesis::utils;

    // Error and boundary checks
    validate_cathedral_plot_record( record );
    if( record.value_matrix.rows() != image.rows() || record.value_matrix.cols() != image.cols() ) {
        throw std::invalid_argument(
            "Invalid call to make_cathedral_plot_svg() with image dimensions not fitting "
            "with the data dimensions of the plot record."
        );
    }
    if( record.window_widths.empty() ) {
        throw std::invalid_argument(
            "Invalid call to make_cathedral_plot_svg() with empty window widths list."
        );
    }

    // Recompute window widths.
    // auto const max_win_width = static_cast<double>( record.chromosome_length );
    // auto const min_win_width = max_win_width / static_cast<double>( record.parameters.width );
    // Nope, future proof, we just use the actual entries from the list.
    assert( record.window_widths.size() > 0 );
    auto const min_win_width = record.window_widths.back();
    auto const max_win_width = record.window_widths.front();

    // Make the x-axis.
    auto x_axis_settings = SvgAxisSettings();
    x_axis_settings.position = SvgAxisSettings::Position::kBottom;
    x_axis_settings.length = record.parameters.width;
    auto const x_ticks = Tickmarks().linear_labels( 1, record.chromosome_length, 5 );
    auto const x_axis = make_svg_axis( x_axis_settings, x_ticks, "Genome position" );

    // Make the y-axis ticks, depending on the type of window scaling.
    std::vector<utils::Tickmarks::LabeledTick> y_ticks;
    switch( record.parameters.window_width_method ) {
        case CathedralWindowWidthMethod::kExponential: {
            y_ticks = Tickmarks().logarithmic_labels( min_win_width, max_win_width );
            break;
        }
        case CathedralWindowWidthMethod::kGeometric: {
            // Not implemented in Tickmarks, so we just do the max and min window size instead.
            y_ticks.emplace_back( 0.0, min_win_width );
            y_ticks.emplace_back( 1.0, max_win_width );
            break;
        }
        case CathedralWindowWidthMethod::kLinear: {
            y_ticks = Tickmarks().linear_labels( min_win_width, max_win_width, 5 );
            break;
        }
        default: {
            throw std::invalid_argument(
                "make_cathedral_plot_svg(): Invalid CathedralWindowWidthMethod"
            );
        }
    }

    // Make the y-axis.
    auto y_axis_settings = SvgAxisSettings();
    y_axis_settings.position = SvgAxisSettings::Position::kLeft;
    y_axis_settings.length = record.parameters.height;
    auto const y_axis = make_svg_axis( y_axis_settings, y_ticks, "Window size" );

    // Make a color bar, using the color params.
    auto color_norm = make_heatmap_color_norm( record.value_matrix, heatmap_parameters );
    assert( color_norm );
    auto color_bar_settings = SvgColorBarSettings();
    color_bar_settings.height = record.parameters.height;
    auto const color_bar = make_svg_color_bar(
        color_bar_settings, heatmap_parameters.color_map, *color_norm
    );

    // Make an svg doc from the above elements, and return it.
    auto svg = GenomeHeatmap();
    svg.add( record.title, image, x_axis, y_axis, color_bar );
    return svg.document();
}

genesis::utils::SvgDocument make_cathedral_plot_svg(
    CathedralPlotRecord const& record,
    genesis::utils::HeatmapParameters const& heatmap_parameters
) {
    auto const image = make_cathedral_plot_heatmap( record, heatmap_parameters );
    return make_cathedral_plot_svg( record, heatmap_parameters, image );
}

} // namespace population
} // namespace genesis
