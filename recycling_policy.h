#ifndef RECYCLING_POLICY_H
#define RECYCLING_POLICY_H

#include <random>     /* std::uniform_int_distribution */

class RecyclingPolicy {
 int recycling_rate; /* [0,100] : 0   release every returned chunk (always release),
                                  100    keep every returned chunk (never release)  */
 std::mt19937 gen;
 std::uniform_int_distribution<int> uniform_dist;

public:
  RecyclingPolicy(int _recycling_rate = 0) {
    recycling_rate = std::max(0, std::min(100, _recycling_rate));

    std::random_device seed;
    gen = std::mt19937(seed());
    uniform_dist = std::uniform_int_distribution<int>(1, 99);
  }

  bool should_recycle(int /*size*/) {
    return uniform_dist(gen) < recycling_rate;
  }

  void set_recycling_params(int rate) {
    recycling_rate = std::max(0, std::min(100, rate));
  }

  int get_recycling_params() {
    return recycling_rate;
  }
};

#endif // RECYCLING_POLICY_H
