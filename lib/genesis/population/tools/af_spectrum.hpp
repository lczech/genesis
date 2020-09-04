#ifndef GENESIS_POPULATION_TOOLS_AF_SPECTRUM_H_
#define GENESIS_POPULATION_TOOLS_AF_SPECTRUM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup population
 */

#include <functional>
#include <string>
#include <vector>

#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/population/tools/window.hpp"
#include "genesis/utils/io/output_target.hpp"

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace genesis {
namespace utils {

class Color;

template<typename T>
class Matrix;

} // namespace utils
} // namespace genesis

namespace genesis {
namespace population {

// =================================================================================================
//     Allele Frequency Spectrum
// =================================================================================================

/**
 * @brief
 */
class AlleleFrequencySpectrum
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using AFWindow = Window<double>;

    struct Spectrum
    {
        Spectrum( std::string const& chromosome )
            : chromosome(chromosome)
        {}

        std::string chromosome;
        std::vector<std::vector<size_t>> values;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    AlleleFrequencySpectrum( size_t width, size_t bins = 100 );
    ~AlleleFrequencySpectrum();

    AlleleFrequencySpectrum( AlleleFrequencySpectrum const& ) = default;
    AlleleFrequencySpectrum( AlleleFrequencySpectrum&& )      = default;

    AlleleFrequencySpectrum& operator= ( AlleleFrequencySpectrum const& ) = default;
    AlleleFrequencySpectrum& operator= ( AlleleFrequencySpectrum&& )      = default;

    // -------------------------------------------------------------------------
    //     Event Plugin Functionals
    // -------------------------------------------------------------------------

    std::function<void( Spectrum const& )> on_chromosome_finish;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void enqueue( std::string const& chromosome, size_t position, double frequency );
    void enqueue( VcfRecord const& record );

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

    static size_t spectrum_to_bmp(
        Spectrum const& spectrum,
        std::shared_ptr<utils::BaseOutputTarget> target,
        bool log_scale = false
    );

    static size_t spectrum_to_bmp(
        Spectrum const& spectrum,
        std::vector<utils::Color> const& palette,
        std::shared_ptr<utils::BaseOutputTarget> target,
        bool log_scale = false
    );

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void on_chromosome_start_( std::string const& chromosome, AFWindow::Accumulator& );

    void on_chromosome_end_( std::string const& chromosome, AFWindow::Accumulator& );

    void on_emission_(
        size_t first_position, size_t last_position, size_t reported_position,
        AFWindow::const_iterator begin, AFWindow::const_iterator end, AFWindow::Accumulator&
    );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    size_t bins_;
    Window<double> window_;
    std::vector<Spectrum> spectra_;

};

} // namespace population
} // namespace genesis

#endif // include guard
