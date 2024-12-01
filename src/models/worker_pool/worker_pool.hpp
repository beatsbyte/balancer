#pragma once

#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/clients/http/component.hpp>

#include <string>
#include <unordered_map>
#include "../worker/worker.hpp"

namespace worker_pool {

class WorkerPool : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "worker-pool";
  WorkerPool(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& context)
      : userver::components::LoggableComponentBase(config, context),
        client_(context.FindComponent<userver::components::HttpClient>()) {}

  void Add(const std::string& url);

  std::string GetNext();

  std::string MakeCall(
      const userver::server::http::HttpRequest& request);
  std::unordered_map<std::string, worker::Worker> workers;
 private:
  userver::components::HttpClient &client_;
};

void AppendWorkerPool(userver::components::ComponentList& component_list);

}