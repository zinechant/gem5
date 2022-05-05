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

#include <string>

#include "mem/streamssd/bil/entry.hh"
#include "mem/streamssd/bil/interface.hh"
#include "mem/streamssd/sim/engine.hh"

#ifndef __MEM_STREAMSSD_STREAM_SSD_HH__
#define __MEM_STREAMSSD_STREAM_SSD_HH__

namespace gem5 {
namespace memory {

class StreamSSD {
 private:
  inline static int ssdid = 0;
  SimpleSSD::Engine engine;
  SimpleSSD::ConfigReader ssdConfig;
  std::unique_ptr<BIL::DriverInterface> pInterface;
  std::unique_ptr<BIL::BlockIOEntry> pBIOEntry;
  std::vector<SimpleSSD::Stats> statList;
  SimpleSSD::Event statEvent;
  std::ofstream logOut;
  std::ofstream debugLogOut;
  std::ofstream latencyFile;
  void statistics(uint64_t);

 public:
  StreamSSD(const std::string &ssdConfigPath, bool debug, bool logLatency,
            bool nvme, int logPeriod);
  ~StreamSSD();

  void sync(uint64_t tick);

  void submit(uint64_t id, bool rnw, uint64_t offset, uint64_t length,
              std::function<void(uint64_t)> callback);
  StreamSSD(StreamSSD const &) = delete;
  void operator=(StreamSSD const &) = delete;
};

}  // namespace memory
}  // namespace gem5

#endif  // __MEM_STREAMSSD_STREAM_SSD_HH__