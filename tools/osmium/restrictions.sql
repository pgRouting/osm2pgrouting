
BEGIN;
--DROP TABLE if EXISTS foo.osm_restrictions;

--
-- osm2pgRouting osm_restrictions dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: osm_restrictions; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE IF NOT EXISTS foo.osm_restrictions(
    osm_id BIGINT PRIMARY KEY,
    restriction TEXT,
    osm_from BIGINT,
    osm_to BIGINT,
    osm_via BIGINT,
    type_via char,
    attributes hstore,
    tags hstore);

-- On a created table the temporary table is not needed
CREATE TEMP TABLE __osm_restrictions_tmp
ON COMMIT DROP
AS
SELECT *
FROM foo.osm_restrictions
WITH NO DATA;


-- On a created table Use the original table
COPY __osm_restrictions_tmp (
    osm_id, restriction,
    osm_from, osm_to, osm_via, type_via,
    attributes, tags) FROM STDIN;
