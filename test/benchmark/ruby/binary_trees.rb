
class Tree
    attr_accessor :depth, :item, :left, :right
    def initialize(item, depth)
        @item = item
        @depth = depth
        if depth > 0
            item2 = item + item
            depth = depth - 1
            @left = Tree.new(item2 - 1, depth)
            @right = Tree.new(item2, depth)
        else
            @left = nil
            @right = nil
        end
    end

    def check()
        if @left == nil
            return @item
        end

        return @item + @left.check() - @right.check()
    end
end


minDepth = 4
maxDepth = 14
stretchDepth = maxDepth + 1

start = Time.now

puts("stretch tree of depth:")
p(stretchDepth)
puts("check:")
p(Tree.new(0, stretchDepth).check())

longLivedTree = Tree.new(0, maxDepth)

# iterations = 2 ** maxDepth
iterations = 1
d = 0
while d < maxDepth
    iterations = iterations * 2
    d = d + 1
end

depth = minDepth
while depth < stretchDepth
    check = 0
    i = 1
    while i <= iterations
        check = check + Tree.new(i, depth).check() + Tree.new(-i, depth).check()
        i = i + 1
    end

    puts("num trees:")
    p(iterations * 2)
    puts("depth:")
    p(depth)
    puts("check:")
    p(check)

    iterations = iterations / 4
    depth = depth + 2
end

puts("long lived tree of depth:")
p(maxDepth)
puts("check:")
p(longLivedTree.check())
puts("elapsed:")
p(Time.now - start)