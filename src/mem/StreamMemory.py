#
# Copyright 2022 Chen Zou
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os
from m5.params import *
from m5.objects.SimpleMemory import *


class StreamMemory(SimpleMemory):
    type = 'StreamMemory'
    cxx_header = "mem/stream_mem.hh"
    cxx_class = 'gem5::memory::StreamMemory'

    ssd_config_path = Param.String(
        os.path.join(
            # os.path.split(os.path.split(__file__)[0])[0],
            os.getenv('HOME'),
            "gem5",
            "ext/simplessd/simplessd/config/sample.cfg"),
        "SSD config path")
    debug = Param.Bool(False, "Whether to enable debug log")
    latency_log = Param.Bool(True, "Whether to enable latency log")
    nvme = Param.Bool(False, "Use nvme(True) or none(False) interface")
    log_period = Param.Int(10, "ms in simulation time (not real time)")
    wakeup = Param.Latency('10ns', "Periodic wakeup to sync clocks")

    block_bytes = Param.Int(16384, "bytes per io block")
    stream_blocks = Param.Int(4, "block buffer per stream")
