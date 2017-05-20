

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
#include <cstdlib>  // for std::exit
#include <getopt.h> // for getopt_long
#include <iostream> // for std::cout, std::cerr

// For assembling multipolygons
#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>

// For the DynamicHandler class
#include <osmium/dynamic_handler.hpp>

// For the WKT factory
#include <osmium/geom/wkt.hpp>

// For the Dump handler
#include <osmium/handler/dump.hpp>

// For the NodeLocationForWays handler
#include <osmium/handler/node_locations_for_ways.hpp>

// Allow any format of input files (XML, PBF, ...)
#include <osmium/io/any_input.hpp>

// For osmium::apply()
#include <osmium/visitor.hpp>

// For the location index. There are different types of indexes available.
// This will work for small and medium sized input files.
#include <osmium/index/map/sparse_mem_array.hpp>



using index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;


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

        void node(const osmium::Node& node) {
            std::cout << "node " << node.id() << '\n';
            std::cout << node.location() << '\n';
        }
};


class MyRelCollector : public osmium::relations::Collector<MyRelCollector, true, true, true> {

#if 0
    osmium::memory::Buffer m_output_buffer;

    static constexpr size_t initial_output_buffer_size = 1024 * 1024;
    static constexpr size_t max_buffer_size_for_flush = 100 * 1024;

    void flush_output_buffer() {
        if (this->callback()) {
            osmium::memory::Buffer buffer{initial_output_buffer_size};
            using std::swap;
            swap(buffer, m_output_buffer);
            this->callback()(std::move(buffer));
        }
    }

    void possibly_flush_output_buffer() {
        if (m_output_buffer.committed() > max_buffer_size_for_flush) {
            flush_output_buffer();
        }
    }
#endif
    public:

#if 0
     explicit MyRelCollector() :
         m_output_buffer(initial_output_buffer_size, osmium::memory::Buffer::auto_grow::yes) {
         }
#endif

   /**
    * Interested in all relations tagged with type=restriction
    *
    * OSM WIKI about restrictions
    * http://wiki.openstreetmap.org/wiki/Relation:restriction
    *
    * Overwritten from the base class.
    */
     bool keep_relation(const osmium::Relation& relation) const {
         const char* type = relation.tags().get_value_by_key("type");

         /*
          *  ignore relations without "type" tag
          */
         if (!type) {
             return false;
         }

         if ((!std::strcmp(type, "restriction"))) {
                    return true;
         }

         return false;
     }

     /**
      * Overwritten from the base class.
      */
     bool keep_member(
             const osmium::relations::RelationMeta&,
             const osmium::RelationMember& member) const {
         /*
          * Interested in members of type way & node.
          */
         return member.type() == osmium::item_type::way || 
             member.type() == osmium::item_type::node;
     }

     /**
      * Overwritten from the base class.
      */
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
                             << member.role();
                         auto offset_pair = this->get_availability_and_offset(member.type(), member.ref());
                         if (!offset_pair.first) {
                             std::cout << '\n';
                             break;
                         }
                         const auto& node = static_cast<const osmium::Node&>
                             (this->get_member(this->get_offset(member.type(), member.ref())));
                         std::cout << "\t at "
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
#if 0
          possibly_flush_output_buffer();
#endif
     }

     void flush() {
         this->callback();
     }

#if 0
     osmium::memory::Buffer read() {
         osmium::memory::Buffer buffer{initial_output_buffer_size, osmium::memory::Buffer::auto_grow::yes};

         std::swap(buffer, m_output_buffer);

         return buffer;
     }
#endif
     void print_relations() {
         for (auto r : relations()) {
             complete_relation(r);
         }
     }
};




main() {

     osmium::handler::DynamicHandler handler;
#if 0
    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
#endif
#if 0
    namespace map = osmium::index::map;
    using index_type = map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

    index_type index;
    location_handler_type location_handler{index};

    MyHandler handler;
    osmium::apply(reader, location_handler, handler);
#endif

    osmium::relations::RelationMeta relation_meta;
    MyRelCollector collector;
    std::cerr << "Pass 1...\n";
    osmium::io::Reader reader1{"../../../tools/data/restrictions.osm", osmium::osm_entity_bits::relation};
    collector.read_relations(reader1);
    reader1.close();
    std::cout << "Pass 1 done\n";


    // Output the amount of main memory used so far. All multipolygon relations
    // are in memory now.
    std::cerr << "Memory:\n";
    collector.used_memory();

    // The index storing all node locations.
    index_type index;

    // The handler that stores all node locations in the index and adds them
    // to the ways.
    location_handler_type location_handler{index};

     // If a location is not available in the index, we ignore it. It might
    // not be needed (if it is not part of a multipolygon relation), so why
    // create an error?
    location_handler.ignore_errors();

    // On the second pass we read all objects and run them first through the
    // node location handler and then the multipolygon collector. The collector
    // will put the areas it has created into the "buffer" which are then
    // fed through our "handler".
    std::cerr << "Pass 2...\n";
    osmium::io::Reader reader2{"../../../tools/data/restrictions.osm"};
    osmium::apply(reader2, location_handler, collector.handler([&handler](osmium::memory::Buffer&& buffer) {
        osmium::apply(buffer, handler);
    }));
    reader2.close();
    std::cerr << "Pass 2 done\n";

    // Output the amount of main memory used so far. All complete multipolygon
    // relations have been cleaned up.
    std::cerr << "Memory:\n";
    collector.used_memory();


#if 0
    colector.print_relations();
#endif
}
