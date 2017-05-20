

#include <iostream>
#include <osmium/osm/types.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/segment.hpp>
#include <osmium/osm/undirected_segment.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/osm/object.hpp>
#include <osmium/builder/builder.hpp>
#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/io/any_output.hpp>
#include <osmium/io/any_compression.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/relations/collector.hpp>




class MyHandler : public osmium::handler::Handler {
    public:
        void way(const osmium::Way& way) {
            std::cout << "way " << way.id() << '\n';
            for (const osmium::Tag& t : way.tags()) {
                std::cout << t.key() << "=" << t.value() << '\n';
            }
            for (const auto& n : way.nodes()) {
                std::cout << n.ref() << ": " << n.lon() << ", " << n.lat() << '\n';
            }
        }

#if 0
        void node(const osmium::Node& node) {
            std::cout << "node " << node.id() << '\n';
        }
#endif
};


class MyRelCollector : public osmium::relations::Collector<MyRelCollector, true, true, true> {
 public:
     void handle_incomplete_relations() {
         for (auto* relation : this->get_incomplete_relations()) {
             for (const auto& member : relation->members()) {
                 std::pair<bool, size_t> offset_pair =
                     get_availability_and_offset(member.type(), member.ref());
                 if (offset_pair.first) {
                    std::cout << "I am here\n";
                     // do what you would do with a relation
                 }
             }
         }
     }

     void complete_relation(osmium::relations::RelationMeta& relation_meta) {
         const osmium::Relation& relation = this->get_relation(relation_meta);
         std::cout << "Working on relation "
             << relation.id()
             << " which has following tags:\n";

         for (const osmium::Tag& tag : relation.tags()) {
             std::cout << tag.key() << " = " << tag.value() << '\n';
         }

         for (const auto& member : relation.members()) {
             switch (member.type()) {
                 case osmium::item_type::node :
                     {
                         std::cout << "member node "
                             << member.ref()
                             << " with role "
                             << member.role()
                             << '\n';
                         const auto& node = static_cast<const osmium::Node&>
                             (this->get_member(this->get_offset(member.type(), member.ref())));
                         std::cout << "at "
                             << node.location()
                             << '\n';
                     }
                     break;
                 case osmium::item_type::way :
                     std::cout << "member way "
                         << member.ref()
                         << " with role "
                         << member.role()
                         << '\n';
                     // accessing tags, node references and node locations
                     // works like shown above with nodes, just cast to a
                     // different class
                     break;
                 case osmium::item_type::relation :
                     std::cout << "member relation "
                         << member.ref()
                         << " with role "
                         << member.role()
                         << '\n';
                     break;
             }
         }
     }
};




main() {

#if 0
    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
#endif
    osmium::io::Reader reader{"../../../tools/data/restrictions.osm"};
    namespace map = osmium::index::map;
    using index_type = map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

    index_type index;
    location_handler_type location_handler{index};

#if 0
    MyHandler handler;
    osmium::apply(reader, location_handler, handler);
#endif

    osmium::relations::RelationMeta relation_meta;
    MyRelCollector colector;
    colector.complete_relation(relation_meta);

    reader.close();
}
