#ifndef VDI_H
#define VDI_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    char szFileInfo[64];      // Text description
    uint32_t u32Signature;    // 'VDI'
    uint16_t u16Version;      // 0x100 = v1.0
    uint32_t cbHeader;        // Header size
    uint32_t cbDisk;          // Disk size in bytes
    uint32_t cbBlock;         // Block size
    uint32_t cBlocks;         // Total blocks
    uint32_t cBlocksAllocated;// Allocated blocks
    uint64_t uuidCreate[2];   // Image UUID
    uint64_t uuidModify[2];   // Modification UUID
    uint64_t uuidParent[2];   // Parent UUID
    uint32_t offBlocks;       // Block map offset
    uint32_t offData;         // Data blocks offset
} VDIHeader;

typedef struct {
    int fd;
    uint8_t header[400];
    uint32_t *map;        // Translation map (logical-to-physical mapping)
    size_t cursor;        // Current read/write position
    uint32_t pageSize;    // Page size (from header)
    uint32_t totalPages;  // Number of allocated pages
} VDIFile;

VDIFile *vdiOpen(const char *filename);
void vdiClose(VDIFile *vdi);
ssize_t vdiRead(VDIFile *vdi, void *buf, size_t count);
ssize_t vdiWrite(VDIFile *vdi, void *buf, size_t count);
off_t vdiSeek(VDIFile *vdi, off_t offset, int anchor);
off_t vdiTranslate(VDIFile *vdi, off_t logicalOffset);
void displayVDIHeader(VDIFile *vdi);
void displayVDITranslationMap(VDIFile *vdi);
void displayMBR(VDIFile *vdi);
void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset);

#endif