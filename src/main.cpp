//
//  main.cpp
//  dna-sequencer
//
//  Created by Thomas Johannesmeyer (thomas@geeky.gent) on 26.03.2018.
//  Copyright (c) 2017 www.geeky.gent. All rights reserved.
//


#include <iostream>
#include <ctime>

void log(const std::string message) {
  std::time_t time = std::time(0);
  std::tm* now = std::localtime(&time);
  std::cout << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "]: " << message << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    log("Insufficient arguments supplied:");
    log("<file1> <file2> <number of threads> <output file>");
  } else {

  }

  return 0;
}
