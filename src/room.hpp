//
//  room.hpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/12.
//

#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "soundRay.hpp"

class Room {
    public:
        vector<vector<float>> corners;
        soundRay* mySoundRay;
        float height;
        float microphone[3];
        float source[3];
        int maxOrder;
        float inputVolume;
        float outputVolume;
        Room(string roomSettingPath, int fps, float speedOfSound);
        ~Room();
        void drawRoom(float opacity, float displayView);
        void setInputVolume(float volume);
        void setOutputVolume(float volume);
        void updateSoundRay();
        int zdis;
        int ydis;
    private:
        ofxJSONElement json;
        ofSpherePrimitive microphoneSphere;
        ofBoxPrimitive sourceBox;
        ofEasyCam cam;
        ofPolyline polyline;
        ofMesh mesh;
        float microphoneWidth;
        float sourceWidth;
        float xwidth;
        float ywidth;
        float zwidth;
        float xywidth; // xy方向(ななめ)から見たときの幅
        float xavg;
        float yavg;
        float zavg;
        float xyavg; // xy方向(ななめ)から見たときの原点からの距離
        // 照明
        ofLight light;
};

