#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/utils/assert.hpp>
#include <iostream>
#include <sstream>
#include <chrono>

#include "../../../models/worker_pool/worker_pool.hpp"

namespace balancer {

namespace {

class ImAlive final : public userver::server::handlers::HttpHandlerBase {
  worker_pool::WorkerPool* worker_pool_;
 public:
  static constexpr std::string_view kName = "handler-v1-imalive";

  ImAlive(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        worker_pool_(&component_context.FindComponent<worker_pool::WorkerPool>("worker-pool")) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto request_body = 
        userver::formats::json::FromString(request.RequestBody());

    auto path = request_body["url"].As<std::optional<std::string>>();

    if (!path.has_value() || path.value().empty()) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }

    std::stringstream url;
    url << path.value();

    bool could_add = worker_pool_->Add(url.str());

    if (!could_add) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }

    return {url.str()};
  }
};

}  // namespace

void AppendImAlive(userver::components::ComponentList& component_list) {
  component_list.Append<ImAlive>();
}

}  // namespace balancer
