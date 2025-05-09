/*************************************************************************
 * Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
 * Copyright (c) 2023, Meta Platforms, Inc. and affiliates.
 *
 * See LICENSE.txt for license information
 ************************************************************************/

#ifndef NCCL_TUNER_H_
#define NCCL_TUNER_H_

#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "err.h"

#define NCCL_NUM_FUNCTIONS 5 // Send/Recv not included for now
typedef enum {
  ncclFuncBroadcast = 0,
  ncclFuncReduce = 1,
  ncclFuncAllGather = 2,
  ncclFuncReduceScatter = 3,
  ncclFuncAllReduce = 4,
  ncclFuncSendRecv = 5,
  ncclFuncSend = 6,
  ncclFuncRecv = 7,
  ncclNumFuncs = 8
} ncclFunc_t;

#define NCCL_NUM_ALGORITHMS 7 // Tree/Ring/CollNet*
#define NCCL_ALGO_UNDEF -1
#define NCCL_ALGO_TREE 0
#define NCCL_ALGO_RING 1
#define NCCL_ALGO_COLLNET_DIRECT 2
#define NCCL_ALGO_COLLNET_CHAIN 3
#define NCCL_ALGO_NVLS 4
#define NCCL_ALGO_NVLS_TREE 5
#define NCCL_ALGO_PAT 6

#define NCCL_NUM_PROTOCOLS 3 // Simple/LL/LL128
#define NCCL_PROTO_UNDEF -1
#define NCCL_PROTO_LL 0
#define NCCL_PROTO_LL128 1
#define NCCL_PROTO_SIMPLE 2

#define NCCL_ALGO_PROTO_IGNORE -1.0

// API to be implemented by external tuner
typedef struct {
  // Name of the tuner
  const char* name;

  // Initializes tuner states.
  // Inputs:
  //   - nRanks: number of ranks in current communicator. Each communicator initialize its own tuner.
  //   - nNodes: number of nodes in current communicator.
  //   - logFunction: a logFunction can be useful to integrate logging together with NCCL core.
  // Outputs:
  //   - context: tuner context object
  ncclResult_t (*init)(size_t nRanks, size_t nNodes, ncclDebugLogger_t logFunction, void **context);

  // Gets info (algo, protocol, number of ctas and threads) for a given collective.
  // Inputs:
  //   - context: tuner context object
  //   - collType: collective type , e.g., allreduce, allgather…
  //   - nBytes: collective size in bytes
  //   - numPipeOps: number of operations in the group
  //   - numAlgo: number of algorithms in collCostTable
  //   - numProto: number of protocols in collCostTable
  //   - regBuff: can register user buffer
  //
  // Outputs:
  //   - nChannels: number of channels (hence SMs) to be used.
  //
  // InOut:
  //   - collCostTable: collective cost table, generated by NCCL core, containing algo|proto|time entries for collType.
  //                    NCCL core sets ignored algo/proto cost table entries to -1.0 (NCCL_ALGO_PROTO_IGNORE).
  //
  // If getCollInfo() does not return ncclSuccess, NCCL will fall back to the
  // default tuning for the given collective.
  // Also, the plugin is allowed to not set any output, or set only the
  // algorithm and protocol, but not only the algorithm or only the protocol.
  // Unset fields will be set automatically by NCCL.
  ncclResult_t (*getCollInfo)(void* context, ncclFunc_t collType, size_t nBytes,
                              int numPipeOps, float** collCostTable, int numAlgo, int numProto,
                              int regBuff, int* nChannels);

  // Terminates the plugin and cleans up any resources that the plugin allocated.
  // context: tuner context object
  ncclResult_t (*destroy)(void* context);
} ncclTuner_v4_t;

typedef ncclTuner_v4_t ncclTuner_t;

#define NCCL_TUNER_PLUGIN_SYMBOL "ncclTunerPlugin_v4"

#endif
