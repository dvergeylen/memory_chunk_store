# Memory Chunk Store
A Cpp implementation of a memory pool that stores/releases chunks according to a recycling policy.

#### How to use this?
Include the `memory_chunk.h` file. You must provide a recycling policy class to help the Store making decision. You can use a default one available in `recycling_policy.h`.

```c++
#include "recycling_policy.h"
#include "memory_chunk.h"

/* <typename R, typename RArg, typename T, typename... TArgs >
 * R: RecyClingPolicy or whatever class you defined to make recycling decisions
 * RArg: type of parameters needed to instanciate RecylingPolicy (can be a singleton or a struct for instance)
 * T: Memory chunk type
 * TArgs: parameters needed to instanciate a new T
 */
MemoryChunkStore<RecyclingPolicy, int, int, int> store(0); /* 0: → always release (no chunk recycling) */

[...]
/* Ask a new int memory chunk */
int* my_int = store.pull_chunk(10);

[...]
/* Give the chunk back to store.
 * According to policy, will keep it or not.
 */
store.push_chunk(my_int);
```

#### RAII and thread safe
- This is valgrind clean.
- This is thread safe.

#### Custom RecyclingPolicy class
You should write your own Recycling Policy class. The one given in `recycling_policy.h` is only an example. As you receive the number of currently stored chunk, one could elaborate a policy based on this parameter:

```c++
#ifndef RECYCLING_POLICY_H
#define RECYCLING_POLICY_H
#include <random> /* std::uniform_int_distribution */

class RecyclingPolicy {
 std::mt19937 gen;
 std::uniform_int_distribution<int> uniform_dist;

public:
  RecyclingPolicy() {
    std::random_device seed;
    gen = std::mt19937(seed());
    uniform_dist = std::uniform_int_distribution<int>(1, 99);
  }

  bool should_recycle(int size) {
    if (size < 100)
      return true;                    // always store
    if (size < 200)
      return uniform_dist(gen) < 75;  // 75% recycling rate
    if (size < 1000)
      return uniform_dist(gen) < 50;  // 50% recycling rate

    return false;                     // already enough in store, never recycle
  }

  void set_recycling_params() {
  }

  int get_recycling_params() {
    return null;
  }
};

#endif // RECYCLING_POLICY_H
```
How neat is that? 🏄
