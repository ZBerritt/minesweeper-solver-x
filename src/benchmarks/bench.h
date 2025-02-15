#pragma once
#include <chrono>

class Benchmark {
public:
	Benchmark(int w, int h, int m, bool v);
	void run();
	void print_results();
	static void full_benchmark(bool verbose);
private:
	// Board config
	int width;
	int height;
	int mines;

	// Results
	int successes = 0;
	int failures = 0;
	int timeouts = 0;
	bool verbose;
	std::vector<std::chrono::microseconds> run_times;
	std::vector<double> percent_completion;
};