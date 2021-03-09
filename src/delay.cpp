//
//  delay.cpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/08.
//

#include "delay.hpp"

delay::delay(float _maxHoldTime, int _sampleRate, int delayTime, float decayRate) {
    //! 最大保持時間(ms)
    maxHoldTime = _maxHoldTime;
    //! サンプルレート
    sampleRate = _sampleRate;
    //! 現在のdelaytime(ms)
    currentDelayTime = min(delayTime, maxHoldTime);
    //! 現在のdecayrate
    currentDecayRate = decayRate;
    //! 保持されている音のサンプルサイズ
    size = maxHoldTime * 0.001 * _sampleRate;
    //! バッファ
    buffer.resize(size);
    //! 元のsampleが保持用bufferの今どの位置か
    originalSamplePoint = 0;
}

// sampleを保持用バッファにいれていく
void delay::feed(float sample) {
    buffer[originalSamplePoint] = sample;
    originalSamplePoint++;
    // indexがバッファサイズを超えたら0に戻す
    if(originalSamplePoint >= size){ originalSamplePoint = 0;}
}

float delay::effect(float sample) {
    int point = originalSamplePoint + size - (currentDelayTime*0.001*sampleRate) - 1;
    if(point >= size){point -= size;}
    return buffer[point] * currentDecayRate;
}

void delay::setDelayTime(float newDelayTime) {
    currentDelayTime = newDelayTime;
}

void delay::setDecayRate(float newDecayRate) {
    currentDecayRate = newDecayRate;
}
