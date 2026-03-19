import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.population import *

class VariantTest(unittest.TestCase):

	def test_Header(self):

    # Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/example.vcf";

    # --------------------------------
    #     Generic htslib file stuff
    # --------------------------------

    auto file = HtsFile( infile );
    EXPECT_EQ( "vcf", file.format_extension() );
    # LOG_DBG << "HtsFile descr " << file.format_description();
    # LOG_DBG << "HtsFile ext " << file.format_extension();

    auto header = VcfHeader( file );
    EXPECT_EQ( "VCFv4.3", header.version() );
    # LOG_DBG << "VcfHeader version " << header.version();

    # --------------------------------
    #     Chromosomes
    # --------------------------------

    EXPECT_EQ( 1, header.get_chromosomes().size() );
    EXPECT_EQ( 1435964, header.get_chromosome_length( "20" ));

    # LOG_DBG << "VcfHeader get_chroms";
    # for( auto const& e : header.get_chroms() ) {
    #     LOG_DBG1 << "- " << e;
    #     for( auto const& v : header.get_chrom_values( e ) ) {
    #         LOG_DBG2 << v.first << " --> " << v.second;
    #     }
    # }

    # --------------------------------
    #     Filter
    # --------------------------------

    EXPECT_EQ( 3, header.get_filter_ids().size() );
    EXPECT_NO_THROW( header.assert_filter( "PASS" ));
    EXPECT_NO_THROW( header.assert_filter( "q10" ));
    EXPECT_ANY_THROW( header.assert_filter( "myfilter" ));

    EXPECT_TRUE( header.has_filter( "PASS" ));
    EXPECT_TRUE( header.has_filter( "q10" ));
    EXPECT_FALSE( header.has_filter( "myfilter" ));

    # LOG_DBG << "VcfHeader get_filter_ids";
    # for( auto const& e : header.get_filter_ids() ) {
    #     LOG_DBG1 << "- " << e;
    #     for( auto const& v : header.get_filter_values( e ) ) {
    #         LOG_DBG2 << v.first << " --> " << v.second;
    #     }
    # }

    # --------------------------------
    # Info
    # --------------------------------

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

    # Test non existing line
    EXPECT_ANY_THROW( header.assert_info( "XX" ));
    EXPECT_ANY_THROW( header.assert_info( "XX", VcfValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_info( "XX", VcfValueType::kInteger, 0 ));

    EXPECT_FALSE( header.has_info( "XX" ));
    EXPECT_FALSE( header.has_info( "XX", VcfValueType::kInteger ));
    EXPECT_FALSE( header.has_info( "XX", VcfValueType::kInteger, 0 ));

    # Test wrong specifications
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfValueType::kFloat ));
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfValueType::kInteger, 0 ));
    EXPECT_ANY_THROW( header.assert_info( "NS", VcfValueType::kInteger, VcfValueSpecial::kAllele ));
    EXPECT_ANY_THROW( header.assert_info( "AF", VcfValueType::kFloat, 1 ));

    EXPECT_FALSE( header.has_info( "NS", VcfValueType::kFloat ));
    EXPECT_FALSE( header.has_info( "NS", VcfValueType::kInteger, 0 ));
    EXPECT_FALSE( header.has_info( "NS", VcfValueType::kInteger, VcfValueSpecial::kAllele ));
    EXPECT_FALSE( header.has_info( "AF", VcfValueType::kFloat, 1 ));

    # LOG_DBG << "VcfHeader get_info_ids";
    # for( auto const& e : header.get_info_ids() ) {
    #     LOG_DBG1 << "- " << e;
    #     for( auto const& v : header.get_info_values( e ) ) {
    #         LOG_DBG2 << v.first << " --> " << v.second;
    #     }
    # }

    # --------------------------------
    #     Format
    # --------------------------------

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
    # EXPECT_NO_THROW( header.assert_format( "GL", VcfValueType::kFloat, 0xfffff ));

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
    # EXPECT_TRUE( header.has_format( "GL", VcfValueType::kFloat, 0xfffff ));

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

    # Test non existing line
    EXPECT_ANY_THROW( header.assert_format( "XX" ));
    EXPECT_ANY_THROW( header.assert_format( "XX", VcfValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_format( "XX", VcfValueType::kInteger, 0 ));

    EXPECT_FALSE( header.has_format( "XX" ));
    EXPECT_FALSE( header.has_format( "XX", VcfValueType::kInteger ));
    EXPECT_FALSE( header.has_format( "XX", VcfValueType::kInteger, 0 ));

    # Test wrong specifications
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfValueType::kFloat ));
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfValueType::kString, 0 ));
    EXPECT_ANY_THROW( header.assert_format( "GT", VcfValueType::kString, VcfValueSpecial::kAllele ));

    EXPECT_ANY_THROW( header.assert_format( "GL", VcfValueType::kInteger ));
    EXPECT_ANY_THROW( header.assert_format( "GL", VcfValueType::kFloat, 0 ));
    EXPECT_ANY_THROW( header.assert_format( "GL", VcfValueType::kFloat, VcfValueSpecial::kAllele ));

    # Test wrong specifications
    EXPECT_FALSE( header.has_format( "GT", VcfValueType::kFloat ));
    EXPECT_FALSE( header.has_format( "GT", VcfValueType::kString, 0 ));
    EXPECT_FALSE( header.has_format( "GT", VcfValueType::kString, VcfValueSpecial::kAllele ));

    EXPECT_FALSE( header.has_format( "GL", VcfValueType::kInteger ));
    EXPECT_FALSE( header.has_format( "GL", VcfValueType::kFloat, 0 ));
    EXPECT_FALSE( header.has_format( "GL", VcfValueType::kFloat, VcfValueSpecial::kAllele ));

    # LOG_DBG << "VcfHeader get_format_ids";
    # for( auto const& e : header.get_format_ids() ) {
    #     LOG_DBG1 << "- " << e;
    #     for( auto const& v : header.get_format_values( e ) ) {
    #         LOG_DBG2 << v.first << " --> " << v.second;
    #     }
    # }

    # --------------------------------
    #     Samples
    # --------------------------------

    std::vector<std::string> sample_names{"NA00001", "NA00002", "NA00003"};
    EXPECT_EQ( sample_names, header.get_sample_names() );
    EXPECT_EQ( 3, header.get_sample_names().size() );
    EXPECT_EQ( 3, header.get_sample_count() );
    EXPECT_EQ( std::string( "NA00001" ), header.get_sample_name(0) );
    EXPECT_EQ( std::string( "NA00002" ), header.get_sample_name(1) );
    EXPECT_EQ( std::string( "NA00003" ), header.get_sample_name(2) );


    # LOG_DBG << "VcfHeader get_format";
    # for( auto const& e : header.get_format() ) {
    #     LOG_DBG1 << "- " << e;
    # }

if __name__ == '__main__':
	unittest.main()
