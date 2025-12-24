/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#include "two_product.h"
#include <cmath>

#include "pmf.h"

TwoProduct::TwoProduct(const int T, const int capacity, const double max_I,
                       const double interest_rate, const std::vector<double> &prices,
                       const std::vector<double> &unit_order_costs,
                       const std::vector<double> &unit_salvage_values,
                       const std::vector<std::array<double, 3>> &pmf) :
    T(T), capacity(capacity), max_I(max_I), interest_rate(interest_rate), prices(prices),
    unit_order_costs(unit_order_costs), unit_salvage_values(unit_salvage_values), pmf(pmf) {}

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
  end_inventory1 = max_I < end_inventory1 ? max_I : end_inventory1;
  end_inventory2 = max_I < end_inventory2 ? max_I : end_inventory2;

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

  const double revenue1 =
          prices[0] * (ini_state.get_ini_inventory1() + actions[0] - end_inventory1);
  const double revenue2 =
          prices[1] * (ini_state.get_ini_inventory2() + actions[1] - end_inventory2);
  const double ordering_costs = unit_order_costs[0] * actions[0] + unit_order_costs[1] * actions[1];
  double salvage_value = 0.0;
  if (ini_state.get_period() == T) {
    salvage_value =
            unit_salvage_values[0] * end_inventory1 + unit_salvage_values[1] * end_inventory2;
  }
  const double interest = interest_rate * (ini_state.get_ini_cash() - ordering_costs);
  return revenue1 + revenue2 + salvage_value + interest - ordering_costs;
}


/**
 * get the action value when computing DP using a*
 * @param state
 * @param action
 * @return
 */
double TwoProduct::get_action_value_theorem2(
        const StateMulti &state, const std::array<double, 2> &action) { // NOLINT(misc-no-recursion)
  double this_value = 0;
  for (const auto demand_and_prob: pmf) {
    const auto demands = std::array{demand_and_prob[0], demand_and_prob[1]};
    this_value += demand_and_prob[2] * immediate_value(state, action, demands);
    if (state.get_period() < T) {
      if (auto new_state = state_transition(state, action, demands);
          cache_value_theorem2.contains(new_state))
        this_value += demand_and_prob[2] * cache_value_theorem2[new_state];
      else {
        const double next_value = recursion2(new_state);
        this_value += demand_and_prob[2] * next_value;
      }
    }
  }
  return this_value;
}


double TwoProduct::recursion(const StateMulti &state) { // NOLINT(*-no-recursion)
  double best_q1 = 0.0;
  double best_q2 = 0.0;
  double best_value = std::numeric_limits<double>::lowest();
  const std::vector<std::array<double, 2>> actions = feasible_actions(state);
  for (const std::array action: actions) {
    // if (state.get_period() == 1)
    //   action = {2.0, 2.0};
    double this_value = 0;
    for (const auto demand_and_prob: pmf) {
      const auto demands = std::array{demand_and_prob[0], demand_and_prob[1]};
      this_value += demand_and_prob[2] * immediate_value(state, action, demands);
      if (state.get_period() < T) {
        if (auto new_state = state_transition(state, action, demands);
            cache_values.contains(new_state))
          this_value += demand_and_prob[2] * cache_values[new_state];
        else {
          const double next_value = recursion(new_state); // NOLINT(misc-no-recursion)
          this_value += demand_and_prob[2] * next_value;
        }
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

/**
 * recursion using the values of a*
 * @param state
 * @return
 */
double TwoProduct::recursion2(const StateMulti &state) { // NOLINT(*-no-recursion)
  const int a1_star = astar_G[0][state.get_period() - 1];
  const int a2_star = astar_G[1][state.get_period() - 1];
  if (state.get_ini_inventory1() > a1_star - 1e-1 and state.get_ini_inventory2() > a2_star - 1e-1) {
    cache_value_theorem2[state] = get_action_value_theorem2(state, {0.0, 0.0});
    return cache_value_theorem2[state];
  }
  if (state.get_ini_inventory1() > a1_star - 1e-1 and state.get_ini_inventory2() < a2_star - 1e-1) {
    const double q = std::fmin(a2_star, state.get_ini_cash() / unit_order_costs[1] +
                                                state.get_ini_inventory2());
    cache_value_theorem2[state] = get_action_value_theorem2(state, {0.0, q});
    return cache_value_theorem2[state];
  }
  if (state.get_ini_inventory1() < a1_star - 1e-1 and state.get_ini_inventory2() > a2_star - 1e-1) {
    const double q = std::fmin(a1_star, state.get_ini_cash() / unit_order_costs[0] +
                                                state.get_ini_inventory1());
    cache_value_theorem2[state] = get_action_value_theorem2(state, {q, 0.0});
    return cache_value_theorem2[state];
  }
  if (state.get_ini_inventory1() < a1_star - 1e-1 and state.get_ini_inventory2() < a2_star - 1e-1) {
    if (state.get_ini_cash() >
        unit_order_costs[0] * (a1_star - state.get_ini_inventory1()) +
                unit_order_costs[1] * (a2_star - state.get_ini_inventory2())) {
      cache_value_theorem2[state] = get_action_value_theorem2(
              state, {static_cast<double>(a1_star), static_cast<double>(a2_star)});
      return cache_value_theorem2[state];
    }
    if (state.get_period() < T) {
      const int a1_star_next = astar_G[0][state.get_period() - 1];
      if (const int a2_star_next = astar_G[1][state.get_period() - 1];
          state.get_ini_cash() >
          unit_order_costs[0] * (a1_star_next - state.get_ini_inventory1()) +
                  unit_order_costs[1] * (a2_star_next - state.get_ini_inventory2())) {
        const int t_index = state.get_period() - 1;
        double best_value = std::numeric_limits<double>::lowest();
        double this_value = 0.0;
        int best_y1 = 0;
        for (int y = static_cast<int>(state.get_ini_inventory1()); y < capacity; y++) {
          const int y2 = static_cast<int>(
                  state.get_ini_inventory2() +
                  (state.get_ini_cash() - unit_order_costs[0] * (y - state.get_ini_inventory1())) /
                          unit_order_costs[1]);
          this_value = cache_valuesG[0][t_index][y] + cache_valuesG[1][t_index][y2];
          if (this_value > best_value) {
            best_value = this_value;
            best_y1 = y;
          }
        }
        double addition = state.get_ini_cash() + unit_order_costs[0] * state.get_ini_inventory1() +
                          unit_order_costs[1] * state.get_ini_inventory2();
        addition *= std::pow(1 + interest_rate, T - t_index);
        cache_value_theorem2[state] = best_value; // + addition;
        return cache_value_theorem2[state];

        // const double q1 = best_y1 - state.get_ini_inventory1();
        // const double q2 = (state.get_ini_cash() - unit_order_costs[0] * q1) /
        //                   unit_order_costs[1];
        // cache_value_theorem2[state] = get_action_value_theorem2(state, {q1, q2});
        // return cache_value_theorem2[state];
      }
    }
  }

  double best_value = std::numeric_limits<double>::lowest();
  const std::vector<std::array<double, 2>> actions = feasible_actions(state);
  for (const std::array action: actions) {
    if (const double this_value = get_action_value_theorem2(state, action);
        this_value > best_value) {
      best_value = this_value;
    }
  }
  cache_value_theorem2[state] = best_value;
  return best_value;
}

/**
 * get final value directly from the first stage of G
 * @param state
 * @return
 */
double TwoProduct::heuristic1(const StateMulti &state) const {
  double this_value = 0.0;
  double best_value = std::numeric_limits<double>::lowest();
  for (int y = static_cast<int>(state.get_ini_inventory1()); y < capacity; y++) {
    const int y2 = static_cast<int>(
            state.get_ini_inventory2() +
            (state.get_ini_cash() - unit_order_costs[0] * (y - state.get_ini_inventory1())) /
                    unit_order_costs[1]);
    this_value = cache_valuesG[0][0][y] + cache_valuesG[1][0][y2];
    if (this_value > best_value) {
      best_value = this_value;
    }
  }
  double addition = state.get_ini_cash() + unit_order_costs[0] * state.get_ini_inventory1() +
                    unit_order_costs[1] * state.get_ini_inventory2();
  addition *= std::pow(1 + interest_rate, T);
  return best_value + addition;
}

/**
 * get final value from applying heuristic in recursion
 * @param state
 * @return
 */
double TwoProduct::heuristic1_2(const StateMulti &state) {
  const int a1_star = astar_G[0][state.get_period() - 1];
  const int a2_star = astar_G[1][state.get_period() - 1];
  if (state.get_ini_inventory1() > a1_star - 1e-1 and state.get_ini_inventory2() > a2_star - 1e-1) {
    cache_value_theorem2[state] = get_action_value_theorem2(state, {0.0, 0.0});
    return cache_value_theorem2[state];
  }
  if (state.get_ini_inventory1() > a1_star - 1e-1 and state.get_ini_inventory2() < a2_star - 1e-1) {
    const double q = std::fmin(a2_star, state.get_ini_cash() / unit_order_costs[1] +
                                                state.get_ini_inventory2());
    cache_value_theorem2[state] = get_action_value_theorem2(state, {0.0, q});
    return cache_value_theorem2[state];
  }
  if (state.get_ini_inventory1() < a1_star - 1e-1 and state.get_ini_inventory2() > a2_star - 1e-1) {
    const double q = std::fmin(a1_star, state.get_ini_cash() / unit_order_costs[0] +
                                                state.get_ini_inventory1());
    cache_value_theorem2[state] = get_action_value_theorem2(state, {q, 0.0});
    return cache_value_theorem2[state];
  }
  if (state.get_ini_inventory1() < a1_star - 1e-1 and state.get_ini_inventory2() < a2_star - 1e-1) {
    if (state.get_ini_cash() >
        unit_order_costs[0] * (a1_star - state.get_ini_inventory1()) +
                unit_order_costs[1] * (a2_star - state.get_ini_inventory2())) {
      cache_value_theorem2[state] = get_action_value_theorem2(
              state, {static_cast<double>(a1_star), static_cast<double>(a2_star)});
      return cache_value_theorem2[state];
    }
    if (state.get_period() < T) {
      const int t_index = state.get_period() - 1;
      double best_value = std::numeric_limits<double>::lowest();
      double this_value = 0.0;
      int best_y1 = 0;
      for (int y = static_cast<int>(state.get_ini_inventory1()); y < capacity; y++) {
        const int y2 = static_cast<int>(
                state.get_ini_inventory2() +
                (state.get_ini_cash() - unit_order_costs[0] * (y - state.get_ini_inventory1())) /
                        unit_order_costs[1]);
        this_value = cache_valuesG[0][t_index][y] + cache_valuesG[1][t_index][y2];
        if (this_value > best_value) {
          best_value = this_value;
          best_y1 = y;
        }
      }
      double addition = state.get_ini_cash() + unit_order_costs[0] * state.get_ini_inventory1() +
                        unit_order_costs[1] * state.get_ini_inventory2();
      addition *= std::pow(1 + interest_rate, T - t_index);
      cache_value_theorem2[state] = best_value; // + addition;
      return cache_value_theorem2[state];
    }
  }
}

double TwoProduct::heuristic2(const StateMulti &state) {
  double best_q1 = 0.0;
  double best_q2 = 0.0;
  double best_value = std::numeric_limits<double>::lowest();
  const std::vector<std::array<double, 2>> actions = feasible_actions(state);
  for (const std::array action: actions) {
    double this_value = 0;
    for (const auto demand_and_prob: pmf) {
      const auto demands = std::array{demand_and_prob[0], demand_and_prob[1]};
      this_value += demand_and_prob[2] * immediate_value(state, action, demands);
      if (state.get_period() < T) {
        if (auto new_state = state_transition(state, action, demands);
            cache_values.contains(new_state))
          this_value += demand_and_prob[2] * cache_values[new_state];
        else {
          // const double next_value = recursion(new_state); // NOLINT(misc-no-recursion)
          // this_value += demand_and_prob[2] * next_value;
        }
      }
    }
    if (this_value > best_value) {
      best_value = this_value;
      best_q1 = action[0];
      best_q2 = action[1];
    }
  }
  double this_value = 0.0;
  if (state.get_period() < T) {
    const auto action = std::array{best_q1, best_q2};
    for (const auto demand_and_prob: pmf) {
      const auto demands = std::array{demand_and_prob[0], demand_and_prob[1]};
      this_value += demand_and_prob[2] * immediate_value(state, action, demands);
      if (auto new_state = state_transition(state, action, demands);
          cache_values_heuristic2.contains(new_state))
        this_value += demand_and_prob[2] * cache_values_heuristic2[new_state];
      else {
        const double next_value = heuristic2(new_state); // NOLINT(misc-no-recursion)
        this_value += demand_and_prob[2] * next_value;
      }
    }
  }

  cache_values_heuristic2[state] = best_value + this_value;
  return best_value;
}


std::vector<double> TwoProduct::solve(const StateMulti &state) {
  std::vector<double> results(3);
  results[0] = recursion(state) + state.get_ini_cash();
  results[1] = cache_actions[state][0];
  results[2] = cache_actions[state][1];
  return results;
}

void TwoProduct::get_a_stars() {
  astar_G[0].resize(T + 1); // resize makes default value 0 for each element
  astar_G[1].resize(T + 1);
  cache_valuesG[0].resize(T + 1);
  cache_valuesG[1].resize(T + 1);

  for (int t = 0; t < T + 1; t++) {
    cache_valuesG[0][t].resize(capacity);
    cache_valuesG[1][t].resize(capacity);
  }

  // 最后一阶段边界条件
  for (int i = 0; i < capacity; ++i) {
    cache_valuesG[0][T][i] = (unit_salvage_values[0] - unit_order_costs[0]) * i;
    cache_valuesG[1][T][i] = (unit_salvage_values[1] - unit_order_costs[1]) * i;
  }

  const int end_y = capacity - 1;
  for (int t = T - 1; t >= 0; t--) {
    constexpr int start_y = 0;
    compute_stageG(t, start_y, end_y, 1);
    compute_stageG(t, start_y, end_y, 2);
  }
}

void TwoProduct::compute_stageG(const int t, const int start_y, const int end_y,
                                const int product_index) {
  const int index = product_index - 1;
  double best_value = std::numeric_limits<double>::lowest();
  for (int i = start_y; i <= end_y; i++) {
    double this_value = 0.0;
    for (auto demand_and_prob: pmfs[index]) {
      const auto demand = demand_and_prob[0];
      double this_p_value = 0.0;
      this_p_value += (prices[index] - unit_order_costs[index]) * std::fmin(i, demand);
      this_p_value -= interest_rate * unit_order_costs[index] * i;
      this_p_value *= std::pow(1 + interest_rate, T - t);
      const int next_y =
              static_cast<int>(std::fmax(astar_G[index][t + 1], std::fmax(i - demand, 0.0)));
      this_p_value += cache_valuesG[index][t + 1][next_y];
      this_value += demand_and_prob[1] * this_p_value;
    }
    cache_valuesG[index][t][i] = this_value;
    if (this_value > best_value) {
      best_value = this_value;
      astar_G[index][t] = i;
    }
  }
}

void TwoProduct::set_pmfs(const std::array<double, 2> &means, const std::array<double, 2> &scales,
                          const double truncated_quantile) {
  pmfs = get_pmf_gamma1_products(means, scales, truncated_quantile);
}
