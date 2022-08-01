#include "track_event_category_registry.h"

#include <iostream>

namespace perfetto {
namespace internal {

const Category *GetCategoryAtRunTime(size_t category_id) {
  auto &registry = GetGlobalCategoryRegistry();
  return registry.global_categories.at(category_id - 1);
}

RegisterOnConstructor::RegisterOnConstructor(
    const Category* category_list, size_t* global_category_ids, size_t len) {
  auto &registry = GetGlobalCategoryRegistry();
  for (size_t i = 0; i < len; i++) {
    registry.global_categories.push_back(&category_list[i]);
    global_category_ids[i] = registry.global_categories.size();
  }
}

GlobalCategoryRegistry &GetGlobalCategoryRegistry() {
  static GlobalCategoryRegistry registry;
  return registry;
}

} // namespace internal
} // namespace perfetto
