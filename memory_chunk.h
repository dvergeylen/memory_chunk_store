
/* This Memory Chunk Store creates, stores and deletes
 * chunks of memory, according to T type.
 * When asked for a new chunk (via pull_chunk()), two
 * cases can occur:
 *
 * 1) Either there is at least one chunk in the store.
 * Chunk Store pops the chunk from the queue and instanciates
 * it with the TArgs arguments given to pull_chunk().
 *
 * 2) Either there is no chunk available in the store.
 * Chunk Store then instanciate a new T via 'new' command
 * and the TArgs arguments given to pull_chunk().
 *
 * When given back a memory chunk via (via push_chunk()),
 * Chunk Store will ask R policy (via policy.should_recycle())
 * if this chunk should be stored or released.
 * Policy receives the current number of already available chunks
 * to help making a decision.
 *
 * Extreme decision patterns could be never recycle (always release)
 * or always recycle (never release). In between situations
 * may occur according to a custom recycling policy.
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
