--
-- PostgreSQL database dump
--

-- Dumped from database version 9.2.3
-- Dumped by pg_dump version 9.2.3
-- Started on 2013-07-11 12:04:44 CEST

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_with_oids = false;

--
-- TOC entry 206 (class 1259 OID 23487)
-- Name: network_rules; Type: TABLE; Schema: public; Owner: jchen
--

CREATE TABLE network_rules (
    key character varying,
    type character varying,
    include integer,
    id integer DEFAULT nextval('network_type_rules_id_seq'::regclass) NOT NULL,
    value character varying
);


ALTER TABLE public.network_rules OWNER TO jchen;

--
-- TOC entry 3671 (class 0 OID 23487)
-- Dependencies: 206
-- Data for Name: network_rules; Type: TABLE DATA; Schema: public; Owner: jchen
--

INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560899, 'service');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560864, 'service');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560815, 'motorway');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560816, 'motorway_link');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560879, 'trunk');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560880, 'primary');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560881, 'pedestrian');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560882, 'bridleway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560883, 'secondary_link');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560884, 'tertiary');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560885, 'mini_roundabout');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560830, 'motorway_junction');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560831, 'trunk');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560832, 'trunk_link');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560833, 'primary');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560834, 'primary_link');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560835, 'secondary');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560836, 'secondary_link');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560837, 'tertiary');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560838, 'residential');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560839, 'living_street');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560840, 'service');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560841, 'track');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560842, 'byway');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560843, 'unclassified');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560844, 'road');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560886, 'primary_link');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560887, 'residential');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560888, 'motorway_link');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560889, 'cycleway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560890, 'secondary');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560891, 'living_street');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560892, 'bus_guideway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560893, 'track');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560894, 'ford');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560895, 'motorway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560896, 'tertiary_link');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560898, 'trunk');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560900, 'path');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560901, 'trunk_link');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560902, 'byway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560903, 'rest_area');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560904, 'footway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560905, 'unclassified');
INSERT INTO network_rules VALUES ('highway', 'bike', 0, 560906, 'steps');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560907, 'crossing');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560908, 'raceway');
INSERT INTO network_rules VALUES ('highway', 'bike', 1, 560909, 'road');
INSERT INTO network_rules VALUES ('junction', 'bike', 1, 560897, 'roundabout');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560846, 'motorway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560847, 'motorway_link');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560848, 'trunk');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560849, 'trunk_link');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560869, 'footway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560870, 'bridleway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560850, 'primary');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560851, 'primary_link');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560852, 'secondary');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560853, 'secondary_link');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560854, 'tertiary');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560855, 'tertiary_link');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560856, 'residential');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560857, 'unclassified');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560858, 'road');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560859, 'living_street');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560860, 'service');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560861, 'track');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560862, 'pedestrian');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560863, 'raceway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560865, 'rest_area');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560866, 'bus_guideway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560867, 'path	');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560868, 'cycleway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560871, 'byway');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560872, 'steps');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560873, 'mini_roundabout');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560874, 'crossing');
INSERT INTO network_rules VALUES ('junction', 'walk', 1, 560875, 'roundabout');
INSERT INTO network_rules VALUES ('highway', 'walk', 1, 560876, 'ford');
INSERT INTO network_rules VALUES ('junction', 'car', 1, 560877, 'roundabout');
INSERT INTO network_rules VALUES ('highway', 'car', 1, 560878, 'ford');
INSERT INTO network_rules VALUES ('railway', 'train', 1, 560910, 'rail');
INSERT INTO network_rules VALUES ('railway', 'train', 1, 560911, 'narrow_gauge');


--
-- TOC entry 3668 (class 2606 OID 35967)
-- Name: network_rules_id_key; Type: CONSTRAINT; Schema: public; Owner: jchen
--

ALTER TABLE ONLY network_rules
    ADD CONSTRAINT network_rules_id_key UNIQUE (id);


--
-- TOC entry 3670 (class 2606 OID 35981)
-- Name: pkey_network_rules_id; Type: CONSTRAINT; Schema: public; Owner: jchen
--

ALTER TABLE ONLY network_rules
    ADD CONSTRAINT pkey_network_rules_id PRIMARY KEY (id);


-- Completed on 2013-07-11 12:04:47 CEST

--
-- PostgreSQL database dump complete
--

