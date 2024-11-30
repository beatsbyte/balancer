#pragma once

#include <string>
#include <vector>

#include "../worker/worker.hpp"

namespace balancer {
  struct WorkerPool
  {
    std::unordered_map<std::string, Worker> worker_pool;

    Worker Add(Worker &worker);
    Worker Remove(Worker &worker);
    Worker& GetNext(Worker &worker);


  };
}