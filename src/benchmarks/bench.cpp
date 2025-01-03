#include <iostream>
#include "core/solver.h"
#include "core/board.h"
#include "games/virtual.h"

#include "bench.h"

static const int ATTEMPTS = 2500;

Benchmark::Benchmark(int w, int h, int m, bool v) : width(w), height(h), mines(m), verbose(v) {}

void Benchmark::full_benchmark(bool verbose) {
	verbose = false; // TODO: New verbose breaks benchmarking
	std::cout << "Minesweeper Solver X Algortihm Benchmark:" << std::endl;
	
	std::cout << "Easy board (10x8 m=10)" << std::endl;
	Benchmark bench = Benchmark(10, 8, 10, verbose);
	bench.run();
	bench.print_results();
	
	std::cout << "Medium board (18x14 m=40)" << std::endl;
	Benchmark bench2 = Benchmark(18, 14, 40, verbose);
	bench2.run();
	bench2.print_results();
	
	std::cout << "Hard board (24x20 m=99)" << std::endl;
	Benchmark bench3 = Benchmark(24, 20, 99, verbose);
	bench3.run();
	bench3.print_results();

	//std::cout << "Impossible board (50x50 m=625)" << std::endl;
	//Benchmark bench4 = Benchmark(50, 50, 625, verbose, print_board);
	//bench4.run();
	//bench4.print_results();
}


void Benchmark::run() {
	std::shared_ptr<Virtual> game;
	for (int i = 0; i < ATTEMPTS; i++) {
		auto start = std::chrono::high_resolution_clock::now();
		game = std::make_shared<Virtual>(width, height, mines);
		Solver solver = Solver(game, verbose);
		SolverResult result = solver.solve();

		// Store run time
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		run_times.push_back(duration);

		// Store run completion percentage
		percent_completion.push_back(
			static_cast<double>(game->get_board()->discovered_count()) / (game->get_board()->get_height() * game->get_board()->get_width())
		);

		// Determine if success
		switch (result) {
		case SUCCESS:
			successes++;
			break;
		case FAILURE:
			failures++;
			break;
		case STUCK:
			timeouts++;
			break;
		}
	}
}

void Benchmark::print_results() {
	double success_rate = static_cast<double>(successes) / ATTEMPTS * 100;
	double failure_rate = static_cast<double>(failures) / ATTEMPTS * 100;
	double timeout_rate = static_cast<double>(timeouts) / ATTEMPTS * 100;

	// Calculate total and average completion time
	std::chrono::microseconds elapsed_time = std::chrono::microseconds(0);
	for (std::chrono::microseconds time : run_times) {
		elapsed_time += time;
	}
    double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_time).count();
	double per_attempt_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_time / ATTEMPTS).count();

	// Calculate average completion percentage
	double total_completion = 0.0;
	for (double p : percent_completion) {
		total_completion += p;
	}
	double average_completion = (total_completion / ATTEMPTS) * 100.0;

	// Print results
	std::cout << "Wins: " << successes << " (" << std::fixed << std::setprecision(2) << success_rate << "%)" << std::endl;
	std::cout << "Loses: " << failures << " (" << std::fixed << std::setprecision(2) << failure_rate << "%)" << std::endl;
	std::cout << "Timeouts: " << timeouts << " (" << std::fixed << std::setprecision(2) << timeout_rate << "%)" << std::endl;
	std::cout << "Average Completion: " << std::fixed << std::setprecision(2) << average_completion << "%" << std::endl;
	std::cout << "Elapsed Time: " << std::fixed << std::setprecision(2) << elapsed_seconds << " seconds ("
		<< std::fixed << std::setprecision(5) << per_attempt_seconds << " seconds per attempt)" << std::endl;
}