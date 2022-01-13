function foo(i::Int32)::Int32
    if i > 10
        i = i + 1
    else
        i = i - 1
    end
    return i
end

function foo2(i::Int32)
    if i > 10
        i
    else
        i = i + 1
        i
    end
end

i = 1
a = i + foo(i)
total = (1.0 + 2 * 3) - i
if total > 666.6 && total != 500.0
    i = i + 1
    if total < 10
        i = i + 2
    end
else
    if total < 10
        i = i - 1
    else
        i = i - 2
    end
end
i


