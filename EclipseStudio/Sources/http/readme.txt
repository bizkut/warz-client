HTTP GET does not handle URL re-directs, refresh tags, or any other fanciness like that; it lets you, the user, decide how to handle the data that the server returns, in true minimalist fashion.

Get the source as nettest-src.zip. 

For documentation, look at the unit test function, read the comments in the mysteriously named header , and look at the nettest.cpp implementation file, which implements a very bare-bones wget-like application that snarfs web URLs to disk.

To add to your own project, add mynetwork.cpp and sock_port.cpp to the project. Make sure that sock_port.h and mynetwork.h can be found in your include path. Build. That's it!

To start a new request, call NewHttpRequest(url), passing a full "http://host/path" URL (:port is optional). Then keep polling the request by checking whether it's complete(); when it is, you can read() data out of it until there is no more to read. Call dispose() when you're done.

Reading data while it's streaming (not yet complete()) is also supported, as well as calling rewind() to start reading from the beginning again. That, however, concludes the feature list. I told you it was small!

You will know something went wrong when NewHttpRequest() returns NULL, or when the request is complete() but read() returns 0 bytes. You don't know WHAT went wrong -- but, hey, that's the web for you! 

