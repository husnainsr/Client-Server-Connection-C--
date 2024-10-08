#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <winsock2.h>  // Windows socket API

using namespace std;

// Struct to hold data points (Study Hours, Performance Index)
struct DataPoint {
    float SH;
    float PI;
};

// Function to read a single dataset file (client's local dataset)
vector<DataPoint> readDatasetFile(const string& filePath) {
    vector<DataPoint> dataset;
    ifstream file(filePath);
    if (file.is_open()) {
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
pair<float, float> trainLocalModel(const vector<DataPoint>& data, float learning_rate, int epochs) {
    float w = 0, b = 0;
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float dw = 0, db = 0;
        for (const auto& record : data) {
            float x = record.SH;
            float y = record.PI;
            float y_pred = w * x + b;
            dw += -2 * x * (y - y_pred);
            db += -2 * (y - y_pred);
        }
        w -= learning_rate * dw / data.size();
        b -= learning_rate * db / data.size();
    }
    return {w, b};
}

// Function to send model parameters (w, b) to the server
void sendModelToServer(float w, float b) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Winsock initialization failed" << endl;
        return;
    }

    // Create a socket to connect to the server
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return;
    }

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);  // Server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP (localhost)

    // Connect to the server
    if (connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Unable to connect to the server" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // Send the model parameters (w, b)
    char sendBuf[512];
    sprintf(sendBuf, "%f %f", w, b);
    send(ConnectSocket, sendBuf, sizeof(sendBuf), 0);

    // Cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}

int main() {
    string dataFile = "dataset/trainset_";  

    for (int i = 0; i < 9; i++)
    {
        vector<DataPoint> localData = readDatasetFile(dataFile+to_string(i+1)+".txt");
        auto [w, b] = trainLocalModel(localData, 0.01, 3000);
        cout << i+1<< " Trained model - Weight (w): " << w << ", Bias (b): " << b << endl;
        sendModelToServer(w, b);

    }

    
    return 0;
}
