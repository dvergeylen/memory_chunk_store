template <typename T, typename... Args>
T* MemoryChunkStore<T, Args...>::pull_chunk(Args... args) {
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

template <typename T, typename... Args>
void MemoryChunkStore<T, Args...>::push_chunk(T* chunk) {

  /* Release or not, according to recycling_rate */
  if (uniform_dist(gen) < recycling_rate) {
    std::lock_guard<std::mutex> guard(available_chunks_mtx);
    available_chunks.push(chunk);
  } else {
    delete chunk;
  }
}

template <typename T, typename... Args>
bool MemoryChunkStore<T, Args...>::is_empty(){
  return available_chunks.empty();
}

template <typename T, typename... Args>
int MemoryChunkStore<T, Args...>::size(){
  return available_chunks.size();
}

template <typename T, typename... Args>
void MemoryChunkStore<T, Args...>::set_recycling_rate(int rate){
  std::lock_guard<std::mutex> guard(available_chunks_mtx);
  recycling_rate = std::max(0, std::min(100, rate));
}

template <typename T, typename... Args>
int MemoryChunkStore<T, Args...>::get_recycling_rate(){
  return recycling_rate;
}

template <typename T, typename... Args>
void MemoryChunkStore<T, Args...>::empty_memory_chunks_queue(){
  T* chunk;
  std::lock_guard<std::mutex> guard(available_chunks_mtx);

  while(!available_chunks.empty()) {
    chunk = available_chunks.front();
    available_chunks.pop();
    delete chunk;
  }
}
