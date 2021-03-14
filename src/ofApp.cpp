#include "ofApp.h"


#define BUFFERSIZE 2048; // バッファサイズ(256推奨．大きくすると処理に余裕はでるが遅延が長くなる)
#define SAMPLERATE 44100; // サンプルレート(Hz)
#define MAXHOLDTIME 10000; // 音をメモリに保持する最大秒数
#define FPS 60; // フレームレート
#define SPEEDOFSOUND 340.; // 音速(m/s)
#define BPM 160; // テンポ


//--------------------------------------------------------------
void ofApp::setup(){
    daytimeView.load("MARUNOUCHI_DAYTIMEVIEW.PHOTOSPHERE.jpg");
    nightView.load("MARUNOUCHI_NIGHTVIEW.PHOTOSPHERE.jpg");
    ofEnableNormalizedTexCoords();
    ofEnableAlphaBlending(); // 透明化を有効にする
//    ofEnableSmoothing();
    cam.setAutoDistance(false);
    cam.setPosition(0,0,0);
    cam.setDistance(1.0);
    
    viewOpacity = 0.;
    daytimeViewOpacity = 1.;
    gotoNight = false;
    displayView = false;
    displayRoom = false;
    
    // audio setting
    fps = FPS;
    bpm = BPM;
    speedOfSound = (float)SPEEDOFSOUND;
    bufferSize = BUFFERSIZE;
    sampleRate = SAMPLERATE;
    inputBuffer.resize(bufferSize);
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.setInListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels=2;
    settings.numInputChannels=1;
    settings.bufferSize = bufferSize;
    sound_stream.setup(settings);
    // audiofile setting
    string filepath = ofToDataPath("prestobind.wav");
//    string filepath = ofToDataPath("owarinokisetsuguitar.wav");
    if( ofFile::doesFileExist( filepath ) ){
        audiofile.load( filepath );
        if (!audiofile.loaded()){
            ofLogError()<<"error loading file, double check the file path";
        }
    }else{
        ofLogError()<<"input file does not exists";
    }
    playhead = 0;
    step = audiofile.samplerate() / sampleRate;
    // peaking Filter
    myPeakingFilterL = new peakingFilter(sampleRate, 3200, 0.7, 0.);
    myPeakingFilterR = new peakingFilter(sampleRate, 3200, 0.7, 0.);
    // reverb setting
    schroederReverbWet = 0.;
    schroederReverb.setroomsize(0.9f); // 大きくしていくと反響時間が長くなる
    schroederReverb.setdamp(0.5f); // 大きくしていくと高周波が消えて低周波が増える
    schroederReverb.setwidth(0.8f); //0~1。よくわかんないけど左右にいれるリバーブの感じが変わるらしい。0.5だと多分変わらない
    schroederReverb.setwet(schroederReverbWet); // リバーブ部分の割合
    schroederReverb.setdry(0.); // 原音部分の割合(原音は出力時に足すのでオフにしておく)
    // recording
    myWavWriter = new wavWriter(sampleRate, 16, 2);
    myWavWriter->setRecordingOn();
    // delay
    myShortDelay = new delay(100, sampleRate, 80, 0.3);
    // room setting
    myImageSourceModel = new imageSourceModel("image_source_model.csv");
    myRoom = new Room("room_setting.json", fps, speedOfSound);
    // room reverb setting
    myRoomReverb = new roomReverb(10000, sampleRate);
    roomReverbWet = 0.;
    // 深度テストの有効化
    ofEnableDepthTest();
}

//--------------------------------------------------------------
void ofApp::update(){
    if (gotoNight) {
        // 昼から夜に
        if (daytimeViewOpacity > 0.) {
            daytimeViewOpacity -= 0.01;
        }
    } else {
        // 夜から昼に
        if (daytimeViewOpacity < 1.) {
            daytimeViewOpacity += 0.01;
        }
    }
    if (displayView) {
        // 景色を表示する
        if (viewOpacity < 1.) {
            viewOpacity += 0.01;
        }
        // schroederReverbをかける
        if (schroederReverbWet < 0.2) {
            schroederReverbWet += 0.002;
        }
        // 部屋を非表示にする
        displayRoom = false;
    } else {
        // 景色を非表示にする
        if (viewOpacity > 0.) {
            viewOpacity -= 0.01;
        }
        // schroederReverbをとめる
        if (schroederReverbWet > 0.) {
            schroederReverbWet -= 0.002;
        }
    }
    if (displayRoom) {
        myRoom->updateSoundRay();
        // 部屋を表示する
        if (roomReverbWet < 1.) {
            roomReverbWet += 0.01;
        }
    } else {
        // 部屋を非表示にする
        if (roomReverbWet > 0.) {
            roomReverbWet -= 0.01;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    // 背景色を白にする
    ofBackground(189 + (255 - 189) * viewOpacity, 205 + (255 - 205) * viewOpacity, 238 + (255 - 238) * viewOpacity);
    // 昼
    ofSetColor(255, daytimeViewOpacity * 255 * viewOpacity);
    daytimeView.bind();
    sphere.set(150, 32);
    sphere.draw();
    daytimeView.unbind();
    // 夜
    ofSetColor(255, (1. - daytimeViewOpacity) * 255 * viewOpacity);
    nightView.bind();
    sphere.set(100, 32);
    sphere.draw();
    nightView.unbind();
    // Roomの描画
    myRoom->drawRoom(roomReverbWet, displayView);
    cam.end();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer &buffer){
    const int frames = buffer.getNumFrames();
    float curVol = 0.0;
//    for(int i = 0; i < frames; i++){
//        inputBuffer[i] = buffer[i];
//        curVol += inputBuffer[i] * inputBuffer[i];
//    }
    for (int i = 0; i < frames; i++){
        int n = playhead;
        if( n < audiofile.length()-1 ){
            for( size_t k=0; k<buffer.getNumChannels(); ++k){
                float fract = playhead - (double) n;
                float isample = 0.;
                if (audiofile.data()) {
                    float s0 = audiofile.sample( n, k );
                    float s1 = audiofile.sample( n+1, k );
                    isample = s0*(1.0-fract) + s1*fract; // linear interpolation
                }
                inputBuffer[i] = isample;
                curVol += inputBuffer[i] * inputBuffer[i];
            }
            playhead += step;
        }else{
            inputBuffer[i] = 0.;
            playhead = 0; // ループ
        }
    }
    myRoom->setInputVolume(sqrt(curVol) * 1.);
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &buffer){
    const int frames = buffer.getNumFrames();
    const int channels = buffer.getNumChannels();
    float curVol = 0.0;
    schroederReverb.setroomsize(0.8);
    schroederReverb.setwet(schroederReverbWet);
    schroederReverb.setdamp(daytimeViewOpacity);
    float peakingFilterGain = 2. * viewOpacity * (1. - daytimeViewOpacity);
    for(int i = 0; i < frames; i++){
        float currentSample = inputBuffer[i];
        if (roomReverbWet>0.01 && myRoomReverb) {
            currentSample = myRoomReverb->effect(currentSample, myImageSourceModel->distances, myImageSourceModel->orders, speedOfSound, 0.3, roomReverbWet);
        }
        float currentSampleL = currentSample;
        float currentSampleR = currentSample;
        schroederReverb.processreplace(&currentSampleL, &currentSampleR, &currentSampleL, &currentSampleR, 1, 1);
        if (myPeakingFilterR && myPeakingFilterL) {
            myPeakingFilterL->setGain(peakingFilterGain);
            myPeakingFilterR->setGain(peakingFilterGain);
            currentSampleL = myPeakingFilterL->effect(currentSampleL) * ( 1. - 0.2 * (1. - daytimeViewOpacity)*viewOpacity );
            currentSampleR = myPeakingFilterR->effect(currentSampleR) * ( 1. - 0.2 * (1. - daytimeViewOpacity)*viewOpacity );
        } else {
            //cout << "warning: peakingFilter does not work." << endl;
        }
        currentSampleL += (1. - schroederReverbWet) * currentSample;
        currentSampleR += (1. - schroederReverbWet) * currentSample;
        currentSample = (currentSampleL + currentSampleR) / 2.;
        clipping(currentSampleL, currentSampleR);
        buffer[i*channels+0] = currentSampleL;
        buffer[i*channels+1] = currentSampleR;
        curVol += currentSampleL * currentSampleR;
        myWavWriter->recording(currentSampleL); // L
        myWavWriter->recording(currentSampleR); // R
    }
    myRoom->setOutputVolume(sqrt(curVol) * 1.);
}

//--------------------------------------------------------------
void ofApp::clipping(float &sampleL, float &sampleR){
    if (sampleL > 32767) {sampleL = 32767;}
    if (sampleL < -32767) {sampleL = -32767;}
    if (sampleR > 32767) {sampleR = 32767;}
    if (sampleR < -32767) {sampleR = -32767;}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
    case 'n':
        if (gotoNight) {
            gotoNight = false;
        } else {
            gotoNight = true;
        }
        break;
    case 's':
        if (displayView) {
            displayView = false;
        } else {
            displayView = true;
        }
        break;
    case 'r':
        if (displayRoom) {
            displayRoom = false;
        } else {
            displayRoom = true;
        }
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
    myWavWriter->wave_write("recording.wav");
}
