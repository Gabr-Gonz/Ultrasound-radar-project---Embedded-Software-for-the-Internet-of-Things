#include "Display.h"
#include <math.h>
#include <stdio.h>

#define MAX_SENSOR_DIST 400  // maximum range of the sensor, in cm
#define RADAR_RADIUS    110  // radius of the radar on the display, in pixel

void initDisplayUI(Graphics_Context *ctx){
    Graphics_clearDisplay(ctx);                                 // clear the display
    Graphics_setFont(ctx, &g_sFontFixed6x8);                    // set the font used
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);     // set the color of what we will write next

    // print the title of the project at the given coordinates
    Graphics_drawString(ctx, (int8_t*)"MSP432 RADAR SYSTEM", AUTO_STRING_LENGTH, 10, 10, GRAPHICS_OPAQUE_TEXT);
}

//function that draws the circles on the map to give a visual idea of how far the object using the scale of the map
void displayUI(Graphics_Context *ctx){
    Graphics_clearDisplay(ctx); //clear the display

    // draws the circles on the map, to scale and give a visual idea of how far is the object from the sensor. We draw three grey circles,
    // respectively that represent 100 cm, 200 cm and 300 cm
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DIM_GRAY);
    Graphics_drawCircle(ctx, 64, 125, 27);  // ~100cm
    Graphics_drawCircle(ctx, 64, 125, 55);  // ~200cm
    Graphics_drawCircle(ctx, 64, 125, 82);  // ~300cm

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE); // set the color back to white for the next drawings
    Graphics_drawLine(ctx, 0, 15, 128, 15);                 // draw the line on the top to separate the top section from the map
}

// function that writes on the top-right of the display the distance of the object
void updateUI(Graphics_Context *ctx, int distance){
    // declare a buffer to store the value of the distance and set the font that we will write it
    char buffer[16];
    Graphics_setFont(ctx, &g_sFontFixed6x8);

    // draw a black rectangle on the distance written before to erase it, so we will be able to write the new distance
    Graphics_Rectangle rect = {80, 0, 128, 14};
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(ctx, &rect);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_SPRING_GREEN);  // set the color to write the distance to green
    // check if the sensor captured an object in the range of the possible distance and set the buffer accordingly to what has been/has not been scanned
    if (distance > 0 && distance < 400) {
        sprintf(buffer, "%3d cm", distance);
    } else {
        sprintf(buffer, "---");
    }
    Graphics_drawString(ctx, (int8_t*)buffer, AUTO_STRING_LENGTH, 85, 2, GRAPHICS_OPAQUE_TEXT); // write the value of the distance
}

void drawRadar(Graphics_Context *ctx, int angle, int distance){
    // static variables used to keep track of the last line drawn. Since they are static, the value on the declaration (i.e., 64 and 125) will be
    // stored only on the first call of the function, but on the next calls, their values will be equal to the last value to which they have been
    // updated, so this first line will be ignored on the calls after the first one
    static int lastX = 64, lastY = 125;

    // erase the line used before by drawing another black line on top of it
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_drawLine(ctx, 64, 125, lastX, lastY);

    /*
     * we need to draw the line of the current scan, but since the sensor gives us the angle, we have to convert the polar coordinates to Cartesian
     * coordinates, so we use the variable "rad" to keep the value in radians, which is calculated with the formula angle * pi / 180, and then
     * we calculate the values of the cartesian coordinates by using the cosine and the sine, but we also need to add the value 64
     * to the x-coordinate, since it refers to the center of the display, and we need to subtract the value from 125 for the y-coordinate,
     * since the value of the bottom of the screen is 128, but we start from a little above that
    */
    float rad = angle * M_PI / 180.0f;
    int x = 64 + RADAR_RADIUS * cosf(rad);
    int y = 125 - RADAR_RADIUS * sinf(rad);

    // set the color to green and draw the line of the current scan
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_GREEN);
    Graphics_drawLine(ctx, 64, 125, x, y);

    //update the variables for the next scan
    lastX = x;
    lastY = y;

    // check if the object is within the radar's detection range
    if (distance > 2 && distance <= MAX_SENSOR_DIST) {

        // we compute the r_scaled value by starting from this proportion: distance / max_sensor_dist = r_scaled / radar_radius
        float r_scaled = ((float)distance / MAX_SENSOR_DIST) * RADAR_RADIUS;

        // and then we calculate the coordinates on the display by scaling with the value just computed
        int ox = 64 + r_scaled * cosf(rad);
        int oy = 125 - r_scaled * sinf(rad);

        // lastly, we draw the red dot on the map, representing the scanned object
        Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_RED);
        Graphics_fillCircle(ctx, ox, oy, 2);
    }
}


void clearRadarMap(Graphics_Context *ctx){
    // erase the map of the last scan by drawing a black rectangle on top of it
    Graphics_Rectangle mapArea = {0, 16, 127, 127};
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(ctx, &mapArea);

    // draws the circles on the map, to scale and give a visual idea of how far is the object from the sensor. We draw three grey circles,
    // respectively that represent 100 cm, 200 cm and 300 cm
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DIM_GRAY);
    Graphics_drawCircle(ctx, 64, 125, 27);
    Graphics_drawCircle(ctx, 64, 125, 55);
    Graphics_drawCircle(ctx, 64, 125, 82);
}
