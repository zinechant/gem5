/*
 * Copyright 2022 Chen Zou
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * Block stream implementation
 *
 * Managing streams and block availability of each stream.
 */

#include "mem/streamssd/block_stream.hh"

#include <algorithm>
#include <cassert>

namespace gem5 {
namespace memory {

BlockStreams::stream_it BlockStreams::search(uint64_t block_addr) {
  std::unique_lock<std::mutex> lock(m);
  if (streams.empty()) return streams.end();

  auto it = std::prev(streams.upper_bound(block_addr));
  assert(std::next(it) == streams.end() ||
         std::next(it)->first >= it->second.tail);

  if (it->first > block_addr || it->second.tail <= block_addr) {
    return streams.end();
  }
  return it;
}

BlockStreams::request_t BlockStreams::request(stream_it it,
                                              uint64_t block_addr) {
  uint64_t limit = it->second.head + stream_blocks;
  auto nit = std::next(it);
  if (nit != streams.end()) {
    limit = std::min(limit, nit->first);
  }

  assert(it->second.tail - block_addr < (1 << 8));
  assert(limit - it->second.tail < (1 << 8));

  uint8_t offset = it->second.tail - block_addr;
  uint8_t len = limit - it->second.tail;
  it->second.tail = limit;

  return {offset, len};
}

// 0 ready. -1 not ready. >0 submit request for blocks
std::pair<bool, BlockStreams::request_t> BlockStreams::check(
    uint64_t block_addr) {

  auto it = search(block_addr);

  if (it == streams.end()) {
    std::unique_lock<std::mutex> lock(m);
    streams.emplace(block_addr, block_addr);
    it = streams.find(block_addr);
  }

  if (block_addr >= it->second.avai) {
    return {false, request(it, block_addr)};
  }

  assert(it->second.head <= block_addr);
  if (block_addr > it->second.head) {
    assert(block_addr == it->second.head + 1);
    it->second.head++;
  }


  return {true, request(it, block_addr)};
}

// block_offset, num_blocks
BlockStreams::request_t BlockStreams::block_fetched(uint64_t block_addr) {
  auto it = search(block_addr);
  assert(it != streams.end());

  assert(block_addr < it->second.tail && block_addr >= it->second.avai);
  assert(!it->second.avail.count(block_addr));
  it->second.avail.insert(block_addr);

  for (int i = it->second.avai; it->second.avail.count(i); i++) {
    it->second.avai++;
    it->second.avail.erase(i);
  }

  return request(it, block_addr);
}

}  // namespace memory
}  // namespace gem5