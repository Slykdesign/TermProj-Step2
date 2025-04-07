#include "vdi.h"
#include "partition.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

// Function prototypes
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset);
void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset);

int main() {
    MBRPartition *partition = openPartition("./good-fixed-1k.vdi", 0);
    if (!partition) return 1;

    printf("Partition table entry 0:\n");
    displayPartitionTableEntry(partition->partitionTable);

    printf("\nReading 1KB block from partition:\n");
    char buffer[1024];
    lseekPartition(partition, 1024, SEEK_SET);
    readPartition(partition, buffer, 1024);
    displayBuffer((uint8_t *)buffer, 1024, 0x400);

    closePartition(partition);
    return 0;
}

// Function Definitions
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset) {
    printf("Offset: 0x%lx\n", offset);
    for (uint32_t i = skip; i < count; i += 16) {
        printf("%04x | ", i);
        for (uint32_t j = 0; j < 16 && i + j < count; j++) {
            printf("%02x ", buf[i + j]);
        }
        printf("| ");
        for (uint32_t j = 0; j < 16 && i + j < count; j++) {
            printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
        }
        printf("\n");
    }
}

void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset) {
    uint32_t step = 256;
    for (uint32_t i = 0; i < count; i += step) {
        displayBufferPage(buf, count, i, offset + i);
    }
}
