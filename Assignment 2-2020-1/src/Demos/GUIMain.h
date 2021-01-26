#ifndef GUIMain_Included
#define GUIMain_Included

/* Runs the main loop for the interactive graphical program. */
void guiMain();

/* Locks/unlocks all demo option buttons. Should only be called from the main GUI thread. */
void setDemoOptionsEnabled(bool isEnabled);

#endif
