#include <iostream>
#include "core/solver.h"
#include "core/board.h"
#include "games/virtual.h"

#include "bench.h"

static const int ATTEMPTS = 1000;
static const std::chrono::seconds TIMEOUT = std::chrono::seconds(5);

Benchmark::Benchmark(int w, int h, int m) : width(w), height(h), mines(m), successes(0), failures(0), timeouts(0) {}

void Benchmark::full_benchmark() {
	std::cout << "Minesweeper Solver X Algortihm Benchmark:" << std::endl;
	
	std::cout << "Easy board (10x10 m=10)" << std::endl;
	Benchmark bench(10, 10, 10);
	bench.run();
	bench.print_results();
	
	std::cout << "Medium board (15x15 m=40)" << std::endl;
	Benchmark bench2(15, 15, 40);
	bench2.run();
	bench2.print_results();
	
	std::cout << "Hard board (20x20 m=100)" << std::endl;
	Benchmark bench3(20, 20, 100);
	bench3.run();
	bench3.print_results();

	std::cout << "Impossible board (50x50 m=1000)" << std::endl;
	Benchmark bench4(50, 50, 1000);
	bench4.run();
	bench4.print_results();
}


void Benchmark::run() {
	for (int i = 0; i < ATTEMPTS; i++) {
		Virtual vboard = Virtual(width, height, mines);
		Solver solver = Solver(vboard.get_board());
		auto start = std::chrono::high_resolution_clock::now();
		while (vboard.status() == IN_PROGRESS) {
			std::set<Move> moves = solver.get_moves(true);
			// Check timeout condition
			auto current_time = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start);
			if (elapsed > TIMEOUT) {
				break;
			}
			for (Move move : moves) {
				if (move.action == FLAG_ACTION) {
					//std::cout << "Flag: (" << std::to_string(move.x) << ", " << std::to_string(move.y) << ")" << std::endl;
					vboard.get_board()->set_tile(move.x, move.y, MINE);
				}
				else if (move.action == CLICK_ACTION) {
					//std::cout << "Click: (" << std::to_string(move.x) << ", " << std::to_string(move.y) << ")" << std::endl;
					vboard.click(move.x, move.y);
				}
			}
			vboard.update();
			//vboard.get_board()->print();
			//std::cout << std::endl;
		}
		// Store run time
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		run_times.push_back(duration);

		// Store run completion percentage
		percent_completion.push_back(
			static_cast<double>(vboard.get_board()->discovered_count()) /
			static_cast<double>(vboard.get_board()->get_height() * vboard.get_board()->get_width())
		);

		// Determine if success
		if (vboard.status() == WON) {
			successes++;
		}
		else if (vboard.status() == LOST) {
			failures++;
		}
		else {
			timeouts++;
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
		<< std::fixed << std::setprecision(4) << per_attempt_seconds << " seconds per attempt)" << std::endl;
}