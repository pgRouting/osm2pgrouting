cmake -H. -Bbuild
cd build/
make
sudo make install
#cd ..
cd ../../
dropdb inputdatatest
createdb inputdatatest
psql -c "create extension postgis" inputdatatest
psql -c "create extension pgrouting" inputdatatest
osm2pgrouting -f small.osm -d inputdatatest
cd vicky-osm2pgr
