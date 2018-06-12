#include "blinker2.h"

Blinker2::Blinker2(Adafruit_NeoPixel &pixels) : _pixels(pixels)
{
}

void Blinker2::start(){
    _pixels.begin();
}

void Blinker2::setData(uint32_t data){
    setMode(_mode, _dataMode, data);
}

void Blinker2::setMode(Mode mode){
    setMode(mode, _dataMode, _data);
}

void Blinker2::setDataMode(DataMode dataMode){    
    setMode(_mode, dataMode, _data);
}

void Blinker2::setMode(Mode mode, DataMode dataMode, uint32_t data){
    if(_mode == mode && _dataMode == dataMode && _data == data){
        //Serial.println("setMode but nothing changed");
        return;
    }
    Serial.printf("setMode mode=%d dataMode=%d data=%d\n", mode, dataMode, data);
    //store modes
    _data = data;
    if(_mode != mode || _dataMode != dataMode){
        Serial.println("changed mode or dataMode");
        _mode = mode;    
        _dataMode = dataMode;
        //reset delays for color changes
        _tLastColor = 0;
        _tLastFade = 0;
        //reset seqIndex
        _seqIndex = 0;
    }else{
        Serial.println("changed only data. skip reset _seqIndex");
    }
    
    
    
    int gardientIndex = 0;//for Mode::M_DATA only
    uint32_t dataColor = 0;
    switch (_mode)
    {
        case Mode::M_DATA:
            gardientIndex = (int)(_data / 17.0) - 20;
            if (gardientIndex >= GRADIENT_SIZE)
            {
                gardientIndex = GRADIENT_SIZE - 1;
            }
            else if (gardientIndex < 0)
            {
                gardientIndex = 0;
            }
            dataColor = _gradient[gardientIndex];
            switch(_dataMode){
                case DM_ON:
                    _seq[0] = dataColor;
                    _seqCnt = 1;
                    _fadeDelay = 10;
                    _colorDelay = 10000;
                    break;
                case DM_OFF:                    
                    _seq[0] = 0x000000;
                    _seqCnt = 1;
                    _fadeDelay = 10;
                    _colorDelay = 10000;
                    break;
                case DM_FADEOUT:
                    for(int i=0;i<5;i++){
                        _seq[i] = 0x000000;                        
                    }
                    for(int i=5;i<SEQ_SIZE;i++){
                        _seq[i] = dataColor;
                    }                    
                    _seqCnt = SEQ_SIZE;
                    _fadeDelay = 3;      
                    _delta = 2;              
                    _colorDelay = 100;
                    break;
                case DM_FADEIN:
                    for(int i=0;i<5;i++){                        
                        _seq[i] = dataColor;
                    }
                    for(int i=5;i<SEQ_SIZE;i++){
                        _seq[i] = 0x000000;
                    }                    
                    _seqCnt = SEQ_SIZE;
                    _fadeDelay = 10;                   
                    _delta = 2;
                    _colorDelay = 100;
                    break;
               default:
                    break;
            }
            break;
        case M_CONFIG:
                _seqCnt = 4;
                _seq[0] = 0x0000FF;
                _seq[1] = 0x000000;
                _seq[2] = 0x000000;
                _seq[3] = 0x000000;
                _fadeDelay = 3;
                _colorDelay = 50;
                _delta = 15;
            break;
        case M_RECONFIG:
                _seqCnt = 4;
                _seq[0] = 0x00FF00;
                _seq[1] = 0x000000;
                _seq[2] = 0x000000;
                _seq[3] = 0x000000;
                _fadeDelay = 3;
                _colorDelay = 50;
                _delta = 15;
            break;
        case M_OTA:
                _seqCnt = 3;
                _seq[0] = 0xFF0000;
                _seq[1] = 0x0000FF;
                _seq[2] = 0x00FF00;
                _fadeDelay = 3;
                _delta = 20;
                _colorDelay = 0;
            break;
        default:
            break;
    }
}

void Blinker2::loop(){
    _now = millis();
    
    //check time for fade 
    if(_now - _tLastFade >= _fadeDelay){
        _tLastFade = _now;
        //get target color
        auto r = getC( _seq[_seqIndex], 0);
        auto g = getC( _seq[_seqIndex], 1);
        auto b = getC( _seq[_seqIndex], 2);

       _r = moveToTarget(_r, r);
       _g = moveToTarget(_g, g);
       _b = moveToTarget(_b, b);

       if(_r == r && _b == b && _g == g){//target achived
        //check time for change color to next in sequnece
        if(_now - _tLastColor >= _colorDelay){
            _tLastColor = _now;
            _seqIndex++;
           // Serial.printf("change color to #%d\n", _seqIndex);
            if(_seqIndex>=_seqCnt){//loop colors
            //Serial.println("reset sequence");
                _seqIndex = 0;
                _tLastColor = _now;
            }
        }
       }

       _pixels.setPixelColor(0, //todo support multiple leds
                            (uint8_t)_r, 
                            (uint8_t)_g, 
                            (uint8_t)_b);
       _pixels.show();
    }
    

}

#pragma region util methods

uint32_t Blinker2::getC(uint32_t color, byte i){
    return (color >> (8*(2-i))) & 0xff;
}


uint32_t Blinker2::moveToTarget(uint32_t c, uint32_t t){
    if(c==t || (abs(c - t) <= _delta)){
        return t;
    }
    if(c < t){
        return c + _delta;
    }
    return c - _delta;
}

#pragma endregion