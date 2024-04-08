# Apartment-reservation-system-based-on-hybrid-communication-protocol
In Linux environment, responsible for the design and implementation of a C++ based apartment booking system. The system architecture adopts a modular design, including client, server, and database server, which is committed to providing a safe and efficient room query and reservation solution.

## Overview

This Apartment-reservation-system-based-on-hybrid-communication-protocol System is designed and implemented in C++ under the Linux environment. The system boasts a robust encryption/decryption mechanism to secure sensitive information and utilizes a modular architecture for efficient and secure operations. It features an innovative TCP/UDP mixed communication protocol, optimizing both security and efficiency in data transmission, and leverages advanced multi-threading and thread pool technologies for high concurrency and performance.

## Key Features

- **Advanced Encryption Mechanism**: Ensures all sensitive information transmitted via the client is encrypted securely.
- **Modular Design**: Comprising client, server, and database modules for a streamlined and secure operational flow.
- **Innovative Communication Protocol**: Utilizes a mixed TCP/UDP approach for the optimal balance of reliability and efficiency.
- **High-Concurrency Multithreading**: Employs cutting-edge thread pool technology to boost concurrency handling, enhancing system responsiveness and stability under high load.

## Technologies Used

- **OS**: Linux
- **Language**: C++
- **Networking**: Socket programming with TCP/UDP protocols
- **Concurrency**: Multithreading with thread pool technology

## Getting Started

### Prerequisites

- Linux operating system
- C++ compiler (e.g., g++)
- Necessary libraries for socket programming and thread management

### Installation

1. Clone the project repository:
   git clone <repository-url>
2. Navigate to the project directory:
   cd dormitory-reservation-system
3. Install required libraries (example command below, adjust as necessary):
   sudo apt-get install libsocket++ libthread-pool++
4. Compile the project:
   make all

### Running the Application

- Start the server:
  ./serverM

- Start the data base:
  ./serverS
  ./serverD
  ./serverU

- In a separate terminal, launch the client:
  ./client
  
## Contributing

Contributions to the College-dormitory-reservation-system-based-on-hybrid-communication-protocol System are welcome. Please submit pull requests for any bug fixes, features, or improvements.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.




