#include <Arduino.h>
#include "display_drv.h"
#include "clockspeed/beermat_clockspeed.h"
#include "teensy_display/pin_defines.h"

extern uint32_t targetFrequency;

uint8_t tftRotation = 1;
uint8_t displayRefreshRate = 60;

//////////
// Display
//////////
#if defined(NT35510)
NT35510_t4p tft = NT35510_t4p(TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
#endif

#if defined(SSD1963)
SSD1963_t4p tft = SSD1963_t4p(TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
#endif

////////
// Touch
////////
#if defined(USE_LCD_DISP)
Adafruit_FT6206 ts = Adafruit_FT6206();
#endif

#if defined(TEENSY41)
XPT2046_Touchscreen ts(TSR_CS);
#endif

FLASHMEM bool disp_init(uint8_t displayRefreshRate)
{
#if defined(NT35510) || defined(SSD1963)    
    // Slow CPU down for init. SSD1963 is particularly sensitive to this until configured
    beermat_set_arm_clock(250'000'000, 0);
    
    tft.begin();

    //Check begin worked
#if defined(NT35510)
    uint8_t selfDiagnosisSuccess = 0xF0;
#elif defined(SSD1963)
    uint8_t selfDiagnosisSuccess = 0xFF;
#endif    
    uint8_t selfDiagnosisRes = tft.getSelfDiagnosis();
    bool initSuccess = (selfDiagnosisRes == selfDiagnosisSuccess);
    Serial.printf("Display init self diagnosis: %s\n", initSuccess ? "TRUE" : "FALSE");
    // Restore desired CPU speed
    beermat_set_arm_clock(targetFrequency * 1'000'000, 0);

    if (initSuccess == false) {
        return false;
    }

    tft.setRotation(tftRotation);
#if defined(NT35510)    
    tft.setBitDepth(16);
#endif    
    disp_setRefreshRate(displayRefreshRate);
    disp_setBrightness(80);

#if defined(SSD1963) && defined(USE_TEAR)
    disp_setTearingEffect(true);
    disp_setTearingScanLine(479);
#endif
    return true;
#endif // defined(NT35510) || defined(SSD1963)    
    return true;
}

FLASHMEM void disp_setRefreshRate(uint8_t refreshHz)
{
#if defined(NT35510) || defined(SSD1963)    
    tft.setRefreshRate(refreshHz);
#endif // defined(NT35510) || defined(SSD1963)      
}

FLASHMEM void disp_setBrightness(uint8_t brightness)
{
    //Input is percentage, convert to 0-255
#if defined(NT35510) 
    uint8_t br = (float)(2.55 * (float)brightness);
    analogWrite(TFT_BL, br);
#endif
#if defined(SSD1963) 
    uint8_t br = (float)(2.55 * (float)brightness);
    tft.setBacklight(br);
#endif 
}

FASTRUN void disp_setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
#if defined(NT35510) || defined(SSD1963)        
    tft.setAddrWindow(x1, y1, x2, y2);
#endif // defined(NT35510) || defined(SSD1963)      
}

FASTRUN void disp_pushPixels16bit(uint16_t * pBuf, uint16_t * pBufEnd) {
#if defined(NT35510) || defined(SSD1963)        
    tft.pushPixels16bit(pBuf, pBufEnd);
#endif // defined(NT35510) || defined(SSD1963)      
}

#if defined(SSD1963) && defined(USE_TEAR)
FLASHMEM void disp_setTearingEffect(bool useTearing)
{
    tft.setTearingEffect(useTearing);
}

FLASHMEM void disp_setTearingScanLine(uint16_t scanline)
{
    tft.setTearingScanLine(scanline);
}
#endif //defined(USE_TEAR)


FLASHMEM bool touch_begin()
{
#if defined(USE_LCD_DISP)    
    return ts.begin(20);
#endif    
#if defined(TEENSY41)
    ts.setRotation(tftRotation);
    ts.begin();
#endif  
    return true;
}

FASTRUN POINT_TYPE touch_translatePoint(int16_t x1, int16_t y1)
{
  int16_t tsMinX = 111, tsMaxX = 4008, tsMinY = 230, tsMaxY = 3865;
  POINT_TYPE p;
  p.x = 0;
  p.y = 0;

#if defined(USE_LCD_DISP)
    p.x = x1;
    p.y = y1;  
#endif  
#if defined(NT35510)
    p.x = map(x1, tsMaxX, tsMinX, 0, SCREEN_WIDTH - 1);
    p.y = map(y1, tsMaxY, tsMinY, 0, SCREEN_HEIGHT - 1);
#endif
#if defined(SSD1963)
    p.y = map(x1, tsMinX, tsMaxX, 0, SCREEN_HEIGHT - 1);
    p.x = map(y1, tsMaxY, tsMinY, 0, SCREEN_WIDTH - 1);
#endif

  return p;
}