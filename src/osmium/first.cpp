

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

#if 0
void add_tags(osmium::memory::Buffer& buffer, osmium::builder::Builder* builder) {
    osmium::builder::TagListBuilder tl_builder{buffer, builder};
    tl_builder.add_tag("amenity", "restaurant");
}


void build_way(osmium::memory::Buffer& buffer) {
    osmium::builder::WayBuilder way_builder{buffer};
    way_builder.object().set_id(1);
    // set attributes version, changeset, uid and timestamp (all optional)
    way_builder.set_user("foo");
    {
        osmium::builder::WayNodeListBuilder wnl_builder{buffer, &way_builder};
        wnl_builder.add_node_ref(osmium::NodeRef (1, osmium::Location()));
        wnl_builder.add_node_ref(osmium::NodeRef (2, osmium::Location()));
    }
    add_tags(buffer, &way_builder);
}
#endif

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
            case osmium::item_type::node : {
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
    osmium::Location location{9.3, 49.7};
    std::cout << location << "\n";
    osmium::Box box;
    box.extend(osmium::Location{3.2, 4.3});
    box.extend({4.5, 7.2});
    box.extend({3.3, 8.9});
    std::cout << box << "\n";  // (3.2,4.3,4.5,8.9)

    const int buffer_size = 10240;
    osmium::memory::Buffer node_buffer{buffer_size, osmium::memory::Buffer::auto_grow::yes};

    {
        /* 
         * using the buffer
         */
        osmium::builder::NodeBuilder builder{node_buffer};
        builder.set_user("foo");
        osmium::Node& obj = builder.object();
        obj.set_id(1);
        obj.set_version(1);
        obj.set_changeset(5);
        obj.set_uid(140);
        obj.set_timestamp("2016-01-05T01:22:45Z");
        obj.set_location(osmium::Location{9.0, 49.0});
        add_tags(node_buffer, &builder);
        std::cout << "id=" << obj.id()
            << " version=" << obj.version()
            << " timestamp=" << obj.timestamp()
            << " visible=" << (obj.visible() ? "true" : "false")
            << " changeset=" << obj.changeset()
            << " uid=" << obj.uid()
            << " user=" << obj.user() << "\n";
        for (const auto& tag : obj.tags()) {
            std::cout << tag.key() << '=' << tag.value() << '\n';
        }
        const char* highway = obj.tags().get_value_by_key("amenity");
        if (highway && !std::strcmp(highway, "restaurant")) {
            std::cout << "found tag\n";
        } else {
            std::cout << "NOT found tag\n";
        }


    }
    node_buffer.commit();
    //    std::cout << node_buffer << "\n";

    osmium::memory::Buffer way_buffer{buffer_size, osmium::memory::Buffer::auto_grow::yes};
    build_way(way_buffer);
    way_buffer.commit();

    osmium::io::File input_file{"../../../tools/data/restrictions.osm"}; // OSM format
    osmium::io::Reader reader{input_file};
    osmium::io::Header header = reader.header();
    // std::cout << header << "\n";

    osmium::memory::Buffer buffer;
    osmium::io::Writer writer{"output.osm"};

    if (osmium::memory::Buffer buffer = reader.read()) {
        for (auto& item : buffer) {
            std::cout << item.type() << "\n";
        }

        writer(std::move(buffer));
    }

    writer.close();
#endif

    auto otypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way;
    osmium::io::Reader reader{"../../../tools/data/restrictions.osm", otypes};
    namespace map = osmium::index::map;
    using index_type = map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

    index_type index;
    location_handler_type location_handler{index};

    MyHandler handler;
    osmium::apply(reader, location_handler, handler);

    osmium::relations::RelationMeta relation_meta;
    MyRelCollector colector;
    colector.complete_relation(relation_meta);

    reader.close();
}
