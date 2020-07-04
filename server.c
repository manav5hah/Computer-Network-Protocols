#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 1024

struct packet{
  char data[1024];
};
struct frame{
  int frame_type;
  int seq_no;
  int ack;
  struct packet p;
};

int main(){

  struct sockaddr_in sin;
  char buf[MAX_LINE];
  int len;
  int s, new_s;
  char str[INET_ADDRSTRLEN];

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);
  /* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
 
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);
  /* wait for connection, then receive and print text */
  int frame_id = 1;
  int x1 = 20;
  int count = 1;
  int k = 2;
  int f_limit = 370;
  if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
    perror("simplex-talk: accept");
    exit(1);
  }
  printf("Server Listening.\n");

  for(int i = 1; frame_id<f_limit; i<x1?i=i*2:i++)
  {
    k = 2;
    struct frame f_recv[i], f_recv1;
    struct frame f_send[i], f_send1;
    int dropNo;  
    count = 1;
    int flag1 = 0, flag2 = 0;
    printf("\n");
    for(int j=frame_id; j<i+frame_id; j++)
    {
      if(j%40 != 0)
      {
        len = recv(new_s, (void *)&f_recv1, sizeof(struct frame), 0);
        f_recv[j-frame_id] = f_recv1;
      }
      else
      {
        len = recv(new_s, (void *)&f_recv1, sizeof(struct frame), 0);
        len = 0;
      }
      
      printf("len:  %d\n", len);
      if (len > 0 && f_recv[j-frame_id].seq_no == j && f_recv[j-frame_id].frame_type == 1)
      {
        if (flag1 == 0)
        {
          f_send[j-frame_id].seq_no = 0;
          f_send[j-frame_id].frame_type = 0;
          f_send[j-frame_id].ack = j + 1;
        }
        else
        {
          if(k>0)
          {
            f_send[j-frame_id].seq_no = 0;
            f_send[j-frame_id].frame_type = 0;
            f_send[j-frame_id].ack = dropNo;
            k--;
            if (k == 0)
            {
              flag1 = 0;
              flag2 = 1;
            }
          }
        }
        printf("Frame number %d received: %s\n", f_recv[j-frame_id].seq_no, f_recv[j-frame_id].p.data);
      }
      else
      {
        printf("Frame not received\n");
        x1 = i/2;  
        flag1 = 1;
        dropNo = j;
        count++;
        f_send[j-frame_id].seq_no = 0;
        f_send[j-frame_id].frame_type = 0;
        f_send[j-frame_id].ack = dropNo;
        flag2 = 1;
      }
    }
    printf("\n");
    for(int j=frame_id; j<i+frame_id; j++)
    {
      f_send1 = f_send[j-frame_id];
      send(new_s, (void *)&f_send1, sizeof(struct frame), 0);
      printf("Ack number %d sent\n", f_send[j-frame_id].ack);
      if(f_send[j-frame_id].ack == 0)
      {
        printf("j = %d, k = %d\n", j, k);
      }
    }
    printf("\n");
    if (flag2 == 1)
    {
      flag2 = 0;
      struct frame f_send[frame_id+i-dropNo], f_send1;
      struct frame f_recv;
      len = recv(new_s, (void *)&f_recv, sizeof(struct frame), 0);
      printf("Frame number %d received: %s\n", f_recv.seq_no, f_recv.p.data);
      printf("\n");
      if (len > 0 )
      {
        for(int j = dropNo; j < frame_id+i; j++)
        {
          f_send[j-dropNo].seq_no = 0;
          f_send[j-dropNo].frame_type = 0;
          f_send[j-dropNo].ack = j + 1;
          f_send1 = f_send[j-dropNo]; 
          send(new_s, (void *)&f_send1, sizeof(struct frame), 0);
          printf("Ack number %d sent\n", j+1);
        }
      }
      else
      {
        printf("Frame not received\n");
      }
      printf("\n");
      frame_id = frame_id + i;
      i = 1;
    }
    else
    {
      frame_id = frame_id + i;
    }
    bzero((char *)f_send, sizeof(f_send));
    bzero((char *)f_recv, sizeof(f_recv));
  }
  close(new_s);
}

