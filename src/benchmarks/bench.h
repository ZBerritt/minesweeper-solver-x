#include <chrono>

class Benchmark {
public:
	Benchmark(int w, int h, int m);
	void run();
	void print_results();
	static void full_benchmark();
private:
	// Board config
	int width;
	int height;
	int mines;

	// Results
	int successes;
	int failures;
	int timeouts;
	std::vector<std::chrono::microseconds> run_times;
	std::vector<double> percent_completion;
};