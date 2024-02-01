/*
 * Copyright (C) 2018 Guillaume Perez
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <deque>
#include <string>
#include <thread>

#include "ChronoP.hpp"
#include "gen.hpp"



#define PARALLELMACRO(NAME,CODE)                                  \
  int NAME(double *y, double *x, const int size, const int start, \
           const int end) {                                        \
    double local_var = 0;                                          \
    for (size_t i = start; i < end; i++) {                         \
      CODE;                                                        \
    }                                                              \
    return local_var;                                              \
  };                                                               

// functions to benchmark
PARALLELMACRO(para_sub_1, x++);
PARALLELMACRO(para_sub_2, *x++);
PARALLELMACRO(para_sub_3, *x++ += 1);
PARALLELMACRO(para_sub_3_2, *x++ += 1;*y++ += 1;local_var++);
PARALLELMACRO(para_sub_4, *x++ += *y++);
PARALLELMACRO(para_sub_5, local_var += *y++);
PARALLELMACRO(para_sub_6, local_var++);
PARALLELMACRO(para_sub_7, local_var = std::max(local_var,*y++));
PARALLELMACRO(para_sub_8, local_var = std::max(local_var,std::fabs(*y++)));
PARALLELMACRO(para_sub_9, local_var = std::max(local_var,std::fabs(*y++)); *x++=local_var);
PARALLELMACRO(para_sub_10, *x *= *x++);




inline void ParallelF(double *y, double *x, const int size, int nb_workers,
                      int (&f)(double *y, double *x, int size, int start,
                               int end)) {
  int work_slice = size / nb_workers;
  std::deque<std::thread> threads;
  for (size_t w = 0; w < nb_workers; w++) {
    int end = (w + 1) * work_slice;
    if (w == nb_workers - 1) {
      end = size;
    }
    threads.emplace_back(f, y + w * work_slice, x + w * work_slice,
                                 size, w * work_slice, end);
  }

  for (auto &&t : threads) {
    t.join();
  }
}

int main(int argc, char **argv) {
  int nb_worker_max = 12;
  int iterations = 5;
  std::string delimiter = "#";
  ChronoP TS;

  // sizes to test
  std::vector<int64_t> sizes;
  sizes.push_back(5'00'000);
  sizes.push_back(5'000'000);
  sizes.push_back(50'000'000);
  sizes.push_back(500'000'000);

  // Functions to test
  std::vector<int (*)(double *, double *, int, int,int)> fcts;
  std::vector<std::string> fct_names;

  fcts.emplace_back(&para_sub_1); 
  fct_names.push_back("x++");

  fcts.emplace_back(&para_sub_2);
  fct_names.push_back("*x++");

  fcts.emplace_back(&para_sub_3);
  fct_names.push_back("*x++ += 1");

  fcts.emplace_back(&para_sub_3_2);
  fct_names.push_back("*x++ += 1;*y++ += 1;local_var++");

  fcts.emplace_back(&para_sub_4);
  fct_names.push_back("*x++ += *y++");

  fcts.emplace_back(&para_sub_5);
  fct_names.push_back("local_var += *y++");

  fcts.emplace_back(&para_sub_6);
  fct_names.push_back("local_var++");

  fcts.emplace_back(&para_sub_7);
  fct_names.push_back("local_var = std::max(local_var,*y++)");
  
  fcts.emplace_back(&para_sub_8);
  fct_names.push_back("local_var = std::max(local_var,std::fabs(*y++))");

  fcts.emplace_back(&para_sub_9);
  fct_names.push_back("local_var = std::max(local_var,std::fabs(*y++)); *x++=local_var");

  fcts.emplace_back(&para_sub_10);
  fct_names.push_back("*x *= *x++");

  
  for (std::size_t i = 0; i < fcts.size(); i++) {
    auto fct = fcts[i];
    auto code = fct_names[i];
    for (auto &&size : sizes) {
      // first entry is sequential.
      // then, the sequential processing of the smallest workload
      // then, from second to last parallel implem starting at 1
      std::vector<std::vector<int>> times(1 + 1 + nb_worker_max);
      for (size_t repeat = 0; repeat < iterations; repeat++) {
        double *y = new double[size];
        double *x = new double[size];
        FillRandMatrix(y, size, 1);  // mandatory to force allocation
        FillRandMatrix(x, size, 1);  // mandatory to force allocation

        // Direct complete call
        TS.Start();
        (*fct)(y, x, size, 0, size);
        TS.Stop();
        times[0].push_back(TS.ellapsed_u_second());

        // Direct smallest workload call (best expected)
        TS.Start();
        (*fct)(y, x, size, 0, size/nb_worker_max);
        TS.Stop();
        times[1].push_back(TS.ellapsed_u_second());

        // Parallel call
        for (size_t i = 1; i < nb_worker_max + 1; i++) {
          TS.Start();
          ParallelF(y, x, size, i, *fct);
          TS.Stop();
          times[1+i].push_back(TS.ellapsed_u_second());
        }

        delete[] y;
        delete[] x;
      }
      std::cout << code << delimiter << size << delimiter;
      for (auto &&time : times) {
        auto p = MeanAndStdev(time);
        std::cout << p.first << delimiter << p.second << delimiter;
        // std::cout << p.first << ", ";
      }
      std::cout << "\n";
    }
  }

  return 0;
}
