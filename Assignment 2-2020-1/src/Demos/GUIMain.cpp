#include "ProblemHandler.h"
#include "EmptyProblemHandler.h"
#include "GUIConfig.h"
#include "gwindow.h"
#include "gevents.h"
#include "gbutton.h"
#include "goptionpane.h"
#include "map.h"
#include <chrono>
using namespace std;

namespace {
    /* Window graphics constants. */
    const double kWindowWidth  = 1000;
    const double kWindowHeight = 800;

    /* Milliseconds before a timer event is considered "stale." */
    const long kTimelyCutoff = 100;

    /* Type representing all state necessary to make the graphics work. */
    struct Graphics {
        GWindow window{kWindowWidth, kWindowHeight};      // The window
        shared_ptr<ProblemHandler> handler;               // Current task handler
        Map<GObservable*, GUI::Constructor> constructors; // Map from buttons to constructors.
    };

    /* Creates the graphics window and associated state. */
    Graphics* makeGraphics() {
        auto* result = new Graphics();

        /* Basic window setup. */
        result->window.setTitle(_kGUITitle);
        result->window.setExitOnClose(true);

        /* Basic graphics setup. */
        result->window.setRepaintImmediately(false);
        result->window.setCanvasSize(kWindowWidth, kWindowHeight);

        /* Default handler. */
        result->handler = make_shared<EmptyProblemHandler>();

        /* Problem handlers. */
        for (const auto& entry: _kGUIOptions) {
            auto* button = new GButton(entry.name);
            result->window.addToRegion(button, "NORTH");

            result->constructors[button] = entry.constructor;
        }
        return result;
    }

    /* Sets the active problem. */
    void setProblem(Graphics* graphics, GObservable* source) {
        auto constructor = graphics->constructors.get(source);
        if (!constructor) {
            error("No constructor for that GObservable?");
        }

        /* We do have a handler. Dispose of whatever used to be here. */
        graphics->handler.reset();
        graphics->window.clearCanvas();

        /* Now, load whatever this problem is. */
        graphics->handler.reset();
        graphics->handler = constructor(graphics->window);
    }

    Graphics* theGraphics;
    bool theOptionsEnabled = true;
}

/* Enable/disable all demo buttons. */
void setDemoOptionsEnabled(bool isEnabled) {
    for (GObservable* option: theGraphics->constructors) {
        if (auto* button = dynamic_cast<GButton*>(option)) {
            button->setEnabled(isEnabled);
        }
    }
    theOptionsEnabled = isEnabled;
}

int main() {
    theGraphics = makeGraphics();

    while (true) {
        /* Update the window (no-op if nothing needs to be redrawn.) */
        theGraphics->handler->draw(theGraphics->window);

        GEvent e = waitForEvent(MOUSE_EVENT | ACTION_EVENT | CHANGE_EVENT | TIMER_EVENT);
        if (e.getEventClass() == ACTION_EVENT) {
            auto source = GActionEvent(e).getSource();

            /* We are responsible for the problem buttons. */
            if (theGraphics->constructors.containsKey(source)) {
                if (theOptionsEnabled) setProblem(theGraphics, source);
            }
            /* Any other event is the responsible of the problem handler. */
            else {
                theGraphics->handler->actionPerformed(source);
            }
        } else if (e.getEventClass() == CHANGE_EVENT) {
            theGraphics->handler->changeOccurredIn(GChangeEvent(e).getSource());
        } else if (e.getEventClass() == TIMER_EVENT) {
            /* If one of the handlers is responding slowly, it can cause a positive
             * feedback loop where timer events don't get pulled from the queue at the
             * rate at which they're generated, starving out higher-priority events.
             * To address this, if we pull a timer event out and it hasn't happened
             * sufficiently recently, we're going to assume we're running behind and
             * just swallow that event without processing it.
             */
            long now = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
            if (now - e.getTime() < kTimelyCutoff) {
                theGraphics->handler->timerFired();
            }
        } else if (e.getEventClass() == MOUSE_EVENT) {
            if (e.getEventType() == MOUSE_MOVED) {
                theGraphics->handler->mouseMoved(e.getX(), e.getY());
            } else if (e.getEventType() == MOUSE_PRESSED) {
                theGraphics->handler->mousePressed(e.getX(), e.getY());
            } else if (e.getEventType() == MOUSE_DRAGGED) {
                theGraphics->handler->mouseDragged(e.getX(), e.getY());
            }
        }
    }
}
