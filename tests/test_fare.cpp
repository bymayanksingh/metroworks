#include "test_runner.hpp"
#include "metroworks/fare_calculator.hpp"

using namespace metroworks;

TEST_CASE(FareTest, BasicMatrixLookup) {
    FareCalculator calc;
    std::vector<std::vector<int>> matrix = {
        {10, 20, 30},
        {20, 10, 40},
        {30, 40, 10}
    };
    calc.setMatrix(matrix);

    ASSERT_TRUE(calc.isLoaded());
    ASSERT_EQ(calc.rowCount(), 3);
    ASSERT_EQ(calc.colCount(), 3);

    auto fare12 = calc.getFare(1, 2);
    ASSERT_TRUE(fare12.has_value());
    ASSERT_EQ(*fare12, 20);

    auto fare32 = calc.getFare(3, 2);
    ASSERT_TRUE(fare32.has_value());
    ASSERT_EQ(*fare32, 40);
}

TEST_CASE(FareTest, OutOfBoundsSafety) {
    FareCalculator calc;
    std::vector<std::vector<int>> matrix = {
        {10, 20},
        {20, 10}
    };
    calc.setMatrix(matrix);

    // Negative or 0 index
    ASSERT_FALSE(calc.getFare(0, 1).has_value());
    ASSERT_FALSE(calc.getFare(-1, 1).has_value());

    // Beyond matrix bounds
    ASSERT_FALSE(calc.getFare(5, 1).has_value());
    ASSERT_FALSE(calc.getFare(1, 5).has_value());
}

TEST_CASE(FareTest, MultiCandidateLookup) {
    FareCalculator calc;
    std::vector<std::vector<int>> matrix = {
        {10, 20, 30},
        {20, 10, 40},
        {30, 40, 10}
    };
    calc.setMatrix(matrix);

    std::vector<StationId> srcCandidates = {1, 2};
    std::vector<StationId> dstCandidates = {3};

    auto fare = calc.getFare(srcCandidates, dstCandidates);
    ASSERT_TRUE(fare.has_value());
    // Candidate (1, 3) gives 30, candidate (2, 3) gives 40 -> min is 30
    ASSERT_EQ(*fare, 30);
}

