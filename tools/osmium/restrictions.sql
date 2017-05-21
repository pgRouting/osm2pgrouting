DROP TABLE if EXISTS osm_restrictions;
CREATE TABLE osm_restrictions(
    osm_id BIGINT PRIMARY KEY,
    restriction TEXT,
    osm_from BIGINT,
    osm_to BIGINT,
    osm_via BIGINT,
    type_via char,
    attributes hstore,
    tags hstore);

COPY osm_restrictions (
    osm_id, restriction,
    osm_from, osm_to, osm_via, type_via,
    attributes, tags) FROM STDIN; 

