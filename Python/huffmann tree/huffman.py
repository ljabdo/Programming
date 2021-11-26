class HuffmanNode:
    def __init__(self, char, freq):
        self.char = char   # stored as an integer - the ASCII character code value
        self.freq = freq   # the freqency associated with the node
        self.left = None   # Huffman tree (node) to the left
        self.right = None  # Huffman tree (node) to the right

    def set_left(self, node):
        self.left = node

    def set_right(self, node):
        self.right = node

def comes_before(a, b):
    """Returns True if tree rooted at node a comes before tree rooted at node b, False otherwise"""
    if a.freq == b.freq:
        if a.char < b.char:
            return True
    return a.freq < b.freq

def combine(a, b):
    """Creates and returns a new Huffman node with children a and b, with the "lesser node" on the left
    The new node's frequency value will be the sum of the a and b frequencies
    The new node's char value will be the lesser of the a and b char ASCII values"""
    freq = a.freq + b.freq
    char = b.char
    if a.char < b.char:
        char = a.char
    new_node = HuffmanNode(char, freq)
    if comes_before(a, b):
        new_node.set_left(a)
        new_node.set_right(b)
    #else:
     #   new_node.set_left(b)
      #  new_node.set_right(a)
    #print(new_node.freq)
    return new_node

def cnt_freq(filename):
    """Opens a text file with a given file name (passed as a string) and counts the 
    frequency of occurrences of all the characters within that file"""
    new_list = [0] * 256
    #try:
    f = open(filename, "r")
    #except FileNotFoundError:
    #print("File Not Found")
    new_string = f.read()
    if new_string == "":
        f.close()
        return "No File"
    for x in new_string:
        new_list[ord(x)] += 1
    f.close()
    return new_list

def create_huff_tree(char_freq):
    """Create a Huffman tree for characters with non-zero frequency
    Returns the root node of the Huffman tree"""
    commpare_list = []
    node_list = freq_list_to_node_list(char_freq)
    node_sorted_list = sorted_list(list(node_list))
    while len(node_sorted_list) > 1:
        min1 = node_sorted_list[0]
        min2 = node_sorted_list[1]
        try:
            node_sorted_list[0] = combine(min1, min2)
            #node_sorted_list[0] = combine(node_sorted_list[0], node_sorted_list[1])
            node_sorted_list.remove(node_sorted_list[1])
            node_sorted_list = sorted_list(node_sorted_list)
        except IndexError:
            pass
        #node_sorted_list = [i for i in node_sorted_list if i != 0]
    return node_sorted_list[0]

def freq_list_to_node_list(freq_list):
    for x in range(len(freq_list)):
        if freq_list[x] > 0:
            freq_list[x] = HuffmanNode(x, freq_list[x])
    freq_list = [i for i in freq_list if i != 0]  # removes all 0's from list - still unsorted
    return freq_list


def sorted_list(freq_list):
    new_list = []
    for p in range(len(freq_list)):           #sorts list into asceding order of frequencies
        min = 9999999
        for m in freq_list:
            if m.freq < min:
                min = m.freq
                min_node = m
        new_list.append(min_node)
        freq_list.remove(min_node)
    for i in range(len(new_list)):
        try:
            if comes_before(new_list[i], new_list[i+1]) is False:
                temp = new_list[i]
                new_list[i] = new_list[i+1]
                new_list[i+1] = temp
        except IndexError:
            pass
    return new_list

def create_code(node):
    """Returns an array (Python list) of Huffman codes. For each character, use the integer ASCII representation 
    as the index into the arrary, with the resulting Huffman code for that character stored at that location"""
    new_list = [""] * 256
    new_string = ''
    return create_code_helper(node, new_list, new_string)

def create_code_helper(node, new_list, new_string):
    if node.left is None and node.right is None:
        new_list[node.char] = new_string
    if node.left is not None:
        create_code_helper(node.left, new_list, new_string + '0')
    if node.right is not None:
        create_code_helper(node.right, new_list, new_string + '1')
    return new_list

def create_header(freqs):
    """Input is the list of frequencies. Creates and returns a header for the output file
    Example: For the frequency list asscoaied with "aaabbbbcc, would return “97 3 98 4 99 2” """
    return_string = ''
    for x in range(len(freqs)):
        if freqs[x] > 0:
            return_string += f"{x} {freqs[x]} "
    last_string = return_string.strip()
    return last_string


def huffman_encode(in_file, out_file):
    """Takes inout file name and output file name as parameters
    Uses the Huffman coding process on the text from the input file and writes encoded text to output file
    Take not of special cases - empty file and file with only one unique character"""
    frequency_list = cnt_freq(in_file)
    if frequency_list == "No File":
        p = open(out_file, "w")
        p.close()
        return
    node_list = freq_list_to_node_list(list(frequency_list))
    sorted_node_1st = sorted_list(node_list)
    new_string = huffman_encoder_helper(frequency_list, in_file)
    f = open(out_file, "w", newline = '')
    if len(sorted_node_1st) == 1:
        f.write(create_header(frequency_list))
        f.close()
        return
    #if create_header(frequency_list) == '':
     #   f.close()
      #  return
    f.write(create_header(frequency_list) + "\n")
    #new_string = huffman_encoder_helper(frequency_list, in_file)
    f.write(new_string)
    f.close()
    return


def huffman_encoder_helper(freq_list, in_file):
    hufftree = create_huff_tree(list(freq_list))
    codes = create_code(hufftree)
    f = open(in_file, "r")
    new_string = f.read()
    f.close()
    code_string = ''
    for x in new_string:
        code_string += codes[ord(x)]
    return code_string

def huffman_decode(encoded_file, decode_file):
    try:
        f = open(encoded_file, "r")
    except FileNotFoundError:
        f.close()
        return
    header_string = f.readline()
    if header_string == "":
        x = open(decode_file, 'w')
        x.close()
        return
    freq_list = parse_header(header_string)
    root_node = create_huff_tree(freq_list)
    full_string = f.read()
    if full_string == "":
        y = open(decode_file, "w", newline="")
        temp_string = chr(root_node.char) * root_node.freq
        y.write(temp_string)
        y.close()
        return
    f.close()
    p = open(decode_file, "w", newline="")
    return_string = ''
    current = root_node
    for x in full_string:
        if int(x) == 0:
            current = current.left
        elif int(x) == 1:
            current = current.right
        if current.left is None and current.right is None:
            return_string += chr(current.char)
            current = root_node
    p.write(return_string)
    p.close()
    return


def parse_header(header_string):
    freq_list = [0] * 256
    new_list = header_string.split()
    for x in range(0, len(new_list), 2):
        freq_list[int(new_list[x])] = int(new_list[x + 1])
    return freq_list












