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

#include <atomic>
#include <deque>
#include <iostream>
#include <string>
#include <thread>

#include "ChronoP.hpp"
#include "gen.hpp"

int para_sub_ato1(std::vector<int> &y,
                  std::vector<int> const &x, const int start,
                  const int end) {
  double local_var = 0;
  for (size_t i = start; i < end; i++) {
    local_var += x[i];
    y[i] = x[i];
  }
  return local_var;
};

inline void ParallelF(std::vector<int> &y,
                      std::vector<int> &x, int nb_workers,
                      int (&f)(std::vector<int> &,
                               std::vector<int> const&, const int,
                               const int)) {
  int work_slice = y.size() / nb_workers;
  int w = 0;
  std::deque<std::thread> threads;
  for (size_t w = 0; w < nb_workers; w++) {
    int end = (w + 1) * work_slice;
    if (w == nb_workers - 1) {
      end = y.size();
    }
    threads.emplace_back(f, std::ref(y), std::cref(x), w * work_slice, end);
  }

  for (auto &&t : threads) {
    t.join();
  }
}

int main(int argc, char **argv) {
  int nb_worker_max = 12;
  int iterations = 5;
  std::string delimiter = ", ";
  ChronoP TS;

  // sizes to test
  std::vector<int64_t> sizes;
  //   sizes.push_back(5'00'000);
  sizes.push_back(5'000'000);
  sizes.push_back(50'000'000);
  //   sizes.push_back(500'000'000);

  // Functions to test
  std::vector<int (*)(std::vector<int> &,
                      std::vector<int> const&, const int, const int)>
      fcts;
  std::vector<std::string> fct_names;

  fcts.emplace_back(&para_sub_ato1);
  fct_names.push_back("y[i].fetch_add(x[i])");

  for (std::size_t i = 0; i < fcts.size(); i++) {
    auto fct = fcts[i];
    auto code = fct_names[i];
    for (auto &&size : sizes) {
      // first entry is sequential.
      // then, the sequential processing of the smallest workload
      // then, from second to last parallel implem starting at 1
      std::vector<std::vector<int>> times(1 + 1 + nb_worker_max);
      for (size_t repeat = 0; repeat < iterations; repeat++) {
        std::atomic_int zero(0);
        std::vector<int> y;
        std::vector<int> x;
        for (size_t vec_init = 0; vec_init < size; vec_init++) {
          y.emplace_back(0);
          x.emplace_back(0);
        }

        // std::vector<int> y(size, zero);
        // std::vector<int> x(size, zero);
        // FillRandVector(y);
        // FillRandVector(x);

        // Direct complete call
        TS.Start();
        (*fct)(y, x, 0, size);
        TS.Stop();
        times[0].push_back(TS.ellapsed_u_second());

        // Direct smallest workload call (best expected)
        TS.Start();
        (*fct)(y, x, 0, size / nb_worker_max);
        TS.Stop();
        times[1].push_back(TS.ellapsed_u_second());

        // Parallel call
        for (size_t i = 1; i < nb_worker_max + 1; i++) {
          TS.Start();
          ParallelF(y, x, i, *fct);
          TS.Stop();
          times[1 + i].push_back(TS.ellapsed_u_second());
        }
      }
      std::cout << code << delimiter << size << delimiter;
      for (auto &&time : times) {
        auto p = MeanAndStdev(time);
        // std::cout << p.first << delimiter << p.second << delimiter;
        std::cout << p.first << delimiter;
      }
      std::cout << "\n";
    }
  }

  return 0;
}
