#include "YouGotHufflepuff.h"
#include "Testing/YouGotHufflepuffTests.h"
#include "set.h"
#include "vector.h"

using namespace std;

void administerQuiz(const HashSet<Question>& questions,
                    int numQuestions,
                    const HashSet<Person>& people) {
    /* TODO: Delete this comment and the next three lines, then implement this function. */
    HashMap<char, int> userScores;
    userScores.add('O', 0);
    userScores.add('C', 0);
    userScores.add('E', 0);
    userScores.add('A', 0);
    userScores.add('N', 0);
    Set<string> questionStrSet;
    for(int i = 0; i < numQuestions;)
    {
        Question curQuestion = randomElement(questions);
        if(!questionStrSet.contains(curQuestion.questionText))
        {
            questionStrSet.add(curQuestion.questionText);
            int quesitonScore =  askPersonalityQuestion(curQuestion);
            i++;
            for(char c : curQuestion.factors.keys())
            {
                userScores[c] += curQuestion.factors[c] * quesitonScore;
            }
        }
    }



    //displayScores(userScores);
    HashMap<char, double> userNomal = nomalizeScore(userScores);

    if(nomalizeScore(userScores).isEmpty())
    {
        displayMessage("Don’t have enough information "
                       "to make a determination of who they’re most similar"
                       "to");
        return;
    }
    HashMap<double, string> similartiryMap;
    for (Person p : people)
    {
        HashMap<char, double> personNomal = nomalizeScore(p.scores);
        similartiryMap.add(cosineSimilarity(userNomal, personNomal), p.name);
    }
    Vector<double> cosineSimi = similartiryMap.keys();
    cosineSimi.sort();

    double similarity = cosineSimi[cosineSimi.size() - 1];

    string name = similartiryMap[similarity];

    displayMessage(string("You Got ") + name + "!" + "(Similarity: " + to_string(similarity) + ")");

}

HashMap<char, double> nomalizeScore(HashMap<char, int> &score)
{
    HashMap<char, double> userScores;

    double sum = 0.0;
    for(char c : score)
    {
        sum += score[c] * score[c];
    }

    sum = sqrt(sum);
    if (sum == 0)
    {
        return userScores;
    }

    for(char c : score)
    {
        userScores[c] = score[c] / sum;
    }

    return userScores;

}

double cosineSimilarity(HashMap<char, double> &user, HashMap<char, double> &people){
    double cosineSimilarity = 0.0;
    for (char c : user.keys())
    {
        cosineSimilarity += user[c] * people[c];
    }
    return cosineSimilarity;
}
/* * * * *   Test Cases Below This Point   * * * * */

/* Because administerQuiz is a function that requires interaction with the user,
 * you won't be able to directly test that administerQuiz returns the correct
 * values. Similarly, you can't call the askPersonalityQuestion, displayScores,
 * or displayMessage functions, as those are wired into the demo and don't
 * work in testing.
 *
 * However, you are welcome to add test cases to test any helper functions you
 * write! To do so, use the ADD_TEST macro to define tests, just as you did in
 * Assignment 1 and for Rising Tides.
 *
 * You aren't required to include test cases in this part of the assignment,
 * though we definitely would encourage you to do so! This is a multistep
 * assignment that's going to require each individual function to work
 * correctly, and adding tests for the functions you write is a great way to
 * find and fix bugs early on.
 */

