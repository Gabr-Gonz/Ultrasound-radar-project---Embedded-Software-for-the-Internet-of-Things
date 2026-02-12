#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <ti/grlib/grlib.h>

Graphics_Context g_sContext;


void main(void){
    // stop watchdog timer
    MAP_WDT_A_holdTimer();

    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    // UI colors configuration
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);

    // draw grey concentric circles
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DIM_GRAY);
    Graphics_drawCircle(&g_sContext, 64, 125, 27);
    Graphics_drawCircle(&g_sContext, 64, 125, 55);
    Graphics_drawCircle(&g_sContext, 64, 125, 82);

    // write status text
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_LIME);
    Graphics_drawStringCentered(&g_sContext, (int8_t *)"SCANNING...", AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);

    while(1){
        // draw a scanning line
        static int x = 0;
        Graphics_drawLine(&g_sContext, 64, 120, x, 60);

        x = (x + 5) % 128;

        // wait 100ms between frames
        __delay_cycles(3000000);
    }
}
