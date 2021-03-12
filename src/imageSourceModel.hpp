//
//  imageSourceModel.hpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/12.
//

#pragma once

#include "ofxCsv.h"

class imageSourceModel {
    public:
        imageSourceModel(string imageSourceModelPath);
        ~imageSourceModel();
        void showDistances();
        void showOrders();
        ofxCsv csv;
        vector <float> distances;
        vector <int> orders;
};
