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

TEST_CASE(FareTest, DistanceSlabs2026Official) {
    // 0 km: 0
    auto f0 = FareCalculator::calculateFromDistance(0.0);
    ASSERT_EQ(f0.standardFare, 0);

    // 0-2 km: Rs 11
    auto f1 = FareCalculator::calculateFromDistance(1.5);
    ASSERT_EQ(f1.standardFare, 11);
    ASSERT_EQ(f1.smartCardFare, 10);
    ASSERT_EQ(f1.sundayFare, 11);

    // 2-5 km: Rs 21
    auto f2 = FareCalculator::calculateFromDistance(4.5);
    ASSERT_EQ(f2.standardFare, 21);
    ASSERT_EQ(f2.smartCardFare, 19);
    ASSERT_EQ(f2.sundayFare, 11);

    // 5-12 km: Rs 32
    auto f3 = FareCalculator::calculateFromDistance(10.0);
    ASSERT_EQ(f3.standardFare, 32);
    ASSERT_EQ(f3.smartCardFare, 29);
    ASSERT_EQ(f3.sundayFare, 21);

    // 12-21 km: Rs 43
    auto f4 = FareCalculator::calculateFromDistance(18.0);
    ASSERT_EQ(f4.standardFare, 43);
    ASSERT_EQ(f4.smartCardFare, 39);
    ASSERT_EQ(f4.sundayFare, 32);

    // 21-32 km: Rs 54
    auto f5 = FareCalculator::calculateFromDistance(28.0);
    ASSERT_EQ(f5.standardFare, 54);
    ASSERT_EQ(f5.smartCardFare, 49);
    ASSERT_EQ(f5.sundayFare, 43);

    // >32 km: Rs 64
    auto f6 = FareCalculator::calculateFromDistance(42.0);
    ASSERT_EQ(f6.standardFare, 64);
    ASSERT_EQ(f6.smartCardFare, 58);
    ASSERT_EQ(f6.sundayFare, 54);
}

