#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace balancer {

void AppendBalancer(userver::components::ComponentList& component_list);

}  // namespace balancer
