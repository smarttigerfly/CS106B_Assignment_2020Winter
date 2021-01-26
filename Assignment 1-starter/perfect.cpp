/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "console.h"
#include <iostream>
#include "testing/SimpleTest.h"
using namespace std;

/* This function takes one argument `n` and calculates the sum
 * of all proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division.
 *
 * Note: long is a C++ type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* This function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* This function does an exhaustive search for perfect numbers.
 * It takes one argument `stop` and searches the range 1 to `stop`,
 * checking each number to see whether it is perfect and if so,
 * printing it to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
long smarterSum(long n) {
    /* TODO: Fill in this function. */
    long total = 0;
    if(n == 1)
        return 0;
    long stop = sqrt(n) + 1;
    for (long divisor = 1; divisor < stop; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
            if(divisor != 1 && divisor != n / divisor)
            {
                total += n / divisor;
            }
        }
    }
    return total;
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
bool isPerfectSmarter(long n) {
    /* TODO: Fill in this function. */
    return (n != 0) && (n == smarterSum(n));
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
void findPerfectsSmarter(long stop) {
     /* TODO: Fill in this function. */
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
bool isPrime(long n)
{
    if(n <= 0)
        return false;

    long stop = sqrt(n) + 1;
    for (long divisor = 2; divisor < stop; divisor++) {
        if (n % divisor == 0) {
            return false;
        }
    }
    return true;
}

long findNthPerfectEuclid(long n) {
    /* TODO: Fill in this function. */
    if(n <= 0)
        return 0;
    long k = 2;
    int nofPerfectNum = 0;
    do {
        if( isPrime(pow(2,k) - 1) )
        {
            if( ++nofPerfectNum == n)
            {
                cout << "Found " << n <<"th perfect number: " << (pow(2, k) -  1) * (pow(2,k - 1)) << endl;
                return (pow(2, k) -  1) * (pow(2,k - 1));
            }


        }
        k++;
    } while (true);
}


/* * * * * * Test Cases * * * * * */

/* Note: Do not add or remove any of the PROVIDED_TEST tests.
 * You should add your own STUDENT_TEST tests below the
 * provided tests.
 */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Oddballs, zero, one, negative") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
    EXPECT(!isPerfect(-6));
}

PROVIDED_TEST("33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

//PROVIDED_TEST("Time multiple trials of findPerfects on doubling input sizes") {
//    cout << endl;
//    TIME_OPERATION(20000, findPerfects(20000));
//    TIME_OPERATION(40000, findPerfects(40000));
////    TIME_OPERATION(80000, findPerfects(80000));
////    TIME_OPERATION(160000, findPerfects(160000));
////    TIME_OPERATION(320000, findPerfects(320000));
////    TIME_OPERATION(640000, findPerfects(640000));
//}

// TODO: add your student test cases here

//STUDENT_TEST("Negative"){

//    EXPECT(!isPerfect(-0));
//    EXPECT(!isPerfect(-10));
//}


//STUDENT_TEST("Confirm smarterSum of small inputs") {
//    EXPECT_EQUAL(smarterSum(1), divisorSum(1));
//    EXPECT_EQUAL(smarterSum(6), divisorSum(6));
//    EXPECT_EQUAL(smarterSum(12), divisorSum(12));
//    EXPECT_EQUAL(smarterSum(25), divisorSum(25));
//}


//STUDENT_TEST("Time multiple trials of findPerfectsSmarter on doubling input sizes") {
//    cout << endl;
//    TIME_OPERATION(20000, findPerfectsSmarter(20000));
//    TIME_OPERATION(40000, findPerfectsSmarter(40000));
//    TIME_OPERATION(80000, findPerfectsSmarter(80000));
//    TIME_OPERATION(160000, findPerfectsSmarter(160000));
//    TIME_OPERATION(320000, findPerfectsSmarter(320000));
//    TIME_OPERATION(640000, findPerfectsSmarter(640000));
//}

//STUDENT_TEST("Test isPrime, 0-false, 1-true, 5-true,"){

//    EXPECT(!isPrime(0));
//    EXPECT(isPrime(1));
//    EXPECT(isPrime(5));

//}

//STUDENT_TEST("Test findNthPerfectEuclid, 1th-6, 2th-28, 3th-496"){
//    cout << endl;
//    EXPECT_EQUAL(findNthPerfectEuclid(1),6);
//    EXPECT_EQUAL(findNthPerfectEuclid(2),28);
//    EXPECT_EQUAL(findNthPerfectEuclid(3),496);
//}


STUDENT_TEST("Time multiple trials of findNthPerfectEuclid") {
    cout << endl;
    TIME_OPERATION(1, findNthPerfectEuclid(1));
    TIME_OPERATION(2, findNthPerfectEuclid(2));
    TIME_OPERATION(3, findNthPerfectEuclid(3));
    TIME_OPERATION(4, findNthPerfectEuclid(4));
    TIME_OPERATION(5, findNthPerfectEuclid(5));
}
