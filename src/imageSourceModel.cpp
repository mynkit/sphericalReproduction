//
//  imageSourceModel.cpp
//  sphericalReproduction
//
//  Created by keita miyano on 2021/03/12.
//

#include "imageSourceModel.hpp"

imageSourceModel::imageSourceModel(string imageSourceModelPath) {
    if(csv.load(imageSourceModelPath)) {
        for(int i = 0; i < csv.getNumRows(); i++) {
            ofxCsvRow& row = csv.getRow(i);
            distances.push_back(row.getFloat(0));
            orders.push_back(row.getInt(1));
        }
    } else {
        exit(EXIT_FAILURE);
    }

}

void imageSourceModel::showDistances() {
    cout << "-- distances --" << endl;
    for (int i = 0; i < distances.size(); ++i) {
        cout << distances.at(i) << ", ";
    }
    cout << endl;
}

void imageSourceModel::showOrders() {
    cout << "-- orders --" << endl;
    for (int i = 0; i < orders.size(); ++i) {
        cout << orders.at(i) << ", ";
    }
    cout << endl;
}
