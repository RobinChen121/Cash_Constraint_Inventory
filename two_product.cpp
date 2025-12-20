/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#include "two_product.h"
#include <cmath>

TwoProduct::TwoProduct(const int T, const double capacity, const double max_I,
                       const double quantile, const double interest_rate,
                       const std::vector<double> &prices,
                       const std::vector<double> &unit_order_costs,
                       const std::vector<double> &unit_salvage_values,
                       const std::vector<std::array<double, 3>> &pmf) :
    T(T), capacity(capacity), max_I(max_I), quantile(quantile), interest_rate(interest_rate),
    prices(prices), unit_order_costs(unit_order_costs), unit_salvage_values(unit_salvage_values),
    pmf(pmf) {}

std::vector<std::array<double, 2>> TwoProduct::feasible_actions(const StateMulti &state) const {
  std::vector<std::array<double, 2>> actions;
  actions.reserve(static_cast<int>(capacity) * static_cast<int>(capacity));
  for (int q1 = 0; q1 < capacity; q1++) {
    for (int q2 = 0; q2 < capacity; q2++) {
      if (unit_order_costs[0] * q1 + unit_order_costs[1] * q2 < state.get_ini_cash() + 1e-1) {
        actions.emplace_back(std::array{static_cast<double>(q1), static_cast<double>(q2)});
      }
    }
  }
  return actions;
}

StateMulti TwoProduct::state_transition(const StateMulti &ini_state,
                                        const std::array<double, 2> &actions,
                                        const std::array<double, 2> &demands) const {
  double end_inventory1 =
          std::fmax<double>(ini_state.get_ini_inventory1() + actions[0] - demands[0], 0.0);
  double end_inventory2 =
          std::fmax<double>(ini_state.get_ini_inventory2() + actions[1] - demands[1], 0.0);
  end_inventory1 = max_I > end_inventory1 ? max_I : end_inventory1;
  end_inventory2 = max_I > end_inventory2 ? max_I : end_inventory2;

  const double next_cash = ini_state.get_ini_cash() + immediate_value(ini_state, actions, demands);
  return StateMulti{ini_state.get_period() + 1, end_inventory1, end_inventory2, next_cash};
}

double TwoProduct::immediate_value(const StateMulti &ini_state,
                                   const std::array<double, 2> &actions,
                                   const std::array<double, 2> &demands) const {
  double end_inventory1 =
          std::fmax<double>(ini_state.get_ini_inventory1() + actions[0] - demands[0], 0.0);
  double end_inventory2 =
          std::fmax<double>(ini_state.get_ini_inventory2() + actions[1] - demands[1], 0.0);
  end_inventory1 = max_I < end_inventory1 ? max_I : end_inventory1;
  end_inventory2 = max_I < end_inventory2 ? max_I : end_inventory2;

  const double revenue1 = prices[0] * (actions[0] - end_inventory1);
  const double revenue2 = prices[1] * (actions[1] - end_inventory2);
  const double ordering_costs = unit_order_costs[0] * actions[0] + unit_order_costs[1] * actions[1];
  double salvage_value = 0.0;
  if (ini_state.get_period() == T) {
    salvage_value =
            unit_salvage_values[0] * end_inventory1 + unit_salvage_values[1] * end_inventory2;
  }
  const double interest = interest_rate * (ini_state.get_ini_cash() - ordering_costs);
  return revenue1 + revenue2 + salvage_value + interest - ordering_costs;
}

double TwoProduct::recursion(const StateMulti &state) { // NOLINT(*-no-recursion)
  double best_q1 = 0.0;
  double best_q2 = 0.0;
  double best_value = std::numeric_limits<double>::lowest();
  const std::vector<std::array<double, 2>> actions = feasible_actions(state);
  for (
       const std::array action: actions) {
    double this_value = 0;
    for (auto demand_and_prob: pmf) {
      auto demands = std::array<double, 2>{demand_and_prob[0], demand_and_prob[1]};
      this_value += demand_and_prob[2] * immediate_value(state, action, demands);
      if (state.get_period() < T) {
        if (auto new_state = state_transition(state, action, demands);
            cache_values.contains(new_state))
          this_value += demand_and_prob[2] * cache_values[new_state];
        else
          this_value += demand_and_prob[2] * recursion(new_state);
      }
    }
    if (this_value > best_value) {
      best_value = this_value;
      best_q1 = action[0];
      best_q2 = action[1];
    }
  }
  cache_values[state] = best_value;
  cache_actions[state] = std::array{best_q1, best_q2};
  return best_value;
}


std::vector<double> TwoProduct::solve(const StateMulti &state) {
  std::vector<double> results(3);
  results[0] = recursion(state) + state.get_ini_cash();
  results[1] = cache_actions[state][0];
  results[2] = cache_actions[state][1];
  return results;
}