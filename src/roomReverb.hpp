//
//  roomReverb.hpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/12.
//

#pragma once

#include "ofMain.h"

class roomReverb {
    public:
        int maxHoldTime;
        float* buffer;
        int size;
        int sampleRate;
        int originalSamplePoint;
        int currentDelayTime;
        float currentDecayRate;

        roomReverb(float _maxHoldTime, int _sampleRate);
        ~roomReverb();

        void feed(float sample);
        float effect(float sample, vector<float> distances, vector<int> orders, float speedOfSound, float soundAbsorptionCoefficient, float wet);
};
