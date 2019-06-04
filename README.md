This project was developed as a means of using common password frailties in order to generate and crack
passwords that have been hashed to a SHA256 value. Can be used to test generated passwords against existing
SHA256 hashes, or to simply generate a finite number of guesses.

In order to run the program:\n
In the command line: 'make'\n
Followed by: ./crack {any arguments you wish to include}

If crack is run with no arguments, it should generate guesses, and test them against the
SHA256 hashes (hashes found in pwd4sha256 and pwd6sha256). When it finds one, it should print one line, consisting of the plaintext password, a space, and an integer 1-30 indicating which hash it matches. For example:
abcd 3
Oops 5
adam 1
passwd 15
This mode was used for finding passwords.

If crack is run with one integer argument, the argument specifies how many guesses it should
produce. In this mode, password guesses are printed to stdout, separated by newline
characters ('\n'). This mode simply generates guesses - it does not check whether they match any of the
stored passwords.

If crack is run with two arguments, it should treat the first as the filename of a list of
passwords (one per line), and the second as the filename of a list of SHA256 hashes (in
groups of 32 bytes, with no newline characters). It should then test each of the passwords
given in the first file against each of the hashes given in the second file, and produce output
as for the case of no arguments. It prints nothing else to stdout.
