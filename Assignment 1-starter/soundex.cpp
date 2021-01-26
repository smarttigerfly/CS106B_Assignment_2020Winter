/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "testing/SimpleTest.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include <fstream>
#include <cctype>
#include <string>
#include "vector.h"
using namespace std;

/* This function takes a string s and returns a version of
 * the string that has all non-letter characters removed
 * from it.
 *
 * WARNING: The provided code is buggy!
 *
 * Use unit tests to identify which inputs to this function
 * are incorrectly handled. Then, remove this comment and
 * replace it with a description of the bug you fixed.
 */
string removeNonLetters(string s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
//        if (!isalpha(s[i])) {
//            // remove the character at index i
//            s = s.substr(0,i) + s.substr(i+1);
//        }
        if (isalpha(s[i])) {
            // remove the character at index i
            result += s[i];
        }
    }
    return result;
}

/* This function takes a string s and returns a version of
 * the string that has been encoded as digit using the table below:
 * Digit	represents the letters
 *   0	      A E I O U H W Y
 *   1	      B F P V
 *   2	      C G J K Q S X Z
 *   3	      D T
 *   4	      L
 *   5	      M N
 *   6	      R
 *
 */
string encodeLetters(string s)
{
    string result = "";
    s = toUpperCase(s);
    for (int i = 0; i < s.length(); i++) {
        switch (s[i]) {
        case 'A':
        case 'I':
        case 'E':
        case 'O':
        case 'U':
        case 'H':
        case 'W':
        case 'Y':
            result += "0";
            break;
        case 'B':
        case 'F':
        case 'P':
        case 'V':
            result += "1";
            break;
        case 'C':
        case 'G':
        case 'J':
        case 'K':
        case 'Q':
        case 'S':
        case 'X':
        case 'Z':
            result += "2";
            break;
        case 'D':
        case 'T':
            result += "3";
            break;
        case 'L':
            result += "4";
            break;
        case 'M':
        case 'N':
            result += "5";
            break;
        case 'R':
            result += "6";
            break;
        default:
            break;
        }

    }
    return result;
}

/* This function takes a string s and returns a version of
 * the string that has been Coalesce adjacent duplicate digits.
 */

string CoalAdjDupDig(string s)
{
    string result = "";
    if(s.size() == 0)
        return result;
    char flag = s[0];
    result += flag;
    for (int i = 0; i < s.length(); i++) {

        if (flag != s[i]) {
            flag = s[i];
            result += flag;
        }
    }
    return result;
}

/* This function takes a string s and returns a version of
 * the string that has been replaced the first digit of code
 * with the first letter of the original name, converting to uppercase.
 */
string replaceFirDig(string digi, string origin)
{
    if (digi.size() == 0 || origin.size() == 0)
        return "";
    else{
        origin = toUpperCase(origin);
        digi[0] = origin[0];
        return digi;
    }
}

/* This function takes a string s and returns a version of
 * the string that has been replaced the first digit of code
 * with the first letter of the original name, converting to uppercase.
 */
string removeZero(string s)
{
    string result = "";
    for (int i = 0; i < s.length(); i++) {

        if (s[i] != '0') {
            result += s[i];
        }
    }
    return result;
}



string makeLenth4(string s)
{
    if(s.size() >= 4)
        return s.substr(0, 4);
    else{
        do {
            s += '0';
        } while (s.size() < 4);
        return  s;
    }
}



/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
string soundex(string s) {
    /* TODO: Fill in this function. */
    string result = removeNonLetters(s);
    result = toUpperCase(result);
    string origUpper = result;
    result = encodeLetters(result);
    result = CoalAdjDupDig(result);
    result = replaceFirDig(result, origUpper);
    result = removeZero(result);
    result = makeLenth4(result);
    return result;
}


/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
void soundexSearch(string filepath) {
    // The proivded code opens the file with the given name
    // and then reads the lines of that file into a vector.
    ifstream in;
    Vector<string> databaseNames;

    if (openFile(in, filepath)) {
        readEntireFile(in, databaseNames);
    }
    cout << "Read file " << filepath << ", "
         << databaseNames.size() << " names found." << endl;

    // The names in the database are now stored in the provided
    // vector named databaseNames

    /* TODO: Fill in the remainder of this function. */
    do
    {
        cout << endl << endl;
        string name = getLine("Enter a surname (RETURN to quit):");
        if(name.size() == 0)
            break;
        string result = soundex(name);
        cout << "Soundex code is " << result <<endl;
        Vector<string> resultNames;
        for(int i = 0; i < databaseNames.size(); i++)
        {
            if(result == soundex(databaseNames[i]))
                resultNames.add(databaseNames[i]);
        }
        resultNames.sort();
        cout << "Matches from database: " << resultNames << endl;

    } while(true);
    cout << "All done!";

}


/* * * * * * Test Cases * * * * * */


PROVIDED_TEST("Test removing puntuation, digits, and spaces") {
    string s = "O'Hara";
    string result = removeNonLetters(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "tldr");
}


PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}

// TODO: add your test cases here


STUDENT_TEST("Test removing puntuation, digits, and spaces....") {
    string s = "1'23456";
    string result = removeNonLetters(s);
    EXPECT_EQUAL(result, "");
    s = "';./";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "");
    s = " t      ";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "t");
}

STUDENT_TEST("Test ecoding letters  ....") {
    string s = "A E I O U";
    string result = removeNonLetters(s);
    result = encodeLetters(result);
    EXPECT_EQUAL(result, "00000");
}

STUDENT_TEST("Test CoalAdjDupDig  ....") {
    string s = "222202255";
    string result = CoalAdjDupDig(s);
    EXPECT_EQUAL(result, "2025");
}

STUDENT_TEST("Test Replace the first digit  ....") {
    string s = "Curie";
    string result = encodeLetters(s);
    result = CoalAdjDupDig(result);
    result = replaceFirDig(result, s);
    EXPECT_EQUAL(result, "C060");
}

STUDENT_TEST("Test removeZero....") {
    string s = "C00600";
    string result = removeZero(s);
    EXPECT_EQUAL(result, "C6");
}

STUDENT_TEST("Test soundex....") {
    string s = "Curie";
    string result = soundex(s);
    EXPECT_EQUAL(result, "C600");
}
