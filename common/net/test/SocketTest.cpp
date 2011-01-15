#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
   struct sockaddr_in pin;
   struct hostent * remoteHost;
   char *read_buffer = (char*)malloc(1024);
   char *buffer = NULL;
   char *tmp_buffer = NULL;
   int isock;
   char message[512];
   int done = 0;
   int chars = 0;
   int l = 0;
   
   if( (remoteHost = gethostbyname("www.cnbeta.com")) == 0 ) {
       printf("Error resolving host\n");
       exit(1);
   }

   bzero(message,sizeof(message));
   bzero(&pin,sizeof(pin));
   pin.sin_family = AF_INET;
   pin.sin_port = htons(80);
   pin.sin_addr.s_addr = ( (struct in_addr *)(remoteHost->h_addr) )->s_addr;
   printf("%d\n", pin.sin_addr.s_addr);

   if( (isock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
       printf("Error opening socket!\n");
       exit(1);
   }
   sprintf(message, "GET /backend.php HTTP/1.1\r\n");
   strcat(message, "Host:www.cnbeta.com\r\n");
   strcat(message, "Accept: */*\r\n");
   strcat(message, "User-Agent: Mozilla/4.0(compatible)\r\n");
   strcat(message, "Connection: Close\r\n");
   strcat(message, "\r\n\r\n");

   printf("%s",message);

   if( connect(isock, (sockaddr*)&pin, sizeof(pin)) == -1 ) {
       printf("Error connecting to socket\n");
       exit(1);
   }

   if( send(isock, message, strlen(message), 0) == -1) {
       printf("Error in send\n");
       exit(1);
   }

   while(done == 0) {
       memset(read_buffer, 0, 1024);
       l = recv(isock, read_buffer, 1024, 0);
       if(strlen(read_buffer) <= 0) {
           break;
       }

       printf(read_buffer);
       if(buffer == NULL) {
           buffer = (char*)malloc(strlen(read_buffer));       
       } else {
           tmp_buffer = buffer;
           buffer = (char*)malloc(strlen(buffer) + strlen(read_buffer));
           strcpy(buffer, tmp_buffer);
       }
       strcat(buffer, read_buffer);
       free(tmp_buffer);
   }

   printf(buffer);
   
   close(isock);
   
   return 0;
}
