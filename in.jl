function swap(v::Vector{Int32})
    temp = v[0]
    v[0] = v[1]
    v[1] = temp
    end

function mean(a::Float32, b::Float32)::Float32
    return (a + b) / 2
end

v = [1; 2; 3; 4]
swap(v)
x = 10.0 + 7.0
i = 0
z = x + mean(x - 7, v[3 + i] * 3.5)
