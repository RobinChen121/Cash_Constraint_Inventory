/*
 * Created by Zhen Chen on 2025/12/20.
 * Email: chen.zhen5526@gmail.com
 * Description: 
 *
 *
 */

#ifndef STATE_SINGLE_H
#define STATE_SINGLE_H

#include <boost/functional/hash.hpp>

class State {
  int period;
  int ini_inventory;
  int product_index;

public:
  State(const int period, const int ini_inventory, const int product_index): period(period), ini_inventory(ini_inventory), product_index(product_index) {};

  [[nodiscard]] int get_period() const { return period; };
  [[nodiscard]] double get_ini_inventory() const { return ini_inventory; };
  [[nodiscard]] int get_product_index() const { return product_index; };

  // for unordered map
  bool operator==(const State &other) const {
    return period == other.period && ini_inventory == other.ini_inventory;
  }

  // for ordered map
  bool operator<(const State &other) const {
    if (period != other.period)
      return period < other.period;
    return ini_inventory< other.ini_inventory;
  }

};

// ============================================
// Option 1: std::hash specialization, can be directly used by
// unordered_map without specifying hash functor
// ============================================
template<>
struct std::hash<State> {
  std::size_t operator()(const State &s) const noexcept {
    std::size_t seed = 0;
    boost::hash_combine(seed, s.get_period());
    boost::hash_combine(seed, s.get_ini_inventory());
    return seed;
  }
};

#endif //STATE_SINGLE_H
