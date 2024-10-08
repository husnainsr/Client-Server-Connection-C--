#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")  // Link with Winsock library

using namespace std;

struct DataPoint {
    float SH;
    float PI;
};

vector<DataPoint> readDatasetFile(const string &filePath) {
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

// Function to train the linear regression model
pair<float, float> trainLinearRegression(const vector<DataPoint> &data, float learning_rate, int epochs) {
    float w = 0, b = 0;
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float dw = 0, db = 0;
        for (const auto &record : data) {
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

// Function to send the parameters (weight and bias) to the server
void send_parameters_to_server(float w, float b, const string &server_ip, int server_port) {
    WSADATA wsaData;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed.\n";
        return;
    }

    // Create a socket for connecting to server
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // Set up the sockaddr_in structure with server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Connection failed: " << WSAGetLastError() << endl;
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    // Prepare the message (weight and bias)
    string message = to_string(w) + "," + to_string(b);

    // Send the message to server
    int send_result = send(client_socket, message.c_str(), message.size(), 0);
    if (send_result == SOCKET_ERROR) {
        cerr << "Send failed: " << WSAGetLastError() << endl;
    } else {
        cout << "Sent parameters to server: " << message << endl;
    }

    // Clean up and close the socket
    closesocket(client_socket);
    WSACleanup();
}

int main() {
    string datasetPath = "dataset"; 
    vector<DataPoint> data = readDatasetFile(datasetPath);
    auto [w, b] = trainLinearRegression(data, 0.01, 3000);

    cout << "Trained parameters (weight, bias): " << w << ", " << b << endl;

    // Send the trained parameters to the server
    string server_ip = "127.0.0.1";  // Server IP
    int server_port = 54000;         // Server port
    send_parameters_to_server(w, b, server_ip, server_port);

    return 0;
}
