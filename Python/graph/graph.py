from stack_array import * #Needed for Depth First Search
from queue_array import * #Needed for Breadth First Search


class Vertex:
    '''Add additional helper methods if necessary.'''
    def __init__(self, key):
        '''Add other attributes as necessary'''
        self.key = key
        self.adjacent_to = []
        self.bool = False
        self.color = None

class Graph:
    '''Add additional helper methods if necessary.'''
    def __init__(self, filename):
        '''reads in the specification of a graph and creates a graph using an adjacency list representation.  
           You may assume the graph is not empty and is a correct specification.  E.g. each edge is 
           represented by a pair of vertices.  Note that the graph is not directed so each edge specified 
           in the input file should appear on the adjacency list of each vertex of the two vertices associated 
           with the edge.'''
        self.freq_list = self.create_graph(filename)

    def create_graph(self, filename):
        f = open(filename, "r")
        new_list = f.readlines()
        newer_list = []
        vertex_list = []
        for x in range(len(new_list)):
            newer_list.append(new_list[x].strip().split())
            if vertex_list.count(newer_list[x][0]) < 1:
                vertex_list.append(newer_list[x][0])
            if vertex_list.count(newer_list[x][1]) < 1:
                vertex_list.append(newer_list[x][1])
        #print(vertex_list)
        for l in range(len(vertex_list)):
            vertex_list[l] = Vertex(vertex_list[l])
        for y in range(len(vertex_list)):
            frequency_list = []
            for z in range(len(newer_list)):
                if vertex_list[y].key == newer_list[z][0]:
                    #frequency_list.append(newer_list[z][1])
                    for k in range(len(vertex_list)):
                        if vertex_list[k].key == newer_list[z][1]:
                            vertex_list[y].adjacent_to.append(vertex_list[k])
                            break
                if vertex_list[y].key == newer_list[z][1]:
                    for u in range(len(vertex_list)):
                        if vertex_list[u].key == newer_list[z][0]:
                            vertex_list[y].adjacent_to.append(vertex_list[u])
                            break
                    #frequency_list.append(newer_list[z][0])
            #vertex_list[y].append(frequency_list)
        f.close()
        #for b in range(len(vertex_list)):
        #    print("vertex " + vertex_list[b].key)
        #    for m in range(len(vertex_list[b].adjacent_to)):
        #        print(vertex_list[b].adjacent_to[m].key)
        #print(self.freq_list)
        return vertex_list

    def add_vertex(self, key):
        '''Add vertex to graph, only if the vertex is not already in the graph.'''
        for x in range(len(self.freq_list)):
            if self.freq_list[x].key == key:
                return
        self.freq_list.append(Vertex(key))

    def get_vertex(self, key):
        '''Return the Vertex object associated with the id. If id is not in the graph, return None'''
        for x in range(len(self.freq_list)):
            if self.freq_list[x].key == key:
                return self.freq_list[x]
        return None

    def add_edge(self, v1, v2):
        '''v1 and v2 are vertex id's. As this is an undirected graph, add an 
           edge from v1 to v2 and an edge from v2 to v1.  You can assume that
           v1 and v2 are already in the graph'''
        #print(self.freq_list)
        v1_spot = self.get_vertex(v1)
        v2_spot = self.get_vertex(v2)
        self.freq_list[self.freq_list.index(v2_spot)].adjacent_to.append(v1_spot)
        self.freq_list[self.freq_list.index(v1_spot)].adjacent_to.append(v2_spot)
        #v2_spot.adjacent_to.append(v1)
        #v1_spot.adjacent_to.append(v2)

    def get_vertices(self):
        '''Returns a list of id's representing the vertices in the graph, in ascending order'''
        new_list = []
        adder = 0
        for x in range(len(self.freq_list)):
            new_list.append(self.freq_list[x].key)
        #for y in range(len(new_list)):
        #    for m in new_list[y]:
        #        adder += ord(m)
        #    new_list[y] == adder
        #    adder = 0
        new_list.sort()
        return new_list

    def conn_components(self): 
        '''Returns a list of lists.  For example, if there are three connected components 
           then you will return a list of three lists.  Each sub list will contain the 
           vertices (in ascending order) in the connected component represented by that list.
           The overall list will also be in ascending order based on the first item of each sublist.
           This method MUST use Depth First Search logic!'''
        vertex_list = self.get_vertices()
        new_list = []
        while True:
            id = self.get_vertex(vertex_list[0])
            visited = self.dfs(id)
            new_list.append(visited)
            #print(vertex_list)
            for y in visited:
                if y.key in vertex_list:
                    vertex_list.remove(y.key)
            #print(new_list)
            #for p in range(len(new_list)):
            #    for m in range(len(new_list[p])):
            #        print(new_list[p][m].key)
            if len(vertex_list) == 0:
                break
        #print(new_list)
        return_list = []
        for x in range(len(new_list)):
            adder_list = []
            for p in range(len(new_list[x])):
                adder_list.append(new_list[x][p].key)
            adder_list.sort()
            return_list.append(adder_list)
        #add_list = []
        #for m in range(len(return_list)):
        #    add_list.append(return_list[m][0])
        #add_list.sort()
        #print(add_list)
        #for t in add_list:
        return return_list


    def dfs(self, node):
        visited = []
        stacked = Stack(len(self.freq_list))
        stacked.push(node)
        while stacked.size():
            current = stacked.pop()
            if current not in visited:
                visited.append(current)
            for x in current.adjacent_to:
                if x not in visited:
                    stacked.push(x)
        return visited

    def is_bipartite(self):
        '''Returns True if the graph is bicolorable and False otherwise.
           This method MUST use Breadth First Search logic!'''
        booler = False
        vertex_list = self.get_vertices()
        new_list = []
        while True:
            id = self.get_vertex(vertex_list[0])
            visited = self.bfs(id)
            if visited is None:
                return False
            new_list.append(visited)
            for y in visited:
                if y.key in vertex_list:
                    vertex_list.remove(y.key)
            if len(vertex_list) == 0:
                break
        # print(new_list)
        return True

        return_list = []
        for x in range(len(new_list)):
            adder_list = []
            for p in range(len(new_list[x])):
                adder_list.append(new_list[x][p].key)
            adder_list.sort()
            return_list.append(adder_list)
        re
        return return_list
        return self.bfs(self.freq_list[0])




    def bfs(self, node):
        visited = []
        if len(node.adjacent_to) == 0:
            visited.append(node)
            return visited
        que = Queue(len(self.freq_list))
        que.enqueue(node)
        index = 2
        node.color = index % 2
        index += 1
        while que.is_empty() is False:
            current = que.dequeue()
            for x in current.adjacent_to:
                if current.color == x.color:
                    return None
                if x not in visited:
                    visited.append(x)
                    if x.color is None:
                        x.color = index % 2
                    que.enqueue(x)
            index += 1
        #print(visited)
        #for m in range(len(visited)):
        #    print(visited[m].key)
        return visited
        #print(visited)

    def print_freq_list(self):
        for x in range(len(self.freq_list)):
            print("vertex " + self.freq_list[x].key)
            for y in range(len(self.freq_list[x].adjacent_to)):
                try:
                    print(self.freq_list[x].adjacent_to[y].key)
                except:
                    pass



#p = Graph("test3.txt")
#print(p.freq_list)
#p.add_vertex('v10')
#p.add_edge('v2','v3')
#p.print_freq_list()
#print(p.freq_list)
#print(p.freq_list)
#print(p.get_vertex('v3'))
#p.add_vertex('v11')
#p.add_edge('v6', 'v1')
#print(p.get_vertices())
#print(p.conn_components())
#p.bfs(p.freq_list[0])
#print(p.is_bipartite())
#p.print_freq_list()
