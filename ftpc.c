#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int getFileLength(FILE *fp);

int main(int argc, char *argv[]){
    /*connection variable*/
    int sd;
    struct sockaddr_in server_address; 
    char buffer[BUFFER_SIZE];
    int portNumber;
    char serverIP[29];
    int rc = 0;
    /*content variables*/
    char filename[20];
    int filename_size=0;
    int converted_filename_size=0;
    int file_size=0;
    int converted_file_size=0;
    FILE *input;
    int total_bytes_read;
    int total_bytes_written;
    int number_of_bytes;

    if (argc < 3){
	    printf ("usage is client <ipaddr> <port>\n");
	    exit(1);
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    portNumber = strtol(argv[2], NULL, 10);
    strcpy(serverIP, argv[1]);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = inet_addr(serverIP);
    if(connect(sd, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) < 0) {
	    close(sd);
	    perror("error connecting stream socket"); 
	    exit(1);
    }
    /*content*/
    while(1){
	file_size=0;
	total_bytes_read=0;
	number_of_bytes=0;
	/*get user input of filename*/
	printf("please enter the name of the file to be saved:");
	scanf("%s",filename);
	/*if entered "DONE exit exit the loop and close the socket"*/
	if(!strcmp(filename,"DONE"))
		break;
	/*send file name size*/
        filename_size = strlen(filename);
	converted_filename_size=ntohl(filename_size);
	rc = write (sd, &converted_filename_size, sizeof(converted_filename_size)); 
	if (rc < 0) 
	    perror ("write");
	printf("wrote %d bytes for the size of \"%s\"\n",rc,filename);
	/*send filename*/
	rc = write(sd,filename,filename_size);
	if (rc < 0)
	    perror ("write");
	printf("wrote %d bytes for \"%s\"\n",rc,filename);
    	/*open input file*/
	if((input=fopen(filename,"rb")) == NULL){
		printf("ERROR: File not exist\n");
		exit(1);
	}
	/*send file size*/
	file_size=getFileLength(input);
	converted_file_size=ntohl(file_size);
	rc = write(sd,&converted_file_size,sizeof(int));
	if (rc < 0)
		perror ("write");
	printf("\"%s\"has %d bytes\n",filename,file_size);

	/*send file content*/
	number_of_bytes = fread(buffer,1,BUFFER_SIZE,input);
	while(total_bytes_read<file_size){
	    total_bytes_read+=number_of_bytes;
	    rc = write(sd,buffer,number_of_bytes);
	    if (rc < 0)
		    perror ("write");
	    number_of_bytes = fread(buffer,1,BUFFER_SIZE,input);
	}

	/*close input file*/
	fclose(input);
	/*receive acknowledgement from the server*/
	rc = read(sd, &total_bytes_written, sizeof(int));
	if (rc<0)
		perror("read");
	printf("%d bytes read from \"%s\", server received %d bytes\n",total_bytes_read,filename,total_bytes_written);

	printf("\n");
    }

    return 0 ;

}

int getFileLength(FILE *fp){
    fseek(fp,0,SEEK_END);
    int len = (int) ftell(fp);
    fseek(fp,0,SEEK_SET);
    return len;
}


