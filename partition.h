#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include <stddef.h>
#include "vdi.h"

// Structure to hold partition information
typedef struct {
    VDIFile *vdi;          // VDI file
    uint8_t partitionTable[64]; // Partition table (4 entries of 16 bytes each)
    uint64_t offset;       // Start of the partition in bytes
    uint64_t size;         // Size of the partition in bytes
    size_t cursor;         // Current read/write position within the partition
} MBRPartition;

// Function prototypes
MBRPartition *openPartition(const char *filename, int part);
void closePartition(MBRPartition *part);
ssize_t readPartition(MBRPartition *part, void *buf, size_t count);
ssize_t writePartition(MBRPartition *part, const void *buf, size_t count);
off_t lseekPartition(MBRPartition *part, off_t offset, int whence);
void displayPartitionTableEntry(const uint8_t *entry);

#endif
