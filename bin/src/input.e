-- main.e
class
    MAIN

create
    make

feature
    make
    do
        io.put_string("HELLO, WORLD!");
    end
end
-- EOF

-- a.e
class
    A

inherit
    C

end
-- EOF

-- b.e
class
    B

inherit
    E
    D

end
-- EOF

-- c.e
class
    C

inherit
    F

end
-- EOF

-- d.e
class
    D

inherit
    F

end
-- EOF

-- e.e
class
    E

end
-- EOF

-- f.e
class
    F

end
-- EOF
