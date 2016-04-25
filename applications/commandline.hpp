#pragma once

// C++ standard library
#include <string>

// Armadillo
#include <armadillo>

bool hasOption(
    const int argc,
    const char* argv[],
    const std::string& option);

bool isNumber(
    const std::string& text);
    
std::string vectorToString(
    const arma::Row<double>& vector);
    
arma::Row<double> stringToVector(
    std::string string);