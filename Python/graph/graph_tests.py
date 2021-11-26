import unittest
from graph import *

class TestList(unittest.TestCase):

    def test_01(self):
        g = Graph('test1.txt')
        self.assertEqual(g.conn_components(), [['v1', 'v2', 'v3', 'v4', 'v5'], ['v6', 'v7', 'v8', 'v9']])
        self.assertTrue(g.is_bipartite())
        
    def test_02(self):
        g = Graph('test2.txt')
        self.assertEqual(g.conn_components(), [['v1', 'v2', 'v3'], ['v4', 'v6', 'v7', 'v8']])
        self.assertFalse(g.is_bipartite())

    def test_03(self):
        p = Graph('test3.txt')
        self.assertEqual(p.conn_components(), [['v1', 'v2', 'v3', 'v4', 'v5', 'v7', 'v8', 'v9']])
        #self.assertEqual(p.get_vertex('v3'), )


if __name__ == '__main__':
   unittest.main()
