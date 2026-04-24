import subprocess
import sys
import time

# Ensure your Makefile outputs an executable named exactly this
EXEC = "./codexion"

# Define hardcoded test cases
# Format: "name": (arguments_string, expect_death, timeout_in_seconds)
TEST_CASES = {
    "1. One Coder (Should die immediately)": (
        "1 800 200 200 200 5 0 fifo", True, 3
    ),
    "2. Standard Survival (Should survive)": (
        "5 800 200 200 200 5 0 fifo", False, 10
    ),
    "3. Tight Timing (Should die)": (
        "4 310 200 100 100 5 0 edf", True, 5
    ),
    "4. Even Coders Survival (Should survive)": (
        "4 410 200 200 200 5 0 edf", False, 10
    ),
    "5. Cooldown Impact (Should survive)": (
        "5 800 200 200 200 5 50 fifo", False, 10
    ),
    "6. High compile requirement (Should survive)": (
        "5 800 200 200 200 10 0 edf", False, 15
    )
}

def run_test(name, args_str, expect_death, timeout):
    print(f"Running: {name}")
    print(f"Command: {EXEC} {args_str}")
    
    args = [EXEC] + args_str.split()
    
    try:
        start_time = time.time()
        # Run the C program
        process = subprocess.Popen(
            args, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE, 
            text=True
        )
        
        stdout, stderr = process.communicate(timeout=timeout)
        duration = time.time() - start_time
        
        # Parse the output
        lines = stdout.strip().split('\n')
        death_count = sum(1 for line in lines if "burned out" in line)
        
        # Validation checks
        passed = True
        error_msg = ""
        
        if expect_death and death_count == 0:
            passed = False
            error_msg = "Expected a burnout, but no one died."
        elif not expect_death and death_count > 0:
            passed = False
            error_msg = f"Expected everyone to survive, but found {death_count} burnout(s)."
        elif death_count > 1:
            passed = False
            error_msg = f"Multiple burnouts detected ({death_count}). The simulation must stop after the first burnout."

        if passed:
            print(f"✅ PASS (Time: {duration:.2f}s)\n")
        else:
            print(f"❌ FAIL: {error_msg}")
            print(f"--- Last 5 lines of output ---")
            for line in lines[-5:]:
                print(line)
            print("------------------------------\n")
            
        return passed

    except subprocess.TimeoutExpired:
        process.kill()
        print(f"❌ FAIL: Program timed out after {timeout} seconds. (Possible Deadlock?)\n")
        return False
    except FileNotFoundError:
        print(f"❌ FAIL: Executable '{EXEC}' not found. Did you run 'make'?\n")
        sys.exit(1)

def main():
    print("========================================")
    print("       Codexion Automated Tester        ")
    print("========================================\n")
    
    passed_tests = 0
    total_tests = len(TEST_CASES)
    
    for name, (args, expect_death, timeout) in TEST_CASES.items():
        if run_test(name, args, expect_death, timeout):
            passed_tests += 1
            
    print("========================================")
    print(f"RESULTS: {passed_tests}/{total_tests} Tests Passed")
    print("========================================")

if __name__ == "__main__":
    main()