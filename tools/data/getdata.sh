BBOX="13.15,47.33,13.43,47.45"
wget --progress=dot:mega -O "restrictions.osm" "http://www.overpass-api.de/api/xapi?*[bbox=${BBOX}][@meta]"

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
