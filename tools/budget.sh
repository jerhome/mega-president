#!/usr/bin/env bash
# Vérifie les budgets matériels de la ROM compilée.
#  - ROM <= 2 Mo : au-delà, elle chevaucherait la SRAM mappée à 0x200000.
#  - RAM statique (variables globales) <= 32 Ko : le reste des 64 Ko sert
#    au tas de SGDK (MEM_alloc) et à la pile.
#  - Aucun float/double dans src/ (68000 sans FPU).
set -u
cd "$(dirname "$0")/.."
ROM=out/rom.bin SYM=out/symbol.txt
ROM_MAX=$((2 * 1024 * 1024)) RAM_MAX=$((32 * 1024))
fail=0

[ -f "$ROM" ] && [ -f "$SYM" ] || { echo "❌ ROM absente : lancer make d'abord"; exit 1; }

rom=$(stat -c %s "$ROM")
if [ "$rom" -le "$ROM_MAX" ]; then s=✅; else s=❌; fail=1; fi
printf '%s ROM          : %4d Ko / %d Ko\n' "$s" $((rom / 1024)) $((ROM_MAX / 1024))

bend=$(awk '$3 == "_bend" {print $1; exit}' "$SYM")
ram=$(( 0x$bend - 0xE0FF0000 ))
if [ "$ram" -le "$RAM_MAX" ]; then s=✅; else s=❌; fail=1; fi
printf '%s RAM statique : %4d o  / %d Ko\n' "$s" "$ram" $((RAM_MAX / 1024))

# (les lignes de commentaire // sont ignorées)
fp=$(grep -rnwE 'float|double' src --include='*.c' --include='*.h' --exclude-dir=content | grep -vE '^[^:]+:[0-9]+:\s*//')
if [ -n "$fp" ]; then
    echo "❌ float/double trouvés :"; echo "$fp"; fail=1
else
    echo "✅ Aucun float/double dans src/"
fi
exit $fail
