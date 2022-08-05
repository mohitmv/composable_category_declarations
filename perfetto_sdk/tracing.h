#pragma once

#include "track_event_category_registry.h"

#include <string>
#include <vector>

namespace perfetto {

struct TrackEvent {
  static void Register() {}
};

struct TracePacket {
  enum Type { Event, MappingIID };
  Type type;
  size_t category_id;
  std::string event_name;
  std::string category_name;
};

using Trace = std::vector<TracePacket>;

extern Trace g_packets;

inline void AddPacket(TracePacket &&packet) {
  g_packets.push_back(std::move(packet));
}

inline void AddPacket(size_t category_id, const std::string &event_name) {
  AddPacket(TracePacket{TracePacket::Event, category_id, event_name});
}

struct Session {
  Trace StopAndReturnTrace() { return g_packets; }
};

Session StartSession(const std::vector<std::string> &category_names);

void Print(const Trace &packets);

Trace TraceProcess(const Trace &packets);

} // namespace perfetto

#define TRACE_EVENT(category_name, event_name)                                 \
  {                                                                            \
    struct perfetto_trace_point_unique_type;                                   \
    using SearchResult = ::perfetto::internal::TypedCategorySearchResult<      \
        perfetto_trace_point_unique_type,                                      \
        ::perfetto::internal::GetCategoryId<perfetto_trace_point_unique_type>( \
            category_name)>;                                                   \
    if (SearchResult::type::kIsEnabled[SearchResult::index]) {                 \
      perfetto::AddPacket(SearchResult::type::kGlobalCategoryIdsOffset +       \
                              SearchResult::index,                             \
                          event_name);                                         \
    }                                                                          \
  }
