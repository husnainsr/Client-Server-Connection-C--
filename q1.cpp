#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

// Struct to store the data for each line (pair of float values)
struct DataPoint {
    float SH;
    float PI;
};

// Function to read the content of each text file and store data in a vector
vector<DataPoint> readDatasetFiles(const string& folderPath) {
    string filePrefix = "trainset_";
    string fileSuffix = ".txt";
    int totalFiles = 9;
    vector<DataPoint> dataset;  
    for (int i = 1; i <= totalFiles; ++i) {
        string filePath = folderPath + "/" + filePrefix + to_string(i) + fileSuffix;
        ifstream file(filePath);  
        if (file.is_open()) {
            cout << "Reading file: " << filePath << endl;
            string line;
            bool isFirstLine = true;
            while (getline(file, line)) {
                if (isFirstLine) {
                    isFirstLine = false;  
                    continue;
                }
                stringstream ss(line);
                DataPoint point;
                ss >> point.SH >> point.PI;
                dataset.push_back(point);
            }
            file.close();  
        } else {
            cerr << "Unable to open file: " << filePath << endl;
        }
    }
    
    return dataset;  
}

vector<DataPoint> singleDataFile(const string& folderPath,int number) {
    string filePrefix = "testset_";
    string fileSuffix = ".txt";
    int num = number;
    vector<DataPoint> dataset;  
    string filePath = folderPath + "/" + filePrefix + to_string(num) + fileSuffix;
    ifstream file(filePath);  
    if (file.is_open()) {
        cout << "Reading file: " << filePath << endl;
        string line;
        bool isFirstLine = true;
        while (getline(file, line)) {
            if (isFirstLine) {
                isFirstLine = false;  
                continue;
            }
            stringstream ss(line);
            DataPoint point;
            ss >> point.SH >> point.PI;
            dataset.push_back(point);
        }
        file.close();  
    } else {
        cerr << "Unable to open file: " << filePath << endl;
    }
    
    return dataset;  
}

// Function to train the linear regression model using gradient descent
pair<float, float> train_linear_regression(const vector<DataPoint> &data, float learning_rate, int epochs) {
    float w = 0, b = 0;  // Initialize weight and bias
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float dw = 0, db = 0;
        for (const auto &record : data) {
            float x = record.SH;
            float y = record.PI;
            float y_pred = w * x + b;  // Predicted value
            dw += -2 * x * (y - y_pred);
            db += -2 * (y - y_pred);
        }
        w -= learning_rate * dw / data.size();
        b -= learning_rate * db / data.size();
    }
    return {w, b};
}


// Function to calculate RMSE (Root Mean Squared Error)
float calculate_rmse(const vector<DataPoint> &data, float w, float b) {
    float sum_error = 0;
    for (const auto &record : data) {
        float y_pred = w * record.SH + b;
        sum_error += pow(record.PI - y_pred, 2);
    }
    return sqrt(sum_error / data.size());
}

int main() {
    string folderPath = "dataset";  
    vector<DataPoint> trainingData= readDatasetFiles(folderPath);  
    vector<DataPoint> testData= singleDataFile(folderPath,10);
    auto [w,b]= train_linear_regression(trainingData,0.01,3000);
    cout<<"w = " << w << ", b = " << b << endl;
    
    float rmse = calculate_rmse(testData, w, b);
    cout << "RMSE: " << rmse << endl;
    

    return 0;
}
