#include "Display.h"
#include <math.h>
#include <stdio.h>

#define RADAR_RANGE 200
#define MAX_SENSOR_DIST 400  // Portata massima del sensore in cm
#define RADAR_RADIUS    110  // Raggio del disegno sul display in pixel

void initDisplayUI(Graphics_Context *ctx) {
    Graphics_clearDisplay(ctx);
    Graphics_setFont(ctx, &g_sFontFixed6x8);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);

    Graphics_drawString(ctx, (int8_t*)"MSP432 RADAR SYSTEM", AUTO_STRING_LENGTH, 10, 10, GRAPHICS_OPAQUE_TEXT);
}

// In display.c

void displayUI(Graphics_Context *ctx) {
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);

    // Disegna cerchi di riferimento (opzionale, per i 100, 200, 300 cm)
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DIM_GRAY);
    Graphics_drawCircle(ctx, 64, 125, 27);  // ~100cm
    Graphics_drawCircle(ctx, 64, 125, 55);  // ~200cm
    Graphics_drawCircle(ctx, 64, 125, 82);  // ~300cm

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawLine(ctx, 0, 15, 128, 15);
}

void updateUI(Graphics_Context *ctx, int angle, int distance) {
    char buffer[16];
    Graphics_setFont(ctx, &g_sFontFixed6x8);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);

    // Puliamo solo l'area del testo (una piccola striscia in alto a destra)
    Graphics_Rectangle rect = {80, 0, 128, 14};
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(ctx, &rect);

    // Scriviamo solo la distanza in alto a destra
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_SPRING_GREEN);
    if (distance > 0 && distance < 400) {
        sprintf(buffer, "%3d cm", distance);
    } else {
        sprintf(buffer, "---");
    }
    Graphics_drawString(ctx, (int8_t*)buffer, AUTO_STRING_LENGTH, 85, 2, GRAPHICS_OPAQUE_TEXT);
}

void drawRadar(Graphics_Context *ctx, int angle, int distance) {
    static int lastX = 64, lastY = 125;

    // 1. Cancelliamo la vecchia linea di scansione (il "raggio")
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_drawLine(ctx, 64, 125, lastX, lastY);

    // 2. Calcoliamo la posizione della nuova linea di scansione
    float rad = angle * M_PI / 180.0f;
    int x = 64 + RADAR_RADIUS * cosf(rad);
    int y = 125 - RADAR_RADIUS * sinf(rad);

    // 3. Disegniamo la nuova linea di scansione (Verde)
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_GREEN);
    Graphics_drawLine(ctx, 64, 125, x, y);

    lastX = x;
    lastY = y;

    // 4. Disegniamo l'oggetto riscalato
    // Se la distanza è valida (tra 2cm e 400cm)
    if (distance > 2 && distance <= MAX_SENSOR_DIST) {

        // Calcolo della proporzione: (distanza_reale / distanza_max) * raggio_pixel
        float r_scaled = ((float)distance / MAX_SENSOR_DIST) * RADAR_RADIUS;

        int ox = 64 + r_scaled * cosf(rad);
        int oy = 125 - r_scaled * sinf(rad);

        // Disegniamo il pallino rosso
        Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_RED);
        Graphics_fillCircle(ctx, ox, oy, 2);
    }
}

// In display.c

void clearRadarMap(Graphics_Context *ctx) {
    // Definiamo un rettangolo che copre l'area del radar (sotto la linea dei 15px)
    Graphics_Rectangle mapArea = {0, 16, 127, 127};
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(ctx, &mapArea);

    // (Opzionale) Ridisegna i cerchi della griglia dopo la pulizia
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DIM_GRAY);
    Graphics_drawCircle(ctx, 64, 125, 27);
    Graphics_drawCircle(ctx, 64, 125, 55);
    Graphics_drawCircle(ctx, 64, 125, 82);
}
