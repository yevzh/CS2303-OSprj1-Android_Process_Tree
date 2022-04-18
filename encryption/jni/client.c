// This is the source file implementing an encryption client.
// The clients can receive messages from the shell and forward them to the
// server, and get a returning message.
// Author: 520021911141 Weiming Zhang

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
// show the time of each activity
static char *time_str() {
  static char str[32];
  time_t t;
  time(&t);
  struct tm *tt = localtime(&t);
  sprintf(str, "%02d:%02d:%02d", tt->tm_hour, tt->tm_min, tt->tm_sec);
  return str;
}

int main(int argc, char *argv[]) {
  /////////////////////////////////////////////////////////////////////
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
  portno = 2050;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("ERROR opening socket");
    exit(1);
  }

  server = gethostbyname("127.0.0.1");
  if (server == NULL) {
    printf("ERROR, no such host\n");
    exit(0);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("ERROR connecting\n");
    exit(0);
  }

  /////////////////////////////////////////////////////////////////////
  // CODES HERE!
  while (1) {
    printf("Please enter the message: ");
    // set the buffer to zero
    bzero(buffer, 256 * sizeof(char));
    // read message from stdin
    fgets(buffer, 255, stdin);

    n = write(sockfd, buffer, 255); // write the message to the socket
    if (n < 0) {
      printf("ERROR writing to socket\n");
      exit(1);
    }
    // if the command is ":q", we want the client to quit
    if (strcmp(buffer, ":q\n") == 0) {
      printf("You terminated the client at %s\n", time_str());
      break;
    }

    bzero(buffer, 256 * sizeof(char));
    // read the message replied by the server
    n = read(sockfd, buffer, 255);
    if (n < 0) {
      printf("ERROR reading from socket\n");
      exit(1);
    }
    // print the processed message on the screen
    printf("From server at %s: %s", time_str(), buffer);
  }

  close(sockfd);
  printf("client closing at %s\n", time_str());
  return 0;
}