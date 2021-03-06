emcmake cmake -DCMAKE_C_FLAGS="-Oz" -DAPPLY_EMSCRIPTEN_HACKS=ON -DSONAME=OFF -DUSE_HTTPS=OFF -DBUILD_SHARED_LIBS=OFF -DTHREADSAFE=OFF -DBUILD_CLAR=OFF -DUSE_SSH=OFF ..
emcmake cmake -build .
emmake make
echo "building libgit2.js"
emcc -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s FORCE_FILESYSTEM=1 --post-js libgit2init.js -s "EXTRA_EXPORTED_RUNTIME_METHODS=['FS']" -Oz jslibgit2.c libgit2.a -Isrc -I../src -I../include -o libgit2.js
