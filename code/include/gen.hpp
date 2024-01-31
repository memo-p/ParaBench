/*
 * Copyright (C) 2024 Guillaume Perez
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
 *
 * From the code of Laurent Condat: https://lcondat.github.io
 */

#ifndef PROJCODE_INCLUDE_GEN_HPP
#define PROJCODE_INCLUDE_GEN_HPP

#include <limits>
#include <random>


inline void FillRandMatrix(double* mt, const int nrows, const int ncols) {
  for (std::size_t i = 0; i < nrows; i++) {
    for (std::size_t j = 0; j < ncols; j++) {
      std::size_t id = i * ncols + j;
      mt[id] = ((double)rand()) / std::numeric_limits<int>::max();
    }
  }
}

inline std::pair<double,double> MeanAndStdev(std::vector<int>const & v) {
  double sum = std::accumulate(v.begin(), v.end(), 0.0);
  double mean = sum / v.size();

  double sqr_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
  double stdev = std::sqrt(sqr_sum / v.size() - mean * mean);
  return {mean, stdev};
}


#endif /* PROJCODE_INCLUDE_GEN_HPP */
