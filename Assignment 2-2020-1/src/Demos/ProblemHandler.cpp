#include "ProblemHandler.h"
#include "gthread.h"
using namespace std;

/* Issues a redraw, if necessary. */
void ProblemHandler::draw(GWindow& window) {
    if (isDirty) {
        GThread::runOnQtGuiThread([&, this] {
            repaint(window);
            window.repaint();
        });

        isDirty = false;
    }
}

/* Sets the dirty bit. */
void ProblemHandler::requestRepaint() {
    isDirty = true;
}

/* Default draw does nothing. */
void ProblemHandler::repaint(GWindow &) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::actionPerformed(GObservable *) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::changeOccurredIn(GObservable *) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::timerFired() {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::mouseMoved(double, double) {
    // Do nothing
}

void ProblemHandler::mousePressed(double, double) {
    // Do nothing
}

void ProblemHandler::mouseDragged(double, double) {
    // Do nothing
}
