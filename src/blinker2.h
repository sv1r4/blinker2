#ifndef BLINKER2_H
#define BLINKER2_H


#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

#ifndef SEQ_SIZE
#define SEQ_SIZE  12
#endif

#ifndef PIXELS
#define PIXELS 4
#endif

class Blinker2
{
    private:       
        Adafruit_NeoPixel &_pixels;        
        uint32_t _speed[SEQ_SIZE];//array of ms
        uint32_t _speedCur = 1;
        uint32_t _colorDelay;//ms
        uint32_t _delta = 10;
        uint32_t _seq[PIXELS][SEQ_SIZE];//array of colors
        uint32_t _seqIndex;
        uint32_t _seqCnt = 0;//cnt of colors (should be less then SEQ_SIZE)                
        //util        
        unsigned long _tLastColor[PIXELS];
        bool _isColorDelayDone[PIXELS];
        uint32_t _r[PIXELS];
        uint32_t _g[PIXELS];
        uint32_t _b[PIXELS]; 
        //methods
        uint32_t getC(uint32_t color, byte i);
        uint32_t moveToTarget(uint32_t c, uint32_t t);
        uint8_t _targetBrightness;
        void init();
        Ticker _tickerLoop;
        void updateLoopTicker();
    public:    
        Blinker2(Adafruit_NeoPixel &pixels);
        Blinker2(Adafruit_NeoPixel &pixels, uint8_t brightness);
        void start();     
        void setSeqCnt(int seqCnt);        
        void setColorDelay(int colorDelay);
        void setDelta(int delta);
        void setSeqColor(uint16_t index, int color, uint32_t speed, uint8_t p);
        uint32_t getSeqCnt();
        int getColorDelay();
        int getDelta();
        int getSeqColor(uint16_t index, uint8_t p);
        uint32_t getSpeedColor(uint16_t index);
        void setSeqIndex(uint32_t index);
        uint32_t getSeqIndex();
        void setMaxBrightness(uint8_t val);
        void initBrightness(uint8_t val);
        bool loopPixel(uint8_t p);
        void loop();

};

#endif
