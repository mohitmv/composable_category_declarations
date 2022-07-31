#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

namespace perfetto {

struct Category {
  const char* const name;
};

namespace internal {

template<size_t slot_number>
struct CategoryDeclarationSlot;

static constexpr size_t kInvalidCategoryIndex = static_cast<size_t>(-1);

// Valid category slot numbers are {0, 1, 2, ... 15}
static constexpr size_t kMaxCategorySlotNumber = 15;
static constexpr size_t kNumBitsRequiredForCategorySlotNumber = 4;

static constexpr bool StringEq(const char* a, const char* b) {
  return *a != *b ? false
                  : (!*a || !*b) ? (*a == *b) : StringEq(a + 1, b + 1);
}

template<typename UniqueType, size_t slot, size_t = 0>
struct FindCategory {
  static constexpr bool is_slot_declared = false;
  static constexpr size_t Find(const char*) { return kInvalidCategoryIndex; }
};

template<typename UniqueType, size_t slot>
struct FindCategory<UniqueType, slot, 0 * sizeof(CategoryDeclarationSlot<slot>)> {
  static constexpr bool is_slot_declared = true;
  static constexpr size_t Find(const char* name, int index = 0) {
    return (index == CategoryDeclarationSlot<slot>::kCategoryCount) ?
        kInvalidCategoryIndex :
        StringEq(CategoryDeclarationSlot<slot>::kCategories[index].name, name)
          ? ((index << kNumBitsRequiredForCategorySlotNumber) | slot)
            : Find(name, index + 1);
  }
};

template<typename UniqueType, size_t slot_number = 0>
struct GetCategoryIdImpl {
  static constexpr size_t Get(const char* name) {
    return GetHelper(FindCategory<UniqueType, slot_number>::Find(name), name);
  }
  // To prevent double-evaluation of FindCategory<...>::Find(name).
  // C++11 doesn't allow constexpr local variable declaration.
  static constexpr size_t GetHelper(size_t id, const char* name) {
    return id != kInvalidCategoryIndex ? id : GetCategoryIdImpl<UniqueType, slot_number+1>::Get(name);
  }
};

template<typename UniqueType>
struct GetCategoryIdImpl<UniqueType, 1 + kMaxCategorySlotNumber> {
  static constexpr size_t Get(const char* name) {
    return kInvalidCategoryIndex;
  }
};

// UniqueType is important to prevent template-cache on slot_number. It has no
// other use. Every trace point should supply a unique type.
template<typename UniqueType>
constexpr size_t GetCategoryId(const char* name) {
  return GetCategoryIdImpl<UniqueType, 0>::Get(name);
}


struct TrackEventCategoryRegistry {
  TrackEventCategoryRegistry() { }
  TrackEventCategoryRegistry(const Category* categories, size_t length,
                             const char* compilation_unit_name)
    : categories(categories),
      length(length),
      compilation_unit_name(compilation_unit_name) { }
  const Category* categories = nullptr;
  size_t length = 0;
  // For the purpose of showing error message. Original source: `__FILE__`
  const char* compilation_unit_name = nullptr;
};

using RegistrySlotsType = TrackEventCategoryRegistry[1 + kMaxCategorySlotNumber];

RegistrySlotsType& GetRegistrySlots();

template<size_t category_id>
constexpr const Category& GetCategoryAtCompileTime() {
  return CategoryDeclarationSlot<category_id & kMaxCategorySlotNumber>::kCategories[category_id >> kNumBitsRequiredForCategorySlotNumber];
}

// Returns nullptr if invalid category_id.
// Its thread-safe if called anytime after static initialization.
const Category* GetCategoryAtRunTime(size_t category_id);

// Its thread safe, because static-init happens sequentially.
struct CategoryDeclarationOnConstructor {
  CategoryDeclarationOnConstructor(size_t slot_number, TrackEventCategoryRegistry decl);
};

}  // namespace intenal
}  // namespace perfetto


// Should be used only in global namespace.
#define PERFETTO_DECLARE_CATEGORIES(slot_number, ...) \
namespace perfetto { \
namespace internal { \
template<> struct CategoryDeclarationSlot<slot_number> { \
  constexpr static ::perfetto::Category kCategories[] = {__VA_ARGS__}; \
  constexpr static size_t kCategoryCount =                            \
      sizeof(kCategories) / sizeof(kCategories[0]);    \
}; \
}}

#define PERFETTO_CATEGORY_DECLARATION_SLOT_NUMBER(n) (n)

// Should be used only in global namespace.
//
// Implementation Detail:
// The local variable `category_decl_tmp_var` is fine because one TU will never
// use `PERFETTO_DEFINE_CATEGORIES` multiple times, and this symbol is inside
// anonymous namespace, hence it won't collide with other TUs (i.e. 
// PERFETTO_DEFINE_CATEGORIES in other TUs) at link time.
#define PERFETTO_DEFINE_CATEGORIES(slot_number)   \
namespace perfetto { \
namespace internal { \
const ::perfetto::Category CategoryDeclarationSlot<slot_number>::kCategories[]; \
namespace {   \
::perfetto::internal::CategoryDeclarationOnConstructor category_decl_tmp_var(\
  slot_number, ::perfetto::internal::TrackEventCategoryRegistry{ \
    &::perfetto::internal::CategoryDeclarationSlot<slot_number>::kCategories[0], \
    ::perfetto::internal::CategoryDeclarationSlot<slot_number>::kCategoryCount, \
    __FILE__}); \
}}}

