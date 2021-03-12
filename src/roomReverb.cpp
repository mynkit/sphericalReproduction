//
//  roomReverb.cpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/12.
//

#include "roomReverb.hpp"

roomReverb::roomReverb(float _maxHoldTime, int _sampleRate) {
    //! 最大保持時間(ms)
    maxHoldTime = _maxHoldTime;
    //! サンプルレート
    sampleRate = _sampleRate;
    //! 保持されている音のサンプルサイズ
    size = maxHoldTime * 0.001 * _sampleRate;
    //! バッファ
    buffer = new float[size];
    //! 元のsampleが保持用bufferの今どの位置か
    originalSamplePoint = 0;
}

// sampleを保持用バッファにいれていく
void roomReverb::feed(float sample) {
    buffer[originalSamplePoint] = sample;
    originalSamplePoint++;
    // indexがバッファサイズを超えたら0に戻す
    if(originalSamplePoint >= size){ originalSamplePoint = 0;}
}

float roomReverb::effect(float sample, vector<float> distances, vector<int> orders, float speedOfSound, float soundAbsorptionCoefficient, float wet) {
    // inputの音をそのまま保持用バッファにいれる
    feed(sample);
    int delayTime;
    float decay;
    int point;
    int phase;
    float wetSample = 0;
    if (wet > 1) {
        wet = 1.;
    } else if (wet < 0) {
        wet = 0.;
    }
    if(originalSamplePoint >= size){originalSamplePoint = 0;}
    for(int i = 0; i < distances.size(); i++) {
        delayTime = 1000. * distances.at(i) / speedOfSound; // ms単位にする
        if(delayTime > maxHoldTime){
            continue;
        } // delaytimeが保持時間を超えたときは破棄
        decay = powf(soundAbsorptionCoefficient, orders.at(i)); // soundAbsorptionCoefficient ** order
        orders.at(i) % 2 == 0 ? (phase = 1) : (phase = -1);
        if(point >= size){phase = -1;}
        point = originalSamplePoint + size - (delayTime*0.001*sampleRate) - 1;
        if(point >= size){point -= size;}
        wetSample += buffer[point] * phase * decay;
    }
    return (1. - 0.2 * wet) * (sample + wetSample * wet);
}

