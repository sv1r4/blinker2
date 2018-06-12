#ifndef BLINKER2_H
#define BLINKER2_H


#include "Adafruit_NeoPixel.h"
/*
#ifdef GRADIENT_HSV
uint32_t gradient[64] = {0x00FF00,0x00FF10,0x00FF20,0x00FF30,0x00FF40,0x00FF50,0x00FF61,0x00FF71,0x00FF81,0x00FF91,0x00FFA1,0x00FFB2,0x00FFC2,0x00FFD2,0x00FFE2,0x00FFF2,0x00FAFF,0x00EAFF,0x00DAFF,0x00CAFF,0x00BAFF,0x00A9FF,0x0099FF,0x0089FF,0x0079FF,0x0069FF,0x0059FF,0x0048FF,0x0038FF,0x0028FF,0x0018FF,0x0008FF,0x0800FF,0x1800FF,0x2800FF,0x3800FF,0x4800FF,0x5900FF,0x6900FF,0x7900FF,0x8900FF,0x9900FF,0xAA00FF,0xBA00FF,0xCA00FF,0xDA00FF,0xEA00FF,0xFA00FF,0xFF00F2,0xFF00E2,0xFF00D2,0xFF00C2,0xFF00B2,0xFF00A1,0xFF0091,0xFF0081,0xFF0071,0xFF0061,0xFF0050,0xFF0040,0xFF0030,0xFF0020,0xFF0010,0xFF0000};
#endif
*/
//#ifdef GRADIENT_HSV_INVERT
//uint32_t Gradient[GRADIENT_SIZE] = {0x00FF00,0x08FF00,0x10FF00,0x18FF00,0x20FF00,0x28FF00,0x30FF00,0x38FF00,0x40FF00,0x48FF00,0x50FF00,0x59FF00,0x61FF00,0x69FF00,0x71FF00,0x79FF00,0x81FF00,0x89FF00,0x91FF00,0x99FF00,0xA1FF00,0xAAFF00,0xB2FF00,0xBAFF00,0xC2FF00,0xCAFF00,0xD2FF00,0xDAFF00,0xE2FF00,0xEAFF00,0xF2FF00,0xFAFF00,0xFFFA00,0xFFF200,0xFFEA00,0xFFE200,0xFFDA00,0xFFD200,0xFFCA00,0xFFC200,0xFFBA00,0xFFB200,0xFFAA00,0xFFA100,0xFF9900,0xFF9100,0xFF8900,0xFF8100,0xFF7900,0xFF7100,0xFF6900,0xFF6100,0xFF5900,0xFF5000,0xFF4800,0xFF4000,0xFF3800,0xFF3000,0xFF2800,0xFF2000,0xFF1800,0xFF1000,0xFF0800,0xFF0000};
//#endif
/*
#ifdef GRADIENT_RGB
uint32_t gradient[64] = {0x00FF00,0x04FA00,0x08F600,0x0CF200,0x10EE00,0x14EA00,0x18E600,0x1CE200,0x20DE00,0x24DA00,0x28D600,0x2CD200,0x30CE00,0x34CA00,0x38C600,0x3CC200,0x40BE00,0x44BA00,0x48B600,0x4CB200,0x50AE00,0x55AA00,0x59A500,0x5DA100,0x619D00,0x659900,0x699500,0x6D9100,0x718D00,0x758900,0x798500,0x7D8100,0x817D00,0x857900,0x897500,0x8D7100,0x916D00,0x956900,0x996500,0x9D6100,0xA15D00,0xA55900,0xAA5500,0xAE5000,0xB24C00,0xB64800,0xBA4400,0xBE4000,0xC23C00,0xC63800,0xCA3400,0xCE3000,0xD22C00,0xD62800,0xDA2400,0xDE2000,0xE21C00,0xE61800,0xEA1400,0xEE1000,0xF20C00,0xF60800,0xFA0400,0xFF0000};
#endif
*/
#define GRADIENT_SIZE 64
#define SEQ_SIZE  64

enum Mode
{
  M_DATA = 0,
  M_CONFIG = 1, //config in progress  
  M_OTA = 4,
  M_RECONFIG = 5, //reconfig in progress
};

//co2 data light mode
enum DataMode{
  DM_ON = 10,//always on
  DM_OFF = 11,//alway off
  DM_FADEIN = 12,//mostly off but fade in with interval
  DM_FADEOUT = 13//mostly on but fade out with interval
};

class Blinker2
{
    private:
        Adafruit_NeoPixel _pixels;
        Mode _mode = Mode::M_DATA;
        DataMode _dataMode = DataMode::DM_ON;
        uint32_t _fadeDelay = 5;//ms
        uint32_t _colorDelay = 0;//ms
        uint32_t _delta = 10;
        uint32_t _seq[SEQ_SIZE];//array of colors
        uint32_t _seqIndex = 0;//current color
        uint32_t _seqCnt = 1;//cnt of colors (should be less then SEQ_SIZE)                
        //util
        uint32_t _gradient[GRADIENT_SIZE] = {0x00FF00, 0x08FF00, 0x10FF00, 0x18FF00, 0x20FF00, 0x28FF00, 0x30FF00, 0x38FF00, 0x40FF00, 0x48FF00, 0x50FF00, 0x59FF00, 0x61FF00, 0x69FF00, 0x71FF00, 0x79FF00, 0x81FF00, 0x89FF00, 0x91FF00, 0x99FF00, 0xA1FF00, 0xAAFF00, 0xB2FF00, 0xBAFF00, 0xC2FF00, 0xCAFF00, 0xD2FF00, 0xDAFF00, 0xE2FF00, 0xEAFF00, 0xF2FF00, 0xFAFF00, 0xFFFA00, 0xFFF200, 0xFFEA00, 0xFFE200, 0xFFDA00, 0xFFD200, 0xFFCA00, 0xFFC200, 0xFFBA00, 0xFFB200, 0xFFAA00, 0xFFA100, 0xFF9900, 0xFF9100, 0xFF8900, 0xFF8100, 0xFF7900, 0xFF7100, 0xFF6900, 0xFF6100, 0xFF5900, 0xFF5000, 0xFF4800, 0xFF4000, 0xFF3800, 0xFF3000, 0xFF2800, 0xFF2000, 0xFF1800, 0xFF1000, 0xFF0800, 0xFF0000};
        uint32_t _data = 400;
        unsigned long _tLastFade = 0;
        unsigned long _tLastColor = 0;
        unsigned long _now = 0;
        uint32_t _r = 0; 
        uint32_t _g = 0;
        uint32_t _b = 0;
        void setMode(Mode mode, DataMode dataMode, uint32_t data);
        //methods
        uint32_t getC(uint32_t color, byte i);
        uint32_t moveToTarget(uint32_t c, uint32_t t);
    public:    
        Blinker2(Adafruit_NeoPixel &pixels);
        void start();
        void loop();
        //set predefined modes
        void setMode(Mode mode);
        void setDataMode(DataMode dataMode);
        void setData(uint32_t data);
        //custom seq config (loop fade color sequense)
        void setSeqCnt(int seqCnt);
        void setFadeDelay(int fadeDelay);
        void setColorDelay(int colorDelay);
        void setDelta(int delta);
        void setSeqColor(int index, int color);
        int getSeqCnt();
        int getFadeDelay();
        int getColorDelay();
        int getDelta();
        int getSeqColor(int index);

};

#endif
