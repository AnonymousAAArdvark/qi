class Tree
    attr_accessor :depth, :a, :b, :c, :d, :e
    def initialize(depth)
        @depth = depth
        if depth > 0
            @a = Tree.new(depth - 1)
            @b = Tree.new(depth - 1)
            @c = Tree.new(depth - 1)
            @d = Tree.new(depth - 1)
            @e = Tree.new(depth - 1)
        end
    end

    def walk()
        if @depth == 0
            return 0
        end

        return @depth + @a.walk() + @b.walk() + @c.walk() + @d.walk() + @e.walk()
    end
end

tree = Tree.new(8)
start = Time.now
for i in (1..100)
    if (tree.walk() != 122068)
        puts("Error")
    end
end

puts(Time.now - start)
