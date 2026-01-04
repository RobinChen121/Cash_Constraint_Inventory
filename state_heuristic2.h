/*
 * Created by Zhen Chen on 2026/1/4.
 * Email: chen.zhen5526@gmail.com
 * Description: 
 *
 *
 */

#ifndef STATE_HEURISTIC2_H
#define STATE_HEURISTIC2_H

#include <boost/functional/hash.hpp>

class StateHeuristic {
    double ini_inventory1{};
    double ini_inventory2{};
    double ini_cash{};

public:
    StateHeuristic();
    StateHeuristic(double ini_inventory1, double ini_inventory2, double ini_cash);

    [[nodiscard]] double get_ini_inventory1() const { return ini_inventory1; };
    [[nodiscard]] double get_ini_inventory2() const { return ini_inventory2; };
    [[nodiscard]] double get_ini_cash() const { return ini_cash; };

    // for unordered map
    bool operator==(const StateHeuristic &other) const {
        return ini_inventory1 == other.ini_inventory1 &&
               ini_inventory2 == other.ini_inventory2 && ini_cash == other.ini_cash;
    }
    // for ordered map
    bool operator<(const StateHeuristic &other) const {
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
struct std::hash<StateHeuristic> {
    std::size_t operator()(const StateHeuristic &s) const noexcept {
        std::size_t seed = 0;
        boost::hash_combine(seed, s.get_ini_inventory1());
        boost::hash_combine(seed, s.get_ini_inventory2());
        boost::hash_combine(seed, s.get_ini_cash());
        return seed;
    }
};

#endif //STATE_HEURISTIC2_H
