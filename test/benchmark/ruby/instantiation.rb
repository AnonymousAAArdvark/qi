# This benchmark stresses instance creation and initializer calling.
class Foo
    #Python fields are always public whereas Ruby's are private. attr_accessor makes them public
    attr_accessor
    def initialize()
    end
end


start = Time.now
i = 0
while i < 500000
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    Foo.new
    i = i + 1
end

p(Time.now - start)
