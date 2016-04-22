// C++ standard library
#include <iostream>
#include <thread>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runTest();

int main (const int argc, const char* argv[]) {
  if (hasOption(argc, argv, "-h") || hasOption(argc, argv, "--help")) {
    showHelp();
    // Terminates the program after the help is shown.
    return 0;
  }

  if (hasOption(argc, argv, "--verbose")) {
    ::demo::isVerbose = true;
  }

  int port;
  std::cout << "Specify local port: ";
  std::cin >> port;
  Network network(port);

  bool killThread = false;
  std::thread receiveThread(network, killThread);

  runSendInteractive(network, killThread);
  killThread = true;
  receiveThread.join();

  return 0;
}

void showHelp() {
  std::cout << "Usage:\n";
  std::cout << "  program [options ...]\n";
  std::cout << "    Sends and receives string messages entered via command line. Must be run on at least two PCs simultaneously.\n";
  std::cout << "\n";
  std::cout << "  Options:\n";
  std::cout << "         --verbose    Prints additional (debug) information\n";
  std::cout << "    -h | --help       Displays this help\n";
  std::cout << std::flush;
}

void runSendInteractive() {
  std::cout << "Please enter one of the following patterns:\n"
            << "  exit - terminates the program\n"
            << "  <host>:<port> <message> - sends `message` to the respective device"
            << std::endl;
  while (true) {
    std::string command;
    std::getline(std::cin, command);
    if (command == "exit") {
      return;
    }

    try {
      std::string host = command.substr(0, command.find(':'));
      int port = command.substr(command.find(':') + 1, command.find(' '));
      std::string message command.substr(command.find(' ') + 1);

      network.send(host, port, message);
      std::cout << "Sent.\n";
    } catch(const std::out_of_range& e) { // thrown by `string::substr()`
      std::cout << "Invalid format.\n";
    }
  }
}

void runReceive(Network& network, bool& killThread) {
  std::cout << "  **Now listening on port**" << std::endl;
  while (!killThread) {
    std::cout << "  **Reveiced: `" << network.receive() << "` **" << std::endl;
  }
  std::cout << "  **Stop listening on port**" << std::endl;
}
