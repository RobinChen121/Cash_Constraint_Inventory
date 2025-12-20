/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#include "state_multi.h"

StateMulti::StateMulti() = default;

StateMulti::StateMulti(const int period, const double ini_inventory1, const double ini_inventory2,
                       const double ini_cash) :
    period(period), ini_inventory1(ini_inventory1), ini_inventory2(ini_inventory2),
    ini_cash(ini_cash) {};
