//
//  main.cpp
//  dna-sequencer
//
//  Created by Thomas Johannesmeyer (thomas@geeky.gent) on 26.03.2018.
//  Copyright (c) 2017 www.geeky.gent. All rights reserved.
//


#include <iostream>
#include <fstream>
#include <ctime>

void log(const std::string message) {
  std::time_t time = std::time(0);
  std::tm* now = std::localtime(&time);
  std::cout << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "]: " << message << std::endl;
}


void trim_data(std::string& str) {
    /* Erase headers */
    size_t found_header_pos = str.find('>');

    while(found_header_pos != std::string::npos) {
      size_t eol_pos = str.find('\n', found_header_pos);
      str.erase(found_header_pos, eol_pos);
      found_header_pos = str.find('>');
    }

    /* Trim line ends */
    str.erase(std::remove(str.begin(),
                              str.end(),
                              '\n'),
                  str.end());
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

    /* Calculting filesize */
    int size1 = -1;
    std::ifstream file1(input_file1_name);
    if (file1) {
      file1.seekg(0, file1.end);
      size1 = file1.tellg();
      file1.seekg(0, file1.beg);
    }

    /* printf("Size of Buffer: %d\n", size1); */

    /* Rather than manually heap-allocating a buffer of type char* of sufficient
      size to store the files content, we misuse the much more comfortable std::string
      datatype. By pulling this off we achive fast IO while keeping the comfort of
      std::string (including memory management) */
    std::string buffer1 = std::string(size1, ' ');
    const char * test = buffer1.c_str();
    file1.read((char *)test, size1);

    file1.close();

    /* log(buffer1); */

    trim_data(buffer1);


  }

  return 0;
}
