/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020

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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_header.hpp"

#include <stdexcept>

using namespace genesis::population;
using namespace genesis::utils;

TEST( Vcf, Basics )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    // Generic htslib file stuff
    auto file = HtsFile( infile );
    EXPECT_EQ( "vcf", file.format_extension() );
    // LOG_DBG << "HtsFile descr " << file.format_description();
    // LOG_DBG << "HtsFile ext " << file.format_extension();

    auto header = VcfHeader( file );
    EXPECT_EQ( "VCFv4.3", header.version() );
    EXPECT_EQ( 1, header.get_chroms().size() );
    EXPECT_EQ( 3, header.get_filter_ids().size() );
    EXPECT_EQ( 6, header.get_info_ids().size() );
    EXPECT_EQ( 4, header.get_format_ids().size() );
    EXPECT_EQ( 3, header.get_samples().size() );

    // LOG_DBG << "VcfHeader version " << header.version();
    // LOG_DBG << "VcfHeader get_chroms";
    // for( auto const& e : header.get_chroms() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_chrom_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }
    // LOG_DBG << "VcfHeader get_filter_ids";
    // for( auto const& e : header.get_filter_ids() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_filter_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }
    // LOG_DBG << "VcfHeader get_info_ids";
    // for( auto const& e : header.get_info_ids() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_info_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }
    // LOG_DBG << "VcfHeader get_format_ids";
    // for( auto const& e : header.get_format_ids() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_format_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }
    // LOG_DBG << "VcfHeader get_samples";
    // for( auto const& e : header.get_samples() ) {
    //     LOG_DBG1 << "- " << e;
    // }
}
