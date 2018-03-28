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

    // Assigning CL parameters to temporary variables
    std::string input_file1_name = argv[1];
    std::string input_file2_name = argv[2];
    std::string output_file = argv[4];

    // Parsing thread count including default fallback of one
    int thread_count = 1;
    try {
      int temp_thread_count = std::stoi(argv[3]);

      if (temp_thread_count > 1) {
        thread_count = temp_thread_count;
      }
    } catch (std::exception const &e) {
      log("Invalid thread count parameter. Falling back to 1 threaded execution");
    }

  }

  return 0;
}
