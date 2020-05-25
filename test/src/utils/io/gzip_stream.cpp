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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/gzip_block_ostream.hpp"
#include "genesis/utils/io/gzip_stream.hpp"
#include "genesis/utils/io/output_target.hpp"

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

using namespace genesis;
using namespace genesis::utils;

// Helper function, copied from https://github.com/mateidavid/zstr/blob/master/examples/zc.cpp
// License information: see genesis/lib/genesis/utils/io/gzip.hpp
void cat_stream(std::istream& is, std::ostream& os)
{
    const std::streamsize buff_size = 1 << 16;
    char * buff = new char [buff_size];
    while (true)
    {
        is.read(buff, buff_size);
        std::streamsize cnt = is.gcount();
        if (cnt == 0) break;
        os.write(buff, cnt);
    }
    delete [] buff;
}

TEST( GzipStream, StringCompressDecompress )
{
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const data = file_read( infile );

    std::string compr;
    std::string decompr;

    // Scope, so that we can re-use string names.
    {
        std::istringstream iss( data );
        std::ostringstream oss;

        // Use both gzip streams here. The input should detect that it is not compressed.
        GzipIStream gistr( iss );
        GzipOStream gostr( oss );

        // We here manually flush, because gostr does not go out of scope, so the sync
        // is not called within this scope.
        cat_stream( gistr, gostr );
        gostr.flush();
        compr = oss.str();
    }

    // Now, we should have compressed data.
    EXPECT_NE( data, compr );
    EXPECT_GT( data.size(), compr.size() );

    // Test that it is gzip by probing the magic bytes
    ASSERT_GT( compr.size(), 2 );
    unsigned char b0 = reinterpret_cast< unsigned char& >(compr[0]);
    unsigned char b1 = reinterpret_cast< unsigned char& >(compr[1]);
    EXPECT_TRUE( b0 == 0x1F && b1 == 0x8B );

    // Decopress again
    {
        std::istringstream iss( compr );
        std::ostringstream oss;
        GzipIStream gistr( iss );

        // Here, we only want the decompressing input stream, but not compress again.
        cat_stream( gistr, oss );
        decompr = oss.str();
    }

    // Test that we end up with the same that we started with
    EXPECT_EQ( data, decompr );
}

TEST( GzipStream, FileDecompress )
{
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const data = file_read( infile );

    // First, read compressed file and see if we get the same result as the uncompressed original
    {
        std::string infilegz = environment->data_dir + "sequence/dna_10.fasta.gz";
        GzipIFStream stream( infilegz );
        std::string decomp( std::istreambuf_iterator<char>(stream), {} );
        EXPECT_EQ( data, decomp );
    }

    // Then, also read the uncompressed again, and see if that also gives the same.
    // The gzip input file stream should be able to auto-detect that the file is not compressed.
    {
        GzipIFStream stream( infile );
        std::string decomp( std::istreambuf_iterator<char>(stream), {} );
        EXPECT_EQ( data, decomp );
    }
}

TEST( GzipStream, FileCompressDecompress )
{
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    std::string compfile = infile + ".tmp.gz";
    auto const data = file_read( infile );

    {
        // Compress the file
        GzipIFStream cis( infile );
        GzipOFStream cos( compfile );
        cat_stream( cis, cos );
    }

    std::string decompr;
    {
        // Decompress again
        std::ostringstream dos;
        GzipIFStream dis( compfile );
        cat_stream( dis, dos );
        decompr = dos.str();
    }

    EXPECT_EQ( data, decompr );
    EXPECT_EQ( 0, std::remove( compfile.c_str() ));
}

TEST( GzipStream, BlockStream )
{
    NEEDS_TEST_DATA;

    // Test a variety of file sizes, so that we get all kinds of buffer load
    std::vector<size_t> sizes = { 0, 1, 100, 10000, 1000000 };
    for( auto const size : sizes ) {
        auto const outfile = environment->data_dir + "gzip-block-" + std::to_string( size ) + ".gz";
        std::stringstream groundtruth;

        // Scope, so that data is written
        {
            std::ofstream ofs;
            ofs.open( outfile, std::ios_base::binary );
            GzipBlockOStream ogzs( ofs );

            // Write dummy numbers to the file
            for( size_t i = 0; i < size; ++i ) {
                ogzs << i << "\n";
                groundtruth << i << "\n";
            }
        }

        // Read the file again, and see what we got
        auto const data = file_read( outfile );
        EXPECT_EQ( groundtruth.str(), data );
        EXPECT_EQ( 0, std::remove( outfile.c_str() ));
    }
}

TEST( GzipStream, ToFile )
{
    NEEDS_TEST_DATA;
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    std::string compfile = infile + ".to-file.gz";
    auto const data = file_read( infile );

    // Using normal gzip compression
    {
        // Compress the file
        auto target = to_file( compfile, GzipCompressionLevel::kDefaultCompression );
        *target << data;
    }
    {
        // Decompress again
        auto const decompr = file_read( compfile );

        EXPECT_EQ( data, decompr );
        EXPECT_EQ( 0, std::remove( compfile.c_str() ));
    }

    // Using block gzip compression
    {
        // Compress the file
        auto target = to_gzip_block_file( compfile);
        *target << data;
    }
    {
        // Decompress again
        auto const decompr = file_read( compfile );

        EXPECT_EQ( data, decompr );
        EXPECT_EQ( 0, std::remove( compfile.c_str() ));
    }
}
