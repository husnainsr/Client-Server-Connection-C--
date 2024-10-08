# 🧠 Federated Linear Regression with 9 Clients

Welcome to the **Federated Linear Regression** project! This project simulates 9 clients, each training their own local linear regression models using subsets of data and then sending their model parameters to a central server. The server aggregates the models to create a global model. 🚀

## 🎯 Project Overview

### 📂 Dataset
Each client trains its model on a unique subset of the dataset:
- `trainset_1.txt` to `trainset_9.txt` – Subsets for the 9 clients
- `testset_10.txt` – The test dataset used by the server to evaluate the global model

### ⚙️ Key Steps:
1. **Clients**: Each client reads a subset of the dataset, trains a linear regression model using gradient descent, and sends the model parameters (weights and bias) to the server.
2. **Server**: The server waits for the parameters from all 9 clients, performs a weighted averaging of the parameters to create a global model, and evaluates the performance on the test set using RMSE (Root Mean Squared Error).

## 🛠️ How to Run the Project

### 1️⃣ Setup

Make sure you have the necessary datasets:
- Create a `dataset` folder containing the following files:
  - `trainset_1.txt` to `trainset_9.txt` (training subsets)
  - `testset_10.txt` (test set)

### 2️⃣ Compile and Run Clients

1. **Compile the client program** (simulates all 9 clients):
    ```bash
    g++ -o federated_client federated_client.cpp -lws2_32
    ```
   
2. **Run the client program**:
    ```bash
    ./federated_client
    ```

This will simulate all 9 clients training their models and sending the parameters to the server.

### 3️⃣ Run the Server

1. **Compile the server program**:
    ```bash
    g++ -o server server.cpp -lws2_32
    ```
   
2. **Run the server**:
    ```bash
    ./server
    ```

The server will wait for connections from all 9 clients, aggregate the model parameters, and evaluate the global model using the test set.

## 🖥️ Code Structure

### `federated_client.cpp`
This file simulates the actions of 9 clients:
- Each client reads a different subset of the dataset (`trainset_1.txt` to `trainset_9.txt`)
- Each client trains a local linear regression model using gradient descent
- Each client sends its trained model parameters (weight and bias) to the server

### `server.cpp`
The server performs the following tasks:
- Waits for all 9 clients to send their model parameters
- Aggregates the parameters to form a global model
- Evaluates the global model on the test dataset (`testset_10.txt`)
- Prints the global model's RMSE

## 🚀 How the Federated Learning Works
Each client trains its model independently and sends the parameters to a central server. The server averages the model parameters from all clients to create a global model.

- **Local Training**: The clients use the following linear regression model:
  \[
  y_{\text{pred}} = w \times \text{SH} + b
  \]
  Where `w` is the weight (slope) and `b` is the bias (intercept).

- **Global Aggregation**: After receiving the parameters from all clients, the server performs a weighted average to generate the global model.

## 📊 Evaluation Metric: RMSE
The **Root Mean Squared Error (RMSE)** is used to evaluate the performance of the global model on the test dataset. It measures the differences between predicted values (`y_pred`) and actual values (`y_true`).

## 📂 Example Dataset Format

Each dataset file should have two columns:
1. **Study Hours (SH)**: A floating-point value representing the number of hours a student studied.
2. **Performance Index (PI)**: An integer value between 1 and 100 representing the student's performance score.

### Example Data (`trainset_X.txt`):


## 📝 To Do:
- [ ] Add additional error handling for socket communication.
- [ ] Explore further optimization of the gradient descent method.
- [ ] Visualize training progress and RMSE convergence with plots.

## 🤝 Contributing
Feel free to submit pull requests to improve the project. Fork, improve, and submit a PR to help make this project even better! Contributions are always welcome. 😄


## 🌟 Acknowledgements
Special thanks to all the tutorials and open-source projects that helped make this project possible!

---
Made with ❤️ by [Husnain Sattar]

