/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#ifndef TWO_PRODUCT_H
#define TWO_PRODUCT_H

#include <vector>
#include "state_multi.h"
#include <unordered_map>

class TwoProduct {
  int T;
  double capacity;
  double max_I;
  double quantile;
  double interest_rate;

  std::vector<double> prices;
  std::vector<double> unit_order_costs;
  std::vector<double> unit_salvage_values;

  std::vector<std::array<double, 3>> pmf;

  std::unordered_map<StateMulti, double> cache_values;
  std::unordered_map<StateMulti, std::array<double, 2>> cache_actions;

  public:
  TwoProduct(int T, double capacity, double max_I, double quantile, double interest_rate, const std::vector<double> &prices,
             const std::vector<double> &unit_order_costs,
             const std::vector<double> &unit_salvage_values,
             const std::vector<std::array<double, 3>> &pmf);

  std::vector<std::array<double, 2>> feasible_actions(const StateMulti &state) const;
  StateMulti state_transition(const StateMulti &ini_state, const std::array<double, 2> &actions,
                              const std::array<double, 2> &demands) const;
  double immediate_value(const StateMulti &ini_state, const std::array<double, 2> &actions,
                         const std::array<double, 2> &demands) const;

  double recursion(const StateMulti &state);
  std::vector<double> solve(const StateMulti &state);
};


#endif // TWO_PRODUCT_H
