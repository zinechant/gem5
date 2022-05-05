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

#include "mem/streamssd/stream_ssd.hh"

#include "base/output.hh"
#include "base/trace.hh"
#include "debug/StreamMemory.hh"
#include "mem/streamssd/sil/none/none.hh"
#include "mem/streamssd/sil/nvme/nvme.hh"
#include "mem/streamssd/util/print.hh"

namespace gem5 {
namespace memory {

StreamSSD::StreamSSD(const std::string& ssdConfigPath, bool debug,
                     bool latencyLog, bool nvme, int logPeriod) {
  std::string sid = std::to_string(ssdid++);
  if (logPeriod) {
    logOut.open(simout.resolve("simplessd_" + sid + ".log"));
  }
  if (debug) {
    debugLogOut.open(simout.resolve("simplessd_" + sid + ".debug"));
  }
  if (latencyLog) {
    latencyFile.open(simout.resolve("simplessd_" + sid + ".latency"));
  }

  std::ostream* pDebug = debug ? &debugLogOut : nullptr;
  std::ostream* pLatencyFile = latencyLog ? &latencyFile : nullptr;

  // Initialize SimpleSSD
  ssdConfig = initSimpleSSDEngine(&engine, pDebug, pDebug, ssdConfigPath);
  if (nvme) {
    pInterface = std::make_unique<SIL::NVMe::Driver>(engine, ssdConfig);
  } else {
    pInterface = std::make_unique<SIL::None::Driver>(engine, ssdConfig);
  }

  // Create Block I/O Layer
  pBIOEntry = std::make_unique<BIL::BlockIOEntry>(engine, pInterface.get(),
                                                  pLatencyFile);

  // Insert stat event
  pInterface->initStats(statList);

  if (logPeriod) {
    statEvent = engine.allocateEvent([&](uint64_t tick) {
      statistics(tick);

      engine.scheduleEvent(statEvent, tick + logPeriod * 1000000000ULL);
    });
    engine.scheduleEvent(statEvent, logPeriod * 1000000000ULL);
  }
}

StreamSSD::~StreamSSD() {
  engine.stopEngine();
  if (logOut.is_open()) {
    engine.descheduleEvent(statEvent);
  }

  uint64_t tick = engine.getCurrentTick();

  if (tick == 0) {
    // Exit program
    exit(0);
  }

  // Print last statistics
  statistics(tick);

  // Erase progress
  printf("\33[2K                                                           \r");

  releaseSimpleSSDEngine();

  pBIOEntry->printStats(std::cout);
  engine.printStats(std::cout);

  if (logOut.is_open()) {
    logOut.close();
  }
  if (debugLogOut.is_open()) {
    debugLogOut.close();
  }

  std::cout << "End of SSD simulation @ tick " << tick << std::endl;
}

void StreamSSD::sync(uint64_t tick) {
  DPRINTF(StreamMemory, "StreamSSD::sync %x\n", tick);
  while (engine.doNextEvent(tick)) {
  }
}

void StreamSSD::submit(uint64_t id, bool rnw, uint64_t offset, uint64_t length,
                       std::function<void(uint64_t)> callback) {
  BIL::BIO bio;

  bio.id = id;
  bio.type = rnw ? BIL::BIO_READ : BIL::BIO_WRITE;
  bio.offset = offset;
  bio.length = length;

  bio.callback = callback;
  // Submit to Block I/O entry
  pBIOEntry->submitIO(bio);
}

void StreamSSD::statistics(uint64_t tick) {
  if (!logOut.is_open()) return;

  std::vector<double> stat;
  uint64_t count = 0;

  pInterface->getStats(stat);

  count = statList.size();

  if (count != stat.size()) {
    std::cerr << " Stat list length mismatch" << std::endl;

    std::terminate();
  }

  logOut << "Periodic log printout @ tick " << tick << std::endl;

  for (uint64_t i = 0; i < count; i++) {
    print(logOut, statList[i].name, 40);
    logOut << "\t";
    print(logOut, stat[i], 20);
    logOut << "\t" << statList[i].desc << std::endl;
  }

  logOut << "End of log @ tick " << tick << std::endl;
};

}  // namespace memory
}  // namespace gem5