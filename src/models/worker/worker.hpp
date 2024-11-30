#pragma once

#include <string>
#include <atomic>

namespace balancer {
    struct Worker
    {
        std::string url;
        size_t last_updated;
        std::atomic<std::size_t> active_connections = 0;

        static time_to_die = 10;

        Worker(const std::string &url);

        bool IsAlive();
        std::string MakeCall(const userver::server::http::HttpRequest&);
    };
}