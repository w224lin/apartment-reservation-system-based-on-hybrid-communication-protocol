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

// handleUDP sending to serverM
void handleUDP(int udpSocket, const std::unordered_map<std::string, std::string>& information) {
    struct sockaddr_in serverMAddr;
    memset(&serverMAddr, 0, sizeof(serverMAddr));
    serverMAddr.sin_family = AF_INET;
    serverMAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // local address.
    serverMAddr.sin_port = htons(44078); // serverM port 44078.

    for (const auto& pair : information) {
        std::string message = pair.first + "," + pair.second; // message format "roomcode,capacity"
        // send message
        if (sendto(udpSocket, message.c_str(), message.length(), 0, (struct sockaddr*)&serverMAddr, sizeof(serverMAddr)) < 0) {
            std::cerr << "Error sending message: " << message << std::endl;
        }
        else {
            std::cout << "Message sent: " << message << std::endl;
        }
    }
    std::cout << "The Server <D> has sent the room status to the main server." << std::endl;
}

// Parsing comma-separated room code and capacity.
bool parseLine(const std::string& line, std::string& roomcode, std::string& capacity) {
    std::size_t commaPos = line.find(',');
    if (commaPos == std::string::npos) return false;

    roomcode = line.substr(0, commaPos);
    capacity = line.substr(commaPos + 1);
    return true;
}

int main() {

    // Create the hashMap to store the username and password.
    std::unordered_map<std::string, std::string> information;
    std::ifstream file("./Input_files/double.txt");

    // Read the file.
    if (!file.is_open()) {
        std::cerr << "Error opening file 'double.txt'." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string roomcode, capacity;;
        if (!parseLine(line, roomcode, capacity)) {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue; // Skip format wrong line.
        }

        information[roomcode] = capacity; // store the roomcode and capacity into information hashmap.
    }

    file.close();

    // Port Number.
    const int UDP_PORT = 42078;

    // Create UDP socket
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        std::cerr << "Error opening UDP socket." << std::endl;
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
        return 1;
    }

    std::cout << "The Server <D> is up and running using UDP on port <" << UDP_PORT << ">." << std::endl;

    handleUDP(udpSocket, information);
    close(udpSocket);

    return 0;
}
