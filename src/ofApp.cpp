#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    daytimeView.load("MARUNOUCHI_DAYTIMEVIEW.PHOTOSPHERE.jpg");
    nightView.load("MARUNOUCHI_NIGHTVIEW.PHOTOSPHERE.jpg");
    ofEnableNormalizedTexCoords();
    cam.setAutoDistance(false);
    cam.setPosition(0,0,0);
    cam.setDistance(1.0);
    
    // 背景色を白にする
    ofBackground(255);
    daytimeViewOpacity = 1.;
    gotoNight = false;
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
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    // 昼
    ofSetColor(255, daytimeViewOpacity * 255);
    daytimeView.bind();
    sphere.set(10, 32);
    sphere.draw();
    daytimeView.unbind();
    // 夜
    ofSetColor(255, (1. - daytimeViewOpacity) * 255);
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
void ofApp::keyPressed(int key){
    switch (key) {
    case 'n':
        if (gotoNight) {
            gotoNight = false;
        } else {
            gotoNight = true;
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
