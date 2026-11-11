#include "test_runner.hpp"

int main() {
    return metroworks::test::TestRegistry::instance().runAll();
}
