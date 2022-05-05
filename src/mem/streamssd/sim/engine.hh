/*
 * Copyright (C) 2017 CAMELab
 *
 * This file is part of SimpleSSD.
 *
 * SimpleSSD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleSSD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleSSD.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef __SIM_ENGINE__
#define __SIM_ENGINE__

#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "mem/streamssd/util/stopwatch.hh"
#include "simplessd/sim/simulator.hh"

namespace SimpleSSD {

class Engine : public Simulator {
 private:
  std::mutex mTick;
  uint64_t simTick;
  Event counter;
  bool forceStop;
  std::unordered_map<Event, EventFunction> eventList;
  std::list<std::pair<Event, uint64_t>> eventQueue;

  Stopwatch watch;

  std::mutex m;
  uint64_t eventHandled;

  bool insertEvent(Event, uint64_t, uint64_t * = nullptr);
  bool removeEvent(Event);
  bool isEventExist(Event, uint64_t * = nullptr);

 public:
  Engine();
  ~Engine();

  uint64_t getCurrentTick() override;

  Event allocateEvent(EventFunction) override;
  void scheduleEvent(Event, uint64_t) override;
  void descheduleEvent(Event) override;
  bool isScheduled(Event, uint64_t * = nullptr) override;
  void deallocateEvent(Event) override;

  bool doNextEvent(uint64_t tick);
  void stopEngine();
  void printStats(std::ostream &);
  void getStat(uint64_t &);
};

}  // namespace SimpleSSD

#endif
