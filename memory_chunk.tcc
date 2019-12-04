template <typename T, typename R, typename RArg, typename... TArgs >
T* MemoryChunkStore<T, R, RArg, TArgs...>::pull_chunk(TArgs... args) {
  T* ref;

  if (!available_chunks.empty()) {
    available_chunks_mtx.lock();

    /* Still at least one available, dequeue */
    if (!available_chunks.empty()) {
      ref = available_chunks.front();
      available_chunks.pop();
      available_chunks_mtx.unlock();

      /* Reset object */
      ref->~T();
      return new (ref) T(args...);

    /* No more chunk available, create new one! */
    } else {
      available_chunks_mtx.unlock();

      try {
        return new T(args...);
      } catch  (const std::bad_alloc& e) {
        return nullptr; /* out-of-memory */
      }
    }

  /* No more chunk available, create new one! */
  } else {
    try {
      return new T(args...);
    } catch  (const std::bad_alloc& e) {
      return nullptr; /* out-of-memory */
    }
  }
}

template <typename T, typename R, typename RArg, typename... TArgs >
void MemoryChunkStore<T, R, RArg, TArgs...>::push_chunk(T* chunk) {
  available_chunks_mtx.lock();

  /* Release or not, according to recycling_rate */
  if (policy->should_recycle(available_chunks.size())) {
    available_chunks.push(chunk);
    available_chunks_mtx.unlock();
  } else {
    available_chunks_mtx.unlock();
    delete chunk;
  }
}

template <typename T, typename R, typename RArg, typename... TArgs >
bool MemoryChunkStore<T, R, RArg, TArgs...>::is_empty(){
  return available_chunks.empty();
}

template <typename T, typename R, typename RArg, typename... TArgs >
int MemoryChunkStore<T, R, RArg, TArgs...>::size(){
  return available_chunks.size();
}

template <typename T, typename R, typename RArg, typename... TArgs >
void MemoryChunkStore<T, R, RArg, TArgs...>::set_recycling_params(RArg params) {
  std::lock_guard<std::mutex> guard(available_chunks_mtx);
  policy->set_recycling_params(params);
}

template <typename T, typename R, typename RArg, typename... TArgs >
RArg MemoryChunkStore<T, R, RArg, TArgs...>::get_recycling_params(){
  std::lock_guard<std::mutex> guard(available_chunks_mtx);
  return policy->get_recycling_params();
}

template <typename T, typename R, typename RArg, typename... TArgs >
void MemoryChunkStore<T, R, RArg, TArgs...>::empty_memory_chunks_queue(){
  T* chunk;
  std::lock_guard<std::mutex> guard(available_chunks_mtx);

  while(!available_chunks.empty()) {
    chunk = available_chunks.front();
    available_chunks.pop();
    delete chunk;
  }
}
