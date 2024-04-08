#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>

#include <arpa/inet.h>

#include <thread>

std::unordered_map<std::string, std::string> credentials;// Create the hashMap to store the username and password.
std::unordered_map<std::string, std::string> singleRoom; // Store received single room information.


// Decryption function
std::string decrypt(const std::string& input) {
    std::string decrypted = input;
    for (char& c : decrypted) {
        if (c >= 'a' && c <= 'z') {
            c = (c - 'a' - 3 + 26) % 26 + 'a';
        }
        else if (c >= 'A' && c <= 'Z') {
            c = (c - 'A' - 3 + 26) % 26 + 'A';
        }
        else if (c >= '0' && c <= '9') {
            c = (c - '0' - 3 + 10) % 10 + '0';
        }
    }
    return decrypted;
}


void handleUDP(int udpSocket) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[1024]; // buffer size < 1024.

    while (true) {
        std::memset(buffer, 0, sizeof(buffer)); // clear buffer.
        ssize_t messageLen = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (messageLen < 0) {
            std::cerr << "Error receiving message." << std::endl;
            continue; // error detection.
        }

        // Converts the received message to a string
        std::string message(buffer, messageLen);

        // Parse message
        std::size_t commaPos = message.find(',');
        if (commaPos != std::string::npos) {
            std::string key = message.substr(0, commaPos);
            std::string value = message.substr(commaPos + 1);
            singleRoom[key] = value; // update reived data.
        }

        // print received message.
        std::cout << "Received: " << message << " and stored in singleRoom hashtable." << std::endl;
    }
}

void clientHandler(int clientSocket) {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 1) {
        std::cout << "Connection closed or error." << std::endl;
        close(clientSocket);
        return;
    }

    // Parse the received user name and password
    std::string loginInfo(buffer, bytesReceived);
    size_t commaPos = loginInfo.find(',');
    std::string username = loginInfo.substr(0, commaPos);
    std::string password = loginInfo.substr(commaPos + 1);

    // Decryption code
    password = decrypt(password);

    // Verify user name and password
    std::string message;
    if (credentials.find(username) != credentials.end() && credentials[username] == password) {
        message = "Welcome member/guest " + username + " ! Please enter the room layout code.";
    }
    else {
        message = "Failed login. Invalid username/password. ";
    }

    // The verification result is sent to the client
    send(clientSocket, message.c_str(), message.length(), 0);

    // Close connection.
    close(clientSocket);
}

void handleTCP(int tcpSocket) {
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(tcpSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        std::thread(clientHandler, clientSocket).detach();
    }
}

// Parsing comma-separated usrname and password.
bool parseLine(const std::string& line, std::string& username, std::string& password) {
    std::size_t commaPos = line.find(',');
    if (commaPos == std::string::npos) return false;

    username = line.substr(0, commaPos);
    password = line.substr(commaPos + 1);
    return true;
}

int main() {

    std::ifstream file("./Input_files/member.txt");

    // Read the file.
    if (!file.is_open()) {
        std::cerr << "Error opening file 'member.txt'." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string username, encryptedPassword;
        if (!parseLine(line, username, encryptedPassword)) {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue; // Skip format wrong line.
        }

        std::string decryptedPassword = decrypt(encryptedPassword); // decrpyt.
        credentials[username] = decryptedPassword; // store the credentials in hashmap.
    }

    file.close();

    // Port Number.
    const int UDP_PORT = 44078;
    const int TCP_PORT = 45078;

    // Create UDP socket
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        std::cerr << "Error opening UDP socket." << std::endl;
        return 1;
    }

    // Create TCP socket
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket < 0) {
        std::cerr << "Error opening TCP socket." << std::endl;
        close(udpSocket);
        return 1;
    }

    // bind UDP socket to UDP_PORT.
    struct sockaddr_in udpServerAddr;
    std::memset(&udpServerAddr, 0, sizeof(udpServerAddr));
    udpServerAddr.sin_family = AF_INET;
    udpServerAddr.sin_addr.s_addr = INADDR_ANY;
    udpServerAddr.sin_port = htons(UDP_PORT);

    if (bind(udpSocket, (struct sockaddr*)&udpServerAddr, sizeof(udpServerAddr)) < 0) {
        std::cerr << "Error binding UDP socket." << std::endl;
        close(udpSocket);
        close(tcpSocket);
        return 1;
    }

    // bind TCP socket to TCP_PORT.
    struct sockaddr_in tcpServerAddr;
    std::memset(&tcpServerAddr, 0, sizeof(tcpServerAddr));
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_addr.s_addr = INADDR_ANY;
    tcpServerAddr.sin_port = htons(TCP_PORT);

    if (bind(tcpSocket, (struct sockaddr*)&tcpServerAddr, sizeof(tcpServerAddr)) < 0) {
        std::cerr << "Error binding TCP socket." << std::endl;
        close(udpSocket);
        close(tcpSocket);
        return 1;
    }

    // TCP listen port.
    if (listen(tcpSocket, 5) < 0) {
        std::cerr << "Error listening on TCP socket." << std::endl;
        close(udpSocket);
        close(tcpSocket);
        return 1;
    }

    // Start the threads monitored by UDP
    std::thread udpThread(handleUDP, udpSocket);

    // Make the TCP thread run in the background
    std::thread tcpThread(handleTCP, tcpSocket);
    tcpThread.detach(); // Make the thread run in the background

    std::cout << "The main server is up and running. " << std::endl;

    return 0;
}
