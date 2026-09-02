Tento repozitář obsahuje implementaci vytvořenou v rámci bakalářské práce na téma
[Nové typy hlubokých zásobníkových automatů](https://dspace.vut.cz/items/27071039-f05f-47d1-b8cc-f48015a2b61b).

# Simulátor hloubkových zásobníkových automatů

Simulátor přijímá definici automatu ve formátu JSON a vstupní slovo. Pomocí BFS prohledávání stavového prostoru rozhodne, zda automat dané slovo přijímá.

Podporované typy automatů:
- **HZA** – zásobníkový automat s přístupem pod vrchol zásobníku
- **LHZA** – HZA s pravidly využívajícími lookahead
- **ČPHZA** – HZA s paralelními přepisy na více hloubkách
- **VŘČPHZA** – ČPHZA který při výběru pravidla vyžaduje na vstupu konkrétní symbol

---

## Sestavení

```bash
make
```

Výsledný spustitelný soubor je `./automaton`.

---

## Spuštění

```bash
./automaton [přepínače] <automat.json> <vstup>
```

Vstupní slovo se zadává ve formátu `"symbol-1,symbol-2,..."`, například `"a,a,b,b,c,c"`.

**Přepínače:**

| Přepínač | Popis |
|----------|-------|
| `-v` | Vypíše počet navštívených konfigurací na stderr |
| `-d` | Ladící výpis průběhu BFS na stderr |
| `-l <N>` | Limit navštívených konfigurací (výchozí: 10000) |
| `-h` | Nápověda |

**Příklady:**

```bash
# Přijetí slova a^2 b^2 c^2 automatem HZA
./automaton json-examples/1.1-HZA.json "a,a,b,b,c,c"

# Totéž s výpisem statistik
./automaton -v json-examples/1.1-HZA.json "a,a,b,b,c,c"

# Výpis statistik, výpis průběhu BFS a zvýšení limitu počtu navštívených konfigurací
./automaton -v -d -l 100000 json-examples/1.1-HZA.json "a,a,b,b,c,c"
```

**Výstup** je `ACCEPT` nebo `REJECT` na stdout. Při přijetí je před výsledkem vytištěn průběh výpočtu.

---

## Formát JSON

```json
{
  "type": "HZA",
  "n": 2,
  "Q": ["s", "q", "f"],
  "Sigma": ["a", "b"],
  "Gamma": ["A", "S", "#"],
  "s": "s",
  "S": "S",
  "F": ["f"],
  "rules": [
    {
      "from": "s",
      "depth": 1,
      "expand-from": ["A"],
      "expand-to": ["aAb"],
      "to": "q"
    }
  ]
}
```

Klíče specifické pro jednotlivé typy automatů, tyto se objevují pouze u pravidel pro různé typy:
- HZA / LHZA: `"depth"` (číslo), volitelně `"input-depth"` + `"lookahead"` u LHZA
- CPHZA / VRCPHZA: `"depths"` (pole čísel), volitelně `"input"` u VRCPHZA

---

## Testování

Skript `test.sh` spustí sadu testů nad ukázkovými automaty v `json-examples/`:

```bash
./test.sh
```

## Prohlášení o použití AI
Při vývoji této aplikace byly využity nástroje umělé inteligence:
* **Nástroj:** Claude 4.6 Sonnet (Anthropic)
* **Využití:** Tvorba testovacího skriptu `test.sh` nad ukázkovými automaty

testovací skript byl následně validován manuálním spouštěním některých scénářů.