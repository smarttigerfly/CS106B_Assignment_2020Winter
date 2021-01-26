/**
 * Configuration file for general GUI driver. Edit this file to change basic parameters of the
 * GUI system.
 */
#ifndef GUIConfig_Included
#define GUIConfig_Included

#include "ProblemHandler.h"
#include "gwindow.h"
#include <memory>
#include <functional>
#include <string>
#include <vector>

/* This section should remain the same regardless of what you plan to do in your GUI. */
namespace GUI {
    /* Constructor type: Given a GWindow, build a problem handler. */
    using Constructor = std::function<std::shared_ptr<ProblemHandler> (GWindow &)>;

    /* An entry in the top-level GUI control bar. */
    struct GUIOption {
        std::string name;
        Constructor constructor;
    };
}



/* Macro to make it easier to define a constructor. This combines a forward declaration
 * with a call to the function name.
 */
#define GUI_HANDLER(name, constructor) {                           \
    name, [] (GWindow& window) {                                   \
        std::shared_ptr<ProblemHandler> constructor(GWindow &);    \
        return constructor(window);                                \
    }                                                              \
}

/* Edit this list to control what shows in the GUI. */
const std::vector<GUI::GUIOption> _kGUIOptions = {
    GUI_HANDLER("Run Tests",   makeTestingGUI),
    GUI_HANDLER("Towers of Hanoi", makeTowersOfHanoiGUI),
    GUI_HANDLER("Permutations", makePermutationsGUI),
    GUI_HANDLER("Doctors Without Orders", makeDoctorsWithoutOrdersGUI),
    GUI_HANDLER("Disaster Planning", makeDisasterPlanningGUI)
};

/* Edit this string to change the name of the GUI. */
const std::string _kGUITitle = "Recursion to the Rescue";

/* Edit this string to change the text that displays in the default problem handler. */
const std::string _kDefaultMessage = "Select a demo to run";


#endif
