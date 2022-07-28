import time


class Zoo:
    def __init__(self):
        self.aarvark = 1
        self.baboon = 1
        self.cat = 1
        self.donkey = 1
        self.elephant = 1
        self.fox = 1

    def ant(self):
        return self.aarvark

    def banana(self):
        return self.baboon

    def tuna(self):
        return self.cat

    def hay(self):
        return self.donkey

    def grass(self):
        return self.elephant

    def mouse(self):
        return self.fox


zoo = Zoo()
s = 0
start = time.process_time()

while s < 10000000:
    s = (
        s
        + zoo.ant()
        + zoo.banana()
        + zoo.tuna()
        + zoo.hay()
        + zoo.grass()
        + zoo.mouse()
    )

print(s)
print(time.process_time() - start)
