#!/usr/bin/env bash
# Ressources système (WSL2) utiles au suivi du projet.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "Ressources système — $(date '+%Y-%m-%d %H:%M')"
read -r l1 l5 l15 _ < /proc/loadavg
echo "  CPU     : charge $l1 / $l5 / $l15 (1/5/15 min) sur $(nproc) cœurs"
free -m | awk '/^Mem:/  {printf "  Mémoire : %d / %d Mo utilisés (%d %%), %d Mo disponibles\n", $3, $2, $3*100/$2, $7}
               /^Swap:/ {if ($2>0) printf "  Swap    : %d / %d Mo\n", $3, $2}'
df -h "$ROOT" | awk 'NR==2 {printf "  Disque  : %s / %s utilisés (%s), %s libres\n", $3, $2, $5, $4}'
if command -v docker >/dev/null && docker info >/dev/null 2>&1; then
  n=$(docker ps -q | wc -l)
  echo "  Docker  : actif, $n conteneur(s) en cours"
  docker ps --format '            - {{.Image}} ({{.Status}})' 2>/dev/null
else
  echo "  Docker  : INACCESSIBLE"
fi
pgrep -fa 'blastem' >/dev/null && echo "  BlastEm : en cours d'exécution" || echo "  BlastEm : arrêté"
echo "  Top processus (CPU) :"
ps -eo pcpu,pmem,comm --sort=-pcpu | sed -n '2,4p' | awk '{printf "            %5s %% CPU  %5s %% RAM  %s\n", $1, $2, $3}'
