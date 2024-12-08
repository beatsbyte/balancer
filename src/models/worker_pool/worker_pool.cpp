#include "worker_pool.hpp"
#include <chrono>
#include <stdexcept>

namespace worker_pool {

bool WorkerPool::Add(const std::string& url) {
  if (url.empty()) return false;

  if (workers.count(url) > 0) {
    auto& worker = workers[url];
    auto now = std::chrono::system_clock::now();
    auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    worker.SetLastUpdated(now_seconds);
    return true;
  }

  worker::Worker new_worker(url);
  workers[url] = new_worker;
  return true;
}

std::optional<std::string> WorkerPool::GetNext() {
  std::lock_guard<std::mutex> lock(*mtx);
  std::optional<std::string> min_connections_url;
  for (auto x : workers) {
    if (!min_connections_url.has_value()) {
      if (x.second.IsAlive()) {
        min_connections_url = x.first;
      }
      continue;
    }

    if (x.second.IsAlive() &&
        x.second.GetActiveConnections() <
            workers[min_connections_url.value()].GetActiveConnections())
      min_connections_url = x.first;
  }

  return min_connections_url;
}

std::optional<std::string> WorkerPool::MakeCall(
    const userver::server::http::HttpRequest& request
) {
  auto next_url = GetNext();
  if (!next_url.has_value()) return std::nullopt;

  std::string worker_header_name = "X-Proxy-Worker-Url";
  request.GetHttpResponse().SetHeader(worker_header_name, next_url.value());

  return workers[next_url.value()].MakeCall(request, client_);
}

void AppendWorkerPool(userver::components::ComponentList& component_list) {
  component_list.Append<WorkerPool>();
}

} //namespace worker_pool