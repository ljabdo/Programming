# Name: 		Lucas Abdo
# Course:       CPE 101
# Instructor:   Daniel Kauffman
# Assignment:   Calcudoku
# Term:         Winter 2020

import calcudoku

assert calcudoku.find_grid([[3, 2, 1], [7, 3, 4, 2]]) == 8
assert calcudoku.find_grid([[3, 2, 1], [9, 3, 5, 2], [2, 3, 2]]) == 10
assert calcudoku.find_grid([[3, 2, 1], [2, 3, 4, 2], [8, 3, 4]]) == 9

assert calcudoku.find_puzzle_index(16) == [[0, 0, 0, 0], [0, 0, 0, 0], 
	[0, 0, 0, 0], [0, 0, 0, 0]]
assert calcudoku.find_puzzle_index(9) == [[0, 0, 0], [0, 0, 0], [0, 0, 0]]
assert calcudoku.find_puzzle_index(4) == [[0, 0], [0, 0]]

assert calcudoku.duplicates_row([[0, 0, 0], [0, 0, 0], [0, 0, 0]], 3) == True
assert calcudoku.duplicates_row([[1, 0, 1], [3, 5, 0], [2, 0, 1]], 3) == False
assert calcudoku.duplicates_row([[0, 0, 0], [0, 0, 0], [1, 0, 1]], 3) == False

assert calcudoku.duplicates_col([[0, 0, 0], [0, 0, 0], [0, 0, 0]], 3) == True
assert calcudoku.duplicates_col([[0, 0, 0], [1, 0, 0], [1, 0, 0]], 3) == False
assert calcudoku.duplicates_col([[0, 3, 0], [2, 0, 1], [0, 3, 6]], 3) == False

assert calcudoku.sum_check([[3, 1, 2, 0, 0], [2, 2, 3, 0, 5], [0, 3, 0, 4, 0], 
	[1, 0, 0, 0, 4], [3, 0, 0, 2, 0]], 
	[[9, 8, 2], [5, 1, 6], [3, 5, 3]], 5, 3) == True
assert calcudoku.sum_check([[1, 2, 3, 1, 0], [0, 0, 0, 0, 0], [0, 1, 2, 3, 5], 
	[4, 5, 4, 3, 4], [3, 0, 0, 2, 0]], 
	[[9, 8, 2], [5, 1, 6], [3, 5, 3]], 5, 8) == True
assert calcudoku.sum_check([[3, 3, 2, 0, 0], [2, 2, 3, 0, 5], 
	[0, 3, 0, 4, 0], [1, 0, 0, 0, 4], [3, 0, 0, 2, 0]], 
	[[9, 8, 2], [2, 1, 6], [3, 5, 3]], 5, 1) == False


assert calcudoku.validate([[0, 1], [1, 0]], 2, 
	[[2, 0, 1], [2, 1, 2]], 1) == True
assert calcudoku.validate([[0, 1], [1, 0]], 2, 
	[[2, 0, 1], [2, 1, 2]], 2) == True
assert calcudoku.validate([[0, 3], [1, 0]], 2, 
	[[2, 0, 1], [2, 1, 2]], 1) == False




