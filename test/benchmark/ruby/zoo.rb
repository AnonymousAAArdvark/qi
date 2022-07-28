class Zoo
    #Python fields are always public whereas Ruby's are private. attr_accessor makes them public
    attr_accessor :cat, :donkey, :elephant, :aarvark, :baboon, :fox
    def initialize()
        @aarvark = 1
        @baboon = 1
        @cat = 1
        @donkey = 1
        @elephant = 1
        @fox = 1
    end

    def ant()
        return @aarvark
    end

    def banana()
        return @baboon
    end

    def tuna()
        return @cat
    end

    def hay()
        return @donkey
    end

    def grass()
        return @elephant
    end

    def mouse()
        return @fox
    end
end


zoo = Zoo.new
s = 0
start = Time.now

while s < 10000000
    s = s + zoo.ant() + zoo.banana() + zoo.tuna() + zoo.hay() + zoo.grass() + zoo.mouse()
end

p(s)
p(Time.now - start)
