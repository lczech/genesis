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
#include "genesis/utils/text/string.hpp"

#include <stdexcept>

using namespace genesis::population;
using namespace genesis::utils;

TEST( Vcf, Header )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    // --------------------------------
    //     Generic htslib file stuff
    // --------------------------------

    auto file = HtsFile( infile );
    EXPECT_EQ( "vcf", file.format_extension() );
    // LOG_DBG << "HtsFile descr " << file.format_description();
    // LOG_DBG << "HtsFile ext " << file.format_extension();

    auto header = VcfHeader( file );
    EXPECT_EQ( "VCFv4.3", header.version() );
    // LOG_DBG << "VcfHeader version " << header.version();

    // --------------------------------
    //     Chromosomes
    // --------------------------------

    EXPECT_EQ( 1, header.get_chroms().size() );
    EXPECT_EQ( 62435964, header.get_chrom_length( "20" ));

    // LOG_DBG << "VcfHeader get_chroms";
    // for( auto const& e : header.get_chroms() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_chrom_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }

    // --------------------------------
    //     Filter
    // --------------------------------

    EXPECT_EQ( 3, header.get_filter_ids().size() );
    EXPECT_NO_THROW( header.assert_filter( "PASS" ));
    EXPECT_NO_THROW( header.assert_filter( "q10" ));
    EXPECT_ANY_THROW( header.assert_filter( "myfilter" ));

    EXPECT_TRUE( header.has_filter( "PASS" ));
    EXPECT_TRUE( header.has_filter( "q10" ));
    EXPECT_FALSE( header.has_filter( "myfilter" ));

    // LOG_DBG << "VcfHeader get_filter_ids";
    // for( auto const& e : header.get_filter_ids() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_filter_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }

    // --------------------------------
    // Info
    // --------------------------------

    EXPECT_EQ( 6, header.get_info_ids().size() );
    EXPECT_ANY_THROW( header.get_info_specification( "XX" ));
    EXPECT_ANY_THROW( header.get_info_values( "XX" ));
    EXPECT_NO_THROW( header.get_info_values( "NS" ));

    auto const info_NS = header.get_info_specification( "NS" );
    EXPECT_EQ( "NS", info_NS.id );
    EXPECT_EQ( VcfHeader::ValueType::kInteger, info_NS.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, info_NS.special );
    EXPECT_EQ( 1, info_NS.number );
    EXPECT_EQ( "Number of Samples With Data", info_NS.description );

    auto const info_DP = header.get_info_specification( "DP" );
    EXPECT_EQ( "DP", info_DP.id );
    EXPECT_EQ( VcfHeader::ValueType::kInteger, info_DP.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, info_DP.special );
    EXPECT_EQ( 1, info_DP.number );
    EXPECT_EQ( "Total Depth", info_DP.description );

    auto const info_AF = header.get_info_specification( "AF" );
    EXPECT_EQ( "AF", info_AF.id );
    EXPECT_EQ( VcfHeader::ValueType::kFloat, info_AF.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kAllele, info_AF.special );
    EXPECT_EQ( 0xfffff, info_AF.number );
    EXPECT_EQ( "Allele Frequency", info_AF.description );

    auto const info_AA = header.get_info_specification( "AA" );
    EXPECT_EQ( "AA", info_AA.id );
    EXPECT_EQ( VcfHeader::ValueType::kString, info_AA.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, info_AA.special );
    EXPECT_EQ( 1, info_AA.number );
    EXPECT_EQ( "Ancestral Allele", info_AA.description );

    auto const info_DB = header.get_info_specification( "DB" );
    EXPECT_EQ( "DB", info_DB.id );
    EXPECT_EQ( VcfHeader::ValueType::kFlag, info_DB.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, info_DB.special );
    EXPECT_EQ( 0, info_DB.number );
    EXPECT_EQ( "dbSNP membership, build 129", info_DB.description );

    auto const info_H2 = header.get_info_specification( "H2" );
    EXPECT_EQ( "H2", info_H2.id );
    EXPECT_EQ( VcfHeader::ValueType::kFlag, info_H2.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, info_H2.special );
    EXPECT_EQ( 0, info_H2.number );
    EXPECT_EQ( "HapMap2 membership", info_H2.description );

    EXPECT_NO_THROW( header.assert_info( "NS" ));
    EXPECT_NO_THROW( header.assert_info( "NS", VcfHeader::ValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_info( "NS", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "NS", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_info( "DP" ));
    EXPECT_NO_THROW( header.assert_info( "DP", VcfHeader::ValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_info( "DP", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "DP", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_info( "AF" ));
    EXPECT_NO_THROW( header.assert_info( "AF", VcfHeader::ValueType::kFloat ));
    EXPECT_NO_THROW( header.assert_info( "AF", VcfHeader::ValueType::kFloat, VcfHeader::ValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfHeader::ValueType::kFloat, 0 ));

    EXPECT_NO_THROW( header.assert_info( "AA" ));
    EXPECT_NO_THROW( header.assert_info( "AA", VcfHeader::ValueType::kString ));
    EXPECT_NO_THROW( header.assert_info( "AA", VcfHeader::ValueType::kString, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "AA", VcfHeader::ValueType::kString, 1 ));

    EXPECT_NO_THROW( header.assert_info( "DB" ));
    EXPECT_NO_THROW( header.assert_info( "DB", VcfHeader::ValueType::kFlag ));
    EXPECT_NO_THROW( header.assert_info( "DB", VcfHeader::ValueType::kFlag, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "DB", VcfHeader::ValueType::kFlag, 0 ));

    EXPECT_TRUE( header.has_info( "NS" ));
    EXPECT_TRUE( header.has_info( "NS", VcfHeader::ValueType::kInteger ));
    EXPECT_TRUE( header.has_info( "NS", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "NS", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_info( "DP" ));
    EXPECT_TRUE( header.has_info( "DP", VcfHeader::ValueType::kInteger ));
    EXPECT_TRUE( header.has_info( "DP", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "DP", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_info( "AF" ));
    EXPECT_TRUE( header.has_info( "AF", VcfHeader::ValueType::kFloat ));
    EXPECT_TRUE( header.has_info( "AF", VcfHeader::ValueType::kFloat, VcfHeader::ValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfHeader::ValueType::kFloat, 0 ));

    EXPECT_TRUE( header.has_info( "AA" ));
    EXPECT_TRUE( header.has_info( "AA", VcfHeader::ValueType::kString ));
    EXPECT_TRUE( header.has_info( "AA", VcfHeader::ValueType::kString, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "AA", VcfHeader::ValueType::kString, 1 ));

    EXPECT_TRUE( header.has_info( "DB" ));
    EXPECT_TRUE( header.has_info( "DB", VcfHeader::ValueType::kFlag ));
    EXPECT_TRUE( header.has_info( "DB", VcfHeader::ValueType::kFlag, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "DB", VcfHeader::ValueType::kFlag, 0 ));

    // Test non existing line
    EXPECT_ANY_THROW( header.assert_info( "XX" ));
    EXPECT_ANY_THROW( header.assert_info( "XX", VcfHeader::ValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_info( "XX", VcfHeader::ValueType::kInteger, 0 ));

    EXPECT_FALSE( header.has_info( "XX" ));
    EXPECT_FALSE( header.has_info( "XX", VcfHeader::ValueType::kInteger ));
    EXPECT_FALSE( header.has_info( "XX", VcfHeader::ValueType::kInteger, 0 ));

    // Test wrong specifications
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfHeader::ValueType::kFloat ));
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfHeader::ValueType::kInteger, 0 ));
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfHeader::ValueType::kFloat, 1 ));

    EXPECT_FALSE( header.has_info( "NS", VcfHeader::ValueType::kFloat ));
    EXPECT_FALSE( header.has_info( "NS", VcfHeader::ValueType::kInteger, 0 ));
    EXPECT_FALSE( header.has_info( "NS", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kAllele ));
    EXPECT_FALSE( header.has_info( "AF", VcfHeader::ValueType::kFloat, 1 ));

    // LOG_DBG << "VcfHeader get_info_ids";
    // for( auto const& e : header.get_info_ids() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_info_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }

    // --------------------------------
    //     Format
    // --------------------------------

    EXPECT_EQ( 4, header.get_format_ids().size() );
    EXPECT_ANY_THROW( header.get_format_specification( "XX" ));
    EXPECT_ANY_THROW( header.get_format_values( "XX" ));
    EXPECT_NO_THROW( header.get_format_values( "GT" ));

    auto const format_GT = header.get_format_specification( "GT" );
    EXPECT_EQ( "GT", format_GT.id );
    EXPECT_EQ( VcfHeader::ValueType::kString, format_GT.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, format_GT.special );
    EXPECT_EQ( 1, format_GT.number );
    EXPECT_EQ( "Genotype", format_GT.description );

    auto const format_GQ = header.get_format_specification( "GQ" );
    EXPECT_EQ( "GQ", format_GQ.id );
    EXPECT_EQ( VcfHeader::ValueType::kInteger, format_GQ.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, format_GQ.special );
    EXPECT_EQ( 1, format_GQ.number );
    EXPECT_EQ( "Genotype Quality", format_GQ.description );

    auto const format_DP = header.get_format_specification( "DP" );
    EXPECT_EQ( "DP", format_DP.id );
    EXPECT_EQ( VcfHeader::ValueType::kInteger, format_DP.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, format_DP.special );
    EXPECT_EQ( 1, format_DP.number );
    EXPECT_EQ( "Read Depth", format_DP.description );

    auto const format_HQ = header.get_format_specification( "HQ" );
    EXPECT_EQ( "HQ", format_HQ.id );
    EXPECT_EQ( VcfHeader::ValueType::kInteger, format_HQ.type );
    EXPECT_EQ( VcfHeader::ValueSpecial::kFixed, format_HQ.special );
    EXPECT_EQ( 2, format_HQ.number );
    EXPECT_EQ( "Haplotype Quality", format_HQ.description );

    EXPECT_NO_THROW( header.assert_format( "GT" ));
    EXPECT_NO_THROW( header.assert_format( "GT", VcfHeader::ValueType::kString ));
    EXPECT_NO_THROW( header.assert_format( "GT", VcfHeader::ValueType::kString, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "GT", VcfHeader::ValueType::kString, 1 ));

    EXPECT_NO_THROW( header.assert_format( "GQ" ));
    EXPECT_NO_THROW( header.assert_format( "GQ", VcfHeader::ValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_format( "GQ", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "GQ", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_format( "DP" ));
    EXPECT_NO_THROW( header.assert_format( "DP", VcfHeader::ValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_format( "DP", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "DP", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_format( "HQ" ));
    EXPECT_NO_THROW( header.assert_format( "HQ", VcfHeader::ValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_format( "HQ", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "HQ", VcfHeader::ValueType::kInteger, 2 ));

    EXPECT_TRUE( header.has_format( "GT" ));
    EXPECT_TRUE( header.has_format( "GT", VcfHeader::ValueType::kString ));
    EXPECT_TRUE( header.has_format( "GT", VcfHeader::ValueType::kString, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "GT", VcfHeader::ValueType::kString, 1 ));

    EXPECT_TRUE( header.has_format( "GQ" ));
    EXPECT_TRUE( header.has_format( "GQ", VcfHeader::ValueType::kInteger ));
    EXPECT_TRUE( header.has_format( "GQ", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "GQ", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_format( "DP" ));
    EXPECT_TRUE( header.has_format( "DP", VcfHeader::ValueType::kInteger ));
    EXPECT_TRUE( header.has_format( "DP", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "DP", VcfHeader::ValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_format( "HQ" ));
    EXPECT_TRUE( header.has_format( "HQ", VcfHeader::ValueType::kInteger ));
    EXPECT_TRUE( header.has_format( "HQ", VcfHeader::ValueType::kInteger, VcfHeader::ValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "HQ", VcfHeader::ValueType::kInteger, 2 ));

    // Test non existing line
    EXPECT_ANY_THROW( header.assert_format( "XX" ));
    EXPECT_ANY_THROW( header.assert_format( "XX", VcfHeader::ValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_format( "XX", VcfHeader::ValueType::kInteger, 0 ));

    EXPECT_FALSE( header.has_format( "XX" ));
    EXPECT_FALSE( header.has_format( "XX", VcfHeader::ValueType::kInteger ));
    EXPECT_FALSE( header.has_format( "XX", VcfHeader::ValueType::kInteger, 0 ));

    // Test wrong specifications
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfHeader::ValueType::kFloat ));
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfHeader::ValueType::kString, 0 ));
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfHeader::ValueType::kString, VcfHeader::ValueSpecial::kAllele ));

    // Test wrong specifications
    EXPECT_FALSE( header.has_format( "GT", VcfHeader::ValueType::kFloat ));
    EXPECT_FALSE( header.has_format( "GT", VcfHeader::ValueType::kString, 0 ));
    EXPECT_FALSE( header.has_format( "GT", VcfHeader::ValueType::kString, VcfHeader::ValueSpecial::kAllele ));

    // LOG_DBG << "VcfHeader get_format_ids";
    // for( auto const& e : header.get_format_ids() ) {
    //     LOG_DBG1 << "- " << e;
    //     for( auto const& v : header.get_format_values( e ) ) {
    //         LOG_DBG2 << v.first << " --> " << v.second;
    //     }
    // }

    // --------------------------------
    //     Samples
    // --------------------------------

    EXPECT_EQ( 3, header.get_samples().size() );
    EXPECT_EQ( 3, header.get_sample_count() );

    // LOG_DBG << "VcfHeader get_samples";
    // for( auto const& e : header.get_samples() ) {
    //     LOG_DBG1 << "- " << e;
    // }
}
