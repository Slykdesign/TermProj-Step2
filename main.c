#include "vdi.h"
#include "partition.h"
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

// Function prototypes
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset);
void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset);

int main() {
    // Open Partition
    MBRPartition *partition = openPartition("./good-fixed-1k.vdi", 0);
    if (!partition) {
        printf("Error opening partition.\n");
        return 1;
    }

    // Display Partition Table
    printf("Partition table:\n");
    displayPartitionTable(partition);

    // Read and Display Superblock
    printf("\nSuperblock:\n");
    uint8_t buffer[1024];
    vdiSeekPartition(partition, 1024, SEEK_SET);
    vdiReadPartition(partition, buffer, 1024);
    displayBuffer(buffer, 1024, 0);

    // Close Partition
    closePartition(partition);
    return 0;
}

// Function Definitions
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset) {
    printf("Offset: 0x%lx\n", offset);
    printf("   00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f    0...4...8...c...\n");
    printf("  +-----------------------------------------------+  +----------------+\n");
    for (uint32_t i = skip; i < count; i += 16) {
        printf("%02x|", i % 256);
        for (uint32_t j = 0; j < 16 && i + j < count; j++) {
            printf("%02x ", buf[i + j]);
        }
        for (uint32_t j = 0; i + j >= count && j < 16; j++) {
            printf("   ");
        }
        printf("|%02x| ", i % 256);
        for (uint32_t j = 0; j < 16 && i + j < count; j++) {
            printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
        }
        printf("\n");
    }
    printf("  +-----------------------------------------------+  +----------------+\n");
}

void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset) {
    uint32_t step = 256;  // Process in chunks of 256
    for (uint32_t i = 0; i < count; i += step) {
        displayBufferPage(buf, count, i, offset + i);
    }
}