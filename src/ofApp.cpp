#include "ofApp.h"


#define BUFFERSIZE 1024; // バッファサイズ(256推奨．大きくすると処理に余裕はでるが遅延が長くなる)
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
    cam.setAutoDistance(false);
    cam.setPosition(0,0,0);
    cam.setDistance(1.0);
    
    viewOpacity = 0.;
    daytimeViewOpacity = 1.;
    gotoNight = false;
    displayView = false;
    
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
    string filepath = ofToDataPath("owarinokisetsuguitar.wav");
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
    // reverb setting
    addOnReverb.setroomsize(0.9f); // 大きくしていくと反響時間が長くなる
    addOnReverb.setdamp(0.5f); // 大きくしていくと高周波が消えて低周波が増える
    addOnReverb.setwidth(0.8f); //0~1。よくわかんないけど左右にいれるリバーブの感じが変わるらしい。0.5だと多分変わらない
    addOnReverb.setwet(0.2f); // リバーブ部分の割合
    addOnReverb.setdry(1.0f - 0.2f); // 原音部分の割合
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
    } else {
        if (viewOpacity > 0.) {
            viewOpacity -= 0.01;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    // 背景色を白にする
    ofBackground(150 + (255 - 150) * viewOpacity);
    // 昼
    ofSetColor(255, daytimeViewOpacity * 255 * viewOpacity);
    daytimeView.bind();
    sphere.set(10, 32);
    sphere.draw();
    daytimeView.unbind();
    // 夜
    ofSetColor(255, (1. - daytimeViewOpacity) * 255 * viewOpacity);
    nightView.bind();
    sphere.set(10, 32);
    sphere.draw();
    nightView.unbind();
    // 360度画像空間内にオブジェクトを描画
    ofSetColor(255, 255);
    boxObject.set(1);
    boxObject.setPosition(5, -0.5, -7);
    boxObject.drawWireframe();
    sphereObject.set(3, 16);
    sphereObject.setPosition(-2, 3, -10);
    sphereObject.drawWireframe();
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
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &buffer){
    const int frames = buffer.getNumFrames();
    float curVol = 0.0;
    for(int i = 0; i < frames; i++){
        const int channels = buffer.getNumChannels();
        float currentSample = inputBuffer[i];
        float currentSampleL = currentSample;
        float currentSampleR = currentSample;
        
        addOnReverb.setroomsize(0.9 * viewOpacity);
        addOnReverb.processreplace(&currentSampleL, &currentSampleR, &currentSampleL, &currentSampleR, 1, 1);
        currentSample = (currentSampleL + currentSampleR) / 2.;
        
        buffer[i*channels+0] = currentSampleL;
        buffer[i*channels+1] = currentSampleR;
        curVol += currentSampleL * currentSampleR;
    }
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
    case 'r':
        if (displayView) {
            displayView = false;
        } else {
            displayView = true;
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
//    myWavWriter->wave_write("recording.wav");
}
