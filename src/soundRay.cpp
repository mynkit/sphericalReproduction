//
//  soundRay.cpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/14.
//

#include "soundRay.hpp"

soundRay::soundRay(float _x, float _y, float _z, int _fps, float _speedOfSound, float theta, float phi, vector<vector<float>> _corners, float _height) {
    // 初期位置と初期速度
    x = _x;
    y = _y;
    z = _z;
    fps = _fps;
    speedOfSound = _speedOfSound * 0.7; //あまりにも早すぎると全然壁に跳ね返った感じしないので少し恣意的に遅くしとく
    vx = speedOfSound * cos(theta) * cos(phi);
    vy = speedOfSound * cos(theta) * sin(phi);
    vz = speedOfSound * sin(theta);
    // どんなRoom内での音線か
    corners = _corners;
    height = _height;
    bufferIndex = 0;
    bufferSize = 20;
    coordinatesBuffer.resize(bufferSize);
    for (int i = 0; i < bufferSize; i++) {
        coordinatesBuffer[i].resize(3);
        coordinatesBuffer[i][0] = x;
        coordinatesBuffer[i][1] = y;
        coordinatesBuffer[i][2] = z;
    }
}

void soundRay::update() {
    float x_o = x; // 古い座標を保持しておく
    float y_o = y;
    float x_n = x + vx / (float)fps; // 古い座標を保持しておく
    float y_n = y + vy / (float)fps;
    float z_n = z + vz / (float)fps;
    if ((z_n > height) || (z_n < 0)) {
        vz *= -1;
    }
    if (!pointInCorners(x_n, y_n)) {
        // x, yがcornersに含まれなくなったとき
        // まずぶつかる線分のindexを取得
        int reboundLineIndex = getReboundLineIndex(x_o, y_o, x_n, y_n);
        if (reboundLineIndex == -1) {
            // この場合はcornerの頂点に当たってしまっているので反対方向に向かわせる
            vx *= -1.;
            vy *= -1.;
        } else {
            calcVerocityAfterCollision(vx, vy, reboundLineIndex);
        }

    }
    x += vx / (float)fps;
    y += vy / (float)fps;
    z += vz / (float)fps;
    
    coordinatesBuffer[bufferIndex][0] = x;
    coordinatesBuffer[bufferIndex][1] = y;
    coordinatesBuffer[bufferIndex][2] = z;
    bufferIndex++;
    if(bufferIndex == bufferSize){bufferIndex = 0;}
}

// 点x, yがcorners内に含まれているのか判定する
// 参考: http://sampleyy.hatenablog.com/entry/2015/03/28/110152
bool soundRay::pointInCorners(float x, float y) {
    int k;
    float tmp = 0.;
    float angle = 0.;
    for(k = 0; k < corners.size(); k++) {
        tmp = calcTan(x, y, k);
        angle += tmp;
    }
    if (fabs(2.*M_PI-fabs(angle)) < 0.01) {
        return true;
    } else {
        return false;
    }
}

// 衝突後の速度vx, vyを上書きする
void soundRay::calcVerocityAfterCollision(float& vx, float& vy, int reboundLineIndex) {
    int next = reboundLineIndex + 1;
    if(next == corners.size()){next = 0;}
    float ax = corners[reboundLineIndex][0];
    float ay = corners[reboundLineIndex][1];
    float bx = corners[next][0];
    float by = corners[next][1];
    float ay_by = ay - by;
    float bx_ax = bx - ax;
    float newvx = vx - 2. * (vx * ay_by + vy * bx_ax) * ay_by / ( ay_by*ay_by + bx_ax*bx_ax );
    float newvy = vy - 2. * (vx * ay_by + vy * bx_ax) * bx_ax / ( ay_by*ay_by + bx_ax*bx_ax );
    vx = newvx;
    vy = newvy;
}

// 音線が壁にぶつかる際、どの線分にあたったのかindexを取得する
// x, y: 壁を通り抜けた場合の座標
// old_x, old_y: 壁にぶつかる直前の座標
int soundRay::getReboundLineIndex(float x, float y, float old_x, float old_y) {
    int target_index = -1;
    for (int k = 0; k < corners.size(); k++) {
        int next = k + 1;
        if(next == corners.size()){next = 0;}
        bool judge = twoLineSegmentsIntersect(old_x, old_y, x, y, corners[k][0], corners[k][1], corners[next][0], corners[next][1]);
        if (judge) {
            target_index = k;
            break;
        }
    }
    return target_index;
}

// 2つの線分AB, CDが交差するか判定する
// A(x1, y1), B(x2, y2), C(x3, y3), D(x4, y4)
// 参考: https://qiita.com/zu_rin/items/e04fdec4e3dec6072104
bool soundRay::twoLineSegmentsIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    float s, t;
    s = (x1 - x2) * (y3 - y1) - (y1 - y2) * (x3 - x1);
    t = (x1 - x2) * (y4 - y1) - (y1 - y2) * (x4 - x1);
    if (s * t > 0){
        return false;
    }
    s = (x3 - x4) * (y1 - y3) - (y3 - y4) * (x1 - x3);
    t = (x3 - x4) * (y2 - y3) - (y3 - y4) * (x2 - x3);
    if (s * t > 0){
        return false;
    }
    return true;
}

// 多角形の辺と点との向付き角度を計算する
// 参考: http://sampleyy.hatenablog.com/entry/2015/03/28/110152
float soundRay::calcTan(float x, float y, int k) {
    float Ax, Ay, Bx, By;
    float AxB, AvB;
    float angle;
    int next = k + 1;
    if(next == corners.size()){next = 0;}
    Ax = corners[k][0] - x;
    Ay = corners[k][1] - y;
    Bx = corners[next][0] - x;
    By = corners[next][1] - y;
    AvB = Ax * Bx + Ay * By;
    AxB = Ax * By - Ay * Bx;
    angle = atan2(AxB, AvB);
    return angle;
}
