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
 * @ingroup test
 */

#ifdef GENESIS_HTSLIB

#include "src/common.hpp"

#include "genesis/population/formats/hts_file.hpp"
#include "genesis/population/formats/vcf_common.hpp"
#include "genesis/population/formats/vcf_header.hpp"
#include "genesis/population/formats/vcf_input_stream.hpp"
#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/utils/text/string.hpp"

#include <bitset>
#include <numeric>
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

    EXPECT_EQ( 1, header.get_chromosomes().size() );
    EXPECT_EQ( 1435964, header.get_chromosome_length( "20" ));

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
    EXPECT_EQ( VcfValueType::kInteger, info_NS.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, info_NS.special );
    EXPECT_EQ( 1, info_NS.number );
    EXPECT_EQ( "Number of Samples With Data", info_NS.description );

    auto const info_DP = header.get_info_specification( "DP" );
    EXPECT_EQ( "DP", info_DP.id );
    EXPECT_EQ( VcfValueType::kInteger, info_DP.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, info_DP.special );
    EXPECT_EQ( 1, info_DP.number );
    EXPECT_EQ( "Total Depth", info_DP.description );

    auto const info_AF = header.get_info_specification( "AF" );
    EXPECT_EQ( "AF", info_AF.id );
    EXPECT_EQ( VcfValueType::kFloat, info_AF.type );
    EXPECT_EQ( VcfValueSpecial::kAllele, info_AF.special );
    EXPECT_EQ( 0xfffff, info_AF.number );
    EXPECT_EQ( "Allele Frequency", info_AF.description );

    auto const info_AA = header.get_info_specification( "AA" );
    EXPECT_EQ( "AA", info_AA.id );
    EXPECT_EQ( VcfValueType::kString, info_AA.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, info_AA.special );
    EXPECT_EQ( 1, info_AA.number );
    EXPECT_EQ( "Ancestral Allele", info_AA.description );

    auto const info_DB = header.get_info_specification( "DB" );
    EXPECT_EQ( "DB", info_DB.id );
    EXPECT_EQ( VcfValueType::kFlag, info_DB.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, info_DB.special );
    EXPECT_EQ( 0, info_DB.number );
    EXPECT_EQ( "dbSNP membership, build 129", info_DB.description );

    auto const info_H2 = header.get_info_specification( "H2" );
    EXPECT_EQ( "H2", info_H2.id );
    EXPECT_EQ( VcfValueType::kFlag, info_H2.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, info_H2.special );
    EXPECT_EQ( 0, info_H2.number );
    EXPECT_EQ( "HapMap2 membership", info_H2.description );

    EXPECT_NO_THROW( header.assert_info( "NS" ));
    EXPECT_NO_THROW( header.assert_info( "NS", VcfValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_info( "NS", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "NS", VcfValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_info( "DP" ));
    EXPECT_NO_THROW( header.assert_info( "DP", VcfValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_info( "DP", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "DP", VcfValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_info( "AF" ));
    EXPECT_NO_THROW( header.assert_info( "AF", VcfValueType::kFloat ));
    EXPECT_NO_THROW( header.assert_info( "AF", VcfValueType::kFloat, VcfValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfValueType::kFloat, 0 ));

    EXPECT_NO_THROW( header.assert_info( "AA" ));
    EXPECT_NO_THROW( header.assert_info( "AA", VcfValueType::kString ));
    EXPECT_NO_THROW( header.assert_info( "AA", VcfValueType::kString, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "AA", VcfValueType::kString, 1 ));

    EXPECT_NO_THROW( header.assert_info( "DB" ));
    EXPECT_NO_THROW( header.assert_info( "DB", VcfValueType::kFlag ));
    EXPECT_NO_THROW( header.assert_info( "DB", VcfValueType::kFlag, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_info( "DB", VcfValueType::kFlag, 0 ));

    EXPECT_TRUE( header.has_info( "NS" ));
    EXPECT_TRUE( header.has_info( "NS", VcfValueType::kInteger ));
    EXPECT_TRUE( header.has_info( "NS", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "NS", VcfValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_info( "DP" ));
    EXPECT_TRUE( header.has_info( "DP", VcfValueType::kInteger ));
    EXPECT_TRUE( header.has_info( "DP", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "DP", VcfValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_info( "AF" ));
    EXPECT_TRUE( header.has_info( "AF", VcfValueType::kFloat ));
    EXPECT_TRUE( header.has_info( "AF", VcfValueType::kFloat, VcfValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfValueType::kFloat, 0 ));

    EXPECT_TRUE( header.has_info( "AA" ));
    EXPECT_TRUE( header.has_info( "AA", VcfValueType::kString ));
    EXPECT_TRUE( header.has_info( "AA", VcfValueType::kString, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "AA", VcfValueType::kString, 1 ));

    EXPECT_TRUE( header.has_info( "DB" ));
    EXPECT_TRUE( header.has_info( "DB", VcfValueType::kFlag ));
    EXPECT_TRUE( header.has_info( "DB", VcfValueType::kFlag, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_info( "DB", VcfValueType::kFlag, 0 ));

    // Test non existing line
    EXPECT_ANY_THROW( header.assert_info( "XX" ));
    EXPECT_ANY_THROW( header.assert_info( "XX", VcfValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_info( "XX", VcfValueType::kInteger, 0 ));

    EXPECT_FALSE( header.has_info( "XX" ));
    EXPECT_FALSE( header.has_info( "XX", VcfValueType::kInteger ));
    EXPECT_FALSE( header.has_info( "XX", VcfValueType::kInteger, 0 ));

    // Test wrong specifications
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfValueType::kFloat ));
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfValueType::kInteger, 0 ));
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfValueType::kInteger, VcfValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfValueType::kFloat, 1 ));

    EXPECT_FALSE( header.has_info( "NS", VcfValueType::kFloat ));
    EXPECT_FALSE( header.has_info( "NS", VcfValueType::kInteger, 0 ));
    EXPECT_FALSE( header.has_info( "NS", VcfValueType::kInteger, VcfValueSpecial::kAllele ));
    EXPECT_FALSE( header.has_info( "AF", VcfValueType::kFloat, 1 ));

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

    EXPECT_EQ( 6, header.get_format_ids().size() );
    EXPECT_ANY_THROW( header.get_format_specification( "XX" ));
    EXPECT_ANY_THROW( header.get_format_values( "XX" ));
    EXPECT_NO_THROW( header.get_format_values( "GT" ));

    auto const format_GT = header.get_format_specification( "GT" );
    EXPECT_EQ( "GT", format_GT.id );
    EXPECT_EQ( VcfValueType::kString, format_GT.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, format_GT.special );
    EXPECT_EQ( 1, format_GT.number );
    EXPECT_EQ( "Genotype", format_GT.description );

    auto const format_GQ = header.get_format_specification( "GQ" );
    EXPECT_EQ( "GQ", format_GQ.id );
    EXPECT_EQ( VcfValueType::kInteger, format_GQ.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, format_GQ.special );
    EXPECT_EQ( 1, format_GQ.number );
    EXPECT_EQ( "Genotype Quality", format_GQ.description );

    auto const format_GL = header.get_format_specification( "GL" );
    EXPECT_EQ( "GL", format_GL.id );
    EXPECT_EQ( VcfValueType::kFloat, format_GL.type );
    EXPECT_EQ( VcfValueSpecial::kGenotype, format_GL.special );
    EXPECT_EQ( 0xfffff, format_GL.number );
    EXPECT_EQ( "Genotype likelihoods", format_GL.description );

    auto const format_DP = header.get_format_specification( "DP" );
    EXPECT_EQ( "DP", format_DP.id );
    EXPECT_EQ( VcfValueType::kInteger, format_DP.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, format_DP.special );
    EXPECT_EQ( 1, format_DP.number );
    EXPECT_EQ( "Read Depth", format_DP.description );

    auto const format_HQ = header.get_format_specification( "HQ" );
    EXPECT_EQ( "HQ", format_HQ.id );
    EXPECT_EQ( VcfValueType::kInteger, format_HQ.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, format_HQ.special );
    EXPECT_EQ( 2, format_HQ.number );
    EXPECT_EQ( "Haplotype Quality", format_HQ.description );

    auto const format_STR = header.get_format_specification( "STR" );
    EXPECT_EQ( "STR", format_STR.id );
    EXPECT_EQ( VcfValueType::kString, format_STR.type );
    EXPECT_EQ( VcfValueSpecial::kFixed, format_STR.special );
    EXPECT_EQ( 1, format_STR.number );
    EXPECT_EQ( "Test String", format_STR.description );

    EXPECT_NO_THROW( header.assert_format( "GT" ));
    EXPECT_NO_THROW( header.assert_format( "GT", VcfValueType::kString ));
    EXPECT_NO_THROW( header.assert_format( "GT", VcfValueType::kString, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "GT", VcfValueType::kString, 1 ));

    EXPECT_NO_THROW( header.assert_format( "GQ" ));
    EXPECT_NO_THROW( header.assert_format( "GQ", VcfValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_format( "GQ", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "GQ", VcfValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_format( "GL" ));
    EXPECT_NO_THROW( header.assert_format( "GL", VcfValueType::kFloat ));
    EXPECT_NO_THROW( header.assert_format( "GL", VcfValueType::kFloat, VcfValueSpecial::kGenotype ));
    // EXPECT_NO_THROW( header.assert_format( "GL", VcfValueType::kFloat, 0xfffff ));

    EXPECT_NO_THROW( header.assert_format( "DP" ));
    EXPECT_NO_THROW( header.assert_format( "DP", VcfValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_format( "DP", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "DP", VcfValueType::kInteger, 1 ));

    EXPECT_NO_THROW( header.assert_format( "HQ" ));
    EXPECT_NO_THROW( header.assert_format( "HQ", VcfValueType::kInteger ));
    EXPECT_NO_THROW( header.assert_format( "HQ", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "HQ", VcfValueType::kInteger, 2 ));

    EXPECT_NO_THROW( header.assert_format( "STR" ));
    EXPECT_NO_THROW( header.assert_format( "STR", VcfValueType::kString ));
    EXPECT_NO_THROW( header.assert_format( "STR", VcfValueType::kString, VcfValueSpecial::kFixed ));
    EXPECT_NO_THROW( header.assert_format( "STR", VcfValueType::kString, 1 ));

    EXPECT_TRUE( header.has_format( "GT" ));
    EXPECT_TRUE( header.has_format( "GT", VcfValueType::kString ));
    EXPECT_TRUE( header.has_format( "GT", VcfValueType::kString, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "GT", VcfValueType::kString, 1 ));

    EXPECT_TRUE( header.has_format( "GQ" ));
    EXPECT_TRUE( header.has_format( "GQ", VcfValueType::kInteger ));
    EXPECT_TRUE( header.has_format( "GQ", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "GQ", VcfValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_format( "GL" ));
    EXPECT_TRUE( header.has_format( "GL", VcfValueType::kFloat ));
    EXPECT_TRUE( header.has_format( "GL", VcfValueType::kFloat, VcfValueSpecial::kGenotype ));
    // EXPECT_TRUE( header.has_format( "GL", VcfValueType::kFloat, 0xfffff ));

    EXPECT_TRUE( header.has_format( "DP" ));
    EXPECT_TRUE( header.has_format( "DP", VcfValueType::kInteger ));
    EXPECT_TRUE( header.has_format( "DP", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "DP", VcfValueType::kInteger, 1 ));

    EXPECT_TRUE( header.has_format( "HQ" ));
    EXPECT_TRUE( header.has_format( "HQ", VcfValueType::kInteger ));
    EXPECT_TRUE( header.has_format( "HQ", VcfValueType::kInteger, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "HQ", VcfValueType::kInteger, 2 ));

    EXPECT_TRUE( header.has_format( "STR" ));
    EXPECT_TRUE( header.has_format( "STR", VcfValueType::kString ));
    EXPECT_TRUE( header.has_format( "STR", VcfValueType::kString, VcfValueSpecial::kFixed ));
    EXPECT_TRUE( header.has_format( "STR", VcfValueType::kString, 1 ));

    // Test non existing line
    EXPECT_ANY_THROW( header.assert_format( "XX" ));
    EXPECT_ANY_THROW( header.assert_format( "XX", VcfValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_format( "XX", VcfValueType::kInteger, 0 ));

    EXPECT_FALSE( header.has_format( "XX" ));
    EXPECT_FALSE( header.has_format( "XX", VcfValueType::kInteger ));
    EXPECT_FALSE( header.has_format( "XX", VcfValueType::kInteger, 0 ));

    // Test wrong specifications
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfValueType::kFloat ));
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfValueType::kString, 0 ));
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfValueType::kString, VcfValueSpecial::kAllele ));

    EXPECT_ANY_THROW( header.assert_format( "GL", VcfValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_format( "GL", VcfValueType::kFloat, 0 ));
    EXPECT_ANY_THROW( header.assert_format( "GL", VcfValueType::kFloat, VcfValueSpecial::kAllele ));

    // Test wrong specifications
    EXPECT_FALSE( header.has_format( "GT", VcfValueType::kFloat ));
    EXPECT_FALSE( header.has_format( "GT", VcfValueType::kString, 0 ));
    EXPECT_FALSE( header.has_format( "GT", VcfValueType::kString, VcfValueSpecial::kAllele ));

    EXPECT_FALSE( header.has_format( "GL", VcfValueType::kInteger ));
    EXPECT_FALSE( header.has_format( "GL", VcfValueType::kFloat, 0 ));
    EXPECT_FALSE( header.has_format( "GL", VcfValueType::kFloat, VcfValueSpecial::kAllele ));

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

    std::vector<std::string> sample_names{"NA00001", "NA00002", "NA00003"};
    EXPECT_EQ( sample_names, header.get_sample_names() );
    EXPECT_EQ( 3, header.get_sample_names().size() );
    EXPECT_EQ( 3, header.get_sample_count() );
    EXPECT_EQ( std::string( "NA00001" ), header.get_sample_name(0) );
    EXPECT_EQ( std::string( "NA00002" ), header.get_sample_name(1) );
    EXPECT_EQ( std::string( "NA00003" ), header.get_sample_name(2) );


    // LOG_DBG << "VcfHeader get_format";
    // for( auto const& e : header.get_format() ) {
    //     LOG_DBG1 << "- " << e;
    // }
}

TEST( Vcf, Records )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    auto file = HtsFile( infile );
    auto header = VcfHeader( file );
    auto record = VcfRecord( header );

    // We simply test the properties of all functions by concatenating/adding them for the records.
    std::string chromosomes;
    std::string positions;
    std::string ids;
    std::string references;
    std::string alterantives;
    size_t      alternatives_count = 0;
    std::string variants;
    size_t      variant_sum = 0;
    size_t      snp_sum = 0;
    double      quality_sum = 0.0;
    std::string filter_ids;
    size_t      filter_passing_sum = 0;
    std::string info_ids;
    std::string format_ids;

    // Same for all info fields, individually.
    int         info_ns = 0;
    int         info_dp = 0;
    double      info_af = 0.0;
    std::string info_aa;
    size_t      info_db = 0;
    size_t      info_h2 = 0;

    while( record.read_next( file )) {
        chromosomes        += record.get_chromosome() + " ";
        positions          += std::to_string(record.get_position()) + " ";
        ids                += record.get_id() + " ";
        references         += record.get_reference() + " ";
        alterantives       += join( record.get_alternatives(), "," ) + " ";
        alternatives_count += record.get_alternatives_count();
        variants           += join( record.get_variants(), "," ) + " ";
        variant_sum        += static_cast<int>( record.get_variant_types() );
        snp_sum            += record.is_snp();
        quality_sum        += record.get_quality();
        filter_ids         += join( record.get_filter_ids(), "," ) + " ";
        filter_passing_sum += record.pass_filter();
        info_ids           += join( record.get_info_ids(), "," ) + " ";
        format_ids         += join( record.get_format_ids(), "," ) + " ";

        if( record.has_info("NS") ) {
            auto const ns = record.get_info_int( "NS" );
            info_ns += std::accumulate( ns.begin(), ns.end(), static_cast<int>(0) );
            // LOG_DBG << "get_info_int(NS) " << join( ns, "|");
        }
        if( record.has_info("DP") ) {
            auto const dp = record.get_info_int( "DP" );
            info_dp += std::accumulate( dp.begin(), dp.end(), static_cast<int>(0) );
            // LOG_DBG << "get_info_int(DP) " << join( dp, "|");
        }
        if( record.has_info("AF") ) {
            auto const af = record.get_info_float( "AF" );
            info_af += std::accumulate( af.begin(), af.end(), static_cast<double>(0.0) );
            // LOG_DBG << "get_info_float(AF) " << join( af, "|");
        }
        if( record.has_info("AA") ) {
            auto const aa = record.get_info_string( "AA" );
            info_aa += aa;
            // LOG_DBG << "get_info_string(AA) " << aa;
        }
        if( record.has_info("DB") ) {
            info_db += record.get_info_flag( "DB" );
            // LOG_DBG << "get_info_flag(DB) " << ( record.get_info_flag( "DB" ) ? "true" : "false" );
        }
        if( record.has_info("H2") ) {
            info_h2 += record.get_info_flag( "H2" );
            // LOG_DBG << "get_info_flag(H2) " << ( record.get_info_flag( "H2" ) ? "true" : "false" );
        }

        for( size_t i = 0; i < record.get_alternatives_count(); ++i ) {
            EXPECT_EQ( record.get_alternatives()[i], record.get_alternative(i) );
            EXPECT_EQ( record.get_variant(i+1), record.get_alternative(i) );
        }
        for( size_t i = 0; i < record.get_variant_count(); ++i ) {
            EXPECT_EQ( record.get_variants()[i], record.get_variant(i) );

            if( i == 0 ) {
                EXPECT_EQ( record.get_reference(), record.get_variant(i) );
            } else {
                EXPECT_EQ( record.get_variant(i), record.get_alternative(i-1) );
            }
        }

        // LOG_DBG << record.get_chromosome() << "\t"
        //         << record.get_position() << "\t"
        //         << record.get_id();
        // LOG_DBG << record.get_reference() << "\t"
        //         << join( record.get_alternatives(), ";" ) << "\t"
        //         << record.get_alternatives_count() << "\t"
        //         << join( record.get_variants(), ";" );
        // LOG_DBG << record.get_quality();
        // LOG_DBG << join( record.get_filter_ids(), ";" ) << "\t"
        //         << ( record.pass_filter() ? "P\t" : "F\t" );
        // LOG_DBG << join( record.get_info_ids(), ";" );
        // LOG_DBG << join( record.get_format_ids(), ";" );
        // LOG_DBG << "record.get_variant_types() " << static_cast<int>( record.get_variant_types() );
        // if( record.get_variant_types() & VcfRecord::VariantType::kSnp ) {
        //     LOG_DBG << "found snp";
        // }
        // LOG_DBG << "is_snp " <<( record.is_snp() ? "yep" : "nope" );
        // LOG_BOLD;
    }

    // LOG_DBG << "chromosomes " << chromosomes;
    // LOG_DBG << "positions " << positions;
    // LOG_DBG << "ids " << ids;
    // LOG_DBG << "references " << references;
    // LOG_DBG << "alterantives " << alterantives;
    // LOG_DBG << "alternatives_count " << alternatives_count;
    // LOG_DBG << "variants " << variants;
    // LOG_DBG << "variant_sum " << variant_sum;
    // LOG_DBG << "snp_sum " << snp_sum;
    // LOG_DBG << "quality_sum " << quality_sum;
    // LOG_DBG << "filter_ids " << filter_ids;
    // LOG_DBG << "filter_passing_sum " << filter_passing_sum;
    // LOG_DBG << "info_ids " << info_ids;
    // LOG_DBG << "format_ids " << format_ids;
    // LOG_DBG << "info_ns " << info_ns;
    // LOG_DBG << "info_dp " << info_dp;
    // LOG_DBG << "info_af " << info_af;
    // LOG_DBG << "info_aa " << info_aa;
    // LOG_DBG << "info_db " << info_db;
    // LOG_DBG << "info_h2 " << info_h2;

    EXPECT_EQ( "20 20 20 20 20 ", chromosomes );
    EXPECT_EQ( "14370 17330 1110696 1230237 1234567 ", positions );
    EXPECT_EQ( "rs6054257 . rs6040355 . microsat1 ", ids );
    EXPECT_EQ( "G T A T GTC ", references );
    EXPECT_EQ( "A A G,T  G,GTCT ", alterantives );
    EXPECT_EQ( 6, alternatives_count );
    EXPECT_EQ( "G,A T,A A,G,T T GTC,G,GTCT ", variants );
    EXPECT_EQ( 7, variant_sum );
    EXPECT_EQ( 4, snp_sum );
    EXPECT_EQ( 196, quality_sum );
    EXPECT_EQ( "PASS q10 PASS PASS PASS ", filter_ids );
    EXPECT_EQ( 4, filter_passing_sum );
    EXPECT_EQ( "NS,DP,AF,DB,H2 NS,DP,AF NS,DP,AF,AA,DB NS,DP,AA NS,DP,AA ", info_ids );
    EXPECT_EQ( "GT,GQ,GL,DP,HQ GT,GQ,DP,HQ,STR GT,GQ,DP,HQ GT,GQ,DP,HQ GT,GQ,GL,DP ", format_ids );

    EXPECT_EQ( 14, info_ns );
    EXPECT_EQ( 57, info_dp );
    EXPECT_FLOAT_EQ( 1.517, info_af );
    EXPECT_EQ( "TTG", info_aa );
    EXPECT_EQ( 2, info_db );
    EXPECT_EQ( 1, info_h2 );
}

TEST( Vcf, FormatIterator )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    auto file = HtsFile( infile );
    auto header = VcfHeader( file );
    auto record = VcfRecord( header );

    // while( record.read_next( file )) {
    //     if( !record.has_format("HQ") ) {
    //         LOG_DBG << "no hq";
    //         continue;
    //     }
    //
    //     LOG_DBG << "begin " << record.at();
    //     for( auto& sample : record.get_format_int("HQ") ) {
    //         LOG_DBG1 << "sample_count() " << sample.sample_count() << " values_per_sample() " << sample.values_per_sample();
    //         LOG_DBG1 << "begin at " << sample.sample_index() << " " << sample.sample_name();
    //
    //         while( sample.has_value() ) {
    //             LOG_DBG2 << "at " << sample.value_index() <<  " val " << sample.get_value();
    //             sample.next_value();
    //             LOG_DBG2 << "end loop";
    //         }
    //         LOG_DBG1 << "end at " << sample.sample_name();
    //     }
    //     LOG_DBG << "end " << record.at();
    // }

    // We simply test the properties of all functions by concatenating/adding them for the records.
    size_t      gt_ref = 0;
    size_t      gt_alt = 0;
    size_t      gt_idx = 0;
    size_t      gt_phased = 0;
    size_t      gt_missing = 0;
    int         gq = 0;
    double      gl = 0.0;
    int         dp = 0;
    int         hq = 0;
    std::string str;

    // Iterate all records, iterate all samples and values, and concat everything.
    // We use different iteration types, just to also test them all.
    while( record.read_next( file )) {

        ASSERT_TRUE( record.has_format("GT") );
        // LOG_DBG << "at " << record.at();
        // for( auto& sample : record.get_format_int("GT") ) {
        //     LOG_DBG1 << "at " << sample.sample_index();
        //     while( sample.has_value() ) {
        //         LOG_DBG2 << "at " << sample.sample_index() << " " << sample.value_index();
        //
        //         auto const val = sample.get_value();
        //         LOG_DBG2 << "gt " << val << " --> " << std::bitset<4>(val) << " "
        //                  << "mi " << (bcf_gt_is_missing(val) ? "y " : "n ")
        //                  << "ph " << (bcf_gt_is_phased(val) ? "y " : "n ")
        //                  << "al " << bcf_gt_allele(val);
        //
        //         sample.next_value();
        //     }
        // }

        for( auto& sample : record.get_format_genotype() ) {
            while( sample.has_value() ) {
                auto const gt = sample.get_value();
                gt_ref += gt.is_reference();
                gt_alt += gt.is_alternative();
                gt_idx += gt.variant_index();
                gt_phased += gt.is_phased();
                gt_missing += gt.is_missing();
                sample.next_value();
            }
            EXPECT_EQ( sample.get_values().size(), sample.valid_value_count() );
        }

        if( record.has_format("GQ") ) {
            for( auto sample = record.begin_format_int("GQ"); sample != record.end_format_int(); ++sample ) {
                EXPECT_EQ( 3, sample.sample_count() );
                EXPECT_EQ( "NA00001", sample.sample_name_at(0) );
                EXPECT_EQ( "NA00002", sample.sample_name_at(1) );
                EXPECT_EQ( "NA00003", sample.sample_name_at(2) );

                if( sample.sample_index() == 0 ) {
                    EXPECT_EQ( "NA00001", sample.sample_name() );
                }

                for( size_t i = 0; i < sample.values_per_sample(); ++i ) {
                    if( sample.has_value_at(i) ) {
                        gq += sample.get_value_at(i);
                    }
                }
                EXPECT_EQ( sample.get_values().size(), sample.valid_value_count() );
            }
        }

        if( record.has_format("GL") ) {
            for( auto& sample : record.get_format_float("GL") ) {
                while( sample.has_value() ) {
                    gl += sample.get_value();
                    sample.next_value();
                }
                EXPECT_EQ( sample.get_values().size(), sample.valid_value_count() );
            }
        }

        if( record.has_format("DP") ) {
            for( auto& sample : record.get_format_int("DP") ) {
                while( sample.has_value() ) {
                    dp += sample.get_value();
                    sample.next_value();
                }
                EXPECT_EQ( sample.get_values().size(), sample.valid_value_count() );
            }
        }

        if( record.has_format("HQ") ) {
            for( auto& sample : record.get_format_int("HQ") ) {
                auto const all_hq = sample.get_values();
                for( auto const& hqv : all_hq ) {
                    hq += hqv;
                }
                EXPECT_EQ( sample.get_values().size(), sample.valid_value_count() );
            }
        }

        if( record.has_format("STR") ) {
            for( auto& sample : record.get_format_string("STR") ) {
                while( sample.has_value() ) {
                    str += sample.get_value() + " ";
                    sample.next_value();
                }
                EXPECT_EQ( sample.get_values().size(), sample.valid_value_count() );
            }
        }
    }

    // Genotype data in our example file:
    //     0|.    1|0    1/1
    //     0|0    0|1    0/0
    //     1|2    2|1    2/2
    //     0|0    0|0    0/0
    //     ./1    0/2    1/1
    // Hence: 14 times ref (0), 14 times alt (>0, but not .). The sum of all entries is 19,
    // but missing is encoded as -1, and above, we simply add this on top. So, two missing (.)
    // lead to a total index sum of 17. Furthermore, only the second genotype is counted as phased
    // in htslib (and in our wrapper VcfGenotype as well), so only 8 calls are phased, which hence
    // corresponds to the number of | in the data. Last, 2 times missing (.).

    EXPECT_EQ( 14, gt_ref );
    EXPECT_EQ( 14, gt_alt );
    EXPECT_EQ( 17, gt_idx );
    EXPECT_EQ( 8, gt_phased );
    EXPECT_EQ( 2, gt_missing );

    EXPECT_EQ( gq, 545 );
    EXPECT_FLOAT_EQ( gl, -87.3 );
    EXPECT_EQ( dp, 57 );
    EXPECT_EQ( hq, 668 );
    EXPECT_EQ( str, "Hello beautiful world " );
}

TEST( Vcf, FormatIteratorDoc )
{
    // Here, we use the code that is provided as examples in the documentation of the VcfFormatIterator
    // class, and test whether it compiles. Not much more, as the actual tests are above.
    // Decativate the logging output, so that we can compile with logging, but without
    // polluting our test output.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::LoggingLevel::kNone );

    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    // Load a file and init the data structures.
    auto file = HtsFile( infile );
    auto header = VcfHeader( file );
    auto record = VcfRecord( header );

    // Iterate all records/lines of the VCF file.
    while( record.read_next( file )) {
        // Skip if the read depth (DP) FORMAT is not available for the current record.
        if( !record.has_format("DP") ) {
            continue;
        }

        // Iterate the DP data for all samples of the record, loading them as int.
        for( auto& sample_dp : record.get_format_int("DP") ) {
            LOG_INFO << "At sample " << sample_dp.sample_name();

            // Iterate all individual values for that sample that are given in the data.
            while( sample_dp.has_value() ) {
                LOG_INFO << "- " << sample_dp.get_value();
                sample_dp.next_value();
            }

            // (replacement for the innermost while loop of above)
            for( size_t i = 0; i < sample_dp.values_per_sample(); ++i ) {
                if( sample_dp.has_value_at(i) ) {
                    LOG_INFO << "- " << sample_dp.get_value_at(i);
                }
            }

            // (again, replacement for the innermost while loop of above)
            if( sample_dp.has_value() ) {
                LOG_INFO << "- " << sample_dp.get_value();
            }
        }

        for( auto sample_dp = record.begin_format_int("DP"); sample_dp != record.end_format_int(); ++sample_dp ) {
            // ...
        }

        for( auto sample_gt : record.get_format_genotype() ) {
            std::string gtstr = vcf_genotype_string( sample_gt.get_values() );
        }
    }
}

TEST( Vcf, InputStream )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    std::string at;
    size_t cnt = 0;

    auto it = VcfInputStream( infile, std::vector<std::string>{ "NA00002" }, false, true );
    while( it ) {
        at += it.record().at() + " ";

        // We use a sample filter. Check that there is only one sample.
        size_t gt_cnt = 0;
        for( auto const& gt_it : it.record().get_format_genotype() ) {
            (void) gt_it;
            ++gt_cnt;
        }
        EXPECT_EQ( 1, gt_cnt );

        ++cnt;
        ++it;
    }

    EXPECT_EQ(
        "20:14370 (rs6054257) 20:17330 20:1110696 (rs6040355) 20:1230237 20:1234567 (microsat1) ",
        at
    );
    EXPECT_EQ( 5, cnt );
}

TEST( Vcf, InputStreamFailFilter )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    // Try to filter by a name that does not exist.
    EXPECT_ANY_THROW( VcfInputStream( infile, std::vector<std::string>{ "XYZ" }) );
}

TEST( Vcf, GenomeRegionList )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/regions.vcf";

    std::vector<std::string> exp = { "ABC:10-12", "ABC:25-26", "XYZ:5-7", "XYZ:9-10", "XYZ:19-20" };
    std::vector<std::string> res;

    auto const list = genome_region_list_from_vcf_file( infile );
    for( auto const& chr : list.chromosome_map() ) {
        for( auto const& region : chr.second ) {
            // LOG_DBG << chr.first << ":" << region.low() << "-" << region.high();
            res.emplace_back(
                chr.first + ":" +
                std::to_string( region.low() ) + "-" +
                std::to_string( region.high() )
            );
        }
    }

    EXPECT_EQ( exp, res );
}

#endif // htslib guard
