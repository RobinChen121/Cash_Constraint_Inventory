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
#include <unordered_map>
#include <map>

#include "state_multi.h"
#include "state_single.h"

class TwoProduct {
  int T;
  double capacity;
  double max_I;
  double interest_rate;

  std::vector<double> prices;
  std::vector<double> unit_order_costs;
  std::vector<double> unit_salvage_values;

  std::vector<std::array<double, 3>> pmf;
  std::vector<std::array<double, 2>> pmf1;
  std::vector<std::array<double, 2>> pmf2;

  std::unordered_map<StateMulti, double> cache_values;
  std::unordered_map<StateMulti, std::array<double, 2>> cache_actions;

  std::unordered_map<State, double> cache_valuesG1;
  std::map<State, double> cache_actionsG1;
  std::unordered_map<State, double> cache_valuesG2;
  std::map<State, double> cache_actionsG2;

  public:
  TwoProduct(int T, double capacity, double max_I, double interest_rate, const std::vector<double> &prices,
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

  std::vector<double> get_a_stars();
  std::vector<double> feasible_actionsG() const;
  double immediate_valueG(const State &ini_state, double action, double demand) const;
  static State state_transitionG(const State &ini_state, double action, double demand);
  double recursionG(const State &state) const;
};


#endif // TWO_PRODUCT_H
