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
#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../submodules/doctest/doctest/doctest.h"

#include "../recycling_policy.h"
#include "../memory_chunk.h"

MemoryChunkStore<RecyclingPolicy, int, int, int> store(0); /* 0: → always release (no chunk recycling) */
int* elements[300];

TEST_SUITE("Unit Tests") {
  TEST_CASE("Empty Queue"
            * doctest::description("should release all stored chunks")) {
    store.empty_memory_chunks_queue();
    REQUIRE(store.size() == 0);

    store.set_recycling_params(100);

    /* Asking for chunks */
    for (int i=0; i<5; i++) {
      elements[i] = store.pull_chunk(i);
    }

    /* Releasing */
    for (int i=0; i<5; i++) {
      store.push_chunk(elements[i]);
    }

    REQUIRE(store.size() == 5);
    store.empty_memory_chunks_queue();
    REQUIRE(store.size() == 0);
  }

  TEST_CASE("Recycling 0"
            * doctest::description("should release ALL pushed chunks")) {
    store.empty_memory_chunks_queue();
    REQUIRE(store.size() == 0);

    store.set_recycling_params(0);

    /* Asking for chunks */
    for (int i=0; i<5; i++) {
      elements[i] = store.pull_chunk(i);
    }

    /* Releasing */
    for (int i=0; i<5; i++) {
      store.push_chunk(elements[i]);
    }

    REQUIRE(store.size() == 0);
  }

  TEST_CASE("Recycling 100"
            * doctest::description("should keep ALL pushed chunks")) {
    store.empty_memory_chunks_queue();
    REQUIRE(store.size() == 0);

    store.set_recycling_params(100);

    /* Asking for chunks */
    for (int i=0; i<5; i++) {
      elements[i] = store.pull_chunk(i);
    }

    /* Releasing */
    for (int i=0; i<5; i++) {
      store.push_chunk(elements[i]);
    }

    REQUIRE(store.size() == 5);
  }

  TEST_CASE("Recycling 70"
            * doctest::description("should keep part of pushed chunks")) {
    store.empty_memory_chunks_queue();
    REQUIRE(store.size() == 0);

    store.set_recycling_params(70);

    /* Asking for chunks */
    for (int i=0; i<300; i++) {
      elements[i] = store.pull_chunk(i);
    }

    /* Releasing */
    for (int i=0; i<300; i++) {
      store.push_chunk(elements[i]);
    }

    REQUIRE(store.size() > (300*0.55));
    REQUIRE(store.size() < (300*0.85));
  }
}
