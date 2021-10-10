package main

import (
	`bytes`
	`fmt`
	`os`
	`os/exec`
	`path/filepath`
	`strconv`
	`strings`
)

func main() {
	arguments := os.Args[1:]
	if len(arguments) == 0 {
		fmt.Println("Usage: go run benchmark.go [interpreters...] <benchmark>")
		os.Exit(1)
	}

	interpreters := []string{"../src/cmake-build-release/qi"}
	benchmark := arguments[len(arguments) - 1]
	if len(arguments) > 1 {
		interpreters = arguments[:len(arguments) - 1]
	}

	if len(interpreters) > 1 {
		runComparison(interpreters, benchmark)
	} else {
		runBenchmark(interpreters[0], benchmark)
	}
}

func runBenchmark(interpreter string, benchmark string) {
	trial := 1
	best := 9999.

	for ;; {
		elapsed := runTrial(interpreter, benchmark)
		if elapsed < best { best = elapsed }

		bestSeconds := strconv.FormatFloat(best, 'f', 2, 64)
		fmt.Printf("trial #%d %s  best %ss\n", trial, interpreter, bestSeconds)
		trial++
	}
}

func runTrial(interpreter string, benchmark string) float64 {
	cmd := exec.Command(interpreter, filepath.Join("..", "test", "benchmark", benchmark + ".qi"))
	var outb, errb bytes.Buffer
	cmd.Stdout = &outb
	cmd.Stderr = &errb
	if err := cmd.Run(); err != nil {
		fmt.Println(err.Error())
		os.Exit(1)
	}
	outputLines := strings.Split(outb.String(), "\n")

	if outputLines[len(outputLines) - 1] == "" {
		outputLines = outputLines[:len(outputLines) - 1]
	}

	result, _ := strconv.ParseFloat(outputLines[len(outputLines) - 1], 64)
	return result
}

func runComparison(interpreters []string, benchmark string) {
	trial := 1
	best := make(map[string]float64)
	for _, interpreter := range interpreters { best[interpreter] = 9999. }

	for ;; {
		for _, interpreter := range interpreters {
			elapsed := runTrial(interpreter, benchmark)
			if elapsed < best[interpreter] { best[interpreter] = elapsed }
		}

		bestTime := 999.
		worstTime := 0.001
		bestInterpreter := ""
		for _, interpreter := range interpreters {
			if best[interpreter] < bestTime {
				bestTime = best[interpreter]
				bestInterpreter = interpreter
			}
			if best[interpreter] > worstTime {
				worstTime = best[interpreter]
			}
		}

		worstWork := 1. / worstTime

		fmt.Printf("trial #%d\n", trial)
		for _, interpreter := range interpreters {
			suffix := ""
			if interpreter == bestInterpreter {
				bestWork := 1. / best[interpreter]
				workRatio := bestWork / worstWork
				faster := 100 * (workRatio - 1.)
				suffix = fmt.Sprintf("%s%% faster",
					strconv.FormatFloat(faster, 'f', 4, 64))
			} else {
				ratio := best[interpreter] / bestTime
				suffix =  fmt.Sprintf("%sx time of best",
					strconv.FormatFloat(ratio, 'f', 4, 64))
			}
			bestString := strconv.FormatFloat(best[interpreter], 'f', 4, 64)
			fmt.Printf("  %30s  best %ss %s\n", interpreter, bestString, suffix)
		}

		trial++
	}
}