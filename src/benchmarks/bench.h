#pragma once
#include <chrono>

class Benchmark {
public:
	Benchmark(int w, int h, int m, bool v, bool pb);
	void run();
	void print_results();
	static void full_benchmark(bool verbose, bool print_board);
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
	bool print_board;
	std::vector<std::chrono::microseconds> run_times;
	std::vector<double> percent_completion;
};