#include <iostream>
#include <vector>
#include <winsock2.h>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

struct DataPoint {
    float SH;
    float PI;
};

struct ModelParams {
    float weight;
    float bias;
};

// Function to receive model parameters (w, b) from clients
vector<ModelParams> receiveFromClients(int num_clients) {
    vector<ModelParams> clientParams;

    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET, ClientSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Winsock initialization failed" << endl;
        return clientParams;
    }

    // Create a listening socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return clientParams;
    }

    // Set up server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);  // Server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP (localhost)

    // Bind and listen
    bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(ListenSocket, SOMAXCONN);

    for (int i = 0; i < num_clients; ++i) {
        // Accept connection from a client
        ClientSocket = accept(ListenSocket, NULL, NULL);

        // Receive model parameters
        char recvBuf[512];
        recv(ClientSocket, recvBuf, sizeof(recvBuf), 0);

        // Parse the received data
        stringstream ss(recvBuf);
        ModelParams params;
        ss >> params.weight >> params.bias;
        clientParams.push_back(params);

        // Cleanup the client socket
        closesocket(ClientSocket);
    }

    // Cleanup listening socket
    closesocket(ListenSocket);
    WSACleanup();

    return clientParams;
}

// Function to calculate weighted average of the models received from clients
ModelParams aggregateModels(const vector<ModelParams>& clientParams) {
    float w_sum = 0, b_sum = 0;
    int num_clients = clientParams.size();

    for (const auto& params : clientParams) {
        w_sum += params.weight;
        b_sum += params.bias;
    }

    return {w_sum / num_clients, b_sum / num_clients};
}

// Function to read the test dataset
vector<DataPoint> readTestDataset(const string& filePath) {
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
        cerr << "Unable to open test dataset file" << endl;
    }
    return dataset;
}

// Function to calculate RMSE (Root Mean Squared Error)
float calculateRMSE(const vector<DataPoint>& testData, float w, float b) {
    float sum_error = 0;
    for (const auto& record : testData) {
        float y_pred = w * record.SH + b;
        sum_error += pow(record.PI - y_pred, 2);
    }
    return sqrt(sum_error / testData.size());
}

int main() {
    int num_clients = 9;  // Expecting parameters from 9 clients

    // Receive models from clients
    vector<ModelParams> clientModels = receiveFromClients(num_clients);

    // Aggregate models to form the global model
    ModelParams globalModel = aggregateModels(clientModels);
    cout << "Global Model - Weight (w): " << globalModel.weight << ", Bias (b): " << globalModel.bias << endl;

    // Read test dataset
    string testFile = "dataset/testset_10.txt";
    vector<DataPoint> testData = readTestDataset(testFile);

    // Evaluate the global model on the test dataset
    float rmse = calculateRMSE(testData, globalModel.weight, globalModel.bias);
    cout << "Global Model RMSE on Test Dataset: " << rmse << endl;

    return 0;
}
