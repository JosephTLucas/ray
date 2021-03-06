// Copyright 2017 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RAY_RPC_CORE_WORKER_SERVER_H
#define RAY_RPC_CORE_WORKER_SERVER_H

#include "ray/rpc/grpc_server.h"
#include "ray/rpc/server_call.h"
#include "src/ray/protobuf/core_worker.grpc.pb.h"
#include "src/ray/protobuf/core_worker.pb.h"

namespace ray {

class CoreWorker;

namespace rpc {

/// NOTE: See src/ray/core_worker/core_worker.h on how to add a new grpc handler.
#define RAY_CORE_WORKER_RPC_HANDLERS                                     \
  RPC_SERVICE_HANDLER(CoreWorkerService, AssignTask)                     \
  RPC_SERVICE_HANDLER(CoreWorkerService, PushTask)                       \
  RPC_SERVICE_HANDLER(CoreWorkerService, DirectActorCallArgWaitComplete) \
  RPC_SERVICE_HANDLER(CoreWorkerService, GetObjectStatus)                \
  RPC_SERVICE_HANDLER(CoreWorkerService, WaitForActorOutOfScope)         \
  RPC_SERVICE_HANDLER(CoreWorkerService, WaitForObjectEviction)          \
  RPC_SERVICE_HANDLER(CoreWorkerService, WaitForRefRemoved)              \
  RPC_SERVICE_HANDLER(CoreWorkerService, KillActor)                      \
  RPC_SERVICE_HANDLER(CoreWorkerService, CancelTask)                     \
  RPC_SERVICE_HANDLER(CoreWorkerService, RemoteCancelTask)               \
  RPC_SERVICE_HANDLER(CoreWorkerService, GetCoreWorkerStats)             \
  RPC_SERVICE_HANDLER(CoreWorkerService, LocalGC)                        \
  RPC_SERVICE_HANDLER(CoreWorkerService, PlasmaObjectReady)

#define RAY_CORE_WORKER_DECLARE_RPC_HANDLERS                              \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(AssignTask)                     \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(PushTask)                       \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(DirectActorCallArgWaitComplete) \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(GetObjectStatus)                \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(WaitForActorOutOfScope)         \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(WaitForObjectEviction)          \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(WaitForRefRemoved)              \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(KillActor)                      \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(CancelTask)                     \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(RemoteCancelTask)               \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(GetCoreWorkerStats)             \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(LocalGC)                        \
  DECLARE_VOID_RPC_SERVICE_HANDLER_METHOD(PlasmaObjectReady)

/// Interface of the `CoreWorkerServiceHandler`, see `src/ray/protobuf/core_worker.proto`.
class CoreWorkerServiceHandler {
 public:
  /// Handlers. For all of the following handlers, the implementations can
  /// handle the request asynchronously. When handling is done, the
  /// `send_reply_callback` should be called. See
  /// src/ray/rpc/node_manager/node_manager_client.h and
  /// src/ray/protobuf/node_manager.proto for a description of the
  /// functionality of each handler.
  ///
  /// \param[in] request The request message.
  /// \param[out] reply The reply message.
  /// \param[in] send_reply_callback The callback to be called when the request is done.
  RAY_CORE_WORKER_DECLARE_RPC_HANDLERS
};

/// The `GrpcServer` for `CoreWorkerService`.
class CoreWorkerGrpcService : public GrpcService {
 public:
  /// Constructor.
  ///
  /// \param[in] main_service See super class.
  /// \param[in] handler The service handler that actually handle the requests.
  CoreWorkerGrpcService(boost::asio::io_service &main_service,
                        CoreWorkerServiceHandler &service_handler)
      : GrpcService(main_service), service_handler_(service_handler) {}

 protected:
  grpc::Service &GetGrpcService() override { return service_; }

  void InitServerCallFactories(
      const std::unique_ptr<grpc::ServerCompletionQueue> &cq,
      std::vector<std::unique_ptr<ServerCallFactory>> *server_call_factories) override {
    RAY_CORE_WORKER_RPC_HANDLERS
  }

 private:
  /// The grpc async service object.
  CoreWorkerService::AsyncService service_;

  /// The service handler that actually handles the requests.
  CoreWorkerServiceHandler &service_handler_;
};

}  // namespace rpc
}  // namespace ray

#endif  // RAY_RPC_CORE_WORKER_SERVER_H
