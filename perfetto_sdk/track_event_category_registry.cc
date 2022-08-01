#include "track_event_category_registry.h"

#include <iostream>

namespace perfetto {
namespace internal {

// TrackEventCategoryRegistry category_declaration_slots[16] = {};

const Category *GetCategoryAtRunTime(size_t category_id) {
  size_t slot_number = category_id & kMaxCategorySlotNumber;
  auto &slot = GetRegistrySlots()[slot_number];
  if (slot.categories == nullptr)
    return nullptr;
  auto index = category_id >> kNumBitsRequiredForCategorySlotNumber;
  if (!(0 <= index && index < slot.length))
    return nullptr;
  return &slot.categories[index];
}

CategoryDeclarationOnConstructor::CategoryDeclarationOnConstructor(
    size_t slot_number, TrackEventCategoryRegistry decl) {
  auto &slot = GetRegistrySlots()[slot_number];
  if (slot.compilation_unit_name != nullptr) {
    std::cerr << "Slot number " << slot_number << " is already used in "
              << slot.compilation_unit_name << " . It cannot be used again in "
              << decl.compilation_unit_name << " .";
    std::abort();
  }
  slot = decl;
}

RegistrySlotsType &GetRegistrySlots() {
  static RegistrySlotsType slots = {};
  return slots;
}

} // namespace internal
} // namespace perfetto
