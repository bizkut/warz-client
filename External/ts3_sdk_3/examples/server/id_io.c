#include <stdio.h>
#include "id_io.h"

int readKeyPairFromFile(const char *fileName, char *keyPair) {
    FILE *file;

    file = fopen(fileName, "r");
    if(file == NULL) {
        printf("Could not open file '%s' for reading keypair\n", fileName);
        return -1;
    }

    fgets(keyPair, BUFSIZ, file);
    if(ferror(file) != 0) {
        fclose (file);
        printf("Error reading keypair from file '%s'.\n", fileName);
        return -1;
    }
    fclose (file);

    printf("Read keypair '%s' from file '%s'.\n", keyPair, fileName);
    return 0;
}

int writeKeyPairToFile(const char *fileName, const char* keyPair) {
    FILE *file;

    file = fopen(fileName, "w");
    if(file == NULL) {
        printf("Could not open file '%s' for writing keypair\n", fileName);
        return -1;
    }
    
    fputs(keyPair, file);
    if(ferror(file) != 0) {
        fclose (file);
        printf("Error writing keypair to file '%s'.\n", fileName);
        return -1;
    }
    fclose (file);

    printf("Wrote keypair '%s' to file '%s'.\n", keyPair, fileName);
    return 0;
}
