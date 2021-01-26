#ifndef TestDriver_Included
#define TestDriver_Included

#include <string>
#include <sstream>

/*** Utility functions to use in the context of your test cases. ***/

/* Checks whether the given condition is true. If so, nothing happens. If the condition
 * is false, then the test fails. For example:
 *
 *     EXPECT(vec.isEmpty());
 *     EXPECT(history.next() == "Dik-dik");
 */
#define EXPECT(condition) /* Something internal you shouldn't worry about. */

/* Checks whether the given expression called the error() handler. If so, nothing happens.
 * If not, then the test fails. You can use this to confirm that an expression that is
 * supposed to trigger an exception indeed does so. For example:
 *
 *     Vector<int> myVec;
 *     EXPECT_ERROR(myVec.get(0));    // Nothing happens, since myVec.get(0) does trigger error().
 *     EXPECT_ERROR(myVec.isEmpty()); // The test fails because no error would be generated here.
 */
#define EXPECT_ERROR(expression) /* Something internal you shouldn't worry about. */

/* Checks whether two objects are equal. If so, nothing happens. If not, then the test fails
 * and reports what the two values were. The expectation is that the first value should be
 * the student's value and the second should be the correct answer. For example:
 *
 *     EXPECT_EQUAL(studentValue, referenceValue)
 */
#define EXPECT_EQUAL(student, reference) /* Something internal you shouldn't worry about. */

#define SHOW_ERROR(message);

/* Defines a new test case. You can write whatever code you want inside of the test case,
 * but you'll likely want to use EXPECT and EXPECT_ERROR in your test cases, as they're
 * what actually perform tests.
 *
 * The syntax for defining a new test case is
 *
 *    ADD_TEST("Description of Test Case") {
 *       ... whatever code you'd like! ...
 *    }
 */
#define ADD_TEST(name) /* Something internal you shouldn't worry about. */






/*** Internal types used by the testing system. You should not need to use anything ***
 *** below this point.                                                              ***/

#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include "error.h"

/* Type representing a key in the top-level testing map. Integer represents the
 * priority, with a string representing the associated name. Lower-priority test
 * groups get processed first.
 */
using TestKey = std::pair<int, std::string>;

/* Type representing a function that can be used for testing. This is any function that
 * can be called with no arguments.
 */
using TestCase = std::function<void()>;

/* The top-level tests map. This is defined as a function rather than a static variable
 * to ensure that it plays well with static initializers.
 */
std::map<TestKey, std::multimap<int, std::pair<std::string, TestCase>>>& gTestsMap();

/* Exception type signifying that a test case failed. */
class TestFailedException: public std::logic_error {
public:
    TestFailedException(const std::string &message, std::size_t line, const char* filename);
};

/* Object whose sole purpose is to put a test case into a group of tests. */
class TestCaseAdder {
public:
    TestCaseAdder(const TestKey& key, int lineNumber, const std::string& name, TestCase test);
};

/**** Defines the macro that adds a new test case. ****/

/* First, undefine ADD_TEST, since we defined it above as a way of "prototyping" it. */
#undef ADD_TEST

/* We need several levels of indirection here because of how the preprocessor works.
 * This first layer expands out to the skeleton of what we want.
 */
#define ADD_TEST(name) DO_ADD_TEST(_testCase, _adder, name, __LINE__)

/* This level of indirection exists so that line will be expanded to __LINE__ and
 * from there to the true line number. We still can't token-paste it here, since
 * the even further level of indirection.
 */
#define DO_ADD_TEST(fn, adder, name, line)\
    static void JOIN(fn, line)();\
    static TestCaseAdder JOIN(adder, line)(GROUP, line, name, JOIN(fn, line));\
    static void JOIN(fn, line)()

#define JOIN(X, Y) X##Y

/***** Macros used to implement testing primitives. *****/
void doFail(const std::string &message, std::size_t line, const char* filename);

#undef EXPECT
#define EXPECT(condition) doExpect(condition, "EXPECT failed: " #condition " is not true.", __LINE__, __FILE__)
void doExpect(bool condition, const std::string& expression, std::size_t line, const char* filename);

#undef EXPECT_ERROR
#define EXPECT_ERROR(condition) do {\
    try {\
        (void)(condition); \
        doFail("EXPECT_ERROR: " #condition " did not call error().", __LINE__, __FILE__); \
    } catch (const ErrorException& ) { \
        /* Do nothing. */ \
    }\
} while(0)

/* Used for EXPECT_EQUAL. */
inline std::string debugFriendlyString(char c) {
    return "'" + std::string(1, c) + "'";
}

inline std::string debugFriendlyString(const std::string& s) {
    return '"' + s + '"';
}
inline std::string debugFriendlyString(const char* s) {
    return '"' + std::string(s) + '"';
}
template <size_t N> std::string debugFriendlyString(char (&s)[N]) {
    return debugFriendlyString((const char *)s);
}
inline std::string debugFriendlyString(bool value) {
    return value? "true" : "false";
}
template <typename T> std::string debugFriendlyString(const T& value) {
    std::ostringstream result;
    result << value;
    return result.str();
}

#undef EXPECT_EQUAL
#define EXPECT_EQUAL(student, reference)                                                    \
    do {                                                                                    \
       auto _studentAnswer   = (student);                                                   \
       auto _referenceAnswer = (reference);                                                 \
                                                                                            \
       std::stringstream _expression;                                                       \
       _expression << std::boolalpha << "EXPECT_EQUAL failed: "                             \
                   << #student << " != " << #reference "\n"                                 \
                   << "                " #student   " = "                                   \
                   << debugFriendlyString(_studentAnswer) << '\n'                           \
                   << "                " #reference " = "                                   \
                   << debugFriendlyString(_referenceAnswer);                                \
       doExpect(_studentAnswer == _referenceAnswer, _expression.str(), __LINE__, __FILE__); \
    } while (0)

#undef SHOW_ERROR
#define SHOW_ERROR(message) showError(message, __LINE__, __FILE__)
void showError(const char* message, std::size_t line, const char* filename);

#endif
