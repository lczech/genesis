/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/containers/dataframe.hpp"
#include "genesis/utils/containers/dataframe/reader.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/math/regression/dataframe.hpp"
#include "genesis/utils/math/regression/factor.hpp"
#include "genesis/utils/math/regression/glm.hpp"
#include "genesis/utils/math/regression/slr.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::utils;

TEST( Math, GlmFactors )
{
    auto const data = std::vector<std::string>{
        "B", "A", "B", "A", "C", "C", "B", "A", "A", "C", "D", "A"
    };
    auto const maxs = std::numeric_limits<std::size_t>::max();

    // Fully automatic (no optional params given).
    auto const factor1 = glm_factor( data.begin(), data.end() );
    auto levels1 = std::vector<std::string>{ "A", "B", "C", "D" };
    auto values1 = std::vector<size_t>{ 1, 0, 1, 0, 2, 2, 1, 0, 0, 2, 3, 0 };
    EXPECT_EQ( levels1.size(), factor1.levels.size() );
    EXPECT_EQ( values1.size(), factor1.values.size() );
    EXPECT_EQ( levels1, factor1.levels );
    EXPECT_EQ( values1, factor1.values );

    // With given list of levels, leaving out "D", and different order.
    auto const factor2 = glm_factor( data.begin(), data.end(), { "C", "B", "A" } );
    auto levels2 = std::vector<std::string>{ "C", "B", "A" };
    auto values2 = std::vector<size_t>{ 1, 2, 1, 2, 0, 0, 1, 2, 2, 0, maxs, 2 };
    EXPECT_EQ( levels2.size(), factor2.levels.size() );
    EXPECT_EQ( values2.size(), factor2.values.size() );
    EXPECT_EQ( levels2, factor2.levels );
    EXPECT_EQ( values2, factor2.values );

    // With given exlusion list of levels.
    auto const factor3 = glm_factor( data.begin(), data.end(), {}, { "B" } );
    auto levels3 = std::vector<std::string>{ "A", "C", "D" };
    auto values3 = std::vector<size_t>{ maxs, 0, maxs, 0, 1, 1, maxs, 0, 0, 1, 2, 0  };
    EXPECT_EQ( levels3.size(), factor3.levels.size() );
    EXPECT_EQ( values3.size(), factor3.values.size() );
    EXPECT_EQ( levels3, factor3.levels );
    EXPECT_EQ( values3, factor3.values );
}

TEST( Math, GlmIndicatorVariables )
{
    auto const data = std::vector<std::string>{
        "B", "A", "B", "A", "C", "C", "B", "A", "A", "C", "D", "A"
    };

    // Basic test
    auto const factor1 = glm_factor( data.begin(), data.end() );
    auto const df_iv1 = glm_indicator_variables( factor1 );
    auto const col10  = std::vector<double>{ 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    auto const col11  = std::vector<double>{ 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 };
    auto const col12  = std::vector<double>{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
    ASSERT_EQ( 3, df_iv1.cols() );
    EXPECT_EQ( 12, df_iv1.rows() );
    EXPECT_EQ( col10, df_iv1[0].as<double>().to_vector() );
    EXPECT_EQ( col11, df_iv1[1].as<double>().to_vector() );
    EXPECT_EQ( col12, df_iv1[2].as<double>().to_vector() );

    auto const nan = std::numeric_limits<double>::quiet_NaN();

    // With excluded value and different order.
    auto const factor2 = glm_factor( data.begin(), data.end(), { "C", "B", "A" }, { "D" } );
    auto const df_iv2 = glm_indicator_variables( factor2, std::string( "B" ));
    auto const col20  = std::vector<double>{ 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, nan, 0.0 };
    auto const col21  = std::vector<double>{ 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, nan, 1.0 };
    ASSERT_EQ( 2, df_iv2.cols() );
    ASSERT_EQ( 12, df_iv2.rows() );

    // Have to compare manually because of nans
    for( size_t i = 0; i < 12; ++i ) {
        if( std::isnan( col20[i] )) {
            EXPECT_TRUE( std::isnan( df_iv2[0].get<double>(i) ));
        } else {
            EXPECT_EQ( col20[i], df_iv2[0].get<double>(i) );
        }
        if( std::isnan( col21[i] )) {
            EXPECT_TRUE( std::isnan( df_iv2[1].get<double>(i) ));
        } else {
            EXPECT_EQ( col21[i], df_iv2[1].get<double>(i) );
        }
    }
    // EXPECT_EQ( col20, df_iv2[0].to_vector() );
    // EXPECT_EQ( col21, df_iv2[1].to_vector() );

    // Error check.
    EXPECT_ANY_THROW( glm_indicator_variables( factor2, std::string( "X" )));
}

TEST( Math, SimpleLinearRegression )
{
    NEEDS_TEST_DATA;

    // Read data
    auto const infile = environment->data_dir + "utils/csv/linear_regression.csv";
    auto reader = DataframeReader<double>();
    reader.row_names_from_first_col(false);
    auto const data = reader.read( from_file( infile ));

    // Perform regression
    auto const slr = simple_linear_regression(
        data["x1"].as<double>().begin(), data["x1"].as<double>().end(),
        data["x5"].as<double>().begin(), data["x5"].as<double>().end()
    );

    // Check results
    EXPECT_NEAR( 0.67483, slr.slope,     0.00001 );
    EXPECT_NEAR( 1.09682, slr.intercept, 0.00001 );
}

TEST( Math, GlmGaussSimple )
{
    NEEDS_TEST_DATA;

    // Read data
    auto const infile = environment->data_dir + "utils/csv/linear_regression.csv";
    auto dfr = DataframeReader<double>();
    dfr.row_names_from_first_col(false);
    auto const data = dfr.read( from_file( infile ));

    // Set up
    int N = data.rows();
    int M = 1;
    auto x = Matrix<double>(N, M);
    x.col(0) = data[ "x1" ].as<double>();

    // Run
    auto const result = glm_fit( x, data[ "x5" ].as<double>(), glm_family_gaussian() );

    // Allowed error for double comparisons.
    double delta = 0.00001;

    // Gaussian algo should not iterate at all.
    EXPECT_TRUE( result.converged );
    EXPECT_EQ( 0, result.num_iterations );

    // Check expected result vectors and matrices.
    // Results were obtained from R, using the script test/data/utils/csv/linear_regression.R
    // We also compared these results to the original snp.matrix and X.snp.matrix implementation,
    // just to be sure. Also works!
    auto xb_exp = Matrix<double>(N, M, {
        -0.986381, 0.063619, 0.403619, 0.443619, -0.296381, -0.376381, 0.073619, -0.246381,
        -0.836381, 0.213619, -0.156381, 0.533619, -0.366381, 0.843619, 0.203619, -0.426381,
        0.863619, -0.936381, 0.143619, 0.343619, -0.656381, 0.443619, 0.373619, 0.413619,
        -0.00638095, -0.846381, 0.203619, -0.406381, 0.263619, 0.333619, -0.246381, 0.343619,
        0.133619, 0.033619, -0.556381, -0.366381, 0.243619, 0.533619, 0.483619, 0.163619,
        0.013619, 0.643619, 0.433619, 0.483619, 0.823619, -0.596381, -0.166381, 0.683619,
        -0.436381, -0.716381, 0.733619, -0.886381, -0.416381, 0.793619, -0.456381, -0.536381,
        -0.266381, 0.093619, 0.043619, 0.433619, -0.246381, -0.596381, 0.383619, 0.163619,
        -0.056381, 0.343619, 0.363619, 0.543619, -0.336381, -0.316381, 0.923619, -0.616381,
        -0.926381, 0.263619, 0.593619, 0.133619, -0.886381, 0.833619, 0.403619, -0.836381,
        -0.236381, -0.366381, 0.383619, 0.203619, -0.186381, -0.556381, 0.063619, -0.976381,
        0.603619, 0.603619, -0.666381, -0.016381, 0.113619, -0.256381, -0.596381, 0.213619,
        0.743619, -0.786381, -1.04638, -0.486381, 0.643619, 0.343619, 0.403619, 0.203619,
        0.433619
    });
    auto fitted_exp = std::vector<double>({
        2.50721777999918, 3.21578917861934, 3.44523134579158, 3.47222454192949, 2.97285041337815,
        2.91886402110233, 3.22253747765382, 3.00659190855054, 2.60844226551636, 3.31701366413651,
        3.06732659986084, 3.53295923323979, 2.92561232013681, 3.7421565033086, 3.31026536510203,
        2.88512252592994, 3.75565310137755, 2.54095927517158, 3.26977557089516, 3.40474155158472,
        2.72991164813696, 3.47222454192949, 3.42498644868815, 3.45197964482606, 3.168551085378,
        2.60169396648188, 3.31026536510203, 2.8986191239989, 3.3507551593089, 3.39799325255024,
        3.00659190855054, 3.40474155158472, 3.26302727186069, 3.19554428151591, 2.79739463848173,
        2.92561232013681, 3.33725856123994, 3.53295923323979, 3.4992177380674, 3.28327216896412,
        3.18204768344696, 3.60719052261905, 3.46547624289501, 3.4992177380674, 3.72865990523964,
        2.77040144234382, 3.06057830082636, 3.63418371875696, 2.87837422689546, 2.68942185393009,
        3.66792521392934, 2.57470077034397, 2.89187082496442, 3.70841500813621, 2.86487762882651,
        2.81089123655069, 2.99309531048158, 3.23603407572278, 3.20229258055039, 3.46547624289501,
        3.00659190855054, 2.77040144234382, 3.43173474772263, 3.28327216896412, 3.13480959020561,
        3.40474155158472, 3.41823814965367, 3.53970753227427, 2.94585721724024, 2.95935381530919,
        3.79614289558442, 2.75690484427487, 2.54770757420606, 3.3507551593089, 3.57344902744666,
        3.26302727186069, 2.57470077034397, 3.73540820427412, 3.44523134579158, 2.60844226551636,
        3.01334020758501, 2.92561232013681, 3.43173474772263, 3.31026536510203, 3.0470817027574,
        2.79739463848173, 3.21578917861934, 2.51396607903367, 3.58019732648113, 3.58019732648113,
        2.72316334910248, 3.16180278634352, 3.24953067379173, 2.99984360951606, 2.77040144234382,
        3.31701366413651, 3.67467351296382, 2.64218376068875, 2.46672798579233, 2.84463273172307,
        3.60719052261905, 3.40474155158472, 3.44523134579158, 3.31026536510203, 3.46547624289501
    });
    auto resid_exp = std::vector<double>({
        0.912782220000818, 0.184210821380657, 0.144768654208418, 0.0677754580705074,
        -0.0628504133781491, 0.251135978897671, -0.052537477653821, -0.00659190855053746,
        -0.268442265516359, 0.0929863358634926, 0.102673400139164, 0.0770407667602088,
        -0.575612320136806, -0.0921565033085972, 0.17973463489797, 0.394877474070059,
        -0.0456531013775523, -0.000959275171582299, 0.200224429104836, -0.394741551584717,
        0.290088351863045, -0.102224541929493, 0.0950135513118506, -0.03197964482606,
        0.081448914622, -0.311693966481881, -0.0202653651020298, 0.181380876001104,
        0.229244840691105, 0.0020067474497609, 0.0634080914494624, -0.234741551584716,
        -0.253027271860687, 0.21445571848409, -0.527394638481732, 0.264387679863194,
        0.0327414387600598, -0.252959233239791, -0.119217738067403, 0.0967278310358803,
        0.107952316553045, 0.102809477380955, -0.145476242895015, 0.120782261932597,
        -0.138659905239642, -0.400401442343821, 0.219421699173641, 0.00581628124304504,
        -0.478374226895463, -0.449421853930089, -0.0679252139293433, -0.36470077034397,
        0.188129175035582, -0.068415008136209, 0.255122371173492, -0.290891236550687,
        0.206904689518418, 0.173965924277224, 0.0777074194496116, -0.125476242895015,
        0.383408091449463, -0.690401442343821, 0.278265252277373, 0.186727831035881,
        0.145190409794388, 0.0352584484152835, 0.101761850346328, -0.029707532274269,
        -0.115857217240239, -0.619353815309194, -0.0361428955844182, 0.523095155725134,
        -0.29770757420606, 0.0692448406911044, -0.103449027446657, 0.156972728139313,
        0.18529922965603, -0.00540820427411955, 0.0647686542084177, 0.601557734483641,
        -0.613340207585015, -0.545612320136806, -0.0217347477226267, 0.16973463489797,
        0.422918297242597, -0.397394638481732, 0.154210821380657, -0.143966079033672,
        0.0198026735188656, -0.180197326481135, 0.466836650897522, 0.0781972136564777,
        0.0304693262082684, -0.59984360951606, -0.530401442343821, -0.0270136641365075,
        0.135326487036179, 0.357816239311254, 0.273272014207672, 0.345367268276925,
        -0.117190522619045, 0.0952584484152836, -0.055231345791582, -0.000265365102029735,
        0.114523757104985
    });

    EXPECT_EQ( 1, result.rank );
    EXPECT_EQ( 103, result.df_resid );
    EXPECT_NEAR( 0.0792109, result.scale, delta );

    EXPECT_NEAR( 20.7981, result.null_deviance, 0.0001 );
    EXPECT_NEAR( 8.1587, result.deviance, 0.0001 );

    EXPECT_ITERABLE_DOUBLE_NEAR( Matrix<double>, xb_exp, result.Xb, delta );
    EXPECT_ITERABLE_DOUBLE_NEAR( std::vector<double>, fitted_exp, result.fitted, delta );
    EXPECT_ITERABLE_DOUBLE_NEAR( std::vector<double>, resid_exp, result.resid, delta );

    // In this case, all weights are 1. No need for storing them in a vector.
    EXPECT_EQ( N, result.weights.size() );
    for( size_t i = 0; i < result.weights.size(); ++i ) {
        EXPECT_NEAR( 1.0, result.weights[i], delta );
    }

    EXPECT_EQ( 1, result.which.size() );
    EXPECT_EQ( 1, result.betaQ.size() );
    EXPECT_EQ( 1, result.tri.size() );
    EXPECT_NEAR( 0.0, result.which[0], delta );
    EXPECT_NEAR( 0.6748299, result.betaQ[0], delta );
    EXPECT_NEAR( 27.7548, result.tri[0], 0.0001 );
}

TEST( Math, GlmGaussNoIntercept )
{
    NEEDS_TEST_DATA;

    // Read data
    auto const infile = environment->data_dir + "utils/csv/linear_regression.csv";
    auto dfr = DataframeReader<double>();
    dfr.row_names_from_first_col(false);
    auto const data = dfr.read( from_file( infile ));

    int N = data.rows();
    int M = 1;
    auto x = Matrix<double>(N, M);
    x.col(0) = data[ "x1" ].as<double>();

    // No intercept
    GlmExtras extras;
    extras.with_intercept = false;

    // Run
    auto const result = glm_fit( x, data[ "x5" ].as<double>(), glm_family_gaussian(), extras );

    // Allowed error for double comparisons.
    double delta = 0.00001;

    // Gaussian algo should not iterate at all.
    EXPECT_TRUE( result.converged );
    EXPECT_EQ( 0, result.num_iterations );

    // Check expected result vectors and matrices.
    // Results were obtained from R, using the script test/data/utils/csv/linear_regression.R
    // We also compared these results to the original snp.matrix and X.snp.matrix implementation,
    // just to be sure. Also works!
    auto xb_exp = Matrix<double>(N, M, {
        2.09, 3.14, 3.48, 3.52, 2.78, 2.7, 3.15, 2.83, 2.24, 3.29, 2.92, 3.61, 2.71, 3.92, 3.28,
        2.65, 3.94, 2.14, 3.22, 3.42, 2.42, 3.52, 3.45, 3.49, 3.07, 2.23, 3.28, 2.67, 3.34, 3.41,
        2.83, 3.42, 3.21, 3.11, 2.52, 2.71, 3.32, 3.61, 3.56, 3.24, 3.09, 3.72, 3.51, 3.56, 3.9,
        2.48, 2.91, 3.76, 2.64, 2.36, 3.81, 2.19, 2.66, 3.87, 2.62, 2.54, 2.81, 3.17, 3.12, 3.51,
        2.83, 2.48, 3.46, 3.24, 3.02, 3.42, 3.44, 3.62, 2.74, 2.76, 4, 2.46, 2.15, 3.34, 3.67,
        3.21, 2.19, 3.91, 3.48, 2.24, 2.84, 2.71, 3.46, 3.28, 2.89, 2.52, 3.14, 2.1, 3.68, 3.68,
        2.41, 3.06, 3.19, 2.82, 2.48, 3.29, 3.82, 2.29, 2.03, 2.59, 3.72, 3.42, 3.48, 3.28, 3.51
    });
    auto fitted_exp = std::vector<double>({
        2.13529657732442, 3.20805323100412, 3.55542205219565, 3.59628897233583, 2.8402509497425,
        2.75851710946214, 3.21826996103917, 2.89133459991773, 2.28854752785007, 3.3613041815298,
        2.98328517023313, 3.68823954265124, 2.76873383949719, 4.00495817373763, 3.35108745149475,
        2.70743345928692, 4.02539163380772, 2.18638022749962, 3.28978707128448, 3.49412167198538,
        2.47244866848088, 3.59628897233583, 3.52477186209052, 3.5656387822307, 3.13653612075881,
        2.27833079781503, 3.35108745149475, 2.72786691935701, 3.41238783170502, 3.48390494195034,
        2.89133459991773, 3.49412167198538, 3.27957034124944, 3.17740304089899, 2.57461596883133,
        2.76873383949719, 3.39195437163493, 3.68823954265124, 3.63715589247601, 3.31022053135457,
        3.1569695808289, 3.80062357303673, 3.58607224230079, 3.63715589247601, 3.98452471366754,
        2.53374904869115, 2.97306844019809, 3.84149049317691, 2.69721672925187, 2.41114828827061,
        3.89257414335214, 2.23746387767485, 2.71765018932196, 3.95387452356241, 2.67678326918178,
        2.59504942890142, 2.87090113984764, 3.23870342110926, 3.18761977093403, 3.58607224230079,
        2.89133459991773, 2.53374904869115, 3.53498859212556, 3.31022053135457, 3.08545247058358,
        3.49412167198538, 3.51455513205547, 3.69845627268628, 2.79938402960232, 2.81981748967241,
        4.08669201401799, 2.51331558862106, 2.19659695753467, 3.41238783170502, 3.74953992286151,
        3.27957034124944, 2.23746387767485, 3.99474144370259, 3.55542205219565, 2.28854752785007,
        2.90155132995277, 2.76873383949719, 3.53498859212556, 3.35108745149475, 2.952634980128,
        2.57461596883133, 3.20805323100412, 2.14551330735944, 3.75975665289655, 3.75975665289655,
        2.46223193844584, 3.12631939072376, 3.25913688117935, 2.88111786988268, 2.53374904869115,
        3.3613041815298, 3.90279087338718, 2.3396311780253, 2.07399619711413, 2.64613307907665,
        3.80062357303673, 3.49412167198538, 3.55542205219565, 3.35108745149475, 3.58607224230079
    });
    auto resid_exp = std::vector<double>({
        1.28470342267558, 0.191946768995878, 0.0345779478043488, -0.0562889723358307,
        0.0697490502574973, 0.411482890537857, -0.0482699610391673, 0.108665400082272,
        0.0514524721499253, 0.0486958184702034, 0.186714829766868, -0.0782395426512358,
        -0.418733839497188, -0.35495817373763, 0.138912548505249, 0.572566540713082,
        -0.31539163380772, 0.353619772500375, 0.180212928715518, -0.484121671985381,
        0.547551331519116, -0.226288972335831, -0.00477186209051664, -0.145638782230696,
        0.113463879241193, 0.0116692021849705, -0.0610874514947514, 0.352133080642992,
        0.167612168294979, -0.0839049419503365, 0.178665400082272, -0.324121671985381,
        -0.269570341249437, 0.232596959101014, -0.304615968831334, 0.421266160502812,
        -0.0219543716349311, -0.408239542651236, -0.257155892476011, 0.0697794686454278,
        0.133030419171103, -0.0906235730367304, -0.266072242300786, -0.0171558924760108,
        -0.39452471366754, -0.163749048691154, 0.306931559801912, -0.20149049317691,
        -0.297216729251874, -0.171148288270614, -0.292574143352136, -0.0274638776748495,
        0.362349810678037, -0.313874523562405, 0.443216730818217, -0.0750494289014235,
        0.329098860152363, 0.171296578890743, 0.0923802290659678, -0.246072242300786,
        0.498665400082272, -0.453749048691154, 0.175011407874439, 0.159779468645428,
        0.194547529416418, -0.054121671985381, 0.00544486794452875, -0.188456272686281,
        0.0306159703976765, -0.479817489672413, -0.32669201401799, 0.766684411378936,
        0.0534030424653308, 0.00761216829497846, -0.279539922861505, 0.140429658750564,
        0.52253612232515, -0.264741443702585, -0.0454220521956512, 0.921452472149925,
        -0.501551329952773, -0.388733839497188, -0.124988592125561, 0.128912548505249,
        0.517365019872002, -0.174615968831334, 0.161946768995878, 0.224486692640556,
        -0.159756652896551, -0.359756652896551, 0.727768061554161, 0.113680609276238,
        0.0208631188206528, -0.481117869882683, -0.293749048691154, -0.0713041815297968,
        -0.0927908733871806, 0.660368821974701, 0.666003802885871, 0.543866920923352,
        -0.31062357303673, 0.00587832801461904, -0.165422052195651, -0.0410874514947514,
        -0.00607224230078576
    });

    EXPECT_EQ( 1, result.rank );
    EXPECT_EQ( 103, result.df_resid );
    EXPECT_NEAR( 0.112533, result.scale, delta );

    EXPECT_NEAR( 1077.835, result.null_deviance, 0.001 );
    EXPECT_NEAR( 11.591, result.deviance, 0.001 );

    EXPECT_ITERABLE_DOUBLE_NEAR( Matrix<double>, xb_exp, result.Xb, delta );
    EXPECT_ITERABLE_DOUBLE_NEAR( std::vector<double>, fitted_exp, result.fitted, delta );
    EXPECT_ITERABLE_DOUBLE_NEAR( std::vector<double>, resid_exp, result.resid, delta );

    // In this case, all weights are 1. No need for storing them in a vector.
    EXPECT_EQ( N, result.weights.size() );
    for( size_t i = 0; i < result.weights.size(); ++i ) {
        EXPECT_NEAR( 1.0, result.weights[i], delta );
    }

    EXPECT_EQ( 1, result.which.size() );
    EXPECT_EQ( 1, result.betaQ.size() );
    EXPECT_EQ( 1, result.tri.size() );
    EXPECT_NEAR( 0.0, result.which[0], delta );
    EXPECT_NEAR( 1.021673, result.betaQ[0], delta );
    EXPECT_NEAR( 1021.4874, result.tri[0], 0.0001 );
}

TEST( Math, GlmBinomial )
{
    NEEDS_TEST_DATA;

    // Read data
    auto const infile = environment->data_dir + "utils/csv/logistic_regression.csv";
    auto dfr = DataframeReader<double>();
    dfr.row_names_from_first_col(false);
    auto const data = dfr.read( from_file( infile ));

    int N = data.rows();
    int M = 1;
    auto x = Matrix<double>(N, M);
    x.col(0) = data[ "Hours" ].as<double>();

    // No intercept
    GlmExtras extras;
    // extras.with_intercept = false;
    // extras.residual_type = GlmExtras::kPearsonResiduals;

    // Run
    auto const result = glm_fit( x, data[ "Pass" ].as<double>(), glm_family_binomial(), extras );

    // Allowed error for double comparisons.
    double delta = 0.00001;

    // Gaussian algo should not iterate at all.
    EXPECT_TRUE( result.converged );

    // Check expected result vectors and matrices.
    // Results were obtained from R, using the script test/data/utils/csv/logistic_regression.R

    auto fitted_exp = std::vector<double>({
        0.0347228, 0.049788, 0.0709093, 0.100047, 0.139363, 0.190852, 0.190769, 0.255714,
        0.333522, 0.421626, 0.51501, 0.607373, 0.692598, 0.766568, 0.874418, 0.910251,
        0.936601, 0.955592, 0.969082, 0.985186
    });
    // auto resid_exp = std::vector<double>({
    //     -0.252506, -0.304082, -0.365612, -0.438609, -0.524547, -0.624673, 1.8602, -0.739748,
    //     1.52013, -1.01364, 1.1856, -1.33347, 0.883778, -1.67365, 0.534062, 0.446733, 0.372485,
    //      0.309859, 0.257338, 0.176942
    // });

    EXPECT_EQ( 1, result.rank );
    EXPECT_EQ( 18, result.df_resid );
    EXPECT_NEAR( 1.0, result.scale, delta );

    EXPECT_NEAR( 27.726, result.null_deviance, 0.01 );
    EXPECT_NEAR( 16.060, result.deviance, 0.01 );

    // LOG_DBG << "result.fitted " << join( result.fitted );
    // LOG_DBG << "result.resid " << join( result.resid );

    // EXPECT_ITERABLE_DOUBLE_NEAR( Matrix<double>, xb_exp, result.Xb, delta );
    EXPECT_ITERABLE_DOUBLE_NEAR( std::vector<double>, fitted_exp, result.fitted, delta );
    // EXPECT_ITERABLE_DOUBLE_NEAR( std::vector<double>, resid_exp, result.resid, delta );

    // In this case, all weights are 1. No need for storing them in a vector.
    // EXPECT_EQ( N, result.weights.size() );
    // for( size_t i = 0; i < result.weights.size(); ++i ) {
    //     EXPECT_NEAR( 1.0, result.weights[i], delta );
    // }

    EXPECT_EQ( 1, result.which.size() );
    EXPECT_EQ( 1, result.betaQ.size() );
    EXPECT_EQ( 1, result.tri.size() );
    EXPECT_NEAR( 0.0, result.which[0], delta );
    EXPECT_NEAR( 1.504645, result.betaQ[0], delta );
    EXPECT_NEAR( 2.530456, result.tri[0], 0.0001 );
}

TEST( Math, GlmDataframe )
{
    NEEDS_TEST_DATA;

    // Read data
    auto const infile = environment->data_dir + "utils/csv/mixed.csv";
    auto const dfr = DataframeReader<std::string>();
    auto const dfs = dfr.read( from_file( infile ));

    std::string report;
    auto const dfd = glm_prepare_dataframe( dfs, report );
    // LOG_DBG << join(dfd.col_names());
    // LOG_DBG << report;

    auto const md = glm_convert_dataframe( dfd, {
        "alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota","kappa"
    });
    // LOG_DBG << md;

    auto const md_exp = Matrix<double>( 10, 6, {
        0, 0, 4.5, 1, 0, 0,
        1, 8, 5, 1, 0, 1,
        1, 8, 4.7, 1, 1, 0,
        1, 8, 5.3, 0, 0, 0,
        1, 10, 5.5, 0, 0, 0,
        1, 10, 5.3, 0, 0, 1,
        1, 10, 5.3, 1, 0, 0,
        1, 8, 5.3, 1, 1, 0,
        1, 1, 5.3, 1, 0, 0,
        0, 0, 5, 0, 1, 0
    });

    EXPECT_EQ( md_exp, md );
}
