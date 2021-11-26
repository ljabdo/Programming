# Name:         Lucas Abdo
# Course:       CPE 101
# Instructor:   Daniel Kauffman
# Assignment:   Pixel Magic
# Term:         Winter 2020
import sys
from typing import List


def main(argv: List[str]) -> None:
	if error_check(argv) != True:
		print (error_check(argv))
		return None
	lines = file_open(argv)
	width_height = find_width_height(lines[1])
	width = width_height[0]
	height = width_height[1]
	pixel_list = picture_list(width, height, lines)
	if argv[1] == "gray":
		gray_txt = gray(width, height, pixel_list)
		#print(pixel_list)
		write_file(width, height, gray_txt, argv)
	if argv[1] == "fade":
		fade_txt = fade(width, height, pixel_list, argv)
		write_file(width, height, fade_txt, argv)
	if argv[1] == "flip":
		flip_txt = flip(width, height, pixel_list)
		write_file(width, height, flip_txt, argv)
	if argv[1] == "turn":
		if (int(argv[3]) // 90) % 2 == 1:
			turn_txt = turn(width, height, pixel_list, argv)
			write_file(height, width, turn_txt, argv)
		else:
			turn_txt = turn(width, height, pixel_list, argv)
			write_file(width, height, turn_txt, argv)
	if argv[1] == "blur":
		blur_txt = blur(width, height, pixel_list, argv)
		write_file(width, height, blur_txt, argv)
	return None


def error_check(argv):
	if len(argv) <= 2:
		return "Usage: python3 pixelmagic.py <mode> <image>"
	#elif len(argv) == 1:
	#	return "Usage: python3 pixelmagic.py <mode> <image>"
	try:
		file = open(argv[2], "r")
	except (FileNotFoundError, IndexError):
		return "Error: Unable to open " + argv[2]
	if argv[1] == "fade" and len(argv) != 6:
		return "Usage: python3 pixelmagic.py fade <image> <row> <col> <radius>"
	elif argv[1] == "turn" and len(argv) < 4:
		return "Usage: python3 pixelmagic.py turn <image> <degree>"
	elif argv[1] == "turn" and (int(argv[3]) % 90 != 0):
		return "Error: Turn must be in 90 degree increments"
	new_list = ["gray", "fade", "turn", "flip", "blur"]
	check = False
	for x in range(len(new_list)):
		if argv[1] == new_list[x]:
			check = True
	if check == False:
		return "Error: Invalid mode"
	
	return True


def write_file(width, height, changed_list, argv):
	file = open((argv[1] + ".ppm"),  "w")
	file.write("P3\n")
	file.write(f"{width} {height}\n")
	file.write("255\n")
	for x in range((width * height)):
		for v in range(2):
			add = str(changed_list[x][v])
			file.write(add + " ")
		file.write(str(changed_list[x][2]))
		file.write("\n")
	file.close()
	return None


def file_open(argv: list):
	fp = open(argv[2], 'r')
	lines = fp.readlines()
	fp.close()
	return lines


def find_width_height(new_string: str):
	new_list = new_string
	new_list = new_list.split()
	new_list = [int(new_list[0]), int(new_list[1])]
	return new_list


def picture_list(width, height, lines):
	pixel_list = []
	for x in range(3, (width * height) + 3):
		new_list = lines[x].split()
		for x in range(0, 3):
			new_list[x] = int(new_list[x])
		pixel_list.append(new_list)
		new_list = []
		#if len(pixel_list) == width * height:
		#	break
	return pixel_list


def gray(width, height, pixel_list):
	average = 0
	for x in range((width * height)):
		average = (pixel_list[x][0] + 
			pixel_list[x][1] + pixel_list[x][2])
		average = average // 3
		for v in range(3):
			pixel_list[x][v] = average
		average = 0
	return pixel_list


def fade(width, height, pixel_list, argv):
	#python3 pixelmagic.py fade <image> <row> <col> <radius>
	for x in range(len(pixel_list)):
		col = x % width
		row = x // width
		distance = ((((int(argv[4]) - col) ** 2) + 
			((int(argv[3]) - row) ** 2)) ** 0.5)
		multi = ((int(argv[5]) - distance) / int(argv[5]))
		if multi < 0.2:
			multi = 0.2
		pixel_list[x][0] = int(pixel_list[x][0] * multi)
		pixel_list[x][1] = int(pixel_list[x][1] * multi)
		pixel_list[x][2] = int(pixel_list[x][2] * multi)
	return pixel_list


def flip(width, height, pixel_list):
	storage = []
	length = width - 1
	stop = width // 2
	start = 0
	for p in range(height): 
		for x in range(start, stop, 1):
			storage = pixel_list[x]
			pixel_list[x] = pixel_list[length]
			pixel_list[length] = storage
			length -= 1
		start += width
		stop += width
		length = ((p + 2) * width) - 1
	return pixel_list


def turn (width, height, pixel_list, argv):
	flip_num = (int(argv[3]) // 90)
	new_width = width
	for m in range(flip_num):
		transpose_list = []
		colomn_count = 0
		for i in range(new_width):
			for x in range(colomn_count, len(pixel_list), new_width):
				transpose_list.append(pixel_list[x])
			colomn_count += 1
		new_width = width
		new_height = height
		if m % 2 == 0:
			new_width = height
			new_height = width
		transpose_list = flip(new_width, new_height, transpose_list)
		pixel_list = transpose_list
	return transpose_list


def blur(width, height, pixel_list, argv): #or 4
	blur_list = []
	color_average = [0, 0, 0]
	count = 0
	for x in range(len(pixel_list)):
		col = x % width
		row = int(x // width)
		dim = blur_boundary(width, height, x, argv)
		for y in range(x - (width * dim[2]), (x + (width * dim[3]) + 1), width):
			for z in range((y - dim[0]), (y + dim[1]) + 1):
				for p in range(3):
					color_average[p] += pixel_list[z][p]
				count += 1
		for m in range(3):
			color_average[m] = color_average[m] // count
		blur_list.append(color_average)
		color_average = [0, 0, 0]
		count = 0
	return blur_list


def blur_boundary(width, height, index, argv):
	reach = int(argv[3])
	col = index % width
	row = int(index // width)
	x_reach_forward = reach
	x_reach_backward = reach
	y_reach_down = reach
	y_reach_up = reach
	if (col - reach < 0):
		x_reach_backward = reach + (col - reach)
	if (col + reach >= width):
		x_reach_forward = ((width - 1) - col)
	if (row - reach < 0):
		y_reach_up = reach + (row - reach)
	if (row + reach >= height):
		y_reach_down = (height - 1) - row
	return [x_reach_backward, x_reach_forward, y_reach_up, y_reach_down]

#print(blur(2,2,[[3,5,6],[2,3,34],[64,3,6],[62,45,34]], [3,3,3,4]))
#print(blur_boundary(360, 240, 86035, [3, 3, 3, 4]))
#print(turn(4, 2, [[1, 1, 1], [2, 2, 2], [3, 3, 3], [4, 4, 4], [5,5,5],[6,6,6],[7,7,7],[8,8,8]],[4, 4, 4, 90]))
#for each row from reach above to reach below current pixel
    #for each column from reach left to reach right of current pixel
     #   add color components of pixel to running totals
    #average color component totals and add this new pixel to blurred list

'''
2 * reach + 1 
while loop for each picture
while n != max (length * width)
	new = [] = color averages
	for variable in range(2 * reach + 1)
	2 for loops
	iterate through pixels --- set max and min value
	max and min = index of (pixel - reach) * width
	variable value = 
	pixels_list[n - max - reach + second for loop variable + first for loop * width]
	start at first pixel - goes across then goes down
	list comp - every helper function add RGB values
	new index i + helper variable[i] for i in range 3
	for loop to find averages // divide with # of pixels [i] // 
	n = index of pixel
return averages

helper function
while variable not = to reach
	max value on right   max value % width != 359 max += 1
	if value sent in between max and min values
	return [0, 0, 0] if pixel not valid
	min 

'''
#def blur:
#	for p in pixels          PSUEDO code
#	for x in y 
#	for n in x

if __name__ == "__main__":
    main(sys.argv)