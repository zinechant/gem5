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
 * Block stream declaration
 *
 * Managing streams and block availability of each stream.
 */

#ifndef __MEM_STREAMSSD_PAGE_STREAMS_HH__
#define __MEM_STREAMSSD_PAGE_STREAMS_HH__

#include <cstdint>
#include <map>
#include <mutex>
#include <unordered_set>

namespace gem5 {

namespace memory {

class BlockStreams {
 private:
  const uint32_t stream_blocks;
  std::mutex m;

  struct BlockStream {
   public:
    uint64_t head;
    uint64_t avai;
    uint64_t tail;
    std::unordered_set<uint64_t> avail;
    BlockStream(uint64_t addr) { head = avai = tail = addr; }
  };
  typedef std::map<uint64_t, BlockStream> streams_t;

 public:
  BlockStreams(uint32_t blocks) : stream_blocks(blocks) {}
  typedef streams_t::iterator stream_it;
  // offset, length
  typedef std::pair<uint8_t, uint8_t> request_t;

  std::pair<bool, request_t> check(uint64_t block_addr);
  request_t block_fetched(uint64_t block_addr);

 private:
  streams_t streams;
  stream_it search(uint64_t block_addr);
  request_t request(stream_it it, uint64_t block_addr);
};

}  // namespace memory

}  // namespace gem5

#endif  // __MEM_STREAMSSD_PAGE_STREAMS_HH__