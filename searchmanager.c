#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include "longest_word_search.h"
#include "queue_ids.h"

// Helper function for copying string lengths
size_t                  /* O - Length of string */
strlcpy(char       *dst,        /* O - Destination string */
        const char *src,      /* I - Source string */
        size_t      size)     /* I - Size of destination string buffer */
{
    size_t    srclen;         /* Length of source string */


    /*
     * Figure out how much room is needed...
     */

    size --;

    srclen = strlen(src);

    /*
     * Copy the appropriate amount...
     */

    if (srclen > size)
        srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}

// Driving force of C-side, sends prefix requests via command line arguments and receives
// Results from PassageProcessor to be assembled in reports
int main (int argc, char* argv[]) {

    // Set up queue variables and message buffers
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    prefix_buf sBuffer;
    size_t sBLength;
    response_buf rBuffer;
    size_t rBLength;

    if (argc <= 2) { // Not enough arguments

        fprintf(stderr, "Error: please provide at least three command line arguments\n");
        fprintf(stderr, "Usage: %s <delay> <prefix>\n", argv[0]);
        exit(-1);

    }

    // Set up queue
    key = ftok(CRIMSON_ID,QUEUE_NUMBER);
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error msgget: %s\n", strerror( errnum ));
    }
    else {}
        // fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid); Optional for output

    // Iterates for each prefix
    for (int i = 2; i < argc; i ++) {

        if (argc < 3 || strlen(argv[i]) < 3 || strlen(argv[i]) > 20) { // Invalid prefix

            fprintf(stderr, "Error: please provide prefix of at least three characters for search\n");
            fprintf(stderr, "Usage: %s <delay> <prefix>\n", argv[0]);

        } else {

            // Send prefix request
            // We'll send message type 1
            sBuffer.mtype = 1;
            strlcpy(sBuffer.prefix,argv[i],WORD_LENGTH);
            sBuffer.id= i - 1;
            sBLength = strlen(argv[i]) + sizeof(int)+1;//struct size without long int type
            
            // Send a message.
            if((msgsnd(msqid, &sBuffer, sBLength, IPC_NOWAIT)) < 0) {
                int errnum = errno;
                fprintf(stderr,"%d, %ld, %s, %d\n", msqid, sBuffer.mtype, argv[i], (int)sBLength);
                perror("(msgsnd)");
                fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
                exit(1);
            }
            else
                fprintf(stderr,"Message(%d): \"%s\" Sent (%d bytes)\n", sBuffer.id, argv[i],(int)sBLength);


            bool allReceived = false; // Condition for infinite wait for receiving messages
            int rCount = 0; // Number of messages received
            // Call first msgrcv outside of infinite loop in order to know number of passages to expect
            // msgrcv to receive message
            int ret;
            do {
            ret = msgrcv(msqid, &rBuffer, sizeof(response_buf), 2, 0);//receive type 2 message
            int errnum = errno;
            if (ret < 0 && errno !=EINTR){
                fprintf(stderr, "Value of errno: %d\n", errno);
                perror("Error printed by perror");
                fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
            }
            } while ((ret < 0 ) && (errno == 4));
            
            // Set up array of responses to manually sort for reports
            // This ensures that regardless of what order the responses come in, the report will
            // Print them in ascending order of passage
            int passageCount = rBuffer.count;
            response_buf responses[passageCount];
            responses[rBuffer.index] = rBuffer;
            rCount ++;
            
            // Infinite wait for rest of messages from PassageProcessor
            while (!allReceived) {

                // msgrcv to receive message
                int ret;
                do {
                ret = msgrcv(msqid, &rBuffer, sizeof(response_buf), 2, 0);//receive type 2 message
                int errnum = errno;
                if (ret < 0 && errno !=EINTR){
                    fprintf(stderr, "Value of errno: %d\n", errno);
                    perror("Error printed by perror");
                    fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
                }
                } while ((ret < 0 ) && (errno == 4));
                
                // Add response to response array
                responses[rBuffer.index] = rBuffer;
                rCount ++;
                if (rCount >= passageCount) { // All responses received

                    allReceived = true; // Terminate loop

                }

            }

            // Setup and formatting of reports
            printf("Report \"%s\"\n", argv[i]);
            for (int j = 0; j < passageCount; j ++) {

                if (strlen(responses[j].longest_word) > 2) { // Checks if longest word was found or not, prints accordingly

                    printf("Passage %d - %s - %s\n", j, responses[j].location_description, responses[j].longest_word);

                } else {

                    printf("Passage %d - %s - no word found\n", j, responses[j].location_description);

                }

            }
            printf("\n");

            sleep(atoi(argv[1])); // Sleep based on delay specified in command line argument, this is the only sleep used in whole project

        }

    }
    
    // Send final message of ID 0 to terminate PassageProcessor
    // We'll send message type 1
    sBuffer.mtype = 1;
    sBuffer.id= 0;
    sBLength = strlen("   ") + sizeof(int)+1;//struct size without long int type
        
    // Send a message.
    if((msgsnd(msqid, &sBuffer, sBLength, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        fprintf(stderr,"%d, %ld, %s, %d\n", msqid, sBuffer.mtype, "   ", (int)sBLength);
        perror("(msgsnd)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }
    else
        fprintf(stderr,"Message(%d): \"%s\" Sent (%d bytes)\n\n", sBuffer.id, "   ", (int)sBLength);

    printf("Exiting...\n"); // Terminate searchmanager

}