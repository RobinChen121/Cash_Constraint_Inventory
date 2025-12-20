/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description: get the probability mass function values for dynamic programming
 *
 *
 */

#ifndef PMF_H
#define PMF_H

#include <vector>

/**
 *  get the probability mass function values for 2 products with gamma distribution
 * @param means mean values
 * @param scales
 * @param quantile
 * @return
 */
std::vector<std::array<double, 3>> get_pmf_gamma2(const std::array<double, 2> &means,
                                                  const std::array<double, 2> &scales,
                                                  double quantile);

#endif // PMF_H
