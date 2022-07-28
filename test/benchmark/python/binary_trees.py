import time


class Tree:
    def __init__(self, item, depth):
        self.item = item
        self.depth = depth
        if depth > 0:
            item2 = item + item
            depth = depth - 1
            self.left = Tree(item2 - 1, depth)
            self.right = Tree(item2, depth)
        else:
            self.left = None
            self.right = None

    def check(self):
        if self.left == None:
            return self.item

        return self.item + self.left.check() - self.right.check()


minDepth = 4
maxDepth = 14
stretchDepth = maxDepth + 1

start = time.process_time()

print("stretch tree of depth:")
print(stretchDepth)
print("check:")
print(Tree(0, stretchDepth).check())

longLivedTree = Tree(0, maxDepth)

# iterations = 2 ** maxDepth
iterations = 1
d = 0
while d < maxDepth:
    iterations = iterations * 2
    d = d + 1

depth = minDepth
while depth < stretchDepth:
    check = 0
    i = 1
    while i <= iterations:
        check = check + Tree(i, depth).check() + Tree(-i, depth).check()
        i = i + 1

    print("num trees:")
    print(iterations * 2)
    print("depth:")
    print(depth)
    print("check:")
    print(check)

    iterations = iterations / 4
    depth = depth + 2

print("long lived tree of depth:")
print(maxDepth)
print("check:")
print(longLivedTree.check())
print("elapsed:")
print(time.process_time() - start)
