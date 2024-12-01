#pragma once

#include <string>
#include <atomic>
#include <mutex>
#include <userver/server/handlers/http_handler_base.hpp>

namespace worker {
    struct Worker
    {
        std::string url;
        size_t last_updated;
        int active_connections;
        mutable std::mutex* mtx;

        static const size_t time_to_die = 10;

        Worker();
        Worker(const std::string &url);

        bool IsAlive();
        std::string MakeCall(const userver::server::http::HttpRequest& request,
                             userver::components::HttpClient&);
    };
} // worker