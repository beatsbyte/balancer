#pragma once

#include <string>
#include <atomic>
#include <optional>
#include <userver/server/handlers/http_handler_base.hpp>

namespace worker {
    struct Worker
    {
     private:
        std::string url;
        size_t last_updated;
        std::atomic<int> *active_connections;

        static const size_t time_to_die = 10;
     public:
        Worker();
        Worker(const std::string &url);

        void SetLastUpdated(size_t time) {
          last_updated = time;
        }
        int GetLastUpdated() const {
          return last_updated;
        }
        int GetActiveConnections() const {
          return *active_connections;
        }
        bool IsAlive();
        std::optional<std::string> MakeCall(const userver::server::http::HttpRequest& request,
                             userver::components::HttpClient&);
    };
} // worker