#!/bin/bash
# Captures d'écran automatiques des ROM de démonstration (tâche 4.1.01).
#
#   tools/shots.sh [options] DEMO[:SCRIPT] ...
#     -t "2 5 9"   instants de capture, en secondes depuis le démarrage de l'émulateur
#                  (BlastEm tourne en temps réel : 1 s = 50 ou 60 images). Défaut : "3"
#     -n           NTSC (défaut : PAL)
#     -k           ne pas recompiler (réutilise out/demo_<n>_<script>.bin s'il existe)
#     -o DIR       dossier de sortie (défaut : out/shots)
#   Exemples :  tools/shots.sh -t "2 4" 11 3 12
#               tools/shots.sh -t "6" "1:w15 S w6 A"
#
# Sortie : DIR/<demo>_<instant>s.png (320x240 : l'image de la fenêtre BlastEm,
# 640x480, réduite exactement par 2 sans lissage).
#
# Méthode : un serveur Xvfb privé par exécution (aucune autre fenêtre ne peut
# polluer la capture), capture par identifiant de fenêtre (xwd -id), arrêt par PID.
cd "$(dirname "$0")/.." || exit 1

INSTANTS="3"; REGION=E; BUILD=1; OUT=out/shots
while getopts "t:nko:" o; do
    case $o in
        t) INSTANTS=$OPTARG ;;
        n) REGION=U ;;
        k) BUILD=0 ;;
        o) OUT=$OPTARG ;;
        *) exit 2 ;;
    esac
done
shift $((OPTIND-1))
[ $# -gt 0 ] || { sed -n 2,15p "$0"; exit 2; }

BLASTEM=tools/blastem64-0.6.2/blastem
[ -x "$BLASTEM" ] || { echo "BlastEm absent : make blastem" >&2; exit 1; }
mkdir -p "$OUT" out
[ "$REGION" = U ] && SUF=_ntsc || SUF=""

XPID=""; BPID=""
cleanup() {
    for p in $BPID $XPID; do
        [ -n "$p" ] && kill "$p" 2>/dev/null
    done
    sleep 0.3
    for p in $BPID $XPID; do
        [ -n "$p" ] && kill -9 "$p" 2>/dev/null
    done
    BPID=""; XPID=""
}
trap cleanup EXIT INT TERM

# Numéro d'affichage libre
D=$((200 + $$ % 700))
while [ -e /tmp/.X11-unix/X$D ]; do D=$((D+1)); done
Xvfb :$D -screen 0 640x480x24 >/dev/null 2>&1 &
XPID=$!
sleep 1
export DISPLAY=:$D SDL_VIDEODRIVER=x11 SDL_AUDIODRIVER=dummy

status=0
for spec in "$@"; do
    demo=${spec%%:*}
    script=""; [ "$spec" != "$demo" ] && script=${spec#*:}
    tag=$(printf '%s' "$script" | tr -c 'A-Za-z0-9' '_')
    rom="out/demo_${demo}${tag:+_$tag}.bin"
    if [ "$BUILD" = 1 ] || [ ! -f "$rom" ]; then
        make -s rom-demo DEMO="$demo" SCRIPT="$script" >/dev/null 2>&1 || { echo "échec compilation démo $demo" >&2; status=1; continue; }
        cp out/demo.bin "$rom"
    fi
    "$BLASTEM" -r $REGION "$rom" >/dev/null 2>&1 &
    BPID=$!
    t0=$(date +%s.%N)
    wid=""
    for _ in $(seq 50); do
        wid=$(xwininfo -root -tree | awk '/BlastEm/ {print $1; exit}')
        [ -n "$wid" ] && break
        sleep 0.1
    done
    [ -n "$wid" ] || { echo "fenêtre BlastEm introuvable (démo $demo)" >&2; cleanup; status=1; continue; }
    for t in $INSTANTS; do
        now=$(date +%s.%N)
        wait_s=$(awk -v t="$t" -v n="$now" -v t0="$t0" 'BEGIN { w = t - (n - t0); print (w > 0 ? w : 0) }')
        sleep "$wait_s"
        f="$OUT/${demo}${tag:+_$tag}_${t}s${SUF}.png"
        xwd -id "$wid" -silent | convert xwd:- -filter point -resize 50% "$f" && echo "$f"
    done
    kill "$BPID" 2>/dev/null; sleep 0.3; kill -9 "$BPID" 2>/dev/null; wait "$BPID" 2>/dev/null; BPID=""
done
exit $status
