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
 * StreamMemory implementation
 *
 * Stream Memory extends from Simple mem and contains simplessd modeling for
 * streaming access into storage data.
 *
 * Each StreamMem is backed by one simple ssd.
 * Cores can share a StreamMem (and thus one ssd) through non-concurrent bars.
 */

#include "mem/stream_mem.hh"

#include <cassert>

#include "base/trace.hh"
#include "debug/StreamMemory.hh"

namespace gem5 {

namespace memory {

StreamMemory::StreamMemory(const StreamMemoryParams& p)
    : SimpleMemory(p),
      ssd(p.ssd_config_path, p.debug, p.latency_log, p.nvme, p.log_period),
      streams(p.stream_blocks),
      block_bytes(p.block_bytes),
      failed(false),
      wakeup(p.wakeup),
      sync_event([this] { sync(); }, "periodic sync event"),
      bioid(42) {
  callback = [this](uint64_t id) { complete(id); };
}

void StreamMemory::sync() {
  ssd.sync(curTick());
  if (outstanding.size()) {
    schedule(sync_event, curTick() + wakeup);
  }
  DPRINTF(StreamMemory, "sync %x, failed=%d\n", curTick(), failed);
  if (failed) {
    failed = false;
    gem5::ResponsePort* port =
        reinterpret_cast<gem5::ResponsePort*>(&getPort("port"));
    port->sendRetryReq();
  }
}

void StreamMemory::request(bool rnw, uint64_t block,
                           const BlockStreams::request_t& req) {
  if (!req.second) return;
  if (!outstanding.size()) {
    schedule(sync_event, curTick() + wakeup);
  }
  ssd.sync(curTick());
  DPRINTF(StreamMemory, "request rnw=%d block=%x (%x, %x)\n", rnw, block,
          req.first, req.second);
  block += req.first;
  for (int i = 0; i < req.second; i++) {
    ssd.submit(bioid, rnw, (block + i) * block_bytes, block_bytes, callback);
    outstanding[bioid++] = {rnw, block + i};
  }
}

bool StreamMemory::avail(bool rnw, uint64_t offset, unsigned length) {
  uint64_t block1 = offset / block_bytes;
  uint64_t block2 = (offset + length - 1) / block_bytes;

  auto p = streams.check(block1);
  if (p.second.second) request(rnw, block1, p.second);
  if (!p.first) return false;
  if (block2 == block1) return true;

  assert(block2 == block1 + 1);
  auto q = streams.check(block2);
  if (q.second.second) request(rnw, block2, q.second);
  if (!q.first) return false;
  return true;
}

void StreamMemory::complete(uint64_t id) {
  assert(outstanding.count(id));
  const auto& p = outstanding[id];
  DPRINTF(StreamMemory, "complete id=%x rnw=%d addr=%x\n", id, p.first,
          p.second);
  request(p.first, p.second, streams.block_fetched(p.second));
  outstanding.erase(id);
}

bool StreamMemory::recvTimingReq(PacketPtr pkt) {
  if ((pkt->isRead() || pkt->isWrite())) {
    assert(!(pkt->isRead() && pkt->isWrite()));
    bool ok = avail(pkt->isRead(), pkt->getAddr(), pkt->getSize());
    DPRINTF(StreamMemory, "ok=%d, read=%d, addr=%x, size=%x\n", ok,
            pkt->isRead(), pkt->getAddr(), pkt->getSize());
    if (!ok) {
      failed = true;
      return false;
    }
  }
  return SimpleMemory::recvTimingReq(pkt);
}

}  // namespace memory

}  // namespace gem5
