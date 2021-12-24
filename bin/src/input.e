-- main.e
class
    MAIN

create
    make

feature
    obj : OBJECT

    make
    local
        obj2 : INTEGER
    do
        create obj
        create obj2
    end
end
-- EOF

-- object.e
class
    OBJECT

feature
    f
    do
        io.put_string("A::f()%N")
    end

end
-- EOF

-- object2.e
class
    OBJECT2

create
    make

feature
    make
    do
        io.put_string("HELLO")
    end

end
-- EOF
