#!/usr/bin/env bash
# =============================================================================
# test.sh - spustí všechny json-examples s přepínačem -v a vypíše výsledky
# Použití: ./test.sh
# Proměnná JSON_DIR níže určuje adresář s JSON soubory.
# =============================================================================
 
JSON_DIR="json-examples"        # <-- změňte dle potřeby
AUTOMATON="./automaton"
 
# Barvy (vypnou se automaticky pokud stdout není terminál)
if [ -t 1 ]; then
    BOLD="\033[1m"; RESET="\033[0m"; CYAN="\033[36m"; GREEN="\033[32m"
    YELLOW="\033[33m"; RED="\033[31m"; DIM="\033[2m"
else
    BOLD=""; RESET=""; CYAN=""; GREEN=""; YELLOW=""; RED=""; DIM=""
fi

# -----------------------------------------------------------------------------
# Pomocné funkce
# -----------------------------------------------------------------------------
 
# Generuje {a,a,...,b,b,...} pro opakující se skupiny symbolů
# Použití: repeat_sym sym1 n1 sym2 n2 ...
repeat_sym() {
    local parts=()
    while [ $# -ge 2 ]; do
        local sym=$1; local count=$2; shift 2
        for ((i=0; i<count; i++)); do parts+=("$sym"); done
    done
    local IFS=','; echo "{${parts[*]}}"
}
 
# Generuje výraz se střídajícími se operátory: l, l+l, l+l*l, l+l*l+l, ...
gen_expr() {
    python3 -c "
count=$1
ops=['+','*','+','*','+','*','+','*']
parts=['l']
for i in range(count-1):
    parts.append(ops[i % len(ops)])
    parts.append('l')
print('{' + ','.join(parts) + '}')
"
}
 
# Spustí automat a vytiskne řádek výsledku
# run <json-soubor> <popis-vstupu> <vstup>
run() {
    local json="$1"; local desc="$2"; local input="$3"
    local name
    name=$(basename "$json")
 
    # spustíme s -v; výsledek (ACCEPT/REJECT) jde na stdout, count na stderr
    local out err verdict configs
    out=$("$AUTOMATON" -v "$json" "$input" 2>/tmp/_test_err.txt)
    err=$(cat /tmp/_test_err.txt)
 
    verdict=$(echo "$out" | tail -1)          # ACCEPT nebo REJECT (limit reached)
    configs=$(echo "$err" | grep -o '[0-9]*') # číslo z "Configurations visited: N"
 
    # Formátování výsledku
    if [[ "$verdict" == "ACCEPT" ]]; then
        local vstatus="${GREEN}ACCEPT${RESET}"
    elif [[ "$verdict" == *"limit"* ]]; then
        local vstatus="${RED}REJECT (limit)${RESET}"
    else
        local vstatus="${YELLOW}REJECT${RESET}"
    fi
 
    printf "  ${DIM}%-30s${RESET}  %-28s  %s  configs: ${BOLD}%s${RESET}\n" \
        "$name" "$desc" "$vstatus" "$configs"
}
 
# Tiskne nadpis sekce
section() {
    echo ""
    echo -e "${CYAN}${BOLD}### $* ${RESET}"
    echo -e "${DIM}$(printf '─%.0s' {1..72})${RESET}"
}
 
# =============================================================================
# TESTY
# =============================================================================
 
echo ""
echo -e "${BOLD}╔══════════════════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║           Simulátor HZA – souhrnný test json-examples               ║${RESET}"
echo -e "${BOLD}╚══════════════════════════════════════════════════════════════════════╝${RESET}"
 
# -----------------------------------------------------------------------------
section "1-HZA  &  4-LHZA   │  L = { a^n b^n c^n | n ≥ 1 }"
# -----------------------------------------------------------------------------
for n in 1 2 3 4 5; do
    inp=$(repeat_sym a $n b $n c $n)
    run "$JSON_DIR/1-HZA.json"  "n=$n  $inp" "$inp"
    run "$JSON_DIR/4-LHZA.json" "n=$n  $inp" "$inp"
done
 
# -----------------------------------------------------------------------------
section "2-CPHZA  &  3-VRCPHZA   │  L = { a^n b^m c^n d^m | n,m ≥ 1 }"
# -----------------------------------------------------------------------------
for n in 1 2 3 4; do
    for m in 1 2 3 4; do
        inp=$(repeat_sym a $n b $m c $n d $m)
        run "$JSON_DIR/2-CPHZA.json"   "n=$n m=$m  $inp" "$inp"
        run "$JSON_DIR/3-VRCPHZA.json" "n=$n m=$m  $inp" "$inp"
    done
done
 
# -----------------------------------------------------------------------------
section "5-CPHZA-expr  &  6-VRCPHZA-expr   │  výrazy (l literálů, střídání +/*)"
# -----------------------------------------------------------------------------
for l in 1 2 3 4 5; do
    inp=$(gen_expr $l)
    run "$JSON_DIR/5-CPHZA-expr.json"   "l=$l  $inp" "$inp"
    run "$JSON_DIR/6-VRCPHZA-expr.json" "l=$l  $inp" "$inp"
done
 
# -----------------------------------------------------------------------------
section "7-LHZA-indexed  …  11-HZA-noindex-both   │  L = { a^n b^n c^n d^n e^n f^n | n ≥ 1 }"
# -----------------------------------------------------------------------------
for n in 1 2 3 4 5; do
    inp=$(repeat_sym a $n b $n c $n d $n e $n f $n)
    for json in \
        "$JSON_DIR/7-LHZA-indexed.json" \
        "$JSON_DIR/8-HZA-indexed.json" \
        "$JSON_DIR/9-HZA-noindex-NT.json" \
        "$JSON_DIR/10-HZA-noindex-Q.json" \
        "$JSON_DIR/11-HZA-noindex-both.json"
    do
        run "$json" "n=$n  $inp" "$inp"
    done
    echo ""
done
 
echo ""
echo -e "${DIM}Hotovo.${RESET}"
echo ""