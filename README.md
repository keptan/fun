# fun
c++17, header only,  fun library for use in cutegrab 

persistent data structures 
declarative and pure functional style 
no mutation 

lazy, pure functional, static adapters 
``` c++
const auto toUpper = [](const auto c) -> char { return toupper(c);};

const String input = "hello world";
const List<char> output = ListStream(input) | Take(5) | Map(toUpper) | CollectList();
//hello world -> HELLO

const auto lexpression = Integers() | Zip(UniformDist(0,10000)) | Take(10); 
//std::tuples of ints like {0,1531}, {1, 9035}...

```

