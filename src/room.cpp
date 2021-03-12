//
//  room.cpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/12.
//

#include "room.hpp"

Room::Room(string roomSettingPath, int fps, float speedOfSound) {
    vector<float> xs;
    vector<float> ys;
    vector<float> xys;
    inputVolume = 0.;
    outputVolume = 0.;
    float xmin, ymin, xmax, ymax;
    if (json.open(roomSettingPath)) {
        height = json["height"].asInt();
        corners.resize(json["corners"].size());
        for(int i = 0; i < json["corners"].size(); i++) {
            corners[i].resize(2);
            corners[i][0] = json["corners"][i][0].asFloat();
            corners[i][1] = json["corners"][i][1].asFloat();
            xs.push_back(corners[i][0]);
            ys.push_back(corners[i][1]);
            xys.push_back(corners[i][0] + corners[i][1]);
        }
        for(int i = 0; i < 3; i++) {
            microphone[i] = json["microphone"][i].asFloat();
        }
        for(int i = 0; i < 3; i++) {
            source[i] = json["sound_source"][i].asFloat();
        }
        maxOrder = json["max_order"].asInt();
        // x, y, z軸方向の幅, 平均値を出す。これによってカメラの位置や、点(マイクや音源など)の大きさを決める
        xmin = *min_element(xs.begin(), xs.end());
        xmax = *max_element(xs.begin(), xs.end());
        ymin = *min_element(ys.begin(), ys.end());
        ymax = *max_element(ys.begin(), ys.end());
        xwidth = xmax - xmin;
        ywidth = ymax - ymin;
        zwidth = height;
        xywidth = ( *max_element(xys.begin(), xys.end()) - *min_element(xys.begin(), xys.end()) ) / sqrt(2.);
        xavg = accumulate(xs.begin(), xs.end(), 0.) / xs.size();
        yavg = accumulate(ys.begin(), ys.end(), 0.) / ys.size();
        zavg = height / 2.;
        xyavg = (xavg - yavg) / sqrt(2.);
        float widthmin = min({xwidth, ywidth, zwidth});
        microphoneWidth = widthmin / 5;
        sourceWidth = widthmin / 7;
    } else {
        exit(EXIT_FAILURE);
    }
    // y軸の下方向に図形をずらす
    ydis = -2;
    // z軸の奥方向に図形をずらす
    zdis = 20;
    // 立体描画のsetup
    microphoneSphere.set(microphoneWidth * 0.9, 3);
    microphoneSphere.setPosition(microphone[0], microphone[2] - ydis, microphone[1] - zdis);
    sourceBox.set(sourceWidth * 0.9);
    sourceBox.setPosition(source[0], source[2] - ydis, source[1] - zdis);
    // 部屋の頂点をmeshに設定
    for(int i = 0; i < corners.size(); i++) {
        int next = i + 1;
        if(next == corners.size()){next = 0;}
        mesh.addVertex(ofVec3f(corners[i][0], height - ydis, corners[i][1] - zdis));
        mesh.addVertex(ofVec3f(corners[next][0], height - ydis, corners[next][1] - zdis));
        mesh.addVertex(ofVec3f(corners[i][0], -ydis, corners[i][1] - zdis));
        mesh.addVertex(ofVec3f(corners[next][0], -ydis, corners[next][1] - zdis));
        mesh.addVertex(ofVec3f(corners[i][0], -ydis, corners[i][1] - zdis));
        mesh.addVertex(ofVec3f(corners[i][0], height - ydis, corners[i][1] - zdis));
        
        mesh.addVertex(ofVec3f(corners[i][0], height - ydis, corners[i][1] - zdis));
        mesh.addVertex(ofVec3f(corners[next][0], height - ydis, corners[next][1] - zdis));
        mesh.addVertex(ofVec3f(corners[i][0], -ydis, corners[i][1] - zdis));
        mesh.addVertex(ofVec3f(corners[next][0], -ydis, corners[next][1] - zdis));
        mesh.addVertex(ofVec3f(corners[i][0], -ydis, corners[i][1] - zdis));
        mesh.addVertex(ofVec3f(corners[i][0], height - ydis, corners[i][1] - zdis));
    }
}

void Room::drawRoom(float opacity) {
    // 部屋の枠を描画
    ofSetColor(255, 255, 255, 100 * opacity);
//        ofSetLineWidth(2);
    mesh.drawFaces();
    // マイク, 音源
    ofSetColor(255, 255, 255, 255 * opacity);
    microphoneSphere.drawWireframe();
    ofSetColor(255,255, 255, 255 * opacity);
    sourceBox.drawWireframe();
    // マイク, 音源の中の色
    int micAlpha = outputVolume * 10;
    ofSetColor(250, 125, 127, micAlpha * opacity); // 強い赤色
    microphoneSphere.draw();
    int sourceAlpha = inputVolume * 10;
    ofSetColor(250, 125, 127, sourceAlpha * opacity); // 強い赤色
    sourceBox.draw();
}

void Room::setInputVolume(float volume) {
    if (this) {
        inputVolume = volume;
    }
}

void Room::setOutputVolume(float volume) {
    if (this) {
        outputVolume = volume;
    }
}
