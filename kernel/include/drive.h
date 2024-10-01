#ifndef DRIVE_H
#define DRIVE_H

#define SECTOR_SIZE 512

#include <types.h>
#include <ahci.h>



typedef struct ahciDriveData {
  struct HBA_PORT *port;
  struct HBA_MEM *ahci_bar;
} ahciDriveData;

typedef enum diskType{
    AHCI_SATA
} diskType;


typedef union driveData{
    ahciDriveData ahci_drive_data;
} driveData;

typedef struct disk{
    uint64_t disk_id;
    driveData drive_data;
    diskType disk_type;
    char serial_number[10];
    char model_number[20];
    uint64_t disk_size;
} disk;




#endif