/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#ifndef STATE_MULTI_H
#define STATE_MULTI_H

#include <boost/functional/hash.hpp>


class StateMulti {
  int period{};
  double ini_inventory1{};
  double ini_inventory2{};
  double ini_cash{};

public:
  StateMulti();
  StateMulti(int period, double ini_inventory1, double ini_inventory2, double ini_cash);

  [[nodiscard]] int get_period() const { return period; };
  [[nodiscard]] double get_ini_inventory1() const { return ini_inventory1; };
  [[nodiscard]] double get_ini_inventory2() const { return ini_inventory2; };
  [[nodiscard]] double get_ini_cash() const { return ini_cash; };

  // for unordered map
  bool operator==(const StateMulti &other) const {
    return period == other.period && ini_inventory1 == other.ini_inventory1 &&
           ini_inventory2 == other.ini_inventory2 && ini_cash == other.ini_cash;
  }
  // for ordered map
  bool operator<(const StateMulti &other) const {
    if (period != other.period)
      return period < other.period;
    if (ini_inventory1 != other.ini_inventory1)
      return ini_inventory1 < other.ini_inventory1;
    if (ini_inventory2 != other.ini_inventory2)
      return ini_inventory2 < other.ini_inventory2;
    return ini_cash < other.ini_cash;
  }
};

  // ============================================
  // Option 1: std::hash specialization, can be directly used by
  // unordered_map without specifying hash functor
  // ============================================
  template<>
  struct std::hash<StateMulti> {
    std::size_t operator()(const StateMulti &s) const noexcept {
      std::size_t seed = 0;
      boost::hash_combine(seed, s.get_period());
      boost::hash_combine(seed, s.get_ini_inventory1());
      boost::hash_combine(seed, s.get_ini_inventory2());
      boost::hash_combine(seed, s.get_ini_cash());
      return seed;
    }
  };


  // // ============================================
  // // Option 2: self defined Hash Functor
  // // ============================================
  // struct StateMultiHash {
  //   std::size_t operator()(const StateMulti &s) const noexcept {
  //     std::size_t seed = 0;
  //     boost::hash_combine(seed, s.get_period());
  //     boost::hash_combine(seed, s.get_ini_inventory1());
  //     boost::hash_combine(seed, s.get_ini_inventory2());
  //     boost::hash_combine(seed, s.get_ini_cash());
  //     return seed;
  //   }
  // };




#endif // STATE_MULTI_H
