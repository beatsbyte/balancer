#include "worker_pool.hpp"
#include <chrono>
#include <stdexcept>

namespace worker_pool {

void WorkerPool::Add(const std::string& url) {
  if (workers.count(url) > 0) {
    auto& worker = workers[url];
    auto now = std::chrono::system_clock::now();
    auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    worker.last_updated = now_seconds;
    return;
  }

  worker::Worker new_worker(url);
  workers[url] = new_worker;
}

std::string WorkerPool::GetNext() {
  std::string min_connections_url;
  for (auto x : workers) {
    if (min_connections_url.length() == 0) {
      if (x.second.IsAlive()) {
        min_connections_url = x.first;
      }
      continue;
    }

    if (x.second.IsAlive() &&
        x.second.active_connections <
            workers[min_connections_url].active_connections)
      min_connections_url = x.first;
  }

  return min_connections_url;
}

std::string WorkerPool::MakeCall(
    const userver::server::http::HttpRequest& request) {
  std::string next_url = GetNext();
  if (next_url.empty()) throw std::runtime_error("No services available!");

  return workers[next_url].MakeCall(request, client_);
}

void AppendWorkerPool(userver::components::ComponentList& component_list) {
  component_list.Append<WorkerPool>();
}

} //namespace worker_pool