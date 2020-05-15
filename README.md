# Longest-Word-Finder

This is a project from Spring 2020 for my CS300 class.  It is a rather large project that has two main sides: one written in C, and one in Java.  The goal of the project is, given a set of text passages and a user inputted prefix, find the longest word starting with that prefix in each one of those passages.  Concepts that this project covers include concurrency, multithreading, implementation of a Trie, and use of System V interprocess communication.  
The basic functionality is that the user starts by running the searchmanager.c file in one terminal, inputs a prefix, and this prefix is then sent to a queue using IPC.  On the Java side in another terminal, PassageProcessor.java is then ran, where it picks up the search requests off the queue, uses concurrent multithreading to find the longest word of the given prefix for each one of the passages in the library, and sends the results back over to searchmanager.c on the C-side of things, again using IPC.  Searchmanager.c then picks up the results, and outputs them in a fixed, sorted order to the user.

To Run (on Bash): 
1. Ensure the JAVA_HOME environment variable is properly set up.
2. Compile setup shell scripts using "chmod +x c_test.sh" and "chmod +x java_test.sh".
3. Run setup shell scripts using "./c_test.sh" and "./java_test.sh".
4. Compile PassageProcessor.java using "javac edu/cs300/PassageProcessor.java" in one terminal.
5. Compile searchmanager.c using "gcc -std=c99 -D_GNU_SOURCE searchmanager.c -o searchmanager" in another terminal.
6. Run PassageProcessor.java using "java -cp . -Djava.library.path=. edu/cs300/PassageProcessor" in respective terminal.
7. Run searchmanager.c using "./searchmanager delay-in-seconds prefixes", or for example, "./searchmanager 3 con pro pre" in respective terminal.
8. Results should be outputted in searchmanager.c terminal.
9. Steps 1-5 only need to be completed once.  Repeat steps 6-8 for each set of prefixes.

NOTES:
1. The System V Message Queue is inherently set up within the shell scripts.
2. To view your queue, use "ipcs -a".
3. To delete your queue if issues arise (like messages being sent but not received), use "ipcrm -Q 0xXXXXXXXX" where the last argument is the Message Queue Hex ID found using "ipcs -a".
4. The delay in the searchmanager.c arguments is a delay between prefix results that can be made optional by just inputting "0".
5. The passages being analyzed can be found listed in passages.txt, and the actual passage text files are in the directory.  To add a passage, simply add the passage text file to the directory, and its name to passages.txt.
6. The scenario that a longest word may not being in any one passage is supported by this project and the user will be notified accordingly in searchmanager.c output.

Credit: 
1. The concept and skeleton of the project were written and supplied by my professor, Dr. Monica Anderson-Herzog.  This includes essentially everything except the searchmanager.c and PassageProcessor.java files.
2. The searchmanager.c and PassageProcessor.java files, plus modifications to Trie.java and Worker.java, were written and made by myself.
