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
#include <algorithm>  /* std::max */

template <typename T, typename R, typename RArg, typename... TArgs >
class MemoryChunkStore {
  std::queue<T*> available_chunks;
  std::mutex available_chunks_mtx;
  R* policy;
public:
  MemoryChunkStore(RArg rarg) {
    policy = new R(rarg);
  }

  ~MemoryChunkStore() {
    empty_memory_chunks_queue();
    delete policy;
  }
  T* pull_chunk(TArgs... args); /* A T ptr will be returned with T constructor called with args */
  void push_chunk(T* chunk);
  bool is_empty();
  int size();
  RArg get_recycling_params();
  void set_recycling_params(RArg params);
  void empty_memory_chunks_queue();
};

#include "memory_chunk.tcc"
#endif // MEMORY_CHUNK_H
