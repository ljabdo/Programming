# Name: 		Lucas Abdo
# Course:       CPE 101
# Instructor:   Daniel Kauffman
# Assignment:   pset5
# Term:         Winter 2020

import wordsearch

assert wordsearch.print_puzzle("ABCDEFHIO") == 3
assert wordsearch.print_puzzle("ABCD") == 2
assert wordsearch.print_puzzle("1234567890123456") == 4


assert wordsearch.parse_word(0, "HI HELLO") == "HI"
assert wordsearch.parse_word(3, "HI HELLO") == "HELLO"
assert wordsearch.parse_word(1, "HI HELLO HI") == "HELLO"

assert wordsearch.count_words("HELLO HI YES") == 3
assert wordsearch.count_words("HELLO HI YES HAHA") == 4
assert wordsearch.count_words("HELLO yes") == 2


assert wordsearch.reverse_string("ABCDEFG") == "GFEDCBA"
assert wordsearch.reverse_string("ABC") == "CBA"
assert wordsearch.reverse_string("REVERSE") == "ESREVER"


assert wordsearch.transpose_string(3, "ABCDEFGTT") == "ADGBETCFT"
assert wordsearch.transpose_string(3, "123456789") == "147258369"
assert wordsearch.transpose_string(2, "FOUR") == "FUOR"


assert wordsearch.forward("yes", 3, "ABCYESPLZ") == None
assert wordsearch.forward("hi", 3, "ABCYESPLZ") == None
assert wordsearch.forward("", 3, "ABCYESPLZ") == None


assert wordsearch.backward("YES", 3, "ABCSEYPLZ") == None
assert wordsearch.backward("Hi", 3, "ABCSEYPLZ") == None
assert wordsearch.backward("", 3, "ABCSEYPLZ") == None


assert wordsearch.downward("ASP", 3, "ABCSEYPLZ") == None
assert wordsearch.downward("yes", 3, "ABCSEYPLZ") == None
assert wordsearch.downward("", 3, "ABCSEYPLZ") == None


assert wordsearch.upward("PSA", 3, "ABCSEYPLZ") == None
assert wordsearch.upward("yes", 3, "ABCSEYPLZ") == None
assert wordsearch.upward("", 3, "ABCSEYPLZ") == None


assert wordsearch.diagonal_fd("yes", 3, "ABCDEFGHI") == None
assert wordsearch.diagonal_fd("5", 4, "1234567890123456") == None
assert wordsearch.diagonal_fd(" ", 3, "ABCDEFGHI") == None
assert wordsearch.diagonal_fd("INT", 7, 
	"SABCDNTTEFGOAHRCINOJKLMELPUTINFJOPSQNRSBTIUVTBOOL") == None

assert wordsearch.diagonal_bd("CEG", 3, "ABCDEFGHI") == None
assert wordsearch.diagonal_bd("NONE", 7, 
	"SABCDNTTEFGOAHRCINOJKLMELPUTINFJOPSQNRSBTIUVTBOOL") == None
assert wordsearch.diagonal_bd("24", 3, "123456789") == None
assert wordsearch.diagonal_bd("357", 3, "123456789") == None
assert wordsearch.diagonal_bd("357", 3, "123456789") == None
assert wordsearch.diagonal_bd("HI", 2, "OHIT") == None
assert wordsearch.diagonal_bd("pizza", 4, "THFUEGJSCNGIEJTS") == None
assert wordsearch.diagonal_bd("CEG", 3, "ABCDEFGTS") == None
assert wordsearch.diagonal_bd(" ", 3, "ABCDEFGHI") == None
assert wordsearch.diagonal_bd("PY", 3, "ABCPDEFYP") == None


assert wordsearch.diagonal_bu("951", 3, "123456789") == None
assert wordsearch.diagonal_bu(" ", 3, "123456789") == None
assert wordsearch.diagonal_bu("OBJECT", 7, 
	"SABCDNTTEFGOAHRCINOJKLMELPUTINFJOPSQNRSBTIUVTBOOL") == None


assert wordsearch.diagonal_fu("42", 3, "123456789") == None
assert wordsearch.diagonal_fu(" ", 3, "123456789") == None
assert wordsearch.diagonal_fu("FLOAT", 7, 
	"SABCDNTTEFGOAHRCINOJKLMELPUTINFJOPSQNRSBTIUVTBOOL") == None






























