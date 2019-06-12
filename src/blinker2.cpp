#include "blinker2.h"

Blinker2::Blinker2(Adafruit_NeoPixel &pixels) : _pixels(pixels)
{
    init();
}

Blinker2::Blinker2(Adafruit_NeoPixel &pixels, uint8_t brightness) : _pixels(pixels)
{
    init();
    _targetBrightness = brightness;
    pixels.setBrightness(brightness);
}

void Blinker2::init()
{
    for (uint16_t i = 0; i < SEQ_SIZE; i++)
    {
        for(uint16_t j = 0; j< PIXELS; j++){
            _speed[i] = 10;
        }
    }
}



void Blinker2::start()
{
    _pixels.begin();
    updateLoopTicker();
}

void Blinker2::updateLoopTicker(){
    //todo fix hardcode 3 pixels
    
    _tickerLoop.detach();
    std::function<void(void)> fTickerLoop = std::bind(&Blinker2::loop, this);
    _tickerLoop.attach_ms(_speed[_seqIndex], fTickerLoop);
    
}

void Blinker2::loop(){
    bool allDone = true;    
    for(uint8_t p =0;p<PIXELS;p++){
        if(!loopPixel(p)){
            allDone = false;
        }
    }

    if(allDone){        
        _seqIndex++; 
        if (_seqIndex >= _seqCnt)
        { 
            _seqIndex = 0;                
        }
        Serial.printf("all done move to seqindex %d\n", _seqIndex);
        updateLoopTicker();   
    }
    
}


bool Blinker2::loopPixel(uint8_t p)
{    
    unsigned long now = millis();

    //get target color
    auto r = getC(_seq[p][_seqIndex], 0);
    auto g = getC(_seq[p][_seqIndex], 1);
    auto b = getC(_seq[p][_seqIndex], 2);
    uint8_t currentBrightness = _pixels.getBrightness();
    bool wasOk = _r[p] == r && _b[p] == b && _g[p] == g && currentBrightness == _targetBrightness;

    _r[p] = moveToTarget(_r[p], r);
    _g[p] = moveToTarget(_g[p], g);
    _b[p] = moveToTarget(_b[p], b);

    //if last pixel in seq
    if (_r[p] == r && _b[p] == b && _g[p] == g)
    { 
        //target achived
        //check time for change color to next in sequnece
        bool isColorDelayDone = now - _tLastColor[p] >= _colorDelay;
        if (isColorDelayDone)
        {
            _isColorDelayDone[p] = true;
            _tLastColor[p] = now;
        }
        if (wasOk)
        {
          //  Serial.printf("was ok %d\n", p);
            return _isColorDelayDone[p];
        }
    }else{
        _isColorDelayDone[p] = false;
    }
    

    //Serial.printf("set pixel %d b= %d\n", p, (uint8_t)_b[p]);
    _pixels.setPixelColor(p, (uint8_t)_r[p], (uint8_t)_g[p], (uint8_t)_b[p]);


#pragma region fade brightness
    
    if (currentBrightness < _targetBrightness)
    {
        _pixels.setBrightness(++currentBrightness);
    }
    else if (currentBrightness > _targetBrightness)
    {
        _pixels.setBrightness(--currentBrightness);
    }
#pragma endregion

    _pixels.show();
    return false;
}

void Blinker2::setSeqCnt(int seqCnt)
{
    if (seqCnt > SEQ_SIZE || seqCnt < 1)
    {
        Serial.printf("seqCnt=%d is out of range [1, %d]\n", seqCnt, SEQ_SIZE);
        return;
    }
    _seqCnt = seqCnt;
    Serial.printf("set _seqCnt = %d\n", _seqCnt);
}

void Blinker2::setColorDelay(int colorDelay)
{
    if (colorDelay < 0)
    {
        Serial.printf("colorDelay=%d is out of range [0, maxint]\n", colorDelay);
        return;
    }
    _colorDelay = colorDelay;
    Serial.printf("set  _colorDelay = %d\n", _colorDelay);
}

void Blinker2::setDelta(int delta)
{
    if (delta > 255 || delta < 0)
    {
        Serial.printf("delta=%d is out of range [0, 255]\n", delta);
        return;
    }
    _delta = delta;
    Serial.printf("set _delta = %d\n", _delta);
}

void Blinker2::setSeqColor(uint16_t index, int color, uint32_t speed, uint8_t p)
{
    if (index >= _seqCnt || index < 0)
    {
        Serial.printf("index=%d is out of range [0, %d]\n", index, (_seqCnt - 1));
        return;
    }
    _seq[p][index] = color;
    _speed[index] = speed;
     Serial.printf("set pixel #%d._seq[%d]= %#08x\n", p, index, _seq[p][index]);
}

uint32_t Blinker2::getSeqCnt()
{
    return _seqCnt;
}
int Blinker2::getColorDelay()
{
    return _colorDelay;
}
int Blinker2::getDelta()
{
    return _delta;
}
int Blinker2::getSeqColor(uint16_t index, uint8_t p)
{
    if (index >= _seqCnt || index < 0)
    {
        Serial.printf("index=%d is out of range [0, %d]\n", index, (_seqCnt - 1));
        return 0;
    }
    return _seq[p][index];
}

uint32_t Blinker2::getSpeedColor(uint16_t index)
{
    if (index >= _seqCnt || index < 0)
    {
        Serial.printf("index=%d is out of range [0, %d]\n", index, (_seqCnt - 1));
        return 0;
    }
    return _speed[index];
}

void Blinker2::setSeqIndex(uint32_t index)
{
    if (index >= _seqCnt || index < 0)
    {
        Serial.printf("index=%d is out of range [0, %d]\n", index, (_seqCnt - 1));
        return;
    }
    _seqIndex = index;
}

uint32_t Blinker2::getSeqIndex()
{
    return _seqIndex;
}

void Blinker2::setMaxBrightness(uint8_t val)
{
    Serial.printf("set _targetBrightness= %i\n", val);
    _targetBrightness = val;
}

void Blinker2::initBrightness(uint8_t val){
    Serial.printf("init brightness to %i\n", val);
    _pixels.setBrightness(val);
    _pixels.show();
}

#pragma region util methods

uint32_t Blinker2::getC(uint32_t color, byte i)
{
    return (color >> (8 * (2 - i))) & 0xff;
}

uint32_t Blinker2::moveToTarget(uint32_t c, uint32_t t)
{
    if (c == t || (abs(c - t) <= _delta))
    {
        return t;
    }
    if (c < t)
    {
        return c + _delta;
    }
    return c - _delta;
}

#pragma endregion