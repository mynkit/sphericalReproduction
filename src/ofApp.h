#pragma once

#include "ofMain.h"
#include "ofxReverb.h"
#include "ofxAudioFile.h"
#include "wavWriter.hpp"
#include "delay.hpp"
#include "peakingFilter.hpp"
#include "imageSourceModel.hpp"
#include "room.hpp"
#include "roomReverb.hpp"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    
        // audio setting
        void audioIn(ofSoundBuffer &buffer);
        void audioOut(ofSoundBuffer &buffer);
        ofSoundStream sound_stream;
        int fps;
        int bpm;
        float speedOfSound;
        int bufferSize;
        int sampleRate;
        vector<float> inputBuffer;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofImage daytimeView;
        ofImage nightView;
        float daytimeViewOpacity;
        float viewOpacity;
        bool gotoNight;
        bool displayView;
        bool displayRoom;
        ofEasyCam cam;
        ofSpherePrimitive sphere;
        ofBoxPrimitive boxObject;
        ofSpherePrimitive sphereObject;
    
        // reverb
        ofxReverb schroederReverb;
        roomReverb* myRoomReverb;
        // Play Wavfile
        ofxAudioFile audiofile;
        float playhead;
        float step;
        float schroederReverbWet;
        float roomReverbWet;
        // Recording
        wavWriter* myWavWriter;
        // delay
        delay* myShortDelay;
        // peaking Filter
        peakingFilter* myPeakingFilterL;
        peakingFilter* myPeakingFilterR;
        // room setting
        imageSourceModel* myImageSourceModel;
        Room* myRoom;
    private:
        ofxJSONElement json;
};
