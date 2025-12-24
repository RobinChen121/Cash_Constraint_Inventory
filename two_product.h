/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#ifndef TWO_PRODUCT_H
#define TWO_PRODUCT_H

#include <unordered_map>
#include <vector>

#include "state_multi.h"

class TwoProduct {
  int T;
  int capacity;
  double max_I;
  double interest_rate;

  std::vector<double> prices;
  std::vector<double> unit_order_costs;
  std::vector<double> unit_salvage_values;

  std::vector<std::array<double, 3>> pmf;
  std::array<std::vector<std::array<double, 2>>, 2> pmfs;

  std::unordered_map<StateMulti, double> cache_values;
  std::unordered_map<StateMulti, std::array<double, 2>> cache_actions;

  std::array<std::vector<std::vector<double>>, 2> cache_valuesG;

  std::unordered_map<StateMulti, double> cache_value_theorem2; // for using a* in dynamic programming

  std::unordered_map<StateMulti, double> cache_values_heuristic2;

  public:
  std::array<std::vector<int>, 2> astar_G;

  TwoProduct(int T, int capacity, double max_I, double interest_rate,
             const std::vector<double> &prices, const std::vector<double> &unit_order_costs,
             const std::vector<double> &unit_salvage_values,
             const std::vector<std::array<double, 3>> &pmf);

  [[nodiscard]] std::vector<std::array<double, 2>> feasible_actions(const StateMulti &state) const;
  [[nodiscard]] StateMulti state_transition(const StateMulti &ini_state,
                                            const std::array<double, 2> &actions,
                                            const std::array<double, 2> &demands) const;
  [[nodiscard]] double immediate_value(const StateMulti &ini_state,
                                       const std::array<double, 2> &actions,
                                       const std::array<double, 2> &demands) const;
  [[nodiscard]] double get_action_value_theorem2(const StateMulti &state,
                                        const std::array<double, 2> &action);

  double recursion(const StateMulti &state);
  double recursion2(const StateMulti &state);
  std::vector<double> solve(const StateMulti &state);

  void get_a_stars();
  void compute_stageG(int t, int start_y, int end_y, int product_index);
  void set_pmfs(const std::array<double, 2> &means, const std::array<double, 2> &scales,
                double truncated_quantile);

  double heuristic1(const StateMulti &state) const;
  double heuristic1_2(const StateMulti &state);

  double heuristic2(const StateMulti &state);
};


#endif // TWO_PRODUCT_H
