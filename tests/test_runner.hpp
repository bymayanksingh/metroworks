#ifndef METROWORKS_TEST_RUNNER_HPP
#define METROWORKS_TEST_RUNNER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

namespace metroworks::test {

struct TestCase {
    std::string suite;
    std::string name;
    std::function<void()> func;
};

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry reg;
        return reg;
    }

    void addTest(const std::string& suite, const std::string& name, std::function<void()> func) {
        tests_.push_back({suite, name, std::move(func)});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;

        std::cout << "\n==================================================\n";
        std::cout << "           RUNNING METROWORKS TEST SUITE          \n";
        std::cout << "==================================================\n\n";

        for (const auto& test : tests_) {
            std::cout << " RUNNING: [" << test.suite << "] " << test.name << " ... ";
            try {
                test.func();
                std::cout << "\033[32mPASSED\033[0m\n";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "\033[31mFAILED\033[0m\n";
                std::cout << "    Assertion failed: " << e.what() << "\n";
                failed++;
            } catch (...) {
                std::cout << "\033[31mFAILED\033[0m (unknown exception)\n";
                failed++;
            }
        }

        std::cout << "\n--------------------------------------------------\n";
        std::cout << "Total Tests: " << tests_.size() << " | "
                  << "\033[32mPassed: " << passed << "\033[0m | "
                  << (failed > 0 ? "\033[31m" : "") << "Failed: " << failed << "\033[0m\n";
        std::cout << "--------------------------------------------------\n\n";

        return (failed == 0) ? 0 : 1;
    }

private:
    std::vector<TestCase> tests_;
};

struct AutoRegister {
    AutoRegister(const std::string& suite, const std::string& name, std::function<void()> func) {
        TestRegistry::instance().addTest(suite, name, std::move(func));
    }
};

#define TEST_CASE(suite, name) \
    static void test_##suite##_##name(); \
    static metroworks::test::AutoRegister reg_##suite##_##name(#suite, #name, test_##suite##_##name); \
    static void test_##suite##_##name()

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error(std::string("Condition '") + #cond + "' failed at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(a, b) \
    do { \
        if (!((a) == (b))) { \
            throw std::runtime_error(std::string("Assertion '") + #a + " == " + #b + "' failed at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

#define ASSERT_NEAR(a, b, eps) \
    do { \
        if (std::abs((a) - (b)) > (eps)) { \
            throw std::runtime_error(std::string("Assertion '|") + #a + " - " + #b + "| <= " + #eps + "' failed at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

} // namespace metroworks::test

#endif // METROWORKS_TEST_RUNNER_HPP

