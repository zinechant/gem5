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
 * StreamMemory declaration
 *
 * Stream Memory extends from Simple mem and contains simplessd modeling for
 * streaming access into storage data.
 *
 * Each StreamMem is backed by one simple ssd.
 * Cores can share a StreamMem (and thus one ssd) through non-concurrent bars.
 */

#ifndef __MEM_STREAM_MEMORY_HH__
#define __MEM_STREAM_MEMORY_HH__

#include <cstdint>
#include <unordered_map>

#include "mem/simple_mem.hh"
#include "mem/streamssd/stream_ssd.hh"
#include "params/StreamMemory.hh"
#include "streamssd/block_stream.hh"

namespace gem5 {

namespace memory {

class StreamMemory : public SimpleMemory {
 private:
  StreamSSD ssd;
  BlockStreams streams;

  const uint32_t block_bytes;
  bool failed;
  const Tick wakeup;
  EventFunctionWrapper sync_event;

  uint64_t bioid;
  std::unordered_map<uint64_t, std::pair<bool, uint64_t>> outstanding;

  std::function<void(uint64_t)> callback;

  bool avail(bool rnw, uint64_t offset, unsigned length);
  void request(bool rnw, uint64_t block, const BlockStreams::request_t& req);
  void sync();
  void complete(uint64_t id);

 public:
  StreamMemory(const StreamMemoryParams& p);
  bool recvTimingReq(PacketPtr pkt) override;
};

}  // namespace memory

}  // namespace gem5

#endif  // __MEM_STREAM_MEMORY_HH__
