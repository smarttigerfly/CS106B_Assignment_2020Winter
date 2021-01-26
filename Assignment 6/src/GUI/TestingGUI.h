#ifndef TestingGUI_Included
#define TestingGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Visualizer that runs all the unit tests. */
class TestingGUI: public ProblemHandler {
public:
    TestingGUI(GWindow&);

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(TestingGUI, "Run Tests", 0);

#endif
