#ifndef __snake_h
#define __snake_h

#include <iostream>
#include <vector>
#include <cassert>

class Snake {
public:
	Snake(unsigned int board_length_) {
		board_length = 2*board_length_;
		head_row = 0;
		head_col = 0;
		length = 1;

		board = std::vector<std::vector<int> >(board_length);
		cycle = std::vector<std::vector<int> >(board_length);
		for(unsigned int i = 0; i < board_length; i++) {
			board[i] = std::vector<int>(board_length,0);
			cycle[i] = std::vector<int>(board_length,0);
		}

		board[head_row][head_col] = 1;

		generate_cycle();
		place_apple();
	}

	bool place_apple() {
		if(length == board_length*board_length) {
			return false;
		}
		int apple_loc = std::rand() % (board_length*board_length - length);
		for(unsigned int i = 0; i < board_length; i++) {
			for(unsigned int j = 0; j < board_length; j++) {
				if(board[i][j] == 0) {
					apple_loc--;
				}
				if(apple_loc == -1) {
					board[i][j] = -1;
					apple_row = i;
					apple_col = j;
					return true;
				}
			}
		}
		return false;
	}

	void phase_shift() {
		int shift = std::rand() % (board_length*board_length);
		for(unsigned int i = 0; i < board_length; i++) {
			for(unsigned int j = 0; j < board_length; j++) {
				cycle[i][j] = (cycle[i][j] + shift) % (board_length*board_length) + 1;
			}
		}
	}

	// Generation
	void generate_cycle() {
		cycle[0][0] = 1;
		int n = 1;

		unsigned int r = 0;
		unsigned int c = 0;

		for(;;) {
			if(r+1 < board_length && cycle[r+1][c] == 0) {
				cycle[r+1][c] = ++n;
				r++;
			} else if(r-1 > 0 && cycle[r-1][c] == 0) {
				cycle[r-1][c] = ++n;
				r--;
			} else if(c+1 < board_length && cycle[r][c+1] == 0) {
				cycle[r][c+1] = ++n;
				c++;
			} else if(c > 0 && cycle[r][c-1] == 0) {
				cycle[r][c-1] = ++n;
				c--;
			} else {
				break;
			}
		}
		for(;c > 0; c--) {
			cycle[0][c] = ++n;
		}
	}

	void print_board() {
		for(unsigned int i = 0; i < board_length; i++) {
			for(unsigned int j = 0; j < board_length; j++) {
				if(board[i][j] == 0) {
					std::cout << ". ";
				} else if(board[i][j] == 1) {
					std::cout << "H ";
				} else if(board[i][j] == (int)length) {
					std::cout << "T ";
				} else if(board[i][j] > 0) {
					std::cout << "@ ";
				} else {
					std::cout << "A ";
				}
			}
			std::cout << std::endl;
		}
	}

	void print_cycle() {
		for(unsigned int i = 0; i < board_length; i++) {
			for(unsigned int j = 0; j < board_length; j++) {
				std::cout << std::setw(4) << cycle[i][j];
			}
			std::cout << std::endl;
		}
	}

	void increment_snake(unsigned int new_r, unsigned int new_c) {
		unsigned int max_r = 0;
		unsigned int max_c = 0;

		for(unsigned int i = 0; i < board_length; i++) {
			for(unsigned int j = 0; j < board_length; j++) {
				if(board[i][j] > 0) {
					board[i][j]++;
					if(board[max_r][max_c] < board[i][j]) {
						max_r = i;
						max_c = j;
					}
				}
			}
		}

		// Apple replacement: Generate a random number for the remaining space in board and go to that square
		if(board[new_r][new_c] == -1) {
			board[new_r][new_c] = 1;
			length++;
			place_apple();
		} else {
			board[max_r][max_c] = 0;
			board[new_r][new_c] = 1;
		}
	}

	bool can_move(unsigned int r, unsigned int c) {
		// All spots in the cutoff hamiltonian cycle are free of snake
		unsigned int i = head_row;
		unsigned int j = head_col;
		for(int pos = cycle[i][j]; pos != cycle[r][c]; pos = cycle[i][j]) {
			
			// Movement
			if(i+1 < board_length && cycle[i+1][j] == pos + 1) {
				i++;
			} else if(i > 0 && cycle[i-1][j] == pos + 1) {
				i--;
			} else if(j+1 < board_length && cycle[i][j+1] == pos + 1) {
				j++;
			} else if(j > 0 && cycle[i][j-1] == pos + 1) {
				j--;
			}
			// Logic to restart cycle
			if(pos == (int)(board_length*board_length)) {
				if(i+1 < board_length && cycle[i+1][j] == 1) {
					i++;
				} else if(i > 0 && cycle[i-1][j] == 1) {
					i--;
				} else if(j+1 < board_length && cycle[i][j+1] == 1) {
					j++;
				} else if(j > 0 && cycle[i][j-1] == 1) {
					j--;
				}
			}

			// Check if this spot is vacant
			if(board[i][j] > 0 && board[i][j] != (int)length) {
				return false;
			}
		}
		return true;
	}

	bool won() { return length == board_length*board_length; }

	bool move_random() {
		if(head_row + 1 < board_length && board[head_row + 1][head_col] < 1) {
			head_row++;
			increment_snake(head_row,head_col);
			return true;
		} else if(head_row > 0 && board[head_row - 1][head_col] < 1) {
			head_row--;
			increment_snake(head_row,head_col);
			return true;
		} else if(head_col + 1 < board_length && board[head_row][head_col + 1] < 1) {
			head_col++;
			increment_snake(head_row,head_col);
			return true;
		} else if(head_col > 0 && board[head_row][head_col - 1] < 1) {
			head_col--;
			increment_snake(head_row,head_col);
			return true;
		}
		return false;
	}

	// AI
	bool move() {
		if(length == board_length*board_length) {
			return false;
		}
		
		int apple_cyc = cycle[apple_row][apple_col];
		
		std::string best_direction = "";
		int cyc_distance = board_length*board_length;

		if(head_row + 1 < board_length && can_move(head_row + 1, head_col)) {
			int next_cyc = cycle[head_row + 1][head_col];
			cyc_distance = (next_cyc > apple_cyc) ? board_length*board_length - next_cyc + apple_cyc : apple_cyc - next_cyc;
			best_direction = "DOWN";
		}
		if(head_row > 0 && can_move(head_row - 1, head_col)) {
			int next_cyc = cycle[head_row - 1][head_col];
			int next_cyc_dist = (next_cyc > apple_cyc) ? board_length*board_length - next_cyc + apple_cyc : apple_cyc - next_cyc;
			if(next_cyc_dist < cyc_distance) {
				cyc_distance = next_cyc_dist;
				best_direction = "UP";
			}
		}
		if(head_col + 1 < board_length && can_move(head_row, head_col + 1)) {
			int next_cyc = cycle[head_row][head_col + 1];
			int next_cyc_dist = (next_cyc > apple_cyc) ? board_length*board_length - next_cyc + apple_cyc : apple_cyc - next_cyc;
			if(next_cyc_dist < cyc_distance) {
				cyc_distance = next_cyc_dist;
				best_direction = "RIGHT";
			}
		}
		if(head_col > 0 && can_move(head_row, head_col - 1)) {
			int next_cyc = cycle[head_row][head_col - 1];
			int next_cyc_dist = (next_cyc > apple_cyc) ? board_length*board_length - next_cyc + apple_cyc : apple_cyc - next_cyc;
			if(next_cyc_dist < cyc_distance) {
				cyc_distance = next_cyc_dist;
				best_direction = "LEFT";
			}
		}	

		// Take motion
	 	if(best_direction == "UP") {
			head_row--;
			increment_snake(head_row,head_col);
		} else if(best_direction == "DOWN") {
			head_row++;
			increment_snake(head_row,head_col);
		} else if(best_direction == "LEFT") {
			head_col--;
			increment_snake(head_row,head_col);
		} else if(best_direction == "RIGHT") {
			head_col++;
			increment_snake(head_row,head_col);
		} else {
			return move_random();
		}
		return true;
	}

private:
	std::vector<std::vector<int> > board;
	std::vector<std::vector<int> > cycle;
	unsigned int board_length;

	unsigned int length;
	unsigned int head_row;
	unsigned int head_col;

	unsigned int apple_row;
	unsigned int apple_col;
};




#endif