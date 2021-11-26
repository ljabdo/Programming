# Name: 		Lucas Abdo
# Course:       CPE 101
# Instructor:   Daniel Kauffman
# Assignment:   Calcudoku
# Term:         Winter 2020


def main():
	index = 0
	cage_count = cage_counter()
	cage_list = find_cage_list(cage_count)
	max_val = find_grid(cage_list)
	length = int(max_val ** 0.5)
	puzzle_index = find_puzzle_index(max_val)
	while index < max_val:
		row = index // length
		col = index % length
		if puzzle_index[row][col] >= length:
			puzzle_index[row][col] = 0
			index -= 1
		else:
			puzzle_index[row][col] += 1
			if validate(puzzle_index, length, cage_list, index) == True:
				index += 1
	print_grid(puzzle_index, length)

	
	#while index < 5:
	#	row = index // length
	#	col = index % length
	#	#puzzle_index[row][col] += 1
	#	if sum_check(puzzle_index, cage_list, length, index) == False:
	#		puzzle_index[row][col] = 0
	#		#index -= 1
	#	elif duplicates_row(puzzle_index, length) == False:
	#		puzzle_index[row][col] += 1
	#	elif duplicates_col(puzzle_index, length) == False:
	#		puzzle_index[row][col] += 1
	#	else:
	#		puzzle_index[row][col] += 1
	#	if validate(puzzle_index, length, cage_list, index) == True:
	#		index += 1
	#	print(puzzle_index[row])
	#print_grid(puzzle_index, length)

	# sum_check(puzzle_index: list, cage_list: list, length: int, index: int):
	#duplicates_row(puzzle_index, length)
	#index = 0
	#while index < max_val:
	#	row = index // length
	#	puzzle_index[row][index] += 1
	#return None


def cage_counter():
	cage_total = int(input())
	return cage_total


def find_cage_list(cage_count: int):
	integers = []
	p = 0
	x = cage_count
	while p < x:
		new_string = input()
		new_list = new_string.split()
		for i in range(len(new_list)):
			new_list[i] = int(new_list[i])
		integers.append(new_list)
		p += 1
	return integers


def print_grid(puzzle_index: list, length):
	new = ""
	for i in range(length):
		for x in range(length):
			if x < length - 1:
				new += str(puzzle_index[i][x]) + " "
			else:
				new += str(puzzle_index[i][x])
		print(new)
		new = ""


def find_grid(integers: list):
	max_val = 0
	new_string = ""
	for i in range(len(integers)):
		index_string = integers[i]
		for x in range(len(index_string)):
			if index_string[x] > max_val:
				max_val = index_string[x]
	max_val += 1
	puzzle_size = int(max_val ** 0.5)
	for i in range(puzzle_size):
		for i in range(puzzle_size):
			new_string += "0 "
		#print (new_string)
		new_string = ""
	return max_val

#def puzzle_index(max_val: int):
#	puzzle_index = []
#	for i in range(max_val):
#		puzzle_index.append(0)
#	print (puzzle_index)
#	return puzzle_index

def find_puzzle_index(max_val: int):
	length = int(max_val ** 0.5)
	puzzle_index = []
	for i in range(length):
		add_list = []
		for i in range(length):
			add_list.append(0)
		puzzle_index.append(add_list)
	#print (puzzle_index)
	return puzzle_index


def duplicates_row(puzzle_index, length: int):
	for i in range(length):
		for x in range(length):
			if (puzzle_index[i].count(puzzle_index[i][x]) > 1 
				and puzzle_index[i][x] != 0):
				return False
	return True


def duplicates_col(puzzle_index: list, length: int):
	new_list = [i for row in puzzle_index for i in row]
	transposed_list = [new_list[i::length] for i in range(length)]
	for p in range(length):
		for z in range(length):
			if (transposed_list[p].count(transposed_list[p][z]) > 1 
				and transposed_list[p][z] != 0):
				return False
	return True


def sum_check(puzzle_index: list, cage_list: list, length: int, index: int):
	for x in range(len(cage_list)):
		#print(x)
		for p in range(len(cage_list[x]) - 1):
			if cage_list[x][p + 1] == index:
				cage = cage_list[x]
				#print (cage)
	max_sum = cage[0]
	cage_sum = 0
	cell = 0
	for i in range(len(cage) - 1):
		cell = cage[i + 1]
		row = cell // length
		col = cell % length
		cage_sum += puzzle_index[row][col]
		#print (cage_sum)
	if cage_sum > max_sum:
		return False
	return True


def validate(puzzle_index: list, length: int, cage_list: list, index: int):
	if (sum_check(puzzle_index, cage_list, length, index) 
		and duplicates_col(puzzle_index, length) 
		and duplicates_row(puzzle_index, length)):
		return True
	return False


#main()

#puzzle_index(25)
#puzzle_txt = [[3,1,2,0,0],[2,2,3,0,5],[0,3,0,4,0],[1,0,0,0,4],[3,0,0,2,0]]
#print_grid(puzzle_txt, 5)
#sum_check([[3,1,2,0,0],[2,2,3,0,5],[0,3,0,4,0],[1,0,0,0,4],[3,0,0,2,0]], [[9,8,2],[5,1,6],[3,5,3]], 5, 3)
#length = 5
#print(duplicates_col(puzzle_index, length))
#find_grid(puzzle_txt)
#main()
#print(find_puzzle_index(25))
#print(puzzle_index(16))
#check duplicates
#increment
#check sum
#if sum > cage_sum
#go back one and set to 0
#continue
#main()

