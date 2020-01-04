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
 * The example below instanciates a counter that decrements
 * at every round. When counter is below a threshold,
 * recycle, release otherwise.
 *
 */
struct recycling_options {
  int init_counter;
  int threshold;
};

class RecyclingPolicy {
  int init_counter;
  int counter;
  int threshold;
public:
  RecyclingPolicy(struct recycling_options options) {
    init_counter = options.init_counter;
    threshold = options.threshold;
    counter = init_counter;
  }

  bool should_recycle(int /*size*/) {
    bool res = (counter < threshold);
    counter--;

    if (counter < 0)
      counter = init_counter - 1;

    return res;
  }

  void set_recycling_params(int _init_counter, int _threshold) {
    init_counter = _init_counter;
    threshold = _threshold;
    counter = init_counter;
  }

  int get_recycling_params() {
    return threshold;
  }
};

#endif // RECYCLING_POLICY_H
