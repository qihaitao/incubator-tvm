/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * \file wasm_runtime.cc
 * \brief TVM wasm runtime library pack.
 */

// configurations for the dmlc log.
#define DMLC_LOG_CUSTOMIZE 0
#define DMLC_LOG_STACK_TRACE 0
#define DMLC_LOG_DEBUG 0
#define DMLC_LOG_NODATE 1
#define DMLC_LOG_FATAL_THROW 0

#include <dmlc/logging.h>
#include <tvm/runtime/c_runtime_api.h>

#include "src/runtime/c_runtime_api.cc"
#include "src/runtime/cpu_device_api.cc"
#include "src/runtime/file_utils.cc"
#include "src/runtime/graph/graph_runtime.cc"
#include "src/runtime/library_module.cc"
#include "src/runtime/module.cc"
#include "src/runtime/ndarray.cc"
#include "src/runtime/object.cc"
#include "src/runtime/registry.cc"
#include "src/runtime/rpc/rpc_channel.cc"
#include "src/runtime/rpc/rpc_endpoint.cc"
#include "src/runtime/rpc/rpc_event_impl.cc"
#include "src/runtime/rpc/rpc_local_session.cc"
#include "src/runtime/rpc/rpc_module.cc"
#include "src/runtime/rpc/rpc_session.cc"
#include "src/runtime/system_library.cc"
#include "src/runtime/workspace_pool.cc"

// --- Implementations of backend and wasm runtime API. ---

int TVMBackendParallelLaunch(FTVMParallelLambda flambda, void* cdata, int num_task) {
  TVMParallelGroupEnv env;
  env.num_task = 1;
  flambda(0, &env, cdata);
  return 0;
}

int TVMBackendParallelBarrier(int task_id, TVMParallelGroupEnv* penv) { return 0; }

// --- Environment PackedFuncs for testing ---
namespace tvm {
namespace runtime {

TVM_REGISTER_GLOBAL("testing.echo").set_body([](TVMArgs args, TVMRetValue* ret) {
  *ret = args[0];
});

TVM_REGISTER_GLOBAL("testing.add_one").set_body_typed([](int x) { return x + 1; });

TVM_REGISTER_GLOBAL("testing.wrap_callback").set_body([](TVMArgs args, TVMRetValue* ret) {
  PackedFunc pf = args[0];
  *ret = runtime::TypedPackedFunc<void()>([pf]() { pf(); });
});
}  // namespace runtime
}  // namespace tvm
