# Project for DVD017

This project consists of four parts.
After making the first two parts, these will be put together
in part three and the last part will be to review the work of
another group, so keep modularity as well as documentation in mind
while implementing. The choice of programming language
is limited to C or C++,
and the tasks should be done in groups of maximum two people.

## Part 1

Encryption is one of the basics for secure communication.
This laboration will give you an opportunity to practically
work with the [Blowfish Algorithm](http://en.wikipedia.org/wiki/Blowfish_%28cipher%29)

Implement the Blowfish Algorithm,
which is a symmetric block chipher algorithm.

## Part 2

By using the
[Diffie-Hellman key exchange](http://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange),
two parties may share a secret key via an insecure
communications channel.
Afterwards this key can be used to encrypt further communication
using a symmetric key cipher;
in our case we will be using the Blowfish Algorithm
implemented in Part 1.

Implement the Diffie Hellman keyexchange algorithm with modularity in mind.

## Part 3

Develop a simple TCP server that can listen on a specified port,
and a client that can
communicate with this Server.
Use yor Diffie-Hellman keyexchange implementation to
negotiate a secret key between the server and the client.
Use this key together with the
Blowfish encryption/decryption Algorithm implemented in Part 1
at client to send some data.
Use the same Encryption/Decryption Algorithm at server to
decrypt and understand the data sent.

Note, the client and the server *must* display the communication
for examination! A typical example of this system would be to
implement an [echo server](http://en.wikipedia.org/wiki/Echo_server).

After Part 3, you submit your project work.

