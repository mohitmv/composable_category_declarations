#include "track_event_category_registry.h"

#include <iostream>
#include <string>
#include <unordered_set>

namespace perfetto {
namespace internal {

GlobalCategoryRegistry::IdMap GlobalCategoryRegistry::EnableCategories(
    const std::vector<std::string> &category_names) {
  IdMap output;
  auto &global_registry = GetGlobalCategoryRegistry();
  std::unordered_set<std::string> names_set(category_names.begin(),
                                            category_names.end());
  size_t num_registory = num_registory_;
  for (size_t i = 0; i < num_registory; ++i) {
    auto &registry = global_registry.registories_[i];
    if (!registry.is_registered)
      continue;
    for (size_t j = 0; j < registry.length; ++j) {
      const auto &name = registry.category_list[j].name;
      bool should_enable = names_set.find(name) != names_set.end();
      registry.is_enabled_array[j] = (0 + should_enable);
      size_t category_id = (registry.global_category_id_offset + j);
      if (should_enable) {
        output[category_id] = name;
      }
    }
  }
  return output;
}

// Thread-safe since static-init runs sequentially.
RegisterOnConstructor::RegisterOnConstructor(
    const Category *category_list, size_t *global_category_ids_offset,
    std::atomic<uint8_t> *is_enabled_array, size_t length) {
  auto &global_registry = GetGlobalCategoryRegistry();
  size_t num_registory = global_registry.num_registory_++;
  registory_index_ = num_registory;
  assert(registory_index_ < global_registry.registories_.size());
  CategoryRegistry &registry = global_registry.registories_[registory_index_];
  registry.category_list = category_list;
  registry.length = length;
  registry.is_enabled_array = is_enabled_array;
  size_t global_category_id_offset =
      global_registry.global_category_ids_offset_.fetch_add(length);
  registry.global_category_id_offset = global_category_id_offset;
  *global_category_ids_offset = global_category_id_offset;
  registry.is_registered = true;
}

// After a shared lib is `dlclose`, we don't want to enable/disable it's
// categories.
RegisterOnConstructor::~RegisterOnConstructor() {
  auto &global_registry = GetGlobalCategoryRegistry();
  CategoryRegistry &registry = global_registry.registories_[registory_index_];
  registry.is_registered = false;
}

GlobalCategoryRegistry &GetGlobalCategoryRegistry() {
  static GlobalCategoryRegistry registry;
  return registry;
}

} // namespace internal
} // namespace perfetto
