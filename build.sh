#!/bin/sh
# Build Ego: Repton 4 from your own copy of the disc:
#     ./build.sh "EGO - Repton 4 (1992)(Superior Software).adf"
# then: cd recomp && ./repton4
# The steps are recipe.json's "build", run by SuperSpider's tools/weave.py.
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
SUPERSPIDER="${SUPERSPIDER:-$HOME/stuff/superspider}"
[ $# -gt 0 ] || { echo "usage: $0 DISC.adf | GAME.zip" >&2; exit 2; }
(cd "$SUPERSPIDER" && dune build)
WEAVE_RECIPES="$HERE" exec python3 -I "$SUPERSPIDER/tools/weave.py" build "$@"
