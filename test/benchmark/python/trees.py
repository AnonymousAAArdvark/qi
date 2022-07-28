import time


class Tree:
    def __init__(self, depth):
        self.depth = depth
        if depth > 0:
            self.a = Tree(depth - 1)
            self.b = Tree(depth - 1)
            self.c = Tree(depth - 1)
            self.d = Tree(depth - 1)
            self.e = Tree(depth - 1)

    def walk(self):
        if self.depth == 0:
            return 0

        return (
            self.depth
            + self.a.walk()
            + self.b.walk()
            + self.c.walk()
            + self.d.walk()
            + self.e.walk()
        )


tree = Tree(8)
start = time.process_time()
for i in range(100):
    if tree.walk() != 122068:
        print("Error")

print(time.process_time() - start)
