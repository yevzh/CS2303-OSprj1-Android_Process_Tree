// This is the source file implementing an encryption server.
// The server would receive the message from clients and forward them back after
// processing the messages by Caesar Cipher
// Author: 520021911141 Weiming Zhang

#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
// show the time of each activity
static char *time_str()
{
  static char str[32];
  time_t t;
  time(&t);
  struct tm *tt = localtime(&t);
  sprintf(str, "%02d:%02d:%02d", tt->tm_hour, tt->tm_min, tt->tm_sec);
  return str;
}
// the function is to change the letters 3 positions down the alphabet
static char encodeChar(char c)
{
  if (c >= 'a' && c <= 'z')
  {
    c += 3;
    if (c > 'z')
      c -= 26;
  }
  else if (c >= 'A' && c <= 'Z')
  {
    c += 3;
    if (c > 'Z')
      c -= 26;
  }
  return c;
}

void *serve(void *newsockfd);

int client_total = 0;   // the total number of clients
int client_serving = 0; // the number of clients being served at the moment
int clientID = 0;

pthread_mutex_t mutex_serving = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
  /////////////////////////////////////////////////////////////////////
  int sockfd, newsockfd, portno, n;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    printf("ERROR opening socket\n");
    exit(1);
  }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // the port number (>2000 generally) of server is randomly assigned
  portno = 2050;
  serv_addr.sin_port = htons(portno);
  // the ip address of server
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("Error on binding\n");
    exit(0);
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  printf("Server initiating...\n");
  // Concurrent Multi-thread service here

  pthread_t new_thread;
  int iret;

  while (1)
  {
    // wait for a connection from a client
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
      printf("Error Client Connection.\n");
      continue;
    }
    // if connected successfully, create a new thread to serve the client
    iret = pthread_create(&new_thread, NULL, serve, (void *)&newsockfd);
    if (iret)
    {
      fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
      exit(EXIT_FAILURE);
    }
  }
  close(sockfd);
  printf("Server closing...\n");
  //
  return 0;
}

void *serve(void *sockfd)
{
  clientID++; // I'm the kth client.
  int clientkth = clientID;
  // printf("%d\n", clientID);
  int newsockfd = (int)(*((int *)sockfd));
  char buffer[256];
  int n;
  pthread_mutex_lock(&mutex_total);
  client_total++;
  pthread_mutex_unlock(&mutex_total);
  //   printf("hi\n");
  n = read(newsockfd, buffer, 255);
  if (n < 0)
  {
    printf("Invalid Reading!\n");
    exit(0);
  }
  // if (strcmp(buffer, ":q\n") == 0) {
  //   printf("Client%d has just quitted at %s!", clientkth, time_str());
  //   close(newsockfd);
  //   return NULL;
  // }
  //   printf("hi\n");
  // a while loop to block clients when the serving clients are more than 2
  while (client_serving >= 2)
  {
    if (strcmp(buffer, ":q\n") == 0)
    {
      printf("Client%d has just quitted at %s!\n", clientkth, time_str());
      close(newsockfd);
      return NULL;
    }
    // tell the client to wait until there are less than 2 clients
    n = write(newsockfd, "Please wait...\n", 15);
    if (n < 0)
    {
      printf("Invalid Writing!");
      exit(1);
    }
    // after write, we also need to read to match the previous state
    n = read(newsockfd, buffer, 255);
    if (n < 0)
    {
      printf("Invalid Reading!");
      exit(1);
    }
  }
  // the client begins to enjoy her service!
  pthread_mutex_lock(&mutex_serving);
  client_serving++;
  pthread_mutex_unlock(&mutex_serving);
  // maybe there already exists a message, handle it first
  // print the message on the screen
  printf("Receiving message from Client%d at %s: %s", clientkth, time_str(),
         buffer);
  // encode the message and return a new message to client
  int i;
  for (i = 0; i < 255; i++)
  {
    buffer[i] = encodeChar(buffer[i]);
  }
  if (write(newsockfd, buffer, 255) < 0)
  {
    printf("Invalid Writing!");
    exit(1);
  }

  // printf("hi,client %d\n", clientID);
  while (1)
  {
    bzero(buffer, 256 * sizeof(char));
    // receive message from the client and store the message in the buffer
    n = read(newsockfd, buffer, 255);
    // printf("hi,client %d\n", clientID);
    if (n < 0)
    {
      printf("Invalid Reading!");
      exit(1);
    }
    // print the message on the screen
    printf("Receiving message from Client%d at %s: %s", clientkth, time_str(),
           buffer);
    // check whether the client has quitted
    if (strcmp(buffer, ":q\n") == 0)
    {
      printf("Client%d has just quited at %s!\n", clientkth, time_str());
      break;
    }
    // encode the message and return a new message to client
    for (i = 0; i < 255; i++)
    {
      buffer[i] = encodeChar(buffer[i]);
    }
    if (write(newsockfd, buffer, 255) < 0)
    {
      printf("Invalid Writing!");
      exit(1);
    }
  }

  pthread_mutex_lock(&mutex_serving);
  client_serving--;
  pthread_mutex_unlock(&mutex_serving);
}
