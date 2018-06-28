#ifndef BLINKER2_H
#define BLINKER2_H


#include "Adafruit_NeoPixel.h"

#define SEQ_SIZE  64

class Blinker2
{
    private:       
        uint16_t _pixelsCnt;
        Adafruit_NeoPixel &_pixels;        
        uint32_t _speed[SEQ_SIZE];//array of ms
        uint32_t _speedCur = 1;
        uint32_t _colorDelay = 0;//ms
        uint32_t _delta = 10;
        uint32_t _seq[SEQ_SIZE];//array of colors
        uint32_t _seqIndex = 0;//current color
        uint32_t _seqCnt = 1;//cnt of colors (should be less then SEQ_SIZE)                
        //util        
        unsigned long _tLastFade = 0;
        unsigned long _tLastColor = 0;
        unsigned long _now = 0;
        uint32_t _r = 0; 
        uint32_t _g = 0;
        uint32_t _b = 0;        
        //methods
        uint32_t getC(uint32_t color, byte i);
        uint32_t moveToTarget(uint32_t c, uint32_t t);
        uint8_t _targetBrightness;
        void init();
    public:    
        Blinker2(Adafruit_NeoPixel &pixels);
        Blinker2(Adafruit_NeoPixel &pixels, uint8_t brightness);
        void start();
        void loop();        
        void setSeqCnt(int seqCnt);        
        void setColorDelay(int colorDelay);
        void setDelta(int delta);
        void setSeqColor(uint16_t index, int color, uint32_t speed);
        uint32_t getSeqCnt();
        int getColorDelay();
        int getDelta();
        int getSeqColor(uint16_t index);
        uint32_t getSpeedColor(uint16_t index);
        void setSeqIndex(uint32_t index);
        uint32_t getSeqIndex();
        void setMaxBrightness(uint8_t val);

};

#endif
