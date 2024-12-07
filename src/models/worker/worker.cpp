#include <ctime>
#include <chrono>
#include <userver/clients/http/component.hpp>

#include "worker.hpp"

namespace worker {

    Worker::Worker() {
      auto now = std::chrono::system_clock::now();
      auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
      last_updated = now_seconds;
      active_connections = new std::atomic<int>(0);
    };

    Worker::Worker(const std::string &_url)
    : Worker() {url = _url;};

    bool Worker::IsAlive() {
      auto now = std::chrono::system_clock::now();
      auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

      return now_seconds - last_updated < time_to_die;
    }

    std::optional<std::string> Worker::MakeCall(const userver::server::http::HttpRequest& request,
                                 userver::components::HttpClient& http_client) {
      try {
        ++*active_connections;
        auto &client = http_client.GetHttpClient();
        auto new_request = client.CreateRequest().post(url, request.RequestBody()).timeout(std::chrono::seconds(40));
        new_request.headers(request.GetHeaders());

        auto res = new_request.perform();
        --*active_connections;
        if (res->IsOk()) {
          auto& response = request.GetHttpResponse();

          response.SetContentType("audio/mpeg");
          return std::move(*res).body();  // no copying
        }

        auto& response = request.GetHttpResponse();
        response.SetStatus(res->status_code());
        return {};

      } catch (...) {
        --*active_connections;
        auto& response = request.GetHttpResponse();
        response.SetStatus(userver::server::http::HttpStatus::kServiceUnavailable);
        return {};
      }
    }


} // worker