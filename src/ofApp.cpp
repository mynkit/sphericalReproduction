#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    string filename = "PHOTOSPHERE.jpg";
    img.load(filename);
    ofEnableNormalizedTexCoords();
    cam.setAutoDistance(false);
    cam.setPosition(0,0,0);
    cam.setDistance(1.0);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    img.bind();
    sphere.set(10, 32);
    sphere.draw();
    img.unbind();
    // 360度画像空間内にオブジェクトを描画
    box.set(1);
    box.setPosition(5, 0, -3);
    box.drawWireframe();
    sphere2.set(3, 16);
    sphere2.setPosition(-2, 1, -10);
    sphere2.drawWireframe();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
