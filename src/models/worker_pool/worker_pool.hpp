#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/clients/http/component.hpp>
#include <mutex>
#include <iostream>
#include <string>
#include <unordered_map>
#include <optional>
#include "../worker/worker.hpp"

namespace worker_pool {

class WorkerPool : public userver::components::LoggableComponentBase {
  std::unordered_map<std::string, worker::Worker> workers;
  userver::components::HttpClient &client_;
  mutable std::mutex* mtx = new std::mutex();

 public:
  static constexpr std::string_view kName = "worker-pool";
  WorkerPool(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& context)
      : userver::components::LoggableComponentBase(config, context),
        client_(context.FindComponent<userver::components::HttpClient>()) {}
  ~WorkerPool() {
    delete mtx;
  }

  bool Add(const std::string& url);

  std::optional<std::string> GetNext();

  std::optional<std::string> MakeCall(const userver::server::http::HttpRequest&);
};

void AppendWorkerPool(userver::components::ComponentList&);

}