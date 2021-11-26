import unittest
import filecmp
import subprocess
from huffman import *



class TestList(unittest.TestCase):
    def test_cnt_freq(self):
        freqlist	= cnt_freq("file2.txt")
        anslist = [2, 4, 8, 16, 0, 2, 0]
        self.assertListEqual(freqlist[97:104], anslist)
        freqlist1 = cnt_freq('file1.txt')
        anslist1 = [4, 3, 2, 1]
        self.assertListEqual(freqlist1[97:101], anslist1)
        a = HuffmanNode(100, 5)
        b = HuffmanNode(100, 10)
        self.assertTrue(comes_before(a,b))

    def test_tree1(self):
        freqlist = cnt_freq('file1.txt')
        hufftree1 = create_huff_tree(freqlist)
        self.assertEqual(hufftree1.freq, 13)
        self.assertEqual(hufftree1.char, 32)
        codes = create_code(hufftree1)
        left = hufftree1.left
        self.assertEqual(left.freq, 6)
        self.assertEqual(left.char, 32)
        left_right = left.right
        self.assertEqual(left.right.freq, 3)
        self.assertEqual(left.right.char, 98)
        right = hufftree1.right
        self.assertEqual(right.freq, 7)
        self.assertEqual(right.char, 97)
        self.assertEqual(hufftree1.left.left.freq, 3)
        self.assertEqual(hufftree1.left.left.char, 32)
        print(codes)

    def test_create_huff_tree(self):
        freqlist = cnt_freq("file2.txt")
        hufftree = create_huff_tree(freqlist)
        self.assertEqual(hufftree.freq, 32)
        self.assertEqual(hufftree.char, 97)
        left = hufftree.left
        self.assertEqual(left.freq, 16)
        self.assertEqual(left.char, 97)
        right = hufftree.right
        self.assertEqual(right.freq, 16)
        self.assertEqual(right.char, 100)

    def test_create_code(self):
        freqlist = cnt_freq("file2.txt")
        hufftree = create_huff_tree(freqlist)
        codes = create_code(hufftree)
        self.assertEqual(codes[ord('d')], '1')
        self.assertEqual(codes[ord('a')], '0000')
        self.assertEqual(codes[ord('f')], '0001')

    def test_create_header(self):
        freqlist = cnt_freq("file2.txt")
        self.assertEqual(create_header(freqlist), "97 2 98 4 99 8 100 16 102 2")

    def test_01_textfile(self):
        huffman_encode("file1.txt", "file1_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb file1_out.txt file1_soln.txt", shell = True)
        self.assertEqual(err, 0)

    def test_01_textfile(self):
        huffman_encode("file2.txt", "file2_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb file2_out.txt file2_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_2_textfile(self):
        huffman_encode("declaration.txt", "declaration_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb declaration_out.txt declaration_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_02_textfile(self):
        huffman_encode("test1.txt", "test1_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb test1_out.txt test1_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_03_textfile(self):
        huffman_encode("test35.txt", "test35_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb test1_out.txt test1_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_04_textfile(self):
        huffman_encode("one_char.txt", "one_char_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb one_char_out.txt one_char_soln.txt", shell=True)
        self.assertEqual(err, 0)


    def test_decode(self):
        huffman_decode("declaration_soln.txt", "declaration_decode_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb one_char_out.txt one_char_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_decode1(self):
        huffman_decode("one_char_soln.txt", "one_char_decode_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb one_char_out.txt one_char_soln.txt", shell=True)
        self.assertEqual(err, 0)


    def test_decode_empty(self):
        huffman_decode("test1.soln.txt", "test1_decode_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb one_char_out.txt one_char_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_decode_3(self):
        huffman_decode("multiline_soln.txt", "multiline_decode_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb one_char_out.txt one_char_soln.txt", shell=True)
        self.assertEqual(err, 0)

    def test_decode_4(self):
        huffman_decode("nofile.txt", "multiline_decode_out.txt")
        # capture errors by running 'diff' on your encoded file with a *known* solution file
        err = subprocess.call("diff -wb one_char_out.txt one_char_soln.txt", shell=True)
        self.assertEqual(err, 0)




if __name__ == '__main__': 
   unittest.main()
