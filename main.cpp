/*
 * Created by Zhen Chen on 2025/12/15.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#include <boost/math/distributions/gamma.hpp>
#include <chrono>
#include <iostream>
#include <vector>

#include "pmf.h"
#include "state_multi.h"
#include "two_product.h"

int main() {
  const std::vector prices = {1.2, 2.0};
  const std::vector unit_order_costs = {1.0, 1.5};
  const std::vector unit_salvage_values = {0.5, 0.75};

  constexpr std::array mean_demands = {10.0, 5.0};
  constexpr std::array scales = {1 / 2.5, 1 / 1.25};

  constexpr int T = 1;
  constexpr double interest_rate = 0.0;
  constexpr double ini_cash = 10;
  constexpr double ini_I1 = 0;
  constexpr double ini_I2 = 0;
  constexpr double truncated_quantile = 0.9999;
  constexpr double capacity = 30;
  constexpr double max_I = 100;

  const auto pmf = get_pmf_gamma2(mean_demands, scales, truncated_quantile);
  const auto ini_state = StateMulti(1, ini_I1, ini_I2, ini_cash);

  auto problem = TwoProduct(T, capacity, max_I, truncated_quantile, interest_rate, prices,
                            unit_order_costs, unit_salvage_values, pmf);

  const auto start_time = std::chrono::high_resolution_clock::now();
  const auto result = problem.solve(ini_state);
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time = end_time - start_time;

  std::cout << "running time is " << time << std::endl;
  std::cout << "optimal cash balance is " << result[0] << std::endl;
  std::cout << "optimal q1 at period 1 is: " << result[1] << std::endl;
  std::cout << "optimal q2 at period 1 is: " << result[2] << std::endl;

  return 0;
}
