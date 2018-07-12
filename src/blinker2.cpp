#include "blinker2.h"

Blinker2::Blinker2(Adafruit_NeoPixel &pixels) : _pixels(pixels),
                                                _pixelsCnt(pixels.numPixels())
{
    init();
}

Blinker2::Blinker2(Adafruit_NeoPixel &pixels, uint8_t brightness) : _pixels(pixels),
                                                _pixelsCnt(pixels.numPixels())
{
    init();
    _targetBrightness = brightness;
    pixels.setBrightness(brightness);
}

void Blinker2::init()
{
    for (uint16_t i = 0; i < SEQ_SIZE; i++)
    {
        _speed[i] = 10;
    }
}

void Blinker2::start()
{
    _pixels.begin();
}

void Blinker2::loop()
{
    _now = millis();

    //check time for fade
    if (_now - _tLastFade >= _speed[_seqIndex])
    {
        _tLastFade = _now;
        //get target color
        auto r = getC(_seq[_seqIndex], 0);
        auto g = getC(_seq[_seqIndex], 1);
        auto b = getC(_seq[_seqIndex], 2);
        uint8_t currentBrightness = _pixels.getBrightness();
        bool wasOk = _r == r && _b == b && _g == g && currentBrightness == _targetBrightness;

        _r = moveToTarget(_r, r);
        _g = moveToTarget(_g, g);
        _b = moveToTarget(_b, b);

        if (_r == r && _b == b && _g == g)
        { //target achived
            //check time for change color to next in sequnece
            if (_now - _tLastColor >= _colorDelay)
            {
                _tLastColor = _now;
                _seqIndex++;
                // Serial.printf("change color to #%d\n", _seqIndex);
                if (_seqIndex >= _seqCnt)
                { //loop colors
                    //Serial.println("reset sequence");
                    _seqIndex = 0;
                    _tLastColor = _now;
                }
            }
            if (wasOk)
            {
                return;
            }
        }

        for (uint16_t i = 0; i < _pixelsCnt; i++)
        {
            _pixels.setPixelColor(i, (uint8_t)_r, (uint8_t)_g, (uint8_t)_b);
        }

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
    }
}

void Blinker2::setSeqCnt(int seqCnt)
{
    if (seqCnt > SEQ_SIZE || seqCnt < 1)
    {
        Serial.printf("seqCnt=%d is out of range [1, %d]\n", seqCnt, SEQ_SIZE);
        return;
    }
    _seqCnt = seqCnt;
    //Serial.printf("set _seqCnt = %d\n", _seqCnt);
}

void Blinker2::setColorDelay(int colorDelay)
{
    if (colorDelay < 0)
    {
        Serial.printf("colorDelay=%d is out of range [0, maxint]\n", colorDelay);
        return;
    }
    _colorDelay = colorDelay;
    //Serial.printf("set _colorDelay = %d\n", _colorDelay);
}

void Blinker2::setDelta(int delta)
{
    if (delta > 255 || delta < 0)
    {
        Serial.printf("delta=%d is out of range [0, 255]\n", delta);
        return;
    }
    _delta = delta;
    //Serial.printf("set _delta = %d\n", _delta);
}

void Blinker2::setSeqColor(uint16_t index, int color, uint32_t speed)
{
    if (index >= _seqCnt || index < 0)
    {
        Serial.printf("index=%d is out of range [0, %d]\n", index, (_seqCnt - 1));
        return;
    }
    _seq[index] = color;
    _speed[index] = speed;
    // Serial.printf("set _seq[%d]= %#08x\n", index, _seq[index]);
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
int Blinker2::getSeqColor(uint16_t index)
{
    if (index >= _seqCnt || index < 0)
    {
        Serial.printf("index=%d is out of range [0, %d]\n", index, (_seqCnt - 1));
        return 0;
    }
    return _seq[index];
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