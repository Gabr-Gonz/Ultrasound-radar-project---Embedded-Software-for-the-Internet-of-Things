#include "Display.h"
#include <math.h>

#define RADAR_RANGE 200

void initDisplayUI(Graphics_Context *ctx) {
    Graphics_clearDisplay(ctx);
    Graphics_setFont(ctx, &g_sFontFixed6x8);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);

    Graphics_drawString(ctx, (int8_t*)"MSP432 RADAR SYSTEM", AUTO_STRING_LENGTH, 10, 10, OPAQUE);
}

void displayUI(Graphics_Context *ctx) {
    Graphics_Rectangle rect = {0, 20, 128, 140};
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(ctx, &rect);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawString(ctx, (int8_t*)"STATUS:", AUTO_STRING_LENGTH, 5, 25, OPAQUE);
    Graphics_drawString(ctx, (int8_t*)"ANGLE:",  AUTO_STRING_LENGTH, 5, 45, OPAQUE);
    Graphics_drawString(ctx, (int8_t*)"DIST:",   AUTO_STRING_LENGTH, 5, 65, OPAQUE);
    Graphics_drawString(ctx, (int8_t*)"ALERT:",  AUTO_STRING_LENGTH, 5, 85, OPAQUE);
}

void updateUI(Graphics_Context *ctx, int angle, int distance) {
    char buffer[32];

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);

    // STATUS
    sprintf(buffer, "ACTIVE");
    Graphics_drawString(ctx, (int8_t*)buffer, AUTO_STRING_LENGTH, 60, 20, OPAQUE);

    // ANGLE
    sprintf(buffer, "%3d deg", angle);
    Graphics_drawString(ctx, (int8_t*)buffer, AUTO_STRING_LENGTH, 60, 40, OPAQUE);

    // DISTANCE
    if (distance == -1)
        sprintf(buffer, "--");
    else
        sprintf(buffer, "%3d cm", distance);
    Graphics_drawString(ctx, (int8_t*)buffer, AUTO_STRING_LENGTH, 60, 60, OPAQUE);

    // ALERT
    if (distance != -1 && distance < 30) {
        Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_RED);
        Graphics_drawString(ctx, (int8_t*)"!! VERY CLOSE !!", AUTO_STRING_LENGTH, 5, 100, OPAQUE);
    } else if (distance != -1 && distance < 50) {
        Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_YELLOW);
        Graphics_drawString(ctx, (int8_t*)"Object Close", AUTO_STRING_LENGTH, 5, 100, OPAQUE);
    } else if (distance != -1) {
        Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_GREEN);
        Graphics_drawString(ctx, (int8_t*)"Object Detected", AUTO_STRING_LENGTH, 5, 100, OPAQUE);
    }

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

void drawRadar(Graphics_Context *ctx, int angle, int distance) {
    static int lastX = 64, lastY = 120;

    // erase old line
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_drawLine(ctx, 64, 120, lastX, lastY);

    float rad = angle * M_PI / 180.0f;
    int x = 64 + 70 * cosf(rad);
    int y = 120 - 70 * sinf(rad);

    // new sweep
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_GREEN);
    Graphics_drawLine(ctx, 64, 120, x, y);

    lastX = x;
    lastY = y;

    // draw object
    if (distance > 0 && distance <= RADAR_RANGE) {
        float r = (distance / 200.0f) * 70;
        int ox = 64 + r * cosf(rad);
        int oy = 120 - r * sinf(rad);

        Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_RED);
        Graphics_fillCircle(ctx, ox, oy, 3);
    }
}
