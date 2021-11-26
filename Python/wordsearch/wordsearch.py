# Name: 		Lucas Abdo
# Course:       CPE 101
# Instructor:   Daniel Kauffman
# Assignment:   Wordsearch Project
# Term:         Winter 2020


#puzzle_txt = "SABCDNTTEFGOAHRCINOJKLMELPUTINFJOPSQNRSBTIUVTBOOL"
#puzzle_words = "BOOL CHAR INT FLOAT LIST NONE OBJECT STR TUPLE VOID"


def main() -> None:
	puzzle_txt = input()
	puzzle_words = input()
	width = print_puzzle(puzzle_txt)
	print ("")
	for i in range(count_words(puzzle_words)):         #prints puzzle and sets width variable to puzzle width
		forward(parse_word(i, puzzle_words), width, puzzle_txt)   #go through words with for loop
		backward(parse_word(i, puzzle_words), width, reverse_string(puzzle_txt))
		downward(parse_word(i, puzzle_words)
			, width, transpose_string(width, puzzle_txt))
		upward(parse_word(i, puzzle_words), width, puzzle_txt)
		diagonal_fd(parse_word(i, puzzle_words), width, puzzle_txt)
		diagonal_bd(parse_word(i, puzzle_words), width, puzzle_txt)
		diagonal_bu(parse_word(i, puzzle_words), width, puzzle_txt)
		diagonal_fu(parse_word(i, puzzle_words), width, puzzle_txt)
	#diagonal_bd("CEG", 3, "ABCDEFGTS")
		#negative_diagonal(parse_word(6), width, puzzle_txt)
		#diagonal(width, puzzle_txt)
 


def print_puzzle(puzzle_txt: str):
	width = 0
	new = ""
	count = 0
	for i in puzzle_txt:
		new += i
		if len(new) ** 2 == len(puzzle_txt):
			width = len(new)
			print (new)
			new = ""
	return width


def parse_word(position: int, puzzle_words: str):
	#"BOOL CHAR INT FLOAT LIST NONE OBJECT STR TUPLE VOID"
	new = ""
	count = 0
	for i in puzzle_words:
		if i == " ":
			if count == position:
				return new.strip()
			count += 1
			new = ""
		new += i
	return new.strip()


def count_words(puzzle_words: str):
	new = ""
	count = 0
	for i in puzzle_words:
		if i == " ":
			count += 1
			new = ""
		new += i
	return count + 1


def reverse_string(puzzle: str):
	reverse_string = ""
	addi = -1
	for i in puzzle:
		reverse_string += puzzle[addi]
		addi -= 1
	return reverse_string


def transpose_string(width: int, puzzle: str):
	transpose_string = ""
	colomn_count = 0
	for i in range(width):
		for i in range(colomn_count, len(puzzle), width):
			transpose_string += puzzle[i]
		colomn_count += 1
	return transpose_string


def forward(word: str, width: int, puzzle_txt: str):
	index = puzzle_txt.find(word)               #index returns -1 if find doesnt find word
	if index != -1:
		row = str(index // width)
		col = str(index % width)
		final = ": [FF] (" + row + ", " + col + ")"
		print(f"{word:>{width}}" + final)
	return None


def backward(word: str, width: int, reverse_string: str):
	#BOOL = LOOB
	#fix col and row for string - finding row/cow of reverse string instead of reg
	#reverse_string = ""
	#addi = -1
	#for i in puzzle_txt:
	#	reverse_string += puzzle_txt[addi]
	#	addi -= 1

	index = reverse_string.find(word)                      #index returns -1 if find doesnt find word
	new_index = len(reverse_string) - (index + 1)
	if index != -1:
		row = str(new_index // width)
		col = str(new_index  % width)
		final = ": [BB] (" + row + ", " + col + ")"
		print(f"{word:>{width}}" + final)
	return None
	

def downward(word: str, width: int, transpose_string: str):
	#transpose_string = ""
	#colomn_count = 0
	#for i in range(width):
	#	for i in range(colomn_count, len(puzzle_txt), width):
	#		transpose_string += puzzle_txt[i]
	#	colomn_count += 1
	index = transpose_string.find(word)
	if index != -1:
		col = str(index // width)   #COL AND ROW ARE SWITCH IN TRANPOSITION
		row = str(index % width)    # HAVE ALREADY BEEN SWITCHED IN CODE this line is actually col
		final = ": [DD] (" + row + ", " + col + ")"
		print(f"{word:>{width}}" + final)
	return None


def upward(word: str, width: int, puzzle: str):
	transpose_reverse = transpose_string(width, puzzle)
	transpose_reverse = reverse_string(transpose_reverse)
	index = transpose_reverse.find(word)
	new_index = len(transpose_reverse) - (index + 1)
	if index != -1:
		col = str(new_index // width)
		row = str(new_index % width)
		final = ": [UU] (" + row + ", " + col + ")"
		print(f"{word:>{width}}" + final)
	return None



def diagonal_fd(word: str, width: int, puzzle: str):
	index_final = 0
	first_letter = word[0]
	diagonal_string = ""
	for x in range(len(puzzle)):
		if puzzle[x] == first_letter:
			row = str(x // width)
			col = str(x % width)
			for i in range(x, len(puzzle), width + 1):
				diagonal_string += puzzle[i]
				if i % width == 6:    #check if index of i is in right most col
					break
			index = diagonal_string.find(word)
			if index != -1:
				final = ": [FD] (" + row + ", " + col + ")"
				print(f"{word:>{width}}" + final)
			diagonal_string = ""
	return None


def diagonal_bd(word: str, width: int, puzzle: str):
	#word = reverse_string(word)
	index_final = 0
	first_letter = word[0]
	diagonal_string = ""
	for x in range(len(puzzle)):
		if puzzle[x] == first_letter:
			row = str(x // width)
			col = str(x % width)
			for i in range(x, len(puzzle), width - 1):
				diagonal_string += puzzle[i]
				#print(diagonal_string)
				if i % width == 0:  
					break
				#print(i)
			index = diagonal_string.find(word)
			if index != -1:
				#final = ": [BD] (" + row + ", " + col + ")"
				print(f"{word:>{width}}" + ": [BD] (" + row + ", " + col + ")")
			diagonal_string = ""
	return None


def diagonal_bu(word: str, width: int, puzzle: str):
	word = reverse_string(word)
	index_final = 0
	first_letter = word[0]
	diagonal_string = ""
	for x in range(len(puzzle)):
		if puzzle[x] == first_letter:
			for i in range(x, len(puzzle), width + 1):
				diagonal_string += puzzle[i]
				if word == diagonal_string:
					index_final = i
				if i % width == 6:    #check if index of i is in right most col
					break
			index = diagonal_string.find(word)
			if index != -1:
				word = reverse_string(word)
				row = str(index_final // width)
				col = str(index_final % width)
				#final = ": [BU] (" + row + ", " + col + ")"
				print(f"{word:>{width}}" + ": [BU] (" + row + ", " + col + ")")
			diagonal_string = ""
	return None


def diagonal_fu(word: str, width: int, puzzle: str):
	word = reverse_string(word)
	index_final = 0
	first_letter = word[0]
	diagonal_string = ""
	for x in range(len(puzzle)):
		if puzzle[x] == first_letter:
			for i in range(x, len(puzzle), width - 1):
				diagonal_string += puzzle[i]
				if word == diagonal_string:
					index_final = i
				if i % width == 0:    #check if index of i is in right most col
					break
			index = diagonal_string.find(word)
			if index != -1:
				word = reverse_string(word)
				row = str(index_final // width)
				col = str(index_final % width)
				#final = ": [FU] (" + row + ", " + col + ")"
				print(f"{word:>{width}}" + ": [FU] (" + row + ", " + col + ")")
			diagonal_string = ""
	return None




#main()
#print_puzzle()
#forward("BOOL", width)
#print(parse_word())




	


