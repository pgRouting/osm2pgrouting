#!sh

exit(0);

# https://github.com/GeographicaGS/osm-itinera/blob/master/itinera/const.py#L43
BBOX="1.97180,41.26684,2.26478,41.55818"
wget --progress=dot:mega -O "bcn.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"

BBOX="13.15,47.24,13.52,47.45"
wget --progress=dot:mega -O "restrictions.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"

#query to get germany restrictions
#(53.5,11.0,54.5,11.5)
wget --progress=dot:mega -O "germany_restrictions.osm" "http://overpass-api.de/api/interpreter?data=(relation(53.5,11.0,54.5,11.5)[\"type\"~\"^restriction\"];);(._;>;);out;"

#query to get world restrictions
#times out does not work well
wget --progress=dot:mega -O "world_restrictions.osm" "http://overpass-api.de/api/interpreter?data=(relation[\"type\"~\"^restriction\"];);(._;>;);out;"


BBOX="-122.8,45.4,-122.5,45.6"
wget --progress=dot:mega -O "sampledata.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"
BBOX="-122.65,45.5, -122.5,45.6"
wget --progress=dot:mega -O "sampledataNE.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"
BBOX="-122.65,45.4, -122.5,45.5"
wget --progress=dot:mega -O "sampledataSE.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"
BBOX="-122.8,45.5, -122.65,45.6"
wget --progress=dot:mega -O "sampledataNW.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"
BBOX="-122.8,45.4, -122.65,45.5"
wget --progress=dot:mega -O "sampledataSW.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"

BBOX="-122.65,45.4, -122.6,45.45"
wget --progress=dot:mega -O "small.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"

BBOX="4.2878,50.8139,4.5023,50.8926"
wget --progress=dot:mega -O "brussels.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"


<osm-script>
    <union>
        <query type="relation">
            <bbox-query e="13.52" n="47.45" s="47.34" w="13.15"/>
            <has-kv k="type" v="restriction"/>
        </query>

    </union>
    <union>
        <item/>
        <recurse type="down"/>
    </union>
    <print/>
</osm-script>
