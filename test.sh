#!/usr/bin/env bash
# =============================================================================
# test.sh - spustí všechny json-examples s přepínačem -v a vypíše výsledky
# Použití: ./test.sh
# Proměnná JSON_DIR níže určuje adresář s JSON soubory.
# =============================================================================


# Testy pro validaci uživatelského vstupu.
# Vygenerováno s pomocí Claude Sonnet 4.6 (Anthropic) dne 21.4.2026.
# Prompt: "Vytvoř testy pro json-examples odděleně pro soubory začínající číslem 1,2 atd..."
# následně byly specifikovány požadavky na testované jazyky pro každou skupinu testů



JSON_DIR="json-examples"
AUTOMATON="./automaton"

# Barvy (vypnou se automaticky pokud stdout není terminál)
if [ -t 1 ]; then
    BOLD=$'\033[1m'; RESET=$'\033[0m'; CYAN=$'\033[36m'; GREEN=$'\033[32m'
    YELLOW=$'\033[33m'; RED=$'\033[31m'; DIM=$'\033[2m'
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
    local IFS=','; echo "${parts[*]}"
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
print(','.join(parts))
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
    # parsujeme: "Configurations visited: N (pop: P, expand: E)"
    cfgline=$(echo "$err" | grep "Configurations visited:")
    configs=$(echo "$cfgline" | grep -oP "(?<=visited: )\d+")
    pop=$(echo    "$cfgline" | grep -oP "(?<=pop: )\d+")
    expand=$(echo "$cfgline" | grep -oP "(?<=expand: )\d+")

    # Formátování výsledku
    if [[ "$verdict" == "ACCEPT" ]]; then
        local vstatus="${GREEN}ACCEPT${RESET}"
    elif [[ "$verdict" == *"limit"* ]]; then
        local vstatus="${RED}REJECT (limit)${RESET}"
    else
        local vstatus="${YELLOW}REJECT${RESET}"
    fi

    printf "  ${DIM}%-30s${RESET}  %-28s  %s  configs: ${BOLD}%s${RESET} ${DIM}(pop: %s, expand: %s)${RESET}\n" \
        "$name" "$desc" "$vstatus" "$configs" "$pop" "$expand"
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
echo -e "${BOLD}║           Simulátor HZA – souhrnný test json-examples                ║${RESET}"
echo -e "${BOLD}╚══════════════════════════════════════════════════════════════════════╝${RESET}"

# -----------------------------------------------------------------------------
section "1.1-HZA  ...  1.4-VRCPHZA   │  L = { a^n b^n c^n | n ≥ 1 }"
# -----------------------------------------------------------------------------
for n in 1 2 3 4 5 6 7 8 9 10; do
    inp=$(repeat_sym a $n b $n c $n)
    run "$JSON_DIR/1.1-HZA.json"  "n=$n  $inp" "$inp"
    run "$JSON_DIR/1.2-LHZA.json" "n=$n  $inp" "$inp"
    run "$JSON_DIR/1.3-CPHZA.json" "n=$n  $inp" "$inp"
    run "$JSON_DIR/1.4-VRCPHZA.json" "n=$n  $inp" "$inp"
done

# -----------------------------------------------------------------------------
section "2.1-CPHZA  &  2.2-VRCPHZA  &  2.3-HZA   │  L = { a^n b^m c^n d^m | n,m ≥ 1 }"
# -----------------------------------------------------------------------------
for n in 1 2 3 4 5 6 7 8; do
    for m in 1 2 3 4 5 6 7 8; do
        inp=$(repeat_sym a $n b $m c $n d $m)
        run "$JSON_DIR/2.1-CPHZA.json"   "n=$n m=$m  $inp" "$inp"
        run "$JSON_DIR/2.2-VRCPHZA.json" "n=$n m=$m  $inp" "$inp"
        run "$JSON_DIR/2.3-HZA.json" "n=$n m=$m  $inp" "$inp"
    done
done

# -----------------------------------------------------------------------------
section "3.1-CPHZA-expr  &  3.2-VRCPHZA-expr   │  výrazy (l literálů, střídání +/*)"
# -----------------------------------------------------------------------------
for l in 1 2 3 4 5 6 7 8 9 10; do
    inp=$(gen_expr $l)
    run "$JSON_DIR/3.1-CPHZA-expr.json"   "l=$l  $inp" "$inp"
    run "$JSON_DIR/3.2-VRCPHZA-expr.json" "l=$l  $inp" "$inp"
done

# -----------------------------------------------------------------------------
section "4.1-LHZA-indexed  ...  4.5-HZA-noindex-both   │  L = { a^n b^n c^n d^n e^n f^n | n ≥ 1 }"
# -----------------------------------------------------------------------------
for n in 1 2 3 4 5 6 7 8 9 10; do
    inp=$(repeat_sym a $n b $n c $n d $n e $n f $n)
    for json in \
        "$JSON_DIR/4.1-LHZA-indexed.json" \
        "$JSON_DIR/4.2-HZA-indexed.json" \
        "$JSON_DIR/4.3-HZA-noindex-NT.json" \
        "$JSON_DIR/4.4-HZA-noindex-Q.json" \
        "$JSON_DIR/4.5-HZA-noindex-both.json"
    do
        run "$json" "n=$n  $inp" "$inp"
    done
    echo ""   # prázdný řádek mezi hodnotami n pro přehlednost
done

echo ""
echo -e "${DIM}Hotovo.${RESET}"
echo ""