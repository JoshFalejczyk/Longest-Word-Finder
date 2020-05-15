export JAVA_HOME=/usr/java/latest
gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux system5_msg.c -o edu_cs300_MessageJNI.o
gcc -shared -o libsystem5msg.so edu_cs300_MessageJNI.o -lc
gcc -std=c99 -D_GNU_SOURCE msgsnd_pr.c -o msgsnd
gcc -std=c99 -D_GNU_SOURCE msgrcv_lwr.c -o msgrcv
