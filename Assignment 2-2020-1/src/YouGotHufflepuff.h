#ifndef YouGotHufflepuff_Included
#define YouGotHufflepuff_Included

#include "hashmap.h"
#include "hashset.h"
#include <string>
#include <iostream>

/* Type: Question
 *
 * Type representing a personality quiz question.
 */
struct Question {
    std::string questionText;   // Text of the question
    HashMap<char, int> factors; // Map from factors to +1 or -1
};

/* Type: Person
 *
 * Type representing a person, used to represent people when determining
 * who's the closest match to the user.
 */
struct Person {
    std::string name;          // Name of the person
    HashMap<char, int> scores; // Map from factors to +1 or -1
};



/**
 * Asks the given personality question, returning a number between 1 and 5
 * indicating where on the scale from "Strongly Disagree" (1) to "Strongly
 * Agree" (5) the user lies.
 * <p>
 * We've implemented this function for you. You don't need to write any
 * code for this.
 *
 * @param question The question to ask.
 * @return A number between 1 and 5 indicating the user's preference.
 */
int askPersonalityQuestion(const Question& question);



/**
 * Displays the user's OCEAN scores on the screen. The message will
 * persist until another set of OCEAN scores are displayed.
 *
 * @param scores The user's scores. It is not required that 'O', 'C', 'E',
 *               'A', and 'N' are keys; any missing key will be interpreted
 *               as a zero.
 */
void displayScores(const HashMap<char, int>& scores);


/**
 * Displays the given message on the screen. The message will persist
 * until either another message is displayed or a question is asked.
 * <p>
 * We've implemented this function for you. You don't need to write any
 * code for this.
 *
 * @param message The message to display.
 */
void displayMessage(const std::string& message);


/**
 * Administers a personality quiz, reporting which person matches the user most
 * closely.
 *
 * @param questions The bank of questions to draw from.
 * @param numQuestions The number of questions to ask. You can assume this is no bigger than
 *                     the questions set.
 * @param people The set of people against whom the user should be compared.
 */
void administerQuiz(const HashSet<Question>& questions,
                    int numQuestions,
                    const HashSet<Person>& people);

HashMap<char, double> nomalizeScore(HashMap<char, int> &score);
double cosineSimilarity(HashMap<char, double> &user,HashMap<char, double> &people);

/* Everything below this point is designed to make C++ play nicely with
 * the custom types we defined here. Feel free to check these out if you're
 * curious, and take CS106L if you want to learn more!
 */

/* Functions to read or write questions and people to/from a stream. */
std::istream& operator>> (std::istream& in, Question& toRead);
std::istream& operator>> (std::istream& in, Person& toRead);
std::ostream& operator<< (std::ostream& out, const Question& toWrite);
std::ostream& operator<< (std::ostream& out, const Person& toWrite);

/* Functions needed to store a Question / Person in a HashSet. */
bool operator== (const Question& lhs, const Question& rhs);
bool operator== (const Person& lhs,   const Person& rhs);
int  hashCode(const Question& q);
int  hashCode(const Person& p);

#endif
