#include "Demos/EmptyProblemHandler.h"
#include "Demos/GUIUtils.h"
#include "Demos/GUIConfig.h"
#include "gobjects.h"
#include <string>
using namespace std;

namespace {
    /* Graphics Constants. */
    const string kFont = "Serif-BOLDITALIC-36";
    const string kBackgroundColor = "#400040";
    const string kFontColor       = "#800080";
}

void EmptyProblemHandler::repaint(GWindow& window) {
    /* Clear the window. */
    clearDisplay(window, kBackgroundColor);

    /* Draw some text! */
    GText label(_kDefaultMessage);
    label.setFont(kFont);
    label.setColor(kFontColor);

    double x = (window.getCanvasWidth()  - label.getWidth())      / 2.0;
    double y = (window.getCanvasHeight() + label.getFontAscent()) / 2.0;

    window.draw(&label, x, y);
    window.repaint();
}

void EmptyProblemHandler::actionPerformed(GObservable *) {
    // Do nothing
}
