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

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/plotting/cathedral_plot.hpp"

#include "genesis/utils/containers/matrix/reader.hpp"
#include "genesis/utils/containers/matrix/writer.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Cathedral Plot Parameters
// =================================================================================================

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

    // First we add the parameters, so that those are also part of the document.
    // This also sets up the document to be a Json object, in case it was null (default constructed).
    auto document = cathedral_plot_parameters_to_json_document( record.parameters );

    // Now fill the object with our data.
    auto& obj = document.get_object();
    obj["title"]            = JsonDocument::string( record.title );
    obj["chromosomeName"]   = JsonDocument::string( record.chromosome_name );
    obj["chromosomeLength"] = JsonDocument::number_unsigned( record.chromosome_length );
    obj["windowWidths"]     = JsonDocument( record.window_widths );

    return document;
}

void save_cathedral_plot_record_to_files(
    genesis::utils::JsonDocument const& record_document,
    genesis::utils::Matrix<double> const& record_value_matrix,
    std::string const& base_path
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
    JsonWriter().write( record_document, to_file( base_path + ".json" ));
    MatrixWriter<double>( "," ).write( record_value_matrix, to_file( base_path + ".csv" ));
}

std::pair<genesis::utils::JsonDocument, genesis::utils::Matrix<double>>
load_cathedral_plot_record_from_files(
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
                "load_cathedral_plot_record_from_files(): Cannot find json/csv files "
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

} // namespace population
} // namespace genesis
