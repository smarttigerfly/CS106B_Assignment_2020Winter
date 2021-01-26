#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "Testing/TestDriver.h"
#include "GUIMain.h"
#include "error.h"
using namespace std;

namespace {
    /* Type representing how a test turned out. */
    enum class TestResult {
        WAITING,
        RUNNING,
        PASS,
        FAIL,
        EXCEPTION
    };

    /* Type representing a single test. */
    struct Test {
        string name;
        function<void()> callback;

        TestResult result;
        string detailMessage;
    };

    /* Type representing a group of tests. */
    struct TestGroup {
        string name;

        Vector<Test> tests;
        size_t numTests;
        size_t numPassed;
    };

    /* Runs a single test. */
    void runSingleTest(Test& test, TestGroup& group) {
        try {
            /* Run the test. */
            test.callback();
            test.result = TestResult::PASS;
            group.numPassed++;
        } catch (const TestFailedException& e) {
            test.result = TestResult::FAIL;
            ostringstream out;
            out << "    " << e.what() << endl;
            test.detailMessage = out.str();
        } catch (const ErrorException& e) {
            test.result = TestResult::EXCEPTION;
            ostringstream out;
            out << "    Test failed due to the program triggering an ErrorException." << endl;
            out << endl;
            out << "    This means that the test did not fail because of a call" << endl;
            out << "    to EXPECT() or EXPECT_ERROR() failing, but rather because" << endl;
            out << "    some code explicitly called the error() function." << endl;
            out << endl;
            out << "    Error: " << e.getMessage() << endl;
            test.detailMessage = out.str();
        } catch (const exception& e) {
            test.result = TestResult::EXCEPTION;
            ostringstream out;
            out << "    Test failed due to the program triggering an exception." << endl;
            out << endl;
            out << "    This means that the test did not fail because of a call" << endl;
            out << "    to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
            out << "    some code - probably an internal C++ library - triggered" << endl;
            out << "    an error." << endl;
            out << endl;
            out << "    Error: " << e.what() << endl;
            test.detailMessage = out.str();
        } catch (...) {
            test.result = TestResult::EXCEPTION;
            ostringstream out;
            out << "    Test failed due to the program triggering an unknown type" << endl;
            out << "    of exception. " << endl;
            out << endl;
            out << "    This means that the test did not fail because of a call" << endl;
            out << "    to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
            out << "    some code triggered an error whose format we couldn't" << endl;
            out << "    recognize." << endl;
            out << endl;
            test.detailMessage = out.str();
        }
    }

    /* Displays all the results from the given test group. */
    void displayResults(GColorConsole& console, const Vector<TestGroup>& testGroups) {
        console.clearDisplay();

        for (const auto& group: testGroups) {
            /* Draw Header */
            console.doWithStyle("#000080", GColorConsole::BOLD, [&] {
                console << "==== Tests for " << group.name << " ====" << '\n';
            });

            /* Draw each test */
            for (const auto& test: group.tests) {
                if (test.result == TestResult::WAITING) {
                    console.doWithStyle("#A0A0A0", GColorConsole::ITALIC, [&] {
                        console << "         " << test.name << '\n';
                    });
                } else if (test.result == TestResult::RUNNING) {
                    console.doWithStyle("#0000FF", GColorConsole::ITALIC, [&] {
                        console << "running: " << test.name << '\n';
                    });
                } else if (test.result == TestResult::PASS) {
                    console.doWithStyle("#404040", GColorConsole::ITALIC, [&] {
                        console << "  pass:  " << test.name << '\n';
                    });
                } else if (test.result == TestResult::FAIL) {
                    console.doWithStyle("#800000", GColorConsole::BOLD, [&] {
                        console << "  FAIL:  " << test.name << '\n';
                        console << "    " << test.detailMessage << '\n';
                    });
                }  else if (test.result == TestResult::EXCEPTION) {
                    console.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                        console << "  FAIL:  " << test.name << '\n';
                        console << "    " << test.detailMessage << '\n';
                    });
                }
            }

            /* Draw the overall result. */
            if (group.numPassed == group.numTests && group.numTests > 0) {
                console.doWithStyle("#0000FF", GColorConsole::ITALIC, [&] {
                    console << "All tests in this section passed!" << '\n';
                    console << '\n';
                    console << '\n';
                });
            } else {
                console.doWithStyle("#400000", GColorConsole::BOLD, [&] {
                    console << "Summary: " << group.numPassed << " / " << group.numTests
                         << " test" << (group.numTests == 1? "" : "s") << " passed." << '\n';
                    console << '\n';
                    console << '\n';
                });
            }
        }

        console.flush();
    }

    /* Given raw data from the testing system, produce a visual TestGroup for that data. */
    TestGroup testsToGroup(const pair<const TestKey, multimap<int, pair<string, TestCase>>>& entry) {
        TestGroup result;
        result.name = entry.first.second;
        result.numPassed = 0;
        result.numTests = entry.second.size();

        for (const auto& rawTest: entry.second) {
            Test test;
            test.name = rawTest.second.first;
            test.result = TestResult::WAITING; // It hasn't run yet
            test.callback = rawTest.second.second;
            result.tests += test;
        }

        return result;
    }

    /* Runs the specified tests, updating the display as things progress. */
    void runTests(GColorConsole& console,
                  const map<TestKey, multimap<int, pair<string, TestCase>>>& tests) {
        /* Convert to a nicer format. */
        Vector<TestGroup> displayedTests;
        for (const auto& entry: tests) {
            displayedTests += testsToGroup(entry);
        }

        /* Show everything so there's some basic data available. */
        displayResults(console, displayedTests);

        /* Now, go run the tests. */
        for (auto& group: displayedTests) {
            for (auto& test: group.tests) {
                /* Make clear that we're running the test. */
                test.result = TestResult::RUNNING;
                displayResults(console, displayedTests);

                runSingleTest(test, group);
                displayResults(console, displayedTests);
            }
        }
    }
}

std::shared_ptr<ProblemHandler> makeTestingGUI(GWindow& window) {
    class TestingGUI: public ProblemHandler {
    public:
        TestingGUI(GWindow& window) {
            console = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
            setDemoOptionsEnabled(false);

            runTests(*console, gTestsMap());

            setDemoOptionsEnabled(true);
        }

    private:
        Temporary<GColorConsole> console;
    };

    return make_shared<TestingGUI>(window);
}
