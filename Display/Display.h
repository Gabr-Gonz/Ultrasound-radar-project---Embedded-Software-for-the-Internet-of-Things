#ifndef DISPLAY_H
#define DISPLAY_H

#include <LCD_HX8353E.h>

// Global display instance
extern LCD_HX8353E display;

// Display-related functions
void initDisplayUI();
void displayUI();
void updateUI(int angle, int distance);
void drawRadar(int angle, int distance);

#endif
