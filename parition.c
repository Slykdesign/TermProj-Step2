#include "partition.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define SECTOR_SIZE 512

MBRPartition *openPartition(const char *filename, int part) {
    if (part < 0 || part > 3) return NULL;

    MBRPartition *partition = malloc(sizeof(MBRPartition));
    if (!partition) return NULL;

    partition->vdi = vdiOpen(filename);
    if (!partition->vdi) {
        free(partition);
        return NULL;
    }

    // Read partition table from MBR
    lseek(partition->vdi->fd, 446, SEEK_SET); // MBR partition table starts at offset 446
    read(partition->vdi->fd, partition->partitionTable, 64);

    // Extract partition entry
    uint8_t *entry = partition->partitionTable + (part * 16);
    partition->offset = (*(uint32_t *)(entry + 8)) * SECTOR_SIZE;
    partition->size = (*(uint32_t *)(entry + 12)) * SECTOR_SIZE;
    partition->cursor = 0;

    return partition;
}

void closePartition(MBRPartition *part) {
    if (part) {
        vdiClose(part->vdi);
        free(part);
    }
}

ssize_t readPartition(MBRPartition *part, void *buf, size_t count) {
    if (part->cursor + count > part->size) {
        count = part->size - part->cursor; // Restrict count to avoid reading beyond partition
    }

    vdiSeek(part->vdi, part->offset + part->cursor, SEEK_SET);
    ssize_t bytesRead = vdiRead(part->vdi, buf, count);
    part->cursor += bytesRead;
    return bytesRead;
}

ssize_t writePartition(MBRPartition *part, const void *buf, size_t count) {
    if (part->cursor + count > part->size) {
        count = part->size - part->cursor; // Restrict count to avoid writing beyond partition
    }

    vdiSeek(part->vdi, part->offset + part->cursor, SEEK_SET);
    ssize_t bytesWritten = vdiWrite(part->vdi, buf, count);
    part->cursor += bytesWritten;
    return bytesWritten;
}

off_t lseekPartition(MBRPartition *part, off_t offset, int whence) {
    off_t newCursor;
    if (whence == SEEK_SET) {
        newCursor = offset;
    } else if (whence == SEEK_CUR) {
        newCursor = part->cursor + offset;
    } else if (whence == SEEK_END) {
        newCursor = part->size + offset;
    } else {
        return -1;
    }

    if (newCursor < 0 || newCursor > part->size) {
        return -1; // Restrict cursor to remain within partition
    }

    part->cursor = newCursor;
    return part->cursor;
}

void displayPartitionTableEntry(const uint8_t *entry) {
    printf("Status: %s\n", (entry[0] & 0x80) ? "Active" : "Inactive");
    printf("First sector CHS: %d-%d-%d\n", entry[1], entry[2], entry[3]);
    printf("Last sector CHS: %d-%d-%d\n", entry[5], entry[6], entry[7]);
    printf("Partition type: %02x\n", entry[4]);
    printf("First LBA sector: %u\n", *(uint32_t *)(entry + 8));
    printf("LBA sector count: %u\n", *(uint32_t *)(entry + 12));
}
