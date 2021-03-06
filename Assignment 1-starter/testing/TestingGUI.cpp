#include <QCoreApplication> // for application name
#include "gbrowserpane.h"
#include "gwindow.h"
#include "SimpleTest.h"
#include "TestDriver.h"
#include "TextUtils.h"
#include "MemoryDiagnostics.h"
#include "error.h"
#include "filelib.h"
#include <iomanip>
#include <map>
#include "simpio.h"
using namespace std;

Vector<string> gDetails;

void addDetail(const string& msg)
{
    gDetails.add(msg);
}

// hand prototype to avoid having map in exposed header (and leading students astray)
std::map<TestKey, std::multimap<int, TestCase>>& gTestsMap();

namespace SimpleTest {
    /* Type representing how a test turned out. */
    enum class TestResult {
        WAITING,
        RUNNING,
        PASS,
        FAIL,
        LEAK,
        EXCEPTION
    };

    /* Type representing a single test and its result. */
    struct Test {
        string testname;
        string id; // string identifying owner, module, line number
        std::function<void()> callback;
        TestResult result;
        string detailMessage;
    };

    /* Type representing a group of tests and whether group is selected to run. */
    struct TestGroup {
        string name;
        Vector<Test> tests;
        bool selected;
    };

    string id(const TestResult& tr)
    {
        switch (tr) {
            case TestResult::WAITING:   return "waiting";
            case TestResult::RUNNING:   return "running";
            case TestResult::PASS:      return "pass";
            case TestResult::FAIL:      return "fail";
            case TestResult::LEAK:      return "leak";
            case TestResult::EXCEPTION: return "exception";
        }
        return "";
    }

    string status(const TestResult& tr)
    {
        switch (tr) {
            case TestResult::WAITING:   return "&nbsp;";
            case TestResult::RUNNING:   return "Running...";
            case TestResult::PASS:      return "Correct";
            case TestResult::FAIL:      return "Incorrect";
            case TestResult::LEAK:      return "Leak";
            case TestResult::EXCEPTION: return "Exception";
        }
        return "";
    }
    string affirmation()
    {
        Vector<string> choices = {
            "Excellent!", "Good job!", "Good on ya!", "Sweet!", "Bien hecho!",
            "Way to go!", "Super!", "Awesome!", "Great!", "Perfect!", "Nice work!",
            "You rock!", "Love it!", "Right on!", "No complaints here!", "Que bien!",
            "Buen trabajo!", "Bravo!", "Fantastico!", "Perfecto!", "Lo hiciste!",
            "Muy bien!", "Jolly good!", "Congratulations!", "Nice going!",
            "Terrific!", "Keep it up!", "You did it!", "A+", "Take a bow!"
        };
        return randomElement(choices);
    }

    string sanitize(string s)
    {
        for (int i = 0; i < s.length(); i++) {
            if (s[i] == '<') s.replace(i, 1, "&lt;");
            if (s[i] == '>') s.replace(i, 1, "&gt;");
        }
        return s;
    }

    /* Runs a single test. */
    void runSingleTest(Test& test) {
        try {
            /* Reset memory counters so we don't have carryover across tests. */
            MemoryDiagnostics::clear();
            gDetails.clear();

            /* Run the test. */
            test.callback();

            /* grab any details accumulated during run */
            test.detailMessage = stringJoin(gDetails, "\n"); // will be overwritten in case of actual failure

            /* See if there were any memory leaks. */
            auto errors = MemoryDiagnostics::typesWithErrors();
            if (errors.empty()) {
                test.result = TestResult::PASS;
            } else {
                /* We have memory leaks. */
                test.result = TestResult::LEAK;

                ostringstream out;
                out << endl << "Test had allocation count mismatch with these types:" << endl;
                for (const auto& entry: errors) {
                    string type = entry.first;
                    int    delta = entry.second;

                    if (delta > 0) {
                        out << "    " << type << ": Leaked " << pluralize(delta, "object") << "." << endl;
                    } else {
                        out << "    " << type << ": Deallocated " << pluralize(-delta, "more object") << " than allocated." << endl;
                    }
                }

                test.detailMessage += out.str();
            }
        } catch (const TestFailedException& e) {
            test.result = TestResult::FAIL;
            ostringstream out;
            out << e.what() << endl;
            test.detailMessage = out.str();
        } catch (const ErrorException& e) {
            test.result = TestResult::EXCEPTION;
            ostringstream out;
            out << "Test failed due to the program triggering an ErrorException." << endl;
            out << endl;
            out << "This means that the test did not fail because of a call" << endl;
            out << "to EXPECT() or EXPECT_ERROR() failing, but rather because" << endl;
            out << "some code explicitly called the error() function." << endl;
            out << endl;
            out << "Error: " << e.getMessage();
            test.detailMessage = out.str();
        } catch (const exception& e) {
            test.result = TestResult::EXCEPTION;
            ostringstream out;
            out << "Test failed due to the program triggering an exception." << endl;
            out << endl;
            out << "This means that the test did not fail because of a call" << endl;
            out << "to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
            out << "some code - probably an internal C++ library - triggered" << endl;
            out << "an error." << endl;
            out << endl;
            out << "Error: " << e.what() << endl;
            test.detailMessage = out.str();
        } catch (...) {
            test.result = TestResult::EXCEPTION;
            ostringstream out;
            out << "Test failed due to the program triggering an unknown type" << endl;
            out << "of exception. " << endl;
            out << endl;
            out << "This means that the test did not fail because of a call" << endl;
            out << "to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
            out << "some code triggered an error whose format we couldn't" << endl;
            out << "recognize." << endl;
            out << endl;
            test.detailMessage = out.str();
        }
        string indented;
        for (auto& line : stringSplit(test.detailMessage, "\n")) {
            indented += "    " + line + "\n";
        }
        test.detailMessage = indented;
    }

    /* Displays all the results from the given test group. */
    string displayResults(GBrowserPane *bp, const string& stylesheet, const Vector<TestGroup>& testGroups, int npass=-1, int nrun=-1)
    {
        stringstream h;
        string conclusion;

        h << "<html><style>" << stylesheet << "</style><body><ul>";
        for (const auto& group: testGroups) {
            if (!group.selected) continue;
            h << "<li class=" << quotedVersionOf("title") << "> Tests from " << group.name << "</li>" ;
            /* Display each test as list item */
            for (const auto& test: group.tests) {
                h << "<hr>" << endl;
                string li = "<li class=" + quotedVersionOf(id(test.result)) + ">";
                h << li << "<b>" << status(test.result) << "</b> " << test.id << " " << test.testname << "</li>";
                if (!test.detailMessage.empty()) {
                    h << li << "<pre>" << sanitize(test.detailMessage) << "</pre></li>";
                }
                h << endl;
            }
            h << "<hr>" << endl;
        }
        h << "</ul>";
        if (nrun > 0) {
            h << "<h3> Passed " << npass << " of " << nrun << " tests. ";
            if (npass == nrun) conclusion = affirmation();
            h << conclusion << "</h3>";

        }
        h << "</body></html>";
        if (bp) bp->setText(h.str());   // display in browser pane
        return conclusion;
    }

    void addTestToGroup(Map<string, TestGroup>& map, TestCase tcase, string groupname)
    {
        if (!map.containsKey(groupname)) {
            TestGroup tg;
            tg.name = groupname;
            tg.selected = false;
            map[groupname] = tg;
        }
        Test t;
        t.testname = tcase.testname;
        t.callback = tcase.callback;
        ostringstream os;
        if (tcase.filename == groupname) {
            os << " (" << tcase.owner << ", line " << setw(3) << tcase.line << ") ";
        } else {
            os << " (" << tcase.owner << ", " << tcase.filename << ":" << tcase.line << ") ";
        }
        t.id = os.str();
        t.result = TestResult::WAITING; // It hasn't run yet
        map[groupname].tests += t;
    }

   Vector<TestGroup> prepareGroups(const map<TestKey, multimap<int, TestCase>> &allTests)
   {
        Map<string, TestGroup> grouped;

        for (const auto& module: allTests) {
            for (const auto& rawTest: module.second) {
                TestCase tcase = rawTest.second;
                addTestToGroup(grouped, tcase, tcase.owner);   // add once in owner group
                addTestToGroup(grouped, tcase, module.first);  // add again in module group
            }
        }
        return grouped.values();
    }

    void runSelectedGroups(Vector<TestGroup>& groups, Where where)
    {
        GBrowserPane *bp = nullptr;
        string stylesheet;
        bool displayWindow = (where == CONSOLE_AND_WINDOW || where == WINDOW_ONLY);
        bool displayConsole = (where == CONSOLE_AND_WINDOW || where == CONSOLE_ONLY);

        if (displayWindow) {
            GWindow* window = new GWindow(700, 600);
            bp = new GBrowserPane();
            window->setTitle("SimpleTest " + QCoreApplication::applicationName().toStdString());
            window->setExitOnClose(true);
            window->add(bp);

            window->setVisible(true);
            window->requestFocus();
            // student can customize display by editing the stylesheet
            readEntireFile("testing/styles.css", stylesheet);
        }

        /* Show everything so there's some basic data available. */
        displayResults(bp, stylesheet, groups);

        int nrun=0, npassed=0;
        /* Now, go run the tests. */
        for (auto& group: groups) {
            if (!group.selected) continue;
            if (displayConsole) cout << endl << "[SimpleTest] ---- Tests from " << group.name << " -----" << endl;
            for (auto& test: group.tests) {
                /* Make clear that we're running the test. */
                test.result = TestResult::RUNNING;
                displayResults(bp,stylesheet, groups);
                if (displayConsole) cout << "[SimpleTest] starting" << test.id << left << setfill('.') << setw(25) << test.testname.substr(0,25) << "... " << flush;
                runSingleTest(test);
                nrun++;
                if (test.result == TestResult::PASS) npassed++;
                if (displayConsole) cout << " =  "<< status(test.result) << endl << test.detailMessage;
                displayResults(bp, stylesheet, groups);
            }
        }
        string conclusion = displayResults(bp, stylesheet, groups, npassed, nrun);
         if (displayConsole) cout << "You passed " << npassed << " of " << nrun << " tests. " << conclusion << endl << endl;
    }

    int userChoiceFromMenu(Vector<string>& options)
    {
        cout << endl;
        cout << "Tests are grouped by filename or by type." << endl;
        cout << "Select the test groups you wish to run:" << endl;
        cout << "----------------------------------------" << endl;
        int i = 0;
        for (const auto& entry: options) {
            cout << "  " << i++ << ") " << entry << endl;
        }
        int chosen = getIntegerBetween("Enter your selection:", 0, options.size()-1);
        if (chosen == 0) {
            cout << "No tests selected." << endl << endl;
        }
        return chosen;
    }

    int getChoice(Vector<TestGroup>& groups, Choice ch)
    {
        if (ch == NO_TESTS) {
            return 0;
        } else if (ch == ALL_TESTS ) {
            return groups.size()+1;
        } else {
            Vector<string> names = { "None", "All of the above" };
            for (const auto& entry: groups) {
                names.insert(names.size()-1, "From " + entry.name);
            }
            return userChoiceFromMenu(names);
        }
    }

    bool selectChosenGroup(Vector<TestGroup>& groups, Choice ch)
    {
        int chosen = getChoice(groups, ch);

        if (chosen == 0) {
            return false;
        } else {
            for (int i = 0; i < groups.size(); i++) {
                groups[i].selected = (chosen == i+1 || chosen == groups.size()+1);
            }
            return true;
        }
    }

    bool runSimpleTests(Choice ch, Where where)
    {
        Vector<TestGroup> testGroups = prepareGroups(gTestsMap());

        if (selectChosenGroup(testGroups, ch)) {
            runSelectedGroups(testGroups, where);
            return true;
        } else {
            return false;
        }
    }
}

bool runSimpleTests(Choice ch, Where where) {
    return SimpleTest::runSimpleTests(ch, where);
}

