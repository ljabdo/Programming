# Name:         Lucas Abdo
# Course:       CPE 101
# Instructor:   Daniel Kauffman
# Assignment:   Pixel Magic
# Term:         Winter 2020

import pixelmagic

assert pixelmagic.find_width_height("360 240\n") == [360, 240]
assert pixelmagic.find_width_height("103 503\n") == [103, 503]
assert pixelmagic.find_width_height("592 35") == [592, 35]

assert (pixelmagic.picture_list(2, 2, ["p3", "255 535", "355", "53 53 54", 
	"30 35 54", "35 20 54", "3 75 54"]) == [[53, 53, 54],
	 [30, 35, 54], [35, 20, 54], [3, 75, 54]])
assert (pixelmagic.picture_list(2, 2, ['p3', '255 533', '533', "5 5 5",
 "3 3 5", "3 2 5", "3 7 5"]) == [[5, 5, 5], [3, 3, 5], 
[3, 2, 5], [3, 7, 5]])
assert (pixelmagic.picture_list(2, 1, ['p3', '533 533', '223', "4 5 3",
 "4 5 3"]) == [[4, 5, 3], [4, 5, 3]])
assert (pixelmagic.picture_list(1, 1, ['p3', '255 533', '533', "53 23 12"]) 
	== [[53, 23, 12]])


assert (pixelmagic.gray(2, 2, [[5, 4, 3], [5, 5, 5], [2, 4, 6], 
	[3, 3, 3]]) == [[4, 4, 4], [5, 5, 5], [4, 4, 4], [3, 3, 3]])
assert (pixelmagic.gray(2, 2, [[5, 4, 3], [6, 6, 6], [10, 15, 20], 
	[3, 3, 3]]) == [[4, 4, 4], [6, 6, 6], [15, 15, 15], [3, 3, 3]])
assert pixelmagic.gray(1, 1, [[50, 30, 10]]) == [[30, 30, 30]]

assert (pixelmagic.error_check(["pixel"]) 
	== "Usage: python3 pixelmagic.py <mode> <image>")
assert (pixelmagic.error_check(["pixel", "fade"]) 
	== "Usage: python3 pixelmagic.py <mode> <image>")
assert (pixelmagic.error_check(["pixel", "turn", "oringal"]) 
	== "Error: Unable to open oringal")
# (pixelmagic.error_check(["pixel", "turn", "original.ppm", "40"]) 
#	== "Error: Turn must be in 90 degree increments")
assert (pixelmagic.error_check(["pixel", "yasy"]) 
	== "Usage: python3 pixelmagic.py <mode> <image>")
assert (pixelmagic.error_check(["pixel", "flip", "tstat"])
	== "Error: Unable to open tstat")


assert pixelmagic.fade(1, 1, [[5, 3, 6]], [5, 3, 3, 3, 4, 4]) == [[1, 0, 1]]
assert pixelmagic.fade(1, 1, [[5, 3, 6]], [5, 3, 3, 2, 6, 4]) == [[1, 0, 1]]
assert pixelmagic.fade(1, 1, [[5, 3, 6]], [5, 3, 3, 2, 6, 7]) == [[1, 0, 1]]
assert (pixelmagic.fade(1, 1, [[5, 35, 233]], [5, 3, 3, 152, 354, 62]) 
	== [[1, 7, 46]])
assert pixelmagic.fade(1, 1, [[5, 3, 6]], [5, 3, 3, 0, 0, 150]) == [[5, 3, 6]]

assert pixelmagic.flip(4, 4, [[1, 1, 1], [2, 2, 2], [3, 3, 3], [4, 4, 4], 
	[5, 5, 5], [6, 6, 6], [7, 7, 7], [8, 8, 8], [9, 9, 9], [10, 10, 10],
	[11, 11, 11], [12, 12, 12], [13, 13, 13], [14, 14, 14], [15, 15, 15],
	[16, 16, 16]]) == ([[4, 4, 4], [3, 3, 3], [2, 2, 2], [1, 1, 1], [8, 8, 8], 
	[7, 7, 7], [6, 6, 6], [5, 5, 5], [12, 12, 12], [11, 11, 11], [10, 10, 10], 
	[9, 9, 9], [16, 16, 16], [15, 15, 15], [14, 14, 14], [13, 13, 13]])

assert (pixelmagic.flip(2, 2, [[2, 2, 2], [1, 1, 1], [4, 4, 4], [3, 3, 3]]) 
	== [[1, 1, 1], [2, 2, 2], [3, 3, 3], [4, 4, 4]])
assert (pixelmagic.flip(2, 2, [[3, 3, 3], [5, 5, 5], [1, 1, 1], [4, 4, 4]]) 
	== [[5, 5, 5], [3, 3, 3], [4, 4, 4], [1, 1, 1]])

assert (pixelmagic.turn(4, 4, [[1, 1, 1], [2, 2, 2], [3, 3, 3], [4, 4, 4], 
	[5, 5, 5], [6, 6, 6], [7, 7, 7], [8, 8, 8], [9, 9, 9], [10, 10, 10],
	[11, 11, 11], [12, 12, 12], [13, 13, 13], [14, 14, 14], [15, 15, 15],
	[16, 16, 16]], [4, 4, 4, 90])) == ([[13, 13, 13], [9, 9, 9], [5, 5, 5],
	 [1, 1, 1], [14, 14, 14], [10, 10, 10], [6, 6, 6], [2, 2, 2], [15, 15, 15], 
	 [11, 11, 11], [7, 7, 7], [3, 3, 3], [16, 16, 16], [12, 12, 12], [8, 8, 8], 
	 [4, 4, 4]])
assert (pixelmagic.turn(2, 2, [[2, 2, 2], [1, 1, 1], [4, 4, 4], [3, 3, 3]], 
	[4, 4, 4, 90]) == ([[4, 4, 4], [2, 2, 2], [3, 3, 3], [1, 1, 1]]))
assert (pixelmagic.turn(2, 2, [[2, 2, 2], [1, 1, 1], [4, 4, 4], [3, 3, 3]], 
	[4, 4, 4, 180]) == ([[3, 3, 3], [4, 4, 4], [1, 1, 1], [2, 2, 2]]))



assert (pixelmagic.blur_boundary(360, 240, 360, [3, 3, 3, 4])
	== [0, 4, 1, 4])
assert (pixelmagic.blur_boundary(360, 240, 6035, [3, 3, 3, 4])
	== [4, 4, 4, 4])
assert (pixelmagic.blur_boundary(360, 240, 719, [3, 3, 3, 4])
	== [4, 0, 1, 4])

assert (pixelmagic.blur(2, 2, [[1, 1, 1], [2, 2, 2], [3, 3, 3], 
	[4, 4, 4]], [3, 3, 3, 4])
	== ([[2, 2, 2], [2, 2, 2], [2, 2, 2], [2, 2, 2]]))
assert (pixelmagic.blur(2, 2, [[3, 5, 6], [2, 3, 4], [5, 6, 3], 
	[6, 3, 4]], [3, 3, 3, 4])
	== ([[4, 4, 4], [4, 4, 4], [4, 4, 4], [4, 4, 4]]))
assert (pixelmagic.blur(2, 2, [[3, 5, 6], [2, 3, 34], [64, 3, 6], 
	[62, 45, 34]], [3, 3, 3, 4])
	== ([[32, 14, 20], [32, 14, 20], [32, 14, 20], [32, 14, 20]]))




















