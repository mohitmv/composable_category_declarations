#include "tracing.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;

namespace perfetto {

Trace g_packets;

Session StartSession(const std::vector<std::string> &category_names) {
  g_packets.clear();
  auto &global_registry = internal::GetGlobalCategoryRegistry();
  auto id_map = global_registry.EnableCategories(category_names);
  for (auto &item : id_map) {
    auto category_id = item.first;
    const auto &name = item.second;
    AddPacket(TracePacket{TracePacket::MappingIID, category_id, "", name});
  }
  return Session{};
}

void Print(const std::vector<TracePacket> &packets) {
  for (size_t i = 0; i < packets.size(); i++) {
    const auto &packet = packets[i];
    std::cout << "packet[" << i << "]: {";
    if (packet.type == TracePacket::MappingIID) {
      std::cout << "Mapping: category_id " << packet.category_id << " => '"
                << packet.category_name << "'";
    } else {
      std::cout << "Event: ";
      if (packet.category_name.size() > 0) {
        std::cout << "category_name = " << packet.category_name << ", ";
      }
      std::cout << "category_id=" << packet.category_id
                << ", event_name=" << packet.event_name;
    }
    std::cout << "}\n";
  }
}

// Populate `category_name` on event with only category_id, using the
// mapping info available on initial packets.
Trace TraceProcess(const Trace &packets) {
  Trace output;
  std::unordered_map<size_t, string> category_id_map;
  for (auto &packet : packets) {
    if (packet.type == TracePacket::MappingIID) {
      category_id_map[packet.category_id] = packet.category_name;
      output.push_back(packet);
    } else {
      auto new_packet = packet;
      new_packet.category_name = category_id_map[packet.category_id];
      output.push_back(std::move(new_packet));
    }
  }
  return output;
}

} // namespace perfetto
