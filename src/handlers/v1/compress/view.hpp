#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace balancer {

void AppendCompress(userver::components::ComponentList& component_list);

}  // namespace balancer
