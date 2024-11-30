#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace balancer {

void AppendImAlive(userver::components::ComponentList& component_list);

}  // namespace balancer
