#include <Energia.h>
#include <math.h>
#include "Display.h"

#define RADAR_RANGE 200

// ====== INITIALIZE STATIC UI ======
void initDisplayUI() {
    display.begin();
    display.clearScreen();
    display.setFont(Font_Terminal6x8);
    display.setTextColor(WHITE);
    display.gText(10, 10, "MSP432 RADAR SYSTEM");
}

// ====== DRAW STATIC UI ELEMENTS ======
void displayUI() {
    display.fillRectangle(0, 20, 128, 140, BLACK);
    display.gText(5, 25, "STATUS:");
    display.gText(5, 45, "ANGLE:");
    display.gText(5, 65, "DIST:");
    display.gText(5, 85, "ALERT:");
}

// ====== UPDATE DYNAMIC UI ======
void updateUI(int angle, int distance) {
    char buffer[32];

    // Status area
    display.fillRectangle(60, 20, 128, 12, BLACK);
    display.gText(60, 20, "ACTIVE");  // main code decides active/stop

    // Angle
    sprintf(buffer, "%3d deg", angle);
    display.fillRectangle(60, 40, 128, 12, BLACK);
    display.gText(60, 40, buffer);

    // Distance
    display.fillRectangle(60, 60, 128, 12, BLACK);
    if (distance == -1)
        display.gText(60, 60, "--");
    else {
        sprintf(buffer, "%3d cm", distance);
        display.gText(60, 60, buffer);
    }

    // Alert text
    display.fillRectangle(5, 100, 128, 30, BLACK);
    if (distance != -1 && distance < 30) {
        display.setTextColor(RED);
        display.gText(5, 100, "!! VERY CLOSE !!");
    } else if (distance != -1 && distance < 50) {
        display.setTextColor(YELLOW);
        display.gText(5, 100, "Object Close");
    } else if (distance != -1) {
        display.setTextColor(GREEN);
        display.gText(5, 100, "Object Detected");
    }
    display.setTextColor(WHITE);
}

// ====== RADAR SWEEP GRAPHIC ======
void drawRadar(int angle, int distance) {
    static int lastX = 64, lastY = 120;

    // Delete old sweep line
    display.drawLine(64, 120, lastX, lastY, BLACK);

    // Convert angle to rad
    float rad = angle * 3.14159 / 180.0;
    int x = 64 + 70 * cos(rad);
    int y = 120 - 70 * sin(rad);

    // Draw new sweep line
    display.drawLine(64, 120, x, y, GREEN);

    lastX = x;
    lastY = y;

    // Draw detected object on radar
    if (distance > 0 && distance <= RADAR_RANGE) {
        float r = (distance / 200.0) * 70;
        int ox = 64 + r * cos(rad);
        int oy = 120 - r * sin(rad);
        display.fillCircle(ox, oy, 3, RED);
    }
}
