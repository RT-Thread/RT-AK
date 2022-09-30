# ussl – SSL/TLS 模块

This module implements a subset of the corresponding* [`CPython`](http://docs.micropython.org/en/latest/reference/glossary.html#term-cpython) *module, as described below. For more information, refer to the original CPython documentation: [`ssl`](https://docs.python.org/3.5/library/ssl.html#module-ssl).

This module provides access to Transport Layer Security (previously and widely known as “Secure Sockets Layer”) encryption and peer authentication facilities for network sockets, both client-side and server-side.

## 功能函数

- `ussl.wrap_socket`(sock, server_side=False, key=None, cert=None)

Takes a [`stream`](http://docs.micropython.org/en/latest/reference/glossary.html#term-stream) *sock* (usually usocket.socket instance of `SOCK_STREAM` type), and returns an instance of ssl.SSLSocket, which wraps the underlying stream in an SSL context. Returned object has the usual [`stream`](http://docs.micropython.org/en/latest/reference/glossary.html#term-stream) interface methods like `read()`, `write()`, etc. In MicroPython, the returned object does not expose socket interface and methods like `recv()`, `send()`. In particular, a server-side SSL socket should be created from a normal socket returned from[`accept()`](http://docs.micropython.org/en/latest/library/usocket.html#usocket.socket.accept) on a non-SSL listening server socket. Depending on the underlying module implementation in a particular [`MicroPython port`](http://docs.micropython.org/en/latest/reference/glossary.html#term-micropython-port), some or all keyword arguments above may be not supported.

Warning: Some implementations of `ussl` module do NOT validate server certificates, which makes an SSL connection established prone to man-in-the-middle attacks.

## 异常类型

- `ssl.SSLError`

  This exception does NOT exist. Instead its base class, OSError, is used.

## 常量

- `ussl.CERT_NONE`

- `ussl.CERT_OPTIONAL`

- `ussl.CERT_REQUIRED`
- Supported values for **cert_reqs** parameter.