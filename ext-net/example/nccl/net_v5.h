/*
 * Copyright (c) 2017-2022, NVIDIA CORPORATION. All rights reserved.
 */

#ifndef NET_V5_H_
#define NET_V5_H_

typedef ncclNetProperties_v6_t ncclNetProperties_v5_t;
typedef struct {
  // Name of the network (mainly for logs)
  const char* name;
  // Initialize the network.
  ncclResult_t (*init)(ncclDebugLogger_t logFunction);
  // Return the number of adapters.
  ncclResult_t (*devices)(int* ndev);
  // Get various device properties.
  ncclResult_t (*getProperties)(int dev, ncclNetProperties_v5_t* props);
  // Create a receiving object and provide a handle to connect to it. The
  // handle can be up to NCCL_NET_HANDLE_MAXSIZE bytes and will be exchanged
  // between ranks to create a connection.
  ncclResult_t (*listen)(int dev, void* handle, void** listenComm);
  // Connect to a handle and return a sending comm object for that peer.
  // This call must not block for the connection to be established, and instead
  // should return successfully with sendComm == NULL with the expectation that
  // it will be called again until sendComm != NULL.
  ncclResult_t (*connect)(int dev, void* handle, void** sendComm);
  // Finalize connection establishment after remote peer has called connect.
  // This call must not block for the connection to be established, and instead
  // should return successfully with recvComm == NULL with the expectation that
  // it will be called again until recvComm != NULL.
  ncclResult_t (*accept)(void* listenComm, void** recvComm);
  // Register/Deregister memory. Comm can be either a sendComm or a recvComm.
  // Type is either NCCL_PTR_HOST or NCCL_PTR_CUDA.
  ncclResult_t (*regMr)(void* comm, void* data, int size, int type, void** mhandle);
  ncclResult_t (*deregMr)(void* comm, void* mhandle);
  // Asynchronous send to a peer.
  // May return request == NULL if the call cannot be performed (or would block)
  ncclResult_t (*isend)(void* sendComm, void* data, int size, int tag, void* mhandle, void** request);
  // Asynchronous recv from a peer.
  // May return request == NULL if the call cannot be performed (or would block)
  ncclResult_t (*irecv)(void* recvComm, int n, void** data, int* sizes, int* tags, void** mhandles, void** request);
  // Perform a flush/fence to make sure all data received with NCCL_PTR_CUDA is
  // visible to the GPU
  ncclResult_t (*iflush)(void* recvComm, int n, void** data, int* sizes, void** mhandles, void** request);
  // Test whether a request is complete. If size is not NULL, it returns the
  // number of bytes sent/received.
  ncclResult_t (*test)(void* request, int* done, int* sizes);
  // Close and free send/recv comm objects
  ncclResult_t (*closeSend)(void* sendComm);
  ncclResult_t (*closeRecv)(void* recvComm);
  ncclResult_t (*closeListen)(void* listenComm);
} ncclNet_v5_t;

#endif // end include guard
