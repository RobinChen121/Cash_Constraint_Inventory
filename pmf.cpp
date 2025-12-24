/*
 * Created by Zhen Chen on 2025/12/18.
 * Email: chen.zhen5526@gmail.com
 * Description:
 *
 *
 */

#include "pmf.h"
#include <boost/math/distributions/gamma.hpp>

// shape = demand / scale
// variance = shape * scale^2 = demand * scale
std::vector<std::array<double, 3>> get_pmf_gamma2_product(const std::array<double, 2> &means,
                                                  const std::array<double, 2> &scales,
                                                  const double quantile) {
  const boost::math::gamma_distribution gamma_dist1(means[0] / scales[0], scales[0]);
  const boost::math::gamma_distribution gamma_dist2(means[1] / scales[1], scales[1]);

  const int upper_bound1 = static_cast<int>(boost::math::quantile(gamma_dist1, quantile));
  const int upper_bound2 = static_cast<int>(boost::math::quantile(gamma_dist2, quantile));
  const int lower_bound1 = static_cast<int>(boost::math::quantile(gamma_dist1, 1 - quantile));
  const int lower_bound2 = static_cast<int>(boost::math::quantile(gamma_dist2, 1 - quantile));
  const int demand_length1 = upper_bound1 - lower_bound1 + 1;
  const int demand_length2 = upper_bound2 - lower_bound2 + 1;

  const double denominator = (cdf(gamma_dist1, upper_bound1) - cdf(gamma_dist1, lower_bound1)) *
                             (cdf(gamma_dist2, upper_bound2) - cdf(gamma_dist2, lower_bound2));

  std::vector<std::array<double, 3>> pmf(demand_length1 * demand_length2);
  int index = 0;
  for (int i = 0; i < demand_length1; i++) {
    for (int j = 0; j < demand_length2; j++) {
      pmf[index][0] = i + lower_bound1;
      pmf[index][1] = j + lower_bound2;
      pmf[index][2] = (cdf(gamma_dist1, pmf[index][0] + 0.5) - cdf(gamma_dist1, std::fmax(pmf[index][0] - 0.5, 0))) *
                  (cdf(gamma_dist2, pmf[index][1] + 0.5) - cdf(gamma_dist2, std::fmax(0, pmf[index][1] - 0.5)));
      pmf[index][2] /= denominator;
      index++;
    }
  }

  return pmf;
}

std::array<std::vector<std::array<double, 2>>, 2> get_pmf_gamma1_products(const std::array<double, 2> &means,
                                                  const std::array<double, 2> &scales, const double quantile) {
  const boost::math::gamma_distribution gamma_dist1(means[0] / scales[0], scales[0]);
  const boost::math::gamma_distribution gamma_dist2(means[1] / scales[1], scales[1]);

  const int upper_bound1 = static_cast<int>(boost::math::quantile(gamma_dist1, quantile));
  const int lower_bound1 = static_cast<int>(boost::math::quantile(gamma_dist1, 1 - quantile));
  const int demand_length1 = upper_bound1 - lower_bound1 + 1;
  const int upper_bound2 = static_cast<int>(boost::math::quantile(gamma_dist2, quantile));
  const int lower_bound2 = static_cast<int>(boost::math::quantile(gamma_dist2, 1 - quantile));
  const int demand_length2 = upper_bound2 - lower_bound2 + 1;

  const double denominator1 = cdf(gamma_dist1, upper_bound1) - cdf(gamma_dist1, lower_bound1);
  const double denominator2 = cdf(gamma_dist2, upper_bound2) - cdf(gamma_dist2, lower_bound2);

  std::array<std::vector<std::array<double, 2>>, 2> pmfs;
  pmfs[0].resize(demand_length1);
  pmfs[1].resize(demand_length2);
  for (int i = 0; i < demand_length1; i++) {
    pmfs[0][i][0] = i + lower_bound1;
    pmfs[0][i][1] = cdf(gamma_dist1, pmfs[0][i][0] + 0.5) - cdf(gamma_dist1, std::fmax(pmfs[0][i][0] - 0.5, 0));
    pmfs[0][i][1] /= denominator1;
  }
  for (int i = 0; i < demand_length2; i++) {
    pmfs[1][i][0] = i + lower_bound2;
    pmfs[1][i][1] = cdf(gamma_dist2, pmfs[1][i][0] + 0.5) - cdf(gamma_dist2, std::fmax(pmfs[1][i][0] - 0.5, 0));
    pmfs[1][i][1] /= denominator2;
  }

  return pmfs;
}
