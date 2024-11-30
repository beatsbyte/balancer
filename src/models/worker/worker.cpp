#include <ctime>

#include "worker.hpp"

namespace balancer {
    Worker::Worker(const std::string &_url)
    : url(_url) {
        auto current_time = std::time_t(nullptr_t);
        last_updated = static_cast<size_t>(current_time);
    };

    bool Worker::IsAlive() {
      auto current_time = std::time_t(nullptr_t);
      auto now = static_cast<size_t>(current_time);

      return now - current_time > time_to_die;
    }

//    std::string Worker::MakeCall(const userver::server::http::HttpRequest& request) {
//      userver::clients::http::HttpClient client;
//      request.SetUrl(url);
//
//      auto response = await client.Send(request);
//    }


}