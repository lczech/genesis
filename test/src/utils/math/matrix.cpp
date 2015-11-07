/**
 * @brief Testing Matrix class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <sstream>

#include "lib/utils/math/matrix.hpp"
#include "lib/utils/math/matrix/operators.hpp"

using namespace genesis;

TEST(Matrix, Copy)
{
	auto m1 = Matrix<int>(2, 2, {1, 2, 3, 4});

	auto m2 = m1;
	auto m3(m1);

	EXPECT_EQ(m1, m2);
	EXPECT_EQ(m1, m3);

	EXPECT_TRUE(m1 == m2);
	EXPECT_TRUE(m1 == m3);

	EXPECT_FALSE(m1 != m2);
	EXPECT_FALSE(m1 != m3);

	m1(0, 1) = 9;

	EXPECT_NE(m1, m2);
	EXPECT_NE(m1, m3);

	EXPECT_FALSE(m1 == m2);
	EXPECT_FALSE(m1 == m3);

	EXPECT_TRUE(m1 != m2);
	EXPECT_TRUE(m1 != m3);

	EXPECT_EQ(9, m1(0, 1));
	EXPECT_EQ(2, m2(0, 1));
	EXPECT_EQ(2, m3(0, 1));
}

TEST(Matrix, InitializerList)
{
	EXPECT_THROW(
		Matrix<int>(2, 2, {1, 2, 3}),
		std::length_error
	);

	EXPECT_NO_THROW(
		Matrix<int>(2, 2, {1, 2, 3, 4})
	);

	EXPECT_THROW(
		Matrix<int>(2, 2, {1, 2, 3, 4, 5}),
		std::length_error
	);
}

TEST(Matrix, Access)
{
	auto m = Matrix<int>(2, 2, {1, 2, 3, 4});

	EXPECT_EQ(2, m.rows());
	EXPECT_EQ(2, m.cols());

	EXPECT_EQ(2, m(0, 1));

	EXPECT_THROW(
		m.at(0, 2),
		std::out_of_range
	);
}

TEST(Matrix, Ostream)
{
	auto m = Matrix<int>(2, 2, {1, 2, 3, 4});
	std::ostringstream s;
	s << m;
	EXPECT_EQ("1 2\n3 4\n", s.str());
}