#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Function to encrypt the password
std::string encryptPassword(const std::string& password) {
    std::string encrypted;
    for (char c : password) {
        if (isalpha(c)) { // Check if it's an alphabetic character
            char base = isupper(c) ? 'A' : 'a';
            c = (c - base + 3) % 26 + base; // Encrypt alphabetic character
        }
        else if (isdigit(c)) { // Check if it's a digit
            c = (c - '0' + 3) % 10 + '0'; // Encrypt digit
        }
        // Special characters are not changed.
        encrypted += c;
    }
    return encrypted;
}

// Function to establish TCP connection with the serverM
int connectToServer(const std::string& serverIP, int serverPort) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        return -1;
    }

    return sock;
}

// Function to send login information to the server
bool login(int sock, const std::string& username, const std::string& password) {
    std::string loginInfo = username + "," + password;
    if (send(sock, loginInfo.c_str(), loginInfo.size(), 0) < 0) {
        std::cerr << "Failed to send login information." << std::endl;
        return false;
    }

    char response[1024];
    if (recv(sock, response, sizeof(response), 0) < 0) {
        std::cerr << "Failed to receive response from server." << std::endl;
        return false;
    }

    std::cout << "Server response: " << response << std::endl;
    return true;
}

int main() {
    const std::string serverIP = "127.0.0.1"; // Server IP address.
    const int serverPort = 45078; // Server port: 45078.

    // Connect to the server
    int sock = connectToServer(serverIP, serverPort);
    if (sock < 0) {
        return 1;
    }

    std::string username, password;
    std::cout << "Please enter the username: ";
    getline(std::cin, username);
    std::cout << "Please enter the password (¡°Enter¡± to skip for guests): ";
    getline(std::cin, password);

    // Encrypt username and password
    std::string encryptedUsername = encrypt(username);
    std::string encryptedPassword = encrypt(password);

    if (password.empty()) {
        // Login as guest if password is not provided
        password = "";
    }

    // Send login information to the server
    if (!login(sock, encryptedUsername, encryptedPassword)) {
        close(sock);
        return 1;
    }

    // Add further communication with server as needed

    close(sock);
    return 0;
}
