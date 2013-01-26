This is a project for course DVD017 at Blekinge Institute of Technology,
held in fall 2006 by Fredrik Erlandsson.

# Instructions for inspectors

A Makefile has been provided. To compile the project, simply issue

    make all

The two binaries that should be generated are alice and bob.
To start the server, issue

    ./bob -p 7000

to have Bob the Server listen to port 7000. Establish a connection with

    ./alice -h 127.0.0.1 -p 7000

(change server IP and port if applicable).

If changes are necessary on any of the source files, issue

    make all

to compile all parts again.
To compile only parts, there are also the targets
"alice" and "bob" defined in the Makefile.

# Some background information

The project has been divided into three parts by the lecturer:

1. Blowfish encryption algorithm:
   Source files are `blowfish.{c, h}`, `blowinit.h`
2. Diffie-Hellman key exchange:
   `dh.{c, h}`, `genprime.{c, h}`
3. TCP server/client architecture to use the above:
    `alice.c`, `bob.c`

Alice and Bob are the client and server, respectively.
Bob the Server has been implemented to listen to a socket
and wait for a connection from Alice.
As soon as the connection comes in,
Alice starts negotiating the key by sending p, g, and a.
Bob can calculate the key and responds with his own public key.
After Alice has calculated the key,
a prompt is shown where you can type in something
that will be encrypted, transmitted to Bob by Alice, then decrypted and
displayed by Bob.

**Note:** The input is NOT sent back to Alice.
