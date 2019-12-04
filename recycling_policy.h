/* Copyright 2019-present Daniel Vergeylen daniel@vergeylen.eu
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RECYCLING_POLICY_H
#define RECYCLING_POLICY_H

/* This RecyclingPolicy class is an example of
 * what can be used to help the Memory Chunk Store
 * to take recycling policy decisions.
 * Any RecyclingPolicy class you may write should implement
 * the following functions:
 * - bool should_recycle(int size)
 * - void set_recycling_params(RArgs args)
 * - RArgs get_recycling_params()
 *
 * The example below instanciates a uniform distribution
 * and compares every round result to a recycling policy
 * rate, between 0 and 100.
 *
 * A recycling_rate of 0 means never recycle any chunk
 * given back, while a recycling_rate of 100 means recycle
 * every chunk given back (and never release, which might
 * be problematic at some point). These are extreme value
 * but any in between threshold is valid [0:100].
 */

#include <random> /* std::uniform_int_distribution */

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
