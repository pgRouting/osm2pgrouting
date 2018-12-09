# How to release a new version

This document explains step by step how to release a new version for osm2pgrouting.


## Steps to follow
0. Make sure that the NEWS file has the changes for the release. (review issues closed in milestone).

1. Clone branch master:

  ```
  $ git clone git@github.com:pgRouting/osm2pgrouting.git
  ```

2. Make sure you are in the last update:

  ```
  $ git pull origin master
  ```

3. Make sure last updates was compiled without errors in Travis CI: https://travis-ci.org/pgRouting/osm2pgrouting/builds

4. Make the tag for this version (change version number from example):

  ```
  $ git tag -a -m "Create osm2pgrouting  v2.3.4 tag" v2.3.4
  ```

5. Push the tag (change version number from example):

  ```
  $ git push origin v2.3.4
  ```
  
6. Go to Github repository and make sure the new tag was created: https://github.com/pgRouting/osm2pgrouting/releases

7. Click on the tag number then click on the edit tag button for release title (use the same number, i.e. v2.3.4).

8. Write comments about changes introduced by this new release (review issues closed in milestone or the NEWS file). Click on update release button.

9. Close milestone.

10. Prepare next release:
  - Create new milestone.
  - Update version in this file: https://github.com/pgRouting/osm2pgrouting/blob/master/src/osm_elements/osm2pgrouting.cpp#L101.
