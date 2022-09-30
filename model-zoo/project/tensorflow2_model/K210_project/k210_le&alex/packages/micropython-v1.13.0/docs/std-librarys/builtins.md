# Builtin functions and exceptions

所有的内饰函数和异常类型都在下面描述。

## 函数和类型

- abs()
- all()
- any()
- bin()
- **class** bool
- **class** bytearray
- **class** bytes
- callable()
- chr()
- classmethod()
- compile()
- **class** complex
- delattr(obj, name)

- **class** dict
- dir()

- divmod()
- enumerate()
- eval()
- exec()
- filter()
- **class** float
- **class** frozenset
- getattr()
- globals()
- hasattr()
- hash()
- hex()
- id()
- input()
- **class** int
  - classmethod from_bytes(bytes, byteorder)
    In MicroPython, byteorder parameter must be positional (this is compatible with CPython).
  - to_bytes(size, byteorder)
    In MicroPython, byteorder parameter must be positional (this is compatible with CPython).

- isinstance()
- issubclass()
- iter()
- len()
- class list
- locals()
- map()
- max()
- **class** memoryview
- min()
- next()
- **class** object
- oct()
- open()
- ord()
- pow()
- print()
- property()
- range()
- repr()
- reversed()
- round()
- class set
- setattr()
- **class** slice
  - The slice builtin is the type that slice objects have.
- sorted()
- staticmethod()
- **class** str
- sum()
- super()
- **class** tuple
- type()
- zip()

## 异常类型
- **exception** AssertionError
- **exception** AttributeError
- **exception** Exception
- **exception** ImportError
- **exception** IndexError
- **exception** KeyboardInterrupt
- **exception** KeyError
- **exception** MemoryError
- **exception** NameError
- **exception** NotImplementedError
- **exception** OSError
  - See CPython documentation: OSError. MicroPython doesn’t implement errno attribute, instead use the standard way to access exception arguments: exc.args[0].

- **exception** RuntimeError
- **exception** StopIteration
- **exception** SyntaxError
- **exception** SystemExit
  - See CPython documentation: SystemExit.

- **exception** TypeError
  - See CPython documentation: TypeError.

- **exception** ValueError
- **exception** ZeroDivisionError

