#include "Testing/TestDriver.h"
#include <string>
using namespace std;

/* Why this type? Because we want to be able to
 *
 * 1. find all the tests in a certain group,
 * 2. have them be sorted by line number, and
 * 3. then have a name associated with each.
 */
map<TestKey, multimap<int, pair<string, TestCase>>>& gTestsMap() {
    static map<TestKey, multimap<int, pair<string, TestCase>>> result;
    return result;
}

void doFail(const char* message, size_t line, const char* filename) {
    throw TestFailedException(message, line, filename);
}
void doExpect(bool condition, const char* expression, size_t line, const char* filename) {
    if (!condition) {
        doFail(expression, line, filename);
    }
}

/* TestFailedException implementation. */
TestFailedException::TestFailedException(const char* message, std::size_t line, const char*)
    : logic_error("Line " + to_string(line) + ": " + string(message)) {

}

/* TestCaseAdder implementation. */
TestCaseAdder::TestCaseAdder(const TestKey& key, int line, const string& name, TestCase test) {
    gTestsMap()[key].insert(make_pair(line, make_pair(name, test)));
}
