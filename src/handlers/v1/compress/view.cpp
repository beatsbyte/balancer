#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_list.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/worker_pool/worker_pool.hpp"

namespace compress {

namespace {

class Compress final : public userver::server::handlers::HttpHandlerBase {
  worker_pool::WorkerPool* worker_pool_;

 public:
  static constexpr std::string_view kName = "handler-v1-compress";

  Compress(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context), worker_pool_(&component_context.FindComponent<worker_pool::WorkerPool>("worker-pool")) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override {

    try {
      std::string response = worker_pool_->MakeCall(request);
      return response;
    } catch (...) {
      auto& response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kServiceUnavailable);
      return {};
    }
  }
};

}  // namespace

void AppendCompress(userver::components::ComponentList& component_list) {
  component_list.Append<Compress>();
}

}  // namespace compress
