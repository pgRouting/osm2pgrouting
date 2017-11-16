
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
2654080	'no_right_turn'	30513235	30513221	336812979	n	version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>"Schermy"	except=>hgv,restriction=>no_right_turn,type=>restriction 
2801218	'no_left_turn'	51392054	52385458	666177031	n	version=>1,timestamp=>2013-03-04T22:08:50Z,changeset=>15252425,uid=>191979,user=>"It''s so funny"	restriction=>no_left_turn,type=>restriction 
4185815	'no_u_turn'	71735115	71735116	257680585	n	version=>1,timestamp=>2014-11-11T20:23:20Z,changeset=>26721235,uid=>2675,user=>"Eckhart Wörner"	restriction=>no_u_turn,type=>restriction 
2654079	'no_right_turn'	93448457	93448446	75736605	n	version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>"Schermy"	restriction=>no_right_turn,type=>restriction 
4610880	'no_u_turn'	103233043	30979095	1546525085	n	version=>1,timestamp=>2015-02-20T22:05:31Z,changeset=>28987840,uid=>191979,user=>"It''s so funny"	restriction=>no_u_turn,type=>restriction 
1810089	'only_right_turn'	117539021	117539029	1323971411	n	version=>1,timestamp=>2011-10-25T17:34:28Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1810048	'only_right_turn'	123764395	134667165	283842577	n	version=>2,timestamp=>2012-12-22T17:01:51Z,changeset=>14368535,uid=>381316,user=>"Schermy"	hour_off=>20:00,hour_on=>06:00,restriction=>only_right_turn,type=>restriction 
1810047	'only_right_turn'	32479918	134667172	365181022	n	version=>1,timestamp=>2011-10-25T17:00:11Z,changeset=>9653862,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1811355	'only_straight_on'	134667169	134667172	365181022	n	version=>1,timestamp=>2011-10-26T16:57:11Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1810084	'only_right_turn'	30512516	134667174	75740064	n	version=>1,timestamp=>2011-10-25T17:34:28Z,changeset=>9654142,uid=>381316,user=>"Schermy"	hour_off=>20:00,hour_on=>06:00,restriction=>only_right_turn,type=>restriction 
1810046	'only_right_turn'	134667259	134667174	1480288168	n	version=>2,timestamp=>2013-02-05T21:28:19Z,changeset=>14927175,uid=>62882,user=>"friedl"	restriction=>only_right_turn,type=>restriction 
1810042	'only_straight_on'	134667267	32479917	256343966	n	version=>1,timestamp=>2011-10-25T17:00:10Z,changeset=>9653862,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1810071	'no_left_turn'	134669690	117553923	1324080118	n	version=>1,timestamp=>2011-10-25T17:34:25Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>no_left_turn,type=>restriction 
1810091	'only_right_turn'	117553923	134669690	1324080118	n	version=>1,timestamp=>2011-10-25T17:34:29Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1810082	'only_straight_on'	134669693	117539029	1323971411	n	version=>1,timestamp=>2011-10-25T17:34:27Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1810072	'no_left_turn'	134669694	30512479	1323917172	n	version=>2,timestamp=>2012-12-02T13:12:28Z,changeset=>14124226,uid=>381316,user=>"Schermy"	restriction=>no_left_turn,type=>restriction 
1810090	'only_right_turn'	30512479	134669694	1323917172	n	version=>2,timestamp=>2012-12-02T13:12:28Z,changeset=>14124226,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1810078	'only_straight_on'	134669695	134669693	75740080	n	version=>1,timestamp=>2011-10-25T17:34:27Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1810080	'only_straight_on'	134669695	134669698	1323971156	n	version=>1,timestamp=>2011-10-25T17:34:27Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1810077	'only_straight_on'	134669697	134669698	75740076	n	version=>1,timestamp=>2011-10-25T17:34:26Z,changeset=>9654142,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
2742485	'only_straight_on'	117531897	134669698	75740076	n	version=>1,timestamp=>2013-02-05T21:28:19Z,changeset=>14927175,uid=>62882,user=>"friedl"	restriction=>only_straight_on,type=>restriction 
1810039	'no_left_turn'	134667165	134792461	283842577	n	version=>3,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>"Schermy"	restriction=>no_left_turn,type=>restriction 
1810044	'only_right_turn'	134792461	98501024	283842577	n	version=>2,timestamp=>2011-10-26T15:38:24Z,changeset=>9661291,uid=>381316,user=>"Schermy"	hour_off=>20:00,hour_on=>06:00,restriction=>only_right_turn,type=>restriction 
1811241	'only_left_turn'	134792472	134792461	1481382061	n	version=>1,timestamp=>2011-10-26T15:38:24Z,changeset=>9661291,uid=>381316,user=>"Schermy"	restriction=>only_left_turn,type=>restriction 
1811350	'only_straight_on'	29979299	134800098	130105922	n	version=>1,timestamp=>2011-10-26T16:57:10Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1811346	'no_left_turn'	27786794	134800098	75740038	n	version=>1,timestamp=>2011-10-26T16:57:10Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>no_left_turn,type=>restriction 
1811348	'only_straight_on'	134800098	134800101	75740038	n	version=>1,timestamp=>2011-10-26T16:57:10Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
2253066	'only_straight_on'	134800104	134800101	1481435308	n	version=>1,timestamp=>2012-06-26T21:17:32Z,changeset=>12031072,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1811353	'only_straight_on'	134800103	134800105	1481435078	n	version=>3,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1811352	'only_straight_on'	134800102	134800107	1481434979	n	version=>1,timestamp=>2011-10-26T16:57:11Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1811354	'only_straight_on'	35684420	134800107	1480288621	n	version=>1,timestamp=>2011-10-26T16:57:11Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_straight_on,type=>restriction 
1811356	'only_right_turn'	134800114	134800101	1481435308	n	version=>1,timestamp=>2011-10-26T16:57:11Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1811359	'only_right_turn'	134800168	134800103	1481435062	n	version=>1,timestamp=>2011-10-26T16:57:12Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1811357	'only_right_turn'	134800161	134800168	1481435048	n	version=>1,timestamp=>2011-10-26T16:57:12Z,changeset=>9661858,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
4610881	'no_u_turn'	135384434	43560928	1191909036	n	version=>1,timestamp=>2015-02-20T22:05:31Z,changeset=>28987840,uid=>191979,user=>"It''s so funny"	restriction=>no_u_turn,type=>restriction 
5837233	'only_right_turn'	140130044	111360011	500897772	n	version=>1,timestamp=>2016-01-08T09:20:21Z,changeset=>36440706,uid=>381316,user=>"Schermy"	except=>hgv;psv,restriction=>only_right_turn,type=>restriction 
1811358	'only_right_turn'	169162876	134800100	1481435234	n	version=>2,timestamp=>2012-06-26T21:17:33Z,changeset=>12031072,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
2654078	'no_left_turn'	184614937	178195372	87383165	n	version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>"Schermy"	except=>hgv,restriction=>no_left_turn,type=>restriction 
2533684	'only_right_turn'	93448446	188075348	75736605	n	version=>1,timestamp=>2012-10-28T14:35:15Z,changeset=>13662010,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
1610805	'only_right_turn'	116138561	192504097	310135708	n	version=>2,timestamp=>2012-11-24T18:55:14Z,changeset=>14017349,uid=>381316,user=>"Schermy"	restriction=>only_right_turn,type=>restriction 
2671756	'only_straight_on'	198931052	198931055	500876392	n	version=>1,timestamp=>2012-12-31T14:04:02Z,changeset=>14477789,uid=>8748,user=>"ToniE"	restriction=>only_straight_on,type=>restriction 
2689202	'no_u_turn'	200145938	23037866	248540238	n	version=>1,timestamp=>2013-01-08T12:02:27Z,changeset=>14573821,uid=>202392,user=>"imagic"	restriction=>no_u_turn,type=>restriction 
2689203	'no_u_turn'	200145940	200145943	656081270	n	version=>1,timestamp=>2013-01-08T12:02:27Z,changeset=>14573821,uid=>202392,user=>"imagic"	restriction=>no_u_turn,type=>restriction 
2691024	'only_straight_on'	200257263	82719424	305106223	n	version=>1,timestamp=>2013-01-09T09:51:54Z,changeset=>14584143,uid=>202392,user=>"imagic"	restriction=>only_straight_on,type=>restriction 
2691023	'only_straight_on'	82719424	200257272	2102544542	n	version=>3,timestamp=>2016-08-01T22:00:01Z,changeset=>41178837,uid=>115042,user=>"Filius Martii"	restriction=>only_straight_on,type=>restriction 
2691022	'only_straight_on'	200257279	82719425	305106228	n	version=>1,timestamp=>2013-01-09T09:51:54Z,changeset=>14584143,uid=>202392,user=>"imagic"	restriction=>only_straight_on,type=>restriction 
2691021	'only_straight_on'	200257283	200257271	1806989866	n	version=>3,timestamp=>2016-08-01T22:00:00Z,changeset=>41178837,uid=>115042,user=>"Filius Martii"	restriction=>only_straight_on,type=>restriction 
2742488	'only_straight_on'	204313935	45066207	2143198454	n	version=>1,timestamp=>2013-02-05T21:28:19Z,changeset=>14927175,uid=>62882,user=>"friedl"	restriction=>only_straight_on,type=>restriction 
2742487	'only_straight_on'	204313943	204313937	1323917118	n	version=>1,timestamp=>2013-02-05T21:28:19Z,changeset=>14927175,uid=>62882,user=>"friedl"	restriction=>only_straight_on,type=>restriction 
1810086	'only_right_turn'	117549965	204313945	1324042037	n	version=>2,timestamp=>2013-02-05T21:28:20Z,changeset=>14927175,uid=>62882,user=>"friedl"	restriction=>only_right_turn,type=>restriction 
2742486	'only_straight_on'	204313938	204313945	2143198462	n	version=>1,timestamp=>2013-02-05T21:28:19Z,changeset=>14927175,uid=>62882,user=>"friedl"	restriction=>only_straight_on,type=>restriction 
1811347	'only_straight_on'	134800100	217941423	75740047	n	version=>3,timestamp=>2013-04-19T10:15:20Z,changeset=>15783262,uid=>45347,user=>"eriosw"	restriction=>only_straight_on,type=>restriction 
2982836	'only_left_turn'	133352940	224511834	1467975033	n	version=>1,timestamp=>2013-06-05T16:54:44Z,changeset=>16434595,uid=>12295,user=>"mapper_07"	restriction=>only_left_turn,type=>restriction 
2982837	'only_right_turn'	224511832	224511835	330076467	n	version=>1,timestamp=>2013-06-05T16:54:44Z,changeset=>16434595,uid=>12295,user=>"mapper_07"	restriction=>only_right_turn,type=>restriction 
4185810	'no_left_turn'	224511836	133352942	1546531682	n	version=>1,timestamp=>2014-11-11T20:23:19Z,changeset=>26721235,uid=>2675,user=>"Eckhart Wörner"	restriction=>no_left_turn,type=>restriction 
2982838	'only_right_turn'	133352941	224511836	1467975041	n	version=>1,timestamp=>2013-06-05T16:54:44Z,changeset=>16434595,uid=>12295,user=>"mapper_07"	restriction=>only_right_turn,type=>restriction 
4185813	'only_straight_on'	224511834	224511837	1546531681	n	version=>1,timestamp=>2014-11-11T20:23:20Z,changeset=>26721235,uid=>2675,user=>"Eckhart Wörner"	restriction=>only_straight_on,type=>restriction 
4185811	'no_right_turn'	133352939	312313392	852677628	n	version=>1,timestamp=>2014-11-11T20:23:20Z,changeset=>26721235,uid=>2675,user=>"Eckhart Wörner"	restriction=>no_right_turn,type=>restriction 
4185814	'only_straight_on'	312313393	32952425	1467975030	n	version=>1,timestamp=>2014-11-11T20:23:20Z,changeset=>26721235,uid=>2675,user=>"Eckhart Wörner"	restriction=>only_straight_on,type=>restriction 
4185812	'only_straight_on'	312313393	312313396	371125631	n	version=>1,timestamp=>2014-11-11T20:23:20Z,changeset=>26721235,uid=>2675,user=>"Eckhart Wörner"	restriction=>only_straight_on,type=>restriction 
\.

-- On a created table the delete is not needed
-- FROM pg >= 9.5 this delete is not needed
DELETE FROM __osm_restrictions_tmp WHERE osm_id IN (SELECT osm_id FROM foo.osm_restrictions);

INSERT INTO foo.osm_restrictions
SELECT *
FROM __osm_restrictions_tmp;
-- FROM pg >= 9.5 this line is needed
-- ON CONFLICT DO NOTHING

-- With if the table had the primary key then it wont create it
DO $$
    BEGIN
        BEGIN
            ALTER TABLE ONLY foo.osm_restrictions
            ADD CONSTRAINT foo.osm_restrictions_pkey PRIMARY KEY (osm_id);
            EXCEPTION
               WHEN OTHERS THEN
                RAISE NOTICE 'Primary key "foo.osm_restrictions_pkey" EXISTS'
                USING HINT = 'postgreSQL code #' ||  SQLSTATE || ': ' || SQLERRM;
        END;
    END
$$;

COMMIT;
