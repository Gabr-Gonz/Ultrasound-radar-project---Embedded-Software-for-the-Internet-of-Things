#ifndef DISPLAY_H
#define DISPLAY_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "../LcdDriver/Crystalfontz128x128_ST7735.h"

void initDisplayUI(Graphics_Context *ctx);                          // function that initializes the display for subsequent drawing operations
void displayUI(Graphics_Context *ctx);                              // function to draw the circles to give a better visualization on the display
void updateUI(Graphics_Context *ctx, int distance);      // function that writes the distance of the object scanned on the display
void drawRadar(Graphics_Context *ctx, int angle, int distance);     // function that draws the position of the object on the radar map
void clearRadarMap(Graphics_Context *ctx);                          // function that clear tha map to prepare it for the next scan

#endif
