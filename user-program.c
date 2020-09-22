#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 
int8_t write_buf[1024];
int8_t read_buf[1024];
int main()
{
        int fd;
        char option;
        printf("*********************************\n");
        printf("\nOpening the device driver...");
        printf("The kernel will call the open function of cdev, that will allocate 1024 byes in a memory space.\n");
        fd = open("/dev/chardriver", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
        while(1) {
                printf("\n\n****Please Enter the Option******\n");
                printf("        1. Write in device        \n");
                printf("        2. Read from device       \n");
                printf("        3. Exit program           \n");
                printf("*********************************\n");
                scanf(" %c", &option);
                printf("Your Option = %c\n", option);
                
                switch(option) {
                        case '1':
                                printf("\n\nThe user program tells to the kernel to write in the device.\n");
                                printf("After this, the kernel will use the file_operation structure that is pointing to cdev functions.\n");
                                printf("To write some data, the device will call the write system call and will fill the prealocated\n");
                                printf("buffer with the bytes passed.\n\n");


                                printf("Enter the string to write into driver: ");
                                scanf("  %[^\t\n]s", write_buf);
                                printf("Writing ...");
                                write(fd, write_buf, strlen(write_buf)+1);
                                printf("Done!\n");
                                break;
                        case '2':
                                printf("\n\nThe kernel will call the read function and will return the buffer created by Open() call.\n");
                                printf("Reading ...");
                                read(fd, read_buf, 1024);
                                printf("Done!\n\n");
                                printf("Readed Data = %s\n\n", read_buf);
                                break;
                        case '3':
                                printf("\nThe kernel will call the release function of registered cdev.");
                                printf("\nThis will close the driver and free the allocated memory!");
                                close(fd);
                                printf("\nBye!");
                                exit(1);
                                break;
                        default:
                                printf("Enter Valid option = %c\n",option);
                                break;
                }
        }
        close(fd);
}
