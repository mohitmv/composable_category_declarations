#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

namespace perfetto {

struct Category {
  const char *const name;
};

namespace internal {

// Local slot_id in a translation unit.
template <size_t slot_number> struct LocalSlotIdCounter;

// Error: Failed to find category string. Either the category is not declared
// or some header declaring this category is not included in current TU.
static constexpr size_t kInvalidCategoryIndex = static_cast<size_t>(-1);
static constexpr size_t k32BitMask = static_cast<size_t>(0xFFFFFFFF);
static_assert(sizeof(size_t) == 8, "");

static constexpr bool StringEq(const char *a, const char *b) {
  return *a != *b ? false : (!*a || !*b) ? (*a == *b) : StringEq(a + 1, b + 1);
}

// Stores slot_number and index of found category in that category declaration.
// Stored as ((index << 32) | slot_number).
// This value would be `kInvalidCategoryIndex` if category was not found.
using CategorySearchResult = size_t;

template <typename UniqueType, CategorySearchResult result>
struct TypedCategorySearchResult;

template <typename UniqueType>
struct TypedCategorySearchResult<UniqueType, kInvalidCategoryIndex> {
  using type = void;
  static constexpr size_t index = kInvalidCategoryIndex;
};

struct perfetto_catg_decl_libA;
struct perfetto_catg_decl_libB;

template <typename UniqueType, CategorySearchResult result>
struct TypedCategorySearchResult {
  using type = typename LocalSlotIdCounter<(result & k32BitMask)>::type;
  static constexpr size_t index = (result >> 32);
  static size_t GlobalCategoryId() { return type::kGlobalCategoryIds[index]; }
};

template <typename tag_type>
static constexpr size_t FindCategoryInSlot(const char *name, int index = 0) {
  return (index == tag_type::kCategoryCount)
             ? kInvalidCategoryIndex
             : (StringEq(tag_type::kCategories[index].name, name)
                    ? index
                    : FindCategoryInSlot<tag_type>(name, index + 1));
}

template <size_t v> struct SizeType { static constexpr size_t value = v; };

template <typename UniqueType, size_t slot = 0, int = 0>
struct FindNextUndefinedSlot : SizeType<slot> {};

template <typename UniqueType, size_t slot>
struct FindNextUndefinedSlot<UniqueType, slot,
                             0 * sizeof(LocalSlotIdCounter<slot>)>
    : FindNextUndefinedSlot<UniqueType, slot + 1> {};

template <typename UniqueType, size_t slot = 0, int = 0>
struct GetCategoryIdImpl {
  static constexpr CategorySearchResult Get(const char *name) {
    return kInvalidCategoryIndex;
  }
};

template <typename UniqueType, size_t slot_number>
struct GetCategoryIdImpl<UniqueType, slot_number,
                         0 * sizeof(LocalSlotIdCounter<slot_number>)> {
  static constexpr CategorySearchResult Get(const char *name) {
    return GetHelper(
        FindCategoryInSlot<typename LocalSlotIdCounter<slot_number>::type>(
            name),
        name);
  }

  // This helper method is to prevent double-evaluation of
  // FindCategoryInSlot<...>(name). We need this, because C++11 doesn't allow
  // constexpr local variable declaration.
  static constexpr CategorySearchResult GetHelper(size_t index,
                                                  const char *name) {
    return index != kInvalidCategoryIndex
               ? ((index << 32) | slot_number)
               : GetCategoryIdImpl<UniqueType, slot_number + 1>::Get(name);
  }
};

// UniqueType is important to prevent template-cache on slot_number. It has no
// other use. Every trace point should supply a unique type.
template <typename UniqueType>
static constexpr CategorySearchResult GetCategoryId(const char *name) {
  return GetCategoryIdImpl<UniqueType, 0>::Get(name);
}

// Returns nullptr if invalid category_id.
// Should be called after completion of static initialization.
const Category *GetCategoryAtRunTime(size_t category_id);

struct GlobalCategoryRegistry {
  std::vector<const Category *> global_categories;
};

GlobalCategoryRegistry &GetGlobalCategoryRegistry();

// Its thread safe, because static-init happens sequentially.
struct RegisterOnConstructor {
  RegisterOnConstructor(const Category *category_list,
                        size_t *global_category_ids, size_t len);
};

} // namespace internal
} // namespace perfetto

#define PERFETTO_DECLARE_CATEGORIES_IMPL(tag_type, tag_slot_number_type,       \
                                         unique_type, ...)                     \
  namespace perfetto {                                                         \
  namespace internal {                                                         \
  struct unique_type;                                                          \
  struct tag_slot_number_type {                                                \
    static constexpr size_t slot_number =                                      \
        FindNextUndefinedSlot<unique_type>::value;                             \
  };                                                                           \
  struct tag_type {                                                            \
    static constexpr ::perfetto::Category kCategories[] = {__VA_ARGS__};       \
    static constexpr size_t kCategoryCount =                                   \
        sizeof(kCategories) / sizeof(kCategories[0]);                          \
    static size_t kGlobalCategoryIds[kCategoryCount];                          \
  };                                                                           \
  template <> struct LocalSlotIdCounter<tag_slot_number_type::slot_number> {   \
    using type = tag_type;                                                     \
  };                                                                           \
  }                                                                            \
  }

// Should be used only in global namespace.
#define PERFETTO_DECLARE_CATEGORIES(tag_type, ...)                             \
  PERFETTO_DECLARE_CATEGORIES_IMPL(                                            \
      perfetto_catg_decl_##tag_type, perfetto_catg_decl_tag_slot_##tag_type,   \
      perfetto_catg_decl_unique_type_##tag_type, __VA_ARGS__)

// Should be used only in global namespace.
#define PERFETTO_DEFINE_CATEGORIES_IMPL(tag_type, tmp_var)                     \
  namespace perfetto {                                                         \
  namespace internal {                                                         \
  const ::perfetto::Category tag_type::kCategories[];                          \
  size_t tag_type::kGlobalCategoryIds[] = {};                                  \
  static ::perfetto::internal::RegisterOnConstructor                           \
      tmp_var(&tag_type::kCategories[0], &tag_type::kGlobalCategoryIds[0],     \
              tag_type::kCategoryCount);                                       \
  }                                                                            \
  }

#define PERFETTO_DEFINE_CATEGORIES(tag_type)                                   \
  PERFETTO_DEFINE_CATEGORIES_IMPL(perfetto_catg_decl_##tag_type,               \
                                  perfetto_catg_decl_tmp_var_##tag_type)
