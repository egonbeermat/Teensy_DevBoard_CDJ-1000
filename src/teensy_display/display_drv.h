#ifndef DISPLAY_DRV_H
#define DISPLAY_DRV_H

#include "../globals.h"


#if defined(NT35510)
#if defined(NT35516)
#include <NT35516_t4p_conf.h>
#include <NT35516_t4p.h>
extern NT35516_t4p tft;
#else
#include <NT35510_t4p_conf.h>
#include <NT35510_t4p.h>
extern NT35510_t4p tft;
#endif
#endif

#if defined(SSD1963)
#include <SSD1963_t4p_conf.h>
#include <SSD1963_t4p.h>
extern SSD1963_t4p tft;
#endif

////////
// Touch
////////
#if defined(USE_LCD_DISP)
#include <Adafruit_FT6206.h>
extern Adafruit_FT6206 ts;
#define POINT_TYPE TS_Point
#endif

#if defined(TEENSY41)
#include <XPT2046_Touchscreen.h>
extern XPT2046_Touchscreen ts;
#define POINT_TYPE TS_Point
#endif

bool disp_init(uint8_t refreshHz);
void disp_setRefreshRate(uint8_t refreshHz);
void disp_setBrightness(uint8_t brightness);
void disp_setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void disp_pushPixels8bitPalette(uint8_t * pBuf, uint8_t * pBufEnd, uint16_t * color_palette);
void disp_pushPixels16bit(uint16_t * pBuf, uint16_t * pBufEnd);
#if defined(SSD1963) && defined(USE_TEAR)
void disp_setTearingEffect(bool useTearing);
void disp_setTearingScanLine(uint16_t scanline);
#endif

bool touch_begin();
POINT_TYPE touch_translatePoint(int16_t x1, int16_t y1);

#endif // DISPLAY_DRV_H