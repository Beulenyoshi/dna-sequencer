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
#include <vector>
#include <tuple>
#include <iomanip>
#include <thread>
#include <mutex>

/* Configuration */
#define MATCH 2
#define MISMATCH -1
#define GAP -1
#define THRESHOLD 7

/* Globals */
std::mutex SMS_TRACKER_LOCK;

std::vector<std::tuple<int, int, int>> FINAL_DATA;
std::vector<std::tuple<int, int, int>> SMS_TRACKER;


/* Templates */
template<int index> struct TupleSortAscending {
  /* Implements a way to sort tuples inside a vector using the provided index */
  template<typename Tuple>
    bool operator() (const Tuple& left, const Tuple& right) const {
      return std::get<index>(left) < std::get<index>(right);
    }
};


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


void save_tuple(std::tuple<int, int, int> tuple) {
  std::lock_guard<std::mutex> guard(SMS_TRACKER_LOCK);
  SMS_TRACKER.push_back(tuple);
}


int weight(const char a,
    const char b) {
  if(a == b) return MATCH;
  else return MISMATCH;
}


int smith_waterman(const int i,
                  const int j,
                  std::string& data1_ref,
                  std::string& data2_ref,
                  std::vector<std::vector<int>>& matrix_ref) {

  int local_max = 0;

  local_max = std::max(local_max, matrix_ref[i - 1][j - 1] + weight(data1_ref[i - 1], data2_ref[j - 1]));

  local_max = std::max(local_max, matrix_ref[i - 1][j] + GAP);

  local_max = std::max(local_max, matrix_ref[i][j - 1] + GAP);

  return local_max;
}


void export_data(std::string filename, std::string input_1, std::string input_2) {
  std::ofstream new_file;
  new_file.open(filename);
  new_file << "start in " << input_1 << ",start in " << input_2 << ",score" << std::endl;
  for (const auto& t : FINAL_DATA) {
    new_file << std::get<0>(t) << "," << std::get<1>(t) << "," << std::get<2>(t) << std::endl;
  }
  new_file.close();
}


void post_process_data() {
  /* Since multiple threads may have accessed the data in this vector, we cannot
   * guarantee for a uniform output. We start by sorting the array in ascending
   * order with our first tuple index as reference: */
  std::sort(SMS_TRACKER.begin(), SMS_TRACKER.end(), TupleSortAscending<0>());

  int last_comparator = 0;

  /* Local Maximum Values */
  int lm_a            = 0;
  int lm_b            = 0;
  int lm_score        = 0;

  /* std::tuple<int, int, int> local_max_ref; */

  for (const auto& t : SMS_TRACKER) {
    int current_comparator = std::get<0>(t);
    int current_b          = std::get<1>(t);
    int current_score      = std::get<2>(t);

    if(current_comparator != last_comparator) {
      if (last_comparator != 0) {
        /* We have found a new maximum and therefor have to save it. */
        FINAL_DATA.push_back(std::make_tuple(lm_a, lm_b, lm_score));
      }

      /* New value to compare... */
      last_comparator = current_comparator;
      lm_a            = current_comparator;
      lm_b            = current_b;
      lm_score        = current_score;

    } else {
      if(current_score > lm_score) {
        /* We have found a new LOCAL maximum. */
        lm_a     = current_comparator;
        lm_b     = current_b;
        lm_score = current_score;
      }
    }
  }

  if (lm_score != 0) {
    /* Push last data tuple into data vector */
    FINAL_DATA.push_back(std::make_tuple(lm_a, lm_b, lm_score));
  }
}


int main(int argc, char **argv) {
  if (argc < 5) {
    log("Insufficient arguments supplied:");
    log("<file1> <file2> <number of threads> <output file>");
  } else {

    /* Assigning CL parameters to temporary variables */
    std::string input_file1_name = argv[1];
    std::string input_file2_name = argv[2];
    std::string output_file = argv[4];

    /* Parsing thread count including default fallback of one */
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

    /* Rather than manually heap-allocating a buffer of type char* of sufficient
      size to store the files content, we misuse the much more comfortable std::string
      datatype. By pulling this off we achive fast IO while keeping the comfort of
      std::string (including memory management) */
    std::string buffer1 = std::string(size1, ' ');
    const char *p_buff1 = buffer1.c_str();
    file1.read((char *)p_buff1, size1);

    file1.close();


    /* Same for file2 */
    int size2 = -1;
    std::ifstream file2(input_file2_name);
    if (file2) {
      file2.seekg(0, file2.end);
      size2 = file2.tellg();
      file2.seekg(0, file2.beg);
    }

    std::string buffer2 = std::string(size2, ' ');
    const char *p_buff2 = buffer2.c_str();
    file2.read((char *)p_buff2, size2);

    file2.close();

    trim_data(buffer1);
    trim_data(buffer2);

    /* Test */
    /* buffer2 = "ACACACTA"; */
    /* buffer1 = "AGCACACA"; */
    /* size1 = 8; */
    /* size2 = 8; */

    /* Initialize matrix of size (|m| + 1)*(|n| + 1) with 0 as default */
    std::vector<int> line(size1 + 1, 0);
    std::vector<std::vector<int>> matrix(size2 + 1, line);

    int threshold = THRESHOLD;

    int maximum = 0;
    for (int y = 1; y <= size1; ++y) {
      for (int x = 1; x <= size2; ++x) {

        int score = smith_waterman(x,
                                   y,
                                   buffer1,
                                   buffer2,
                                   matrix);

        /* Keep track of maximum */
        maximum = std::max(maximum, score);

        /* Save score to matrix */
        matrix[x][y] = score;

        /* Add score triple to global score tracker */
        if(score >= threshold) {
          std::tuple<int, int, int> triple(x, y, score);
          save_tuple(triple);
        }
      }
    }

    /* Print Matrix for debugging */
    /* for (const std::vector<int> &v : matrix) { */
         /* for (int x : v) std::cout << std::setw(6) << x << ' '; */
            /* std::cout << std::endl; */
    /* } */

    post_process_data();
    export_data(output_file, input_file1_name, input_file2_name);
  }

  return 0;
}
