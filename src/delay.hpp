//
//  delay.hpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/08.
//

#pragma once

#include <ofMain.h>

class delay {
    public:
        int maxHoldTime;
        vector<float> buffer;
        int size;
        int sampleRate;
        int originalSamplePoint;
        int currentDelayTime;
        float currentDecayRate;

        delay(float _maxHoldTime, int _sampleRate, int delayTime, float decayRate);
        ~delay();

        void feed(float sample);
        float effect(float sample);
        void setDelayTime(float newDelayTime);
        void setDecayRate(float newDecayRate);
};
