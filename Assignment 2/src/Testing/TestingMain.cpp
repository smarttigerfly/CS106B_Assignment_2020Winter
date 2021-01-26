#include "Testing/TestDriver.h"
#include "error.h"
#include "simpio.h"
#include "console.h"
#include <iostream>
using namespace std;

/* Runs all the tests in a given test group. */
void runTestsIn(const string& name, const multimap<int, pair<string, TestCase>>& tests) {
    cout << "==== Testing " << name << " ====" << endl;

    size_t numTests = 0;
    size_t numSuccesses = 0;

    /* Run each test. */
    for (const auto& entry: tests) {
        const auto& test = entry.second;
        try {
            test.second();
            cout << "    PASS: " << test.first << endl;
            numSuccesses++;
        } catch (const TestFailedException& e) {
            cerr << " !! FAIL:  " << test.first << endl;
            cerr << "      " << e.what() << endl;
        } catch (const ErrorException& e) {
            cerr << " !! ERROR: " << test.first << endl;
            cerr << "      Test failed due to the program triggering an ErrorException." << endl;
            cerr << endl;
            cerr << "      This means that the test did not fail because of a call" << endl;
            cerr << "      to fail() or an expect() failing, but rather because" << endl;
            cerr << "      some code explicitly called the error() function." << endl;
            cerr << endl;
            cerr << "      Error: " << e.getMessage() << endl;
        } catch (const exception& e) {
            cerr << " !! ERROR: " << test.first << endl;
            cerr << "      Test failed due to the program triggering an exception." << endl;
            cerr << endl;
            cerr << "      This means that the test did not fail because of a call" << endl;
            cerr << "      to fail() or an expect() failing, but rather because" << endl;
            cerr << "      some code - probably an internal C++ library - triggered" << endl;
            cerr << "      an error." << endl;
            cerr << endl;
            cerr << "      Error: " << e.what() << endl;
        } catch (...) {
            cerr << " !! FAIL: " << test.first << endl;
            cerr << "      Test failed due to the program triggering an unknown type" << endl;
            cerr << "      of exception. " << endl;
            cerr << endl;
            cerr << "      This means that the test did not fail because of a call" << endl;
            cerr << "      to fail() or an expect() failing, but rather because" << endl;
            cerr << "      some code triggered an error whose format we couldn't" << endl;
            cerr << "      recognize." << endl;
            cerr << endl;
        }
        numTests++;
    }

    cout << endl;
    cout << "Summary: " << numSuccesses << " / " << numTests
         << " test" << (numSuccesses == 1? "" : "s") << " passed." << endl;
    getLine("Press ENTER to continue... ");
    cout << endl;
}

/* Required for multimain to work. Nothing to see here, folks; please move along. */
int main() {
    return 0;
}

int multimain(RunTests) {
    for (const auto& entry: gTestsMap()) {
        runTestsIn(entry.first.second, entry.second);
    }
    return 0;
}
