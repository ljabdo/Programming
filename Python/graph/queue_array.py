
class Queue:
    '''Implements an array-based, efficient first-in first-out Abstract Data Type 
       using a Python array (faked using a List)'''

    def __init__(self, capacity):
        '''Creates an empty Queue with a capacity'''
        self.capacity = capacity
        self.front = 0
        self.rear = 0
        self.items = [None] * capacity
        self.num_items = 0

    def is_empty(self):
        '''Returns True if the Queue is empty, and False otherwise'''
        return self.num_items == 0



    def is_full(self):
        '''Returns True if the Queue is full, and False otherwise'''
        return self.num_items == self.capacity


    def enqueue(self, item):
        '''If Queue is not full, enqueues (adds) item to Queue 
           If Queue is full when enqueue is attempted, raises IndexError'''
        if self.is_full():
            raise IndexError
        self.rear += 1
        index = self.rear % self.capacity
        self.items[index] = item
        self.num_items += 1

    def dequeue(self):
        '''If Queue is not empty, dequeues (removes) item from Queue and returns item.
           If Queue is empty when dequeue is attempted, raises IndexError'''
        if self.is_empty():
            raise IndexError
        self.front += 1
        index = self.front % self.capacity
        item = self.items[index]
        self.items[index] = None
        self.num_items -= 1
        return item


    def size(self):
        '''Returns the number of elements currently in the Queue, not the capacity'''
        return self.num_items

    def peek(self):
        if self.is_empty():
            raise ValueError
        index = self.front % self.capacity
        item = self.items[(index + 1) % self.capacity]
        return item

