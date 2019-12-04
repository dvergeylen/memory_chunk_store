/* This class is the Memory chunks store.
 * Once a thread needs a task memory chunk,
 * it simply asks the class for one.
 * If the class does have one available, it gives
 * the thread the chunk and removes it from the available ones.
 * If no memory chunks are available, the store creates a new one
 * and gives it to the asking thread.
 *
 * When a thread finishes using it, it gives it back to the
 * memory chunk store. The memory chunk store decides
 * if it has to be kept or deleted according
 * to (configurable) heuristics.
 */
#ifndef MEMORY_CHUNK_H
#define MEMORY_CHUNK_H

#include <mutex>
#include <queue>
#include <random>     /* std::uniform_int_distribution */
#include <algorithm>  /* std::max                      */

template <typename T, typename... Args>
class MemoryChunkStore {
  std::queue<T*> available_chunks;
  std::mutex available_chunks_mtx;
  int recycling_rate; /* [0,100] : 0   release every returned chunk (always release),
                                   100    keep every returned chunk (never release)  */
  std::mt19937 gen;
  std::uniform_int_distribution<int> uniform_dist;

public:
  MemoryChunkStore(int _recycling_rate = 0) {
    recycling_rate = std::max(0, std::min(100, _recycling_rate));

    std::random_device seed;
    gen = std::mt19937(seed());
    uniform_dist = std::uniform_int_distribution<int>(1, 100);
  }

  ~MemoryChunkStore() {
    empty_memory_chunks_queue();
  }
  T* pull_chunk(Args... args); /* A T ptr will be returned with T constructor called with args */
  void push_chunk(T* chunk);
  bool is_empty();
  int size();
  int get_recycling_rate();
  void set_recycling_rate(int rate);
  void empty_memory_chunks_queue();
};

#include "memory_chunk.tcc"
#endif // MEMORY_CHUNK_H
