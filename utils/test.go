package main

import (
	`bufio`
	`bytes`
	`flag`
	`fmt`
	`os`
	`os/exec`
	`path/filepath`
	`regexp`
	`strconv`
	`strings`
)

var green = "\u001b[32m"
var magenta = "\u001b[35m"
var red = "\u001b[31m"
var yellow = "\u001b[33m"
var gray = "\u001b[1;30m"
var none = "\u001b[0m"
var resetColor = "\u001b[39m"

var expectedOutputPattern, _ = regexp.Compile(`// expect: ?(.*)`)
var expectedErrorPattern, _ = regexp.Compile(`// (错误.*)`)
var errorLinePattern, _ = regexp.Compile(`//【行 (\d+)】(错误.*)`)
var expectedRuntimeErrorPattern, _ = regexp.Compile(`// expect runtime error: (.+)`)
var syntaxErrorPattern, _ = regexp.Compile(`【.*行 (\d+)】(错误.+)`)
var stackTracePattern, _ = regexp.Compile(`\[line (\d+)]`)
var nonTestPattern, _ = regexp.Compile(`// nontest`)

var passed int
var failed int
var skipped int
var expectations int

var interpreter string

func main() {
	interpreterPtr := flag.String("interpreter", "", "Path to interpreter.")
	flag.Parse()
	interpreter = *interpreterPtr

	if interpreter == "" {
		err := fmt.Errorf("must pass an interpreter path (pass -h for help)")
		fmt.Println(err.Error())
		os.Exit(1)
	}

	if !runSuite() {
		os.Exit(1)
	}
}

func runSuite() bool {
	paths, err := WalkMatch("../test", "*.qi")
	if err != nil {
		fmt.Println(err.Error())
	}

	for _, path := range paths {
		runTest(path)
		fmt.Printf("\r\033[K")
	}



	if failed == 0 {
		fmt.Printf("All %s%d%s tests passed (%d expectations).",
			green, passed, resetColor, expectations)
	} else {
		fmt.Printf("%s%d%s tests passed. %s%d%s tests failed.",
			green, passed, resetColor, red, failed, resetColor)
	}
	fmt.Println()

	return failed == 0
}

func runTest(path string) {
	if strings.Contains(path, "benchmark") { return }
	fmt.Printf("Passed: %s%d%s Failed: %s%d%s Skipped: %s%d%s %s%s%s",
		green, passed, resetColor, red, failed, resetColor,
		yellow, skipped, resetColor, gray, path, none)

	test, success := parse(newTest(path))

	if !success { return }

	failures := run(test)

	if len(failures) == 0 {
		passed++
	} else {
		failed++
		fmt.Printf("\r\033[K")
		fmt.Printf("%sFAIL%s %s\n", red, resetColor, path)
		for _, failure := range failures {
			fmt.Printf("     %s%s%s\n", red, failure, resetColor)
		}
	}
}

type ExpectedOutput struct {
	line int
	output string
}

type Test struct {
	path string

	expectedOutput []ExpectedOutput

	expectedErrors []string

	expectedRuntimeError string

	runtimeErrorLine int

	expectedExitCode int

	failures []string
}

func newTest(path string) Test {
	test := Test{}

	test.path = path
	test.runtimeErrorLine = 0
	test.expectedExitCode = 0

	return test
}

func parse(test Test) (Test, bool) {
	parts := strings.Split(test.path, "/")
	var subpath string

	for _, part := range parts {
		if subpath != "" { subpath += "/" }
		subpath += part
	}

	file, err := os.Open(test.path)
	if err != nil {
		fmt.Println(err.Error())
	}

	scanner := bufio.NewScanner(file)

	var lines []string
	for scanner.Scan() {
		lines = append(lines, scanner.Text())
	}

	if err := scanner.Err(); err != nil {
		fmt.Println(err.Error())
	}

	for lineNum := 1; lineNum <= len(lines); lineNum++ {
		line := lines[lineNum - 1]

		match := nonTestPattern.FindStringSubmatch(line)
		if len(match) != 0 { return test, false }

		match = expectedOutputPattern.FindStringSubmatch(line)
		if len(match) != 0  {
			test.expectedOutput = append(test.expectedOutput, ExpectedOutput{lineNum, match[1]})
			expectations++
			continue
		}

		match = expectedErrorPattern.FindStringSubmatch(line)
		if len(match) != 0  {
			test.expectedErrors = append(test.expectedErrors, fmt.Sprintf("[%d] %s", lineNum, match[1]))

			test.expectedExitCode = 65
			expectations++
			continue
		}

		match = errorLinePattern.FindStringSubmatch(line)
		if len(match) != 0 {
            test.expectedErrors = append(test.expectedErrors, fmt.Sprintf("[%s] %s", match[1], match[2]))

            test.expectedExitCode = 65
            expectations++
			continue
		}

		match = expectedRuntimeErrorPattern.FindStringSubmatch(line)
		if len(match) != 0 {
			test.runtimeErrorLine = lineNum
			test.expectedRuntimeError = match[1]

			test.expectedExitCode = 70
			expectations++
		}
	}

	if len(test.expectedErrors) != 0 && test.expectedRuntimeError != "" {
		fmt.Printf("%sTEST_ERROR%s %s\n", magenta, resetColor, test.path)
		fmt.Printf("     Cannot expect both compile and runtime errors.\n\n")
		return test, false
	}

	err = file.Close()
	if err != nil {
		fmt.Println(err.Error())
	}
	return test, true
}

func run(test Test) []string {
	cmd := exec.Command(interpreter, test.path)
	var outb, errb bytes.Buffer
	exitCode := 0
	cmd.Stdout = &outb
	cmd.Stderr = &errb
	if err := cmd.Run(); err != nil {
		exitError, ok := err.(*exec.ExitError)
		if ok {
			exitCode = exitError.ExitCode()
		}
	}

	outputLines := strings.Split(outb.String(), "\n")
	errorLines := strings.Split(errb.String(), "\n")

	if test.expectedRuntimeError != "" {
		test = validateRuntimeError(test, errorLines)
	} else {
		test = validateCompileErrors(test, errorLines)
	}

	test = validateExitCode(test, exitCode, errorLines)
	test = validateOutput(test, outputLines)
	return test.failures
}

func validateRuntimeError(test Test, errorLines []string) Test {
	if len(errorLines) < 2 {
		test = fail(test, fmt.Sprintf("Expected runtime error '%s' and got none.", test.expectedRuntimeError))
		return test
	}

	if errorLines[0] != test.expectedRuntimeError {
		test = fail(test, fmt.Sprintf("Expected runtime error '%s' and got:", test.expectedRuntimeError))
		test = fail(test, errorLines[0])
	}

	var match []string
	stackLines := errorLines[1:]
	for _, line := range stackLines {
		match = stackTracePattern.FindStringSubmatch(line)
		if len(match) != 0 { break }
	}

	if len(match) == 0 {
		test = fail(test, "Expected stack trace and got:", stackLines)
	} else {
		stackLine, _ := strconv.Atoi(match[1])
		if stackLine != test.runtimeErrorLine {
			test = fail(test, fmt.Sprintf("Expected runtime error on line %d but was on line %d",
				test.runtimeErrorLine, stackLine))
		}
	}

	return test
}

func validateCompileErrors(test Test, errorLines []string) Test {
	var foundErrors []string
	var unexpectedCount int
	for _, line := range errorLines {
		match := syntaxErrorPattern.FindStringSubmatch(line)
		if len(match) != 0 {
			err := fmt.Sprintf("[%s] %s", match[1], match[2])
			var found bool
			for _, e := range test.expectedErrors {
				if err == e {
					found = true
					break
				}
			}
			if found {
				foundErrors = append(foundErrors, err)
			} else {
				if unexpectedCount < 10 {
					test = fail(test, "Unexpected error:")
					test = fail(test, line)
				}
				unexpectedCount++
			}
		} else if line != "" {
			if unexpectedCount < 10 {
				test = fail(test, "Unexpected output on stderr:")
				test = fail(test, line)
			}
			unexpectedCount++
		}
	}

	if unexpectedCount > 10 {
		test = fail(test, fmt.Sprintf("(truncated %d more...)", unexpectedCount - 10))
	}

	for _, expected := range test.expectedErrors {
		var isFound bool
		for _, found := range foundErrors {
			if expected == found {
				isFound = true
				break
			}
		}
		if !isFound {
			test = fail(test, fmt.Sprintf("Missing expected error: %s", expected))
		}
	}

	return test
}

func validateExitCode(test Test, exitCode int, errorLines []string) Test {
	if exitCode == test.expectedExitCode { return test }

	if len(errorLines) > 10 {
		errorLines = errorLines[:10]
		errorLines = append(errorLines, "(truncated...)")
	}

	test = fail(test, fmt.Sprintf("Expected return code %d and got %d. Stderr:",
		exitCode, test.expectedExitCode), errorLines)

	return test
}

func validateOutput(test Test, outputLines []string) Test {
	if len(outputLines) != 0 && outputLines[len(outputLines) - 1] == "" {
		outputLines = outputLines[:len(outputLines) - 1]
	}

	index := 0
	for ; index < len(outputLines); index++ {
		line := outputLines[index]
		if index >= len(test.expectedOutput) {
			test = fail(test, fmt.Sprintf("Got output '%s' when none was expected.", line))
			continue
		}

		expected := test.expectedOutput[index]
		if expected.output != line {
			test = fail(test, fmt.Sprintf("Expected output '%s' on line %d and got '%s'.",
				expected.output, expected.line, line))
		}
	}

	return test
}

func fail(test Test, message string, lines ...[]string) Test {
	test.failures = append(test.failures, message)
	if len(lines) != 0 {
		test.failures = append(test.failures, lines[0]...)
	}
	
	return test
}

func WalkMatch(root, pattern string) ([]string, error) {
	var matches []string
	err := filepath.Walk(root, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() {
			return nil
		}
		if matched, err := filepath.Match(pattern, filepath.Base(path)); err != nil {
			return err
		} else if matched {
			matches = append(matches, path)
		}
		return nil
	})
	if err != nil {
		return nil, err
	}
	return matches, nil
}