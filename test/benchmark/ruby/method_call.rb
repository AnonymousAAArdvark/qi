class Toggle
    attr_accessor :state
    def initialize(startState)
        @state = startState
    end

    def value()
        return @state
    end

    def activate()
        @state = !@state
        return self
    end
end


class NthToggle < Toggle
    #Python fields are always public whereas Ruby's are private. attr_accessor makes them public
    attr_accessor
    def initialize(startState, maxCounter)
        super(startState)
        @countMax = maxCounter
        @count = 0
    end

    def activate()
        @count = @count + 1
        if @count >= @countMax
            super
            @count = 0
        end

        return self
    end
end


start = Time.now
n = 100000
val = true
toggle = Toggle.new(val)


for i in (0..n-1)
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
    val = toggle.activate().value()
end


p(toggle.value())

val = true
ntoggle = NthToggle.new(val, 3)

for i in (0..n-1)
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
    val = ntoggle.activate().value()
end

p(ntoggle.value())
p(Time.now - start)
