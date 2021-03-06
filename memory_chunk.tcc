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
template <typename R, typename RArg, typename T, typename... TArgs >
T* MemoryChunkStore<R, RArg, T, TArgs...>::pull_chunk(TArgs... args) {
  T* ref;

  if (!available_chunks.empty()) {
    available_chunks_mtx.lock();

    /* Still at least one available, dequeue */
    if (!available_chunks.empty()) {
      ref = available_chunks.front();
      available_chunks.pop();
      available_chunks_mtx.unlock();

      /* Reset object */
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

template <typename R, typename RArg, typename T, typename... TArgs >
void MemoryChunkStore<R, RArg, T, TArgs...>::push_chunk(T* chunk) {
  available_chunks_mtx.lock();

  /* Release or not, according to recycling_rate */
  if (policy->should_recycle(available_chunks.size())) {
    chunk->~T();
    available_chunks.push(chunk);
    available_chunks_mtx.unlock();
  } else {
    available_chunks_mtx.unlock();
    delete chunk;
  }
}

template <typename R, typename RArg, typename T, typename... TArgs >
bool MemoryChunkStore<R, RArg, T, TArgs...>::is_empty(){
  return available_chunks.empty();
}

template <typename R, typename RArg, typename T, typename... TArgs >
int MemoryChunkStore<R, RArg, T, TArgs...>::size(){
  return available_chunks.size();
}

template <typename R, typename RArg, typename T, typename... TArgs >
void MemoryChunkStore<R, RArg, T, TArgs...>::set_recycling_params(RArg params) {
  std::lock_guard<std::mutex> guard(available_chunks_mtx);
  policy->set_recycling_params(params);
}

template <typename R, typename RArg, typename T, typename... TArgs >
RArg MemoryChunkStore<R, RArg, T, TArgs...>::get_recycling_params(){
  std::lock_guard<std::mutex> guard(available_chunks_mtx);
  return policy->get_recycling_params();
}

template <typename R, typename RArg, typename T, typename... TArgs >
void MemoryChunkStore<R, RArg, T, TArgs...>::empty_memory_chunks_queue(){
  T* chunk;
  std::lock_guard<std::mutex> guard(available_chunks_mtx);

  while(!available_chunks.empty()) {
    chunk = available_chunks.front();
    available_chunks.pop();
    delete chunk;
  }
}
