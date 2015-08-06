cmake -H. -Bbuild
cd build/
make
sudo make install
#cd ..
cd ../tools/data
dropdb inputdatatest
createdb inputdatatest
psql -c "create extension postgis" inputdatatest
psql -c "create extension pgrouting" inputdatatest
osm2pgrouting -f small.osm -d inputdatatest
osm2pgrouting -f small.osm -d inputdatatest
#osm2pgrouting -f sampledataSE.osm -d inputdatatest
#osm2pgrouting -f sampledataSW.osm -d inputdatatest
#osm2pgrouting -f sampledataNE.osm -d inputdatatest
#osm2pgrouting -f sampledataNW.osm -d inputdatatest

cd ../../
