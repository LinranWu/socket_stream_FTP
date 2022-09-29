#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100

int main(int argc, char *argv[]){
    int sd; /* socket descriptor */
    int connected_sd; /* socket descriptor */
    int rc; /* return code from recvfrom */
    struct sockaddr_in server_address;
    struct sockaddr_in from_address;
    char buffer[BUFFER_SIZE];
    int flags = 0;
    socklen_t fromLength;
    int portNumber;
    /*content variables*/
    int filename_size;
    char filename[20];
    int file_size=0;
    FILE *output;
    int total_bytes_written;
    char copy_name[25];

    if (argc < 2){
         printf ("Usage is: server <portNumber>\n");
	 exit (1);
    }
    portNumber = atoi(argv[1]);
    sd = socket (AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = INADDR_ANY;
    bind(sd, (struct sockaddr *)&server_address, sizeof(server_address));
    listen (sd, 5);
    connected_sd = accept (sd, (struct sockaddr *) &from_address, &fromLength);
    /*content*/
    while(1){
	file_size=0;
	bzero(buffer, BUFFER_SIZE);
	total_bytes_written=0;

	bzero(copy_name,25);
	/*get file name size*/
	rc = read(connected_sd, &filename_size, sizeof(int));
	if (rc<=0)
		exit(1);
	filename_size=htonl(filename_size);
	printf ("read %d bytes to get the size of file name\n", rc);
	/*get filename*/
	bzero(filename, 20);
	rc = read(connected_sd, filename, filename_size);
	if (rc<=0)
		exit(1);
	printf ("read %d bytes to get  \"%s\"\n", filename_size,filename);
	/*get file size*/
	rc = read(connected_sd, &file_size, sizeof(int));
	if (rc<=0)
		perror("read");
	file_size=htonl(file_size);
	printf("\"%s\"has %d bytes\n",filename,file_size);
	/*generate a copy name*/
	strcat(copy_name,filename);
	strcat(copy_name,"-copy");
	/*open output file*/
	if((output=fopen(copy_name,"wb")) == NULL){
		printf("ERROR: Cannot open output file\n");
		exit(1);
	}
	/*get content*/
	
	while(BUFFER_SIZE<(file_size-total_bytes_written)){
		rc=read(connected_sd, buffer, BUFFER_SIZE);
		if (rc<0){
			printf("transfer ERROR");
			perror("read");
		}
		total_bytes_written+=rc;
		fwrite(buffer,1,rc,output);
	}
	rc = read(connected_sd,buffer,file_size-total_bytes_written);
	if (rc<0){
		printf("transfer ERROR");
		perror("read error");
	}
	total_bytes_written+=rc;
	fwrite(buffer,1,rc,output);
	printf("%d bytes written to \"%s\",remaining %d bytes\n",total_bytes_written,copy_name,file_size-total_bytes_written);
	/*close output file*/
	fclose(output);
	/*send acknowledgement to client*/
	rc = write(connected_sd,&total_bytes_written,sizeof(int));
	printf("\n\"%s\" uploaded successfully!\n\n",filename);
    }
    return 0;
}


