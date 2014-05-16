.PHONY: test clean
test: pitch_analyzer_test.c ../src/pitch_analyzer.c
	gcc -o pitch_analyzer_test pitch_analyzer_test.c ../src/pitch_analyzer.c;./pitch_analyzer_test samples/* >| test_results

clean:
	rm pitch_analyzer_test test_results
