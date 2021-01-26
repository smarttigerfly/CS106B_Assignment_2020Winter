#ifndef EmptyProblemHandler_Included
#define EmptyProblemHandler_Included

#include "ProblemHandler.h"
#include <istream>

/* A problem handler that just prompts the user to pick a problem to solve. This
 * gets installed whenever there's nothing else to display.
 */
class EmptyProblemHandler: public ProblemHandler {
public:
    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;
};



#endif
