#ifndef DISPLAY_H
#define DISPLAY_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h> // Risolve l'errore undefined Graphics_Context
#include "../LcdDriver/Crystalfontz128x128_ST7735.h"

void initDisplayUI(Graphics_Context *ctx);
void displayUI(Graphics_Context *ctx);
void updateUI(Graphics_Context *ctx, int angle, int distance);
void drawRadar(Graphics_Context *ctx, int angle, int distance);
void clearRadarMap(Graphics_Context *ctx);

#endif
