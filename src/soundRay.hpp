//
//  soundRay.hpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/14.
//

#pragma once

#include "ofMain.h"

class soundRay {
    public:
        float x;
        float y;
        float z;
        float vx;
        float vy;
        float vz;
        int fps;
        float speedOfSound;
        vector<vector<float>> coordinatesBuffer;
        int bufferSize;
        int bufferIndex;
        vector<vector<float>> corners;
        float height;
        soundRay(float _x, float _y, float _z, int _fps, float _speedOfSound, float theta, float phi, vector<vector<float>> _corners, float _height);
        ~soundRay();
        void update();
        bool pointInCorners(float x, float y);
        void calcVerocityAfterCollision(float& vx, float& vy, int reboundLineIndex);
    
    private:
        int getReboundLineIndex(float x, float y, float old_x, float old_y);
        bool twoLineSegmentsIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        float calcTan(float x, float y, int k);
};
