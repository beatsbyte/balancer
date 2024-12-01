#include <ctime>
#include <chrono>
#include <userver/clients/http/component.hpp>

#include "worker.hpp"

namespace worker {

    Worker::Worker() {
      auto current_time = std::time_t(nullptr);
      last_updated = static_cast<size_t>(current_time);
      active_connections = 0;
    };

    Worker::Worker(const std::string &_url)
    : url(_url) {
        auto now = std::chrono::system_clock::now();
        auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        last_updated = now_seconds;
        active_connections = 0;
    };

    bool Worker::IsAlive() {
      auto now = std::chrono::system_clock::now();
      auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

      return now_seconds - last_updated < time_to_die;
    }

    std::string Worker::MakeCall(const userver::server::http::HttpRequest& request,
                                 userver::components::HttpClient& http_client) {

      {
        std::lock_guard<std::mutex> lock(*mtx);
        ++active_connections;
      }
      auto &client = http_client.GetHttpClient();
      auto new_request = client.CreateRequest().post(url, request.RequestBody()).timeout(std::chrono::seconds(40));
      new_request.headers(request.GetHeaders());

      try {
        auto res = new_request.perform();
        if (res->IsOk()) {
          {
            std::lock_guard<std::mutex> lock(*mtx);
            --active_connections;
          }
          auto& response = request.GetHttpResponse();

          response.SetContentType("audio/mpeg");
          return std::move(*res).body();  // no copying
        }

        auto& response = request.GetHttpResponse();
        response.SetStatus(res->status_code());
        return {};

      } catch (...) {
        {
          std::lock_guard<std::mutex> lock(*mtx);
          --active_connections;
        }
        auto& response = request.GetHttpResponse();
        response.SetStatus(userver::server::http::HttpStatus::kServiceUnavailable);
        return {};
      }
    }


} // worker