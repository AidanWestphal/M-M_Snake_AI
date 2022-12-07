#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include "snake.h"

int main(int argc, char* argv[]) {
	srand(time(NULL));
	

	if(argc < 2) {
		std::cerr << "Please specify a function." << std::endl;
		exit(1);
	}

	if(std::string(argv[1]) == "--batch_test") {
		if(argc < 4) {
			std::cerr << "Missing arguments. Format: --batch_test [num_tests] [board_size]" << std::endl;
			exit(1);
		}
		std::vector<int> num_moves;
		std::vector<int> runtime;
		int wins = 0;

		for(int i = 0; i < std::stoi(argv[2]); i++) {
			Snake game(std::stoi(argv[3]));
			int count = 0;

			auto start = std::chrono::high_resolution_clock::now();
			while(game.move()) {
				count++;
			}
			if(game.won()) {
				wins++;
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop-start)/1000.0;
				runtime.push_back(duration.count());
				num_moves.push_back(count);
			} else {
				game.print_board();
				std::cout << std::endl;
			}
		}

		int avg_move = 0;
		double std_dev_move = 0;
		double avg_runtime = 0;
		double std_dev_runtime = 0;
		double win_rate = wins/(double)std::stoi(argv[2]);

		for(int i = 0; i < wins; i++) {
			avg_move += num_moves[i];
			avg_runtime += runtime[i];
		}
		avg_move /= (double)wins;
		avg_runtime /= wins;

		for(int i = 0; i < wins; i++) {
			std_dev_move += (num_moves[i] - avg_move)*(num_moves[i] - avg_move);
			std_dev_runtime += (runtime[i] - avg_runtime)*(runtime[i] - avg_runtime);
		}

		std_dev_move = std::sqrt(std_dev_move);
		std_dev_runtime = std::sqrt(std_dev_runtime);

		std::cout << "FORMAT: avg_moves std_dev_moves avg_runtime std_dev_runtime win_rate" << std::endl;
		std::cout << avg_move << " " << std_dev_move << " " << avg_runtime << " " << std_dev_runtime << " " << win_rate*100 << std::endl;
	} else if(std::string(argv[1]) == "--single_test") {
		if(argc < 4) {
			std::cerr << "Missing arguments. Format: --single_test [time_interval (ms)] [board_size]" << std::endl;
			exit(1);
		}
		Snake game(std::stoi(argv[3]));
		int dx = std::stoi(argv[2]);

		game.print_board();
		std::cout << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds((dx)));

		int num_moves = 0;

		while(game.move()) {
			num_moves++;
			game.print_board();
			std::cout << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds((dx)));
		}

		std::cout << num_moves << " MOVES" << std::endl;
	} else {
		std::cerr << "Unrecognized function." << std::endl;
		exit(1);
	}
}