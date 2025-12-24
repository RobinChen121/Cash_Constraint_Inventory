/*
 * Created by Zhen Chen on 2025/12/15.
 * Email: chen.zhen5526@gmail.com
 * Description: 4 periods, capacity 40, running time is 0.77s, while java is 4s.
 * Only computing a* is very fast. For 30 periods, the running time is less than 0.00001s.
 * 6 periods, capacity 30, running time is 32.59s; running time when using a* is 23.25s;
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

  constexpr int T = 4;
  constexpr double interest_rate = 0.0;
  constexpr double ini_cash = 10;
  constexpr double ini_I1 = 0;
  constexpr double ini_I2 = 0;
  constexpr double truncated_quantile = 0.9999;
  constexpr int capacity = 30;
  constexpr double max_I = 100;

  const auto pmf = get_pmf_gamma2_product(mean_demands, scales, truncated_quantile);
  const auto ini_state = StateMulti(1, ini_I1, ini_I2, ini_cash);

  auto problem = TwoProduct(T, capacity, max_I, interest_rate, prices, unit_order_costs,
                            unit_salvage_values, pmf);

  const auto start_time = std::chrono::high_resolution_clock::now();
  const auto result = problem.solve(ini_state);
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time1 = end_time - start_time;
  std::cout << "running time is " << time1 << std::endl;
  std::cout << "optimal cash balance is " << result[0] << std::endl;
  std::cout << "optimal q1 at period 1 is: " << result[1] << std::endl;
  std::cout << "optimal q2 at period 1 is: " << result[2] << std::endl;

  problem.set_pmfs(mean_demands, scales, truncated_quantile);
  const auto start_time2 = std::chrono::high_resolution_clock::now();
  problem.get_a_stars();
  const double value2 = problem.recursion2(ini_state) + ini_state.get_ini_cash();
  const auto end_time2 = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time2 = end_time2 - start_time2;
  std::cout << std::string(50, '_') << std::endl;
  std::cout << "running time is " << time2 << std::endl;
  std::cout << "optimal cash balance using Theorem2 is " << value2 << std::endl;
  std::cout << "a* for product 1 is " << std::endl;
  for (int t = 0; t < T; t++) {
    std::cout << problem.astar_G[0][t] << " ";
  }
  std::cout << std::endl;
  std::cout << "a* for product 2 is " << std::endl;
  for (int t = 0; t < T; t++) {
    std::cout << problem.astar_G[1][t] << " ";
  }

  std::cout << std::endl;
  std::cout << std::string(50, '_') << std::endl;
  const auto start_time3 = std::chrono::high_resolution_clock::now();
  problem.set_pmfs(mean_demands, scales, truncated_quantile);
  problem.get_a_stars();
  const double value3 =  problem.heuristic1(ini_state);
  const auto end_time3 = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time3 = end_time3 - start_time3;
  std::cout << "running time is " << time3 << std::endl;
  std::cout << "cash balance using heuristic1 is " <<value3 << std::endl;

  std::cout << std::endl;
  std::cout << std::string(50, '_') << std::endl;
  const auto start_time4 = std::chrono::high_resolution_clock::now();
  problem.set_pmfs(mean_demands, scales, truncated_quantile);
  problem.get_a_stars();
  const double value4 =  problem.heuristic1(ini_state);
  const auto end_time4 = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time4 = end_time4 - start_time4;
  std::cout << "running time is " << time4 << std::endl;
  std::cout << "cash balance using heuristic1_2 is " <<value4 << std::endl;

  std::cout << std::endl;
  std::cout << std::string(50, '_') << std::endl;
  const auto start_time5 = std::chrono::high_resolution_clock::now();
  const double value5=  problem.heuristic2(ini_state) + ini_cash;
  const auto end_time5 = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> time5 = end_time5 - start_time5;
  std::cout << "running time is " << time5 << std::endl;
  std::cout << "cash balance using heuristic2 is " <<value5 << std::endl;

  return 0;
}
