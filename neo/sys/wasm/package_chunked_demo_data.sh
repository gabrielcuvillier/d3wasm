#!/usr/bin/env bash
python3 $(dirname $(which emcc))/tools/file_packager.py demo_bootstrap.data --preload $1/demo_bootstrap.pk4@/usr/local/share/d3wasm/base/demo_bootstrap.pk4 --js-output=demo_bootstrap.js --use-preload-cache --no-heap-copy
python3 $(dirname $(which emcc))/tools/file_packager.py demo_game00.data --preload $1/demo_game00.pk4@/usr/local/share/d3wasm/base/demo_game00.pk4 --js-output=demo_game00.js --use-preload-cache --no-heap-copy
python3 $(dirname $(which emcc))/tools/file_packager.py demo_game01.data --preload $1/demo_game01.pk4@/usr/local/share/d3wasm/base/demo_game01.pk4 --js-output=demo_game01.js --use-preload-cache --no-heap-copy
