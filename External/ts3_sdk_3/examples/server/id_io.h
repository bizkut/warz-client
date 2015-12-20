#ifndef ID_IO_H
#define ID_IO_H

int readKeyPairFromFile(const char *fileName, char *keyPair);
int writeKeyPairToFile(const char *fileName, const char* keyPair);

#endif
