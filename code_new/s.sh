#!/bin/bash
# Codexion Shell Test Suite
# Usage: bash test.sh

BINARY="./codexion"
PASS=0
FAIL=0
WARN=0

# ── colors ────────────────────────────────────────────────────────────────────
R="\033[31m"; G="\033[32m"; Y="\033[33m"
C="\033[36m"; W="\033[1m";  NC="\033[0m"

section() { printf "\n${C}${W}── %s ──${NC}\n" "$1"; }
pass()    { printf "  ${G}[PASS]${NC} %s\n" "$1"; PASS=$((PASS+1)); }
fail()    { printf "  ${R}[FAIL]${NC} %s\n  ${Y}       ↳ %s${NC}\n" "$1" "$2"; FAIL=$((FAIL+1)); }
warn()    { printf "  ${Y}[WARN]${NC} %s: %s\n" "$1" "$2"; WARN=$((WARN+1)); }

run() {
	# run <timeout> <args...>
	local timeout=$1; shift
	timeout "$timeout" $BINARY "$@" 2>&1
}

# ── build ─────────────────────────────────────────────────────────────────────
printf "${C}────────────────────────────────────────────────────${NC}\n"
printf "${W}[BUILD]${NC} make ...\n"
make 2>&1
if [ $? -ne 0 ] || [ ! -f "$BINARY" ]; then
	printf "${R}[FAIL]${NC} build failed\n"; exit 1
fi
printf "${G}[OK]${NC}   build succeeded\n"

# ═════════════════════════════════════════════════════════════════════════════
# HELPERS
# ═════════════════════════════════════════════════════════════════════════════

# check exit code
check_exit() {
	local label="$1" expected="$2"; shift 2
	local out rc
	out=$(run "$@")
	rc=$?
	[ $rc -eq "$expected" ] \
		&& pass "$label" \
		|| fail "$label" "exit=$rc want=$expected | out: $(echo "$out" | head -2)"
}

# output must contain string
check_contains() {
	local label="$1" needle="$2"; shift 2
	local out
	out=$(run "$@")
	echo "$out" | grep -qi "$needle" \
		&& pass "$label" \
		|| fail "$label" "missing '$needle' | out: $(echo "$out" | head -3)"
}

# output must NOT contain string
check_not_contains() {
	local label="$1" needle="$2"; shift 2
	local out
	out=$(run "$@")
	echo "$out" | grep -qi "$needle" \
		&& fail "$label" "found forbidden '$needle'" \
		|| pass "$label"
}

# must exit within timeout (not hang)
check_exits() {
	local label="$1" t="$2"; shift 2
	local out
	out=$(run "$t" "$@")
	[ $? -ne 124 ] \
		&& pass "$label" \
		|| fail "$label" "hung > ${t}s"
}

# ═════════════════════════════════════════════════════════════════════════════
# 1. ARG VALIDATION
# ═════════════════════════════════════════════════════════════════════════════
section "ARG VALIDATION"

check_exit "no args"                    1  5
check_exit "7 args (missing sched)"     1  5  3 200 100 50 50 2 10
check_exit "9 args (extra)"             1  5  3 200 100 50 50 2 10 edf x
check_exit "zero coders"                1  5  0 200 100 50 50 2 10 edf
check_exit "negative coders"            1  5 -1 200 100 50 50 2 10 edf
check_exit "zero burnout"               1  5  3   0 100 50 50 2 10 edf
check_exit "negative burnout"           1  5  3  -1 100 50 50 2 10 edf
check_exit "zero compile_time"          1  5  3 200   0 50 50 2 10 edf
check_exit "negative compile_time"      1  5  3 200  -1 50 50 2 10 edf
check_exit "negative debug_time"        1  5  3 200 100 -1 50 2 10 edf
check_exit "negative refactor_time"     1  5  3 200 100 50 -1 2 10 edf
check_exit "negative compiles_req"      1  5  3 200 100 50 50 -1 10 edf
check_exit "negative cooldown"          1  5  3 200 100 50 50 2 -1 edf
check_exit "scheduler=rr (invalid)"     1  5  3 200 100 50 50 2 10 rr
check_exit "scheduler=EDF (uppercase)"  1  5  3 200 100 50 50 2 10 EDF
check_exit "scheduler=FIFO (uppercase)" 1  5  3 200 100 50 50 2 10 FIFO
check_exit "scheduler=bogus"            1  5  3 200 100 50 50 2 10 bogus
check_exit "scheduler=empty"            1  5  3 200 100 50 50 2 10 ""
check_exit "float coder count"          1  5 3.5 200 100 50 50 2 10 edf
check_exit "non-numeric burnout"        1  5  3 abc 100 50 50 2 10 edf

# ═════════════════════════════════════════════════════════════════════════════
# 2. OUTPUT FORMAT
# ═════════════════════════════════════════════════════════════════════════════
section "OUTPUT FORMAT"

for sched in edf fifo; do
	out=$(run 10 3 800 100 50 50 2 0 $sched)

	# every line must match: "DIGITS DIGITS action"
	bad=$(echo "$out" | grep -Ev \
		'^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$')
	[ -z "$bad" ] \
		&& pass "[$sched] all lines match subject format" \
		|| fail "[$sched] all lines match subject format" "bad: $(echo "$bad" | head -2)"

	# timestamps non-decreasing
	prev=0; ok=1
	while read -r ts _rest; do
		[ "$ts" -lt "$prev" ] && ok=0 && break
		prev=$ts
	done <<< "$out"
	[ $ok -eq 1 ] \
		&& pass "[$sched] timestamps non-decreasing" \
		|| fail "[$sched] timestamps non-decreasing" "timestamp went backwards"

	# 2 dongle-takes before each "is compiling" (aggregate check)
	takes=$(echo "$out" | grep -c "has taken a dongle")
	compiles=$(echo "$out" | grep -c "is compiling")
	[ "$takes" -eq $((compiles * 2)) ] \
		&& pass "[$sched] 2 dongle-takes per compile (total: takes=$takes compiles=$compiles)" \
		|| fail "[$sched] 2 dongle-takes per compile" "takes=$takes compiles=$compiles"

	# state sequence: compiling → debugging → refactoring per coder
	seq_ok=1
	for cid in $(echo "$out" | awk '{print $2}' | sort -u); do
		states=$(echo "$out" | awk -v c="$cid" '$2==c {print $3" "$4" "$5}')
		prev_state=""
		while IFS= read -r state; do
			case "$state" in
				"is debugging")
					[ "$prev_state" != "is compiling" ] && seq_ok=0 ;;
				"is refactoring")
					[ "$prev_state" != "is debugging" ] && seq_ok=0 ;;
			esac
			prev_state="$state"
		done <<< "$states"
	done
	[ $seq_ok -eq 1 ] \
		&& pass "[$sched] state sequence compile→debug→refactor" \
		|| fail "[$sched] state sequence" "wrong state order for some coder"
done

# ═════════════════════════════════════════════════════════════════════════════
# 3. SINGLE CODER
# ═════════════════════════════════════════════════════════════════════════════
section "SINGLE CODER (1 dongle)"

for sched in edf fifo; do
	check_exit      "[$sched] 1 coder 1 compile exits 0"   0 5 1 800 100 50 50 1 0 $sched
	check_not_contains "[$sched] 1 coder no burnout"  "burned out" 8 1 800 100 50 50 3 0 $sched

	# single coder: only 1 dongle → 1 take per compile
	out=$(run 5 1 800 100 50 50 1 0 $sched)
	takes=$(echo "$out" | grep -c "has taken a dongle")
	compiles=$(echo "$out" | grep -c "is compiling")
	[ "$takes" -eq "$compiles" ] \
		&& pass "[$sched] 1 coder: 1 dongle-take per compile" \
		|| fail "[$sched] 1 coder: 1 dongle-take per compile" "takes=$takes compiles=$compiles"
done

# ═════════════════════════════════════════════════════════════════════════════
# 4. BURNOUT DETECTION
# ═════════════════════════════════════════════════════════════════════════════
section "BURNOUT DETECTION"

for sched in edf fifo; do
	# compile > burnout → must burn out
	check_contains "[$sched] compile>burnout → burned out" \
		"burned out" 4 2 50 500 30 30 5 0 $sched

	# sim stops after burnout — no lines after "burned out"
	out=$(run 4 2 50 500 30 30 5 0 $sched)
	after=$(echo "$out" | awk '/burned out/{found=1; next} found{print}')
	[ -z "$after" ] \
		&& pass "[$sched] no events after burned out" \
		|| fail "[$sched] no events after burned out" "extra: $(echo "$after" | head -2)"

	# burnout timestamp ≤ burnout_time + 15ms
	out=$(run 4 2 80 500 30 30 5 0 $sched)
	bo_ts=$(echo "$out" | awk '/burned out/{print $1; exit}')
	if [ -n "$bo_ts" ]; then
		[ "$bo_ts" -le 95 ] \
			&& pass "[$sched] burnout timestamp ≤ 95ms (got ${bo_ts}ms)" \
			|| fail "[$sched] burnout timestamp ≤ 95ms" "got ${bo_ts}ms"
	else
		warn "[$sched] burnout timestamp" "no burned out line found"
	fi
done

# tight but survivable
check_not_contains "edf tight survivable → no burnout" \
	"burned out" 8 2 600 100 50 50 1 0 edf
check_exit "edf tight survivable → exit 0" 0 8 2 600 100 50 50 1 0 edf

# ═════════════════════════════════════════════════════════════════════════════
# 5. num_compiles_required
# ═════════════════════════════════════════════════════════════════════════════
section "num_compiles_required STOP CONDITION"

check_exits "0 compiles_req exits" 5 3 800 100 50 50 0 0 edf

for sched in edf fifo; do
	check_exit         "[$sched] 1 compile_req exits 0"    0 10 3 1000 100 50 50 1 0 $sched
	check_not_contains "[$sched] 1 compile_req no burnout" "burned out" 10 3 1000 100 50 50 1 0 $sched

	# verify each coder compiled ≥ 2 times
	out=$(run 12 3 1500 100 50 50 2 0 $sched)
	if echo "$out" | grep -q "burned out"; then
		warn "[$sched] compile count" "burned out — skipping"
	else
		for cid in 1 2 3; do
			cnt=$(echo "$out" | awk -v c="$cid" '$2==c && $3=="is" && $4=="compiling"' | wc -l)
			[ "$cnt" -ge 2 ] \
				&& pass "[$sched] coder $cid compiled ≥ 2 times (got $cnt)" \
				|| fail "[$sched] coder $cid compiled ≥ 2 times" "got $cnt"
		done
	fi
done

# ═════════════════════════════════════════════════════════════════════════════
# 6. DONGLE COOLDOWN
# ═════════════════════════════════════════════════════════════════════════════
section "DONGLE COOLDOWN"

check_exit         "zero cooldown works"         0 10 2 1000 100 50 50 2 0 edf
check_not_contains "zero cooldown no burnout" "burned out" 10 2 1000 100 50 50 2 0 edf
check_contains     "cooldown > burnout → burnout" "burned out" 4 2 100 50 20 20 3 500 edf

# ═════════════════════════════════════════════════════════════════════════════
# 7. EDF
# ═════════════════════════════════════════════════════════════════════════════
section "EDF — Earliest Deadline First"

check_exit         "3 coders generous → exit 0"      0  12 3 1200 100 50 50 2 0 edf
check_not_contains "3 coders generous → no burnout"  "burned out" 12 3 1200 100 50 50 2 0 edf
check_exits        "5 coders no hang"                12 5 1500 80 40 40 2 0 edf
check_contains     "impossible → burnout detected"   "burned out" 5 3 30 500 100 100 5 100 edf
check_not_contains "liveness 10 coders → no burnout" "burned out" 12 10 3000 100 50 50 1 10 edf

# ═════════════════════════════════════════════════════════════════════════════
# 8. FIFO
# ═════════════════════════════════════════════════════════════════════════════
section "FIFO — First In First Out"

check_exit         "3 coders generous → exit 0"     0  12 3 1500 100 50 50 2 0 fifo
check_not_contains "3 coders generous → no burnout" "burned out" 12 3 1500 100 50 50 2 0 fifo
check_exits        "5 coders no hang"               12 5 1500 80 40 40 2 0 fifo
check_exits        "10 coders no hang"              12 10 2000 80 40 40 1 5 fifo
check_contains     "impossible → burnout detected"  "burned out" 5 3 30 500 100 100 5 100 fifo

# fifo timestamps non-decreasing
out=$(run 12 4 1500 80 40 40 1 0 fifo)
prev=0; ok=1
while read -r ts _rest; do
	[ "$ts" -lt "$prev" ] && ok=0 && break
	prev=$ts
done <<< "$out"
[ $ok -eq 1 ] \
	&& pass "[fifo] compile timestamps non-decreasing" \
	|| fail "[fifo] compile timestamps non-decreasing" "timestamp went backwards"

# ═════════════════════════════════════════════════════════════════════════════
# 9. LOG SERIALIZATION
# ═════════════════════════════════════════════════════════════════════════════
section "LOG SERIALIZATION"

for sched in edf fifo; do
	out=$(run 12 5 1500 80 40 40 2 5 $sched)
	bad=$(echo "$out" | grep -Ev \
		'^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$')
	[ -z "$bad" ] \
		&& pass "[$sched] no garbled/interleaved lines" \
		|| fail "[$sched] no garbled/interleaved lines" "bad: $(echo "$bad" | head -2)"
done

# ═════════════════════════════════════════════════════════════════════════════
# 10. NO HANG
# ═════════════════════════════════════════════════════════════════════════════
section "NO HANG"

check_exits "1 coder edf"          5  1 800 100 50 50 1 0 edf
check_exits "1 coder fifo"         5  1 800 100 50 50 1 0 fifo
check_exits "burnout edf"          4  3 30 500 100 100 5 0 edf
check_exits "burnout fifo"         4  3 30 500 100 100 5 0 fifo
check_exits "normal finish edf"   10  2 1000 100 50 50 2 0 edf
check_exits "normal finish fifo"  10  2 1000 100 50 50 2 0 fifo

# ═════════════════════════════════════════════════════════════════════════════
# 11. EDGE CASES
# ═════════════════════════════════════════════════════════════════════════════
section "EDGE CASES"

check_exits        "burnout == compile_time"     5  2 100 100 50 50 2 0 edf
check_exits        "burnout = compile+1 (1ms)"   5  2 101 100 50 50 2 0 edf
check_exit         "compile_time=1ms exits 0"    0  8  3 500   1  1  1 3 0 edf
check_not_contains "compile_time=1ms no burnout" "burned out" 8 3 500 1 1 1 3 0 edf
check_exits        "20 coders stress edf"       12 20 3000 80 40 40 1 5 edf
check_exits        "20 coders stress fifo"      12 20 3000 80 40 40 1 5 fifo

# ═════════════════════════════════════════════════════════════════════════════
# SUMMARY
# ═════════════════════════════════════════════════════════════════════════════
printf "\n${C}════════════════════════════════════════════════════${NC}\n"
printf "${W}  RESULTS${NC}  ${G}%d passed${NC}  ${R}%d failed${NC}  ${Y}%d warned${NC}  / %d total\n" \
	$PASS $FAIL $WARN $((PASS + FAIL + WARN))
printf "${C}════════════════════════════════════════════════════${NC}\n\n"

[ $FAIL -eq 0 ] && exit 0 || exit 1
