// C++ standard library
#include <iostream>
#include <thread>
#include <atomic>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runSend(
    demo::Network& network);
void runReceive(
    demo::Network& network);

int main (const int argc, const char* argv[]) {
  if (hasOption(argc, argv, "-h") || hasOption(argc, argv, "--help")) {
    showHelp();
    // Terminates the program after the help is shown.
    return EXIT_SUCCESS;
  }

  if (hasOption(argc, argv, "--verbose")) {
    ::demo::isVerbose = true;
  }

  unsigned int port;
  std::cout << "Specify local port: ";
  std::cin >> port;
  demo::Network network(port);

  if (hasOption(argc, argv, "receive")) {
    runReceive(network);
    return EXIT_SUCCESS;
  } else if (hasOption(argc, argv, "send")) {
    runSend(network);
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program receive [options ...]\n";
  std::cout << "    Receives messages.\n";
  std::cout << "  program send [options ...]\n";
  std::cout << "    Send messages.\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runSend(
    demo::Network& network) {
  std::cout << "Please enter one of the following patterns:\n"
            << "  exit - terminates the program\n"
            << "  <host>:<port> <message> - sends `message` to the respective device"
            << std::endl;
  while (1) {
    std::string command;
    std::getline(std::cin, command);

    if (command == "") {
      continue;
    }

    if (command == "exit") {
      return;
    }

    try {
      std::string host = command.substr(0, command.find(':'));
      int port = std::stoi(command.substr(command.find(':') + 1, command.find(' ')));
      std::string message = command.substr(command.find(' ') + 1);

      network.send(host, port, message);
      std::cout << "Sent. Type in your next command:" << std::endl;
    } catch (...) {
      std::cout << "Invalid format: '" << command << "'" << std::endl;;
    }
  }
}

void runReceive(
    demo::Network& network) {
  while(1) {
    std::cout << "Waits for messages to receive..." << std::endl;

    const std::string& message = network.receive();
    std::cout << "Received: `" << message << "`" << std::endl;

    if (message == "exit") {
      return;
    }
  }
}
