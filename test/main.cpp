// Catch
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include "catchExtension.hpp"

// Demontrator
#include <demonstrator>

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);

  // The last argument is used as location for the test data directory.
  // Reduced also the number of arguments, in order avoid conflicts with catch command line arguments handling.
  ::rootTestDataDirectory = argv[--argc];
  std::cout << "Using '" << ::rootTestDataDirectory << "' as location for the root test data directory." << std::endl;

  try {
    return Catch::Session().run(argc, argv);
  } catch (const std::exception& exception) {
    std::cout << exception.what();
  }
}
