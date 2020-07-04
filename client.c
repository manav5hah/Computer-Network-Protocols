#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
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

int main(int argc, char * argv[]){
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char buf[MAX_LINE];
  int s;
  int len;
	// fp = fopen("rtt_vs_cwnd.txt","w");
  if (argc==2) {
    host = argv[1];
  }
  else {
    fprintf(stderr, "usage: simplex-talk host\n");
    exit(1);
  }
  /* translate host name into peer's IP address */
  hp = gethostbyname(host);
  if (!hp) {
    fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
    exit(1);
  }
  else
    printf("Client's remote host: %s\n", argv[1]);
  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);
  /* active open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(1);
  }
  else
    printf("Client created socket.\n");

  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(s);
      exit(1);
    }
  else
    printf("Client connected.\n");
  
  int frame_id = 1;
  int a_recv = 1;
  int x1 = 20;
  int count = 1;
  int fcount = 0, flag1 = 0, flag2 = 0;
  int rtt = 0, r_cnt = 0, c_cnt = 0;
  int f_limit = 370;
  int rtts[f_limit], cwnd[f_limit];

  for(int i = 1; frame_id<f_limit; i<x1?i=i*2:i++)
  {
    struct frame f_send[i], f_send1;
    struct frame f_recv[i], f_recv1;
    int dropNo;
    // fprintf(fp,"%d|", i);
    cwnd[c_cnt++] = i;
    for(int j=frame_id;j<i+frame_id;)
    {
      strcpy(f_send[j-frame_id].p.data, "Computer Networks");
      f_send[j-frame_id].seq_no = j;
      f_send[j-frame_id].frame_type = 1;
      f_send1 = f_send[j-frame_id];
      send(s, (void *)&f_send[j-frame_id], sizeof(struct frame), 0);
      printf("Frame number %d Sent\n", j);
      j++;
    }
    printf("\n");
    int prev[10];
    for(int j=frame_id;j<i+frame_id;j++)
    {
      int add_size = sizeof(socklen_t);
      int len = recv(s, (void *)&f_recv1, sizeof(struct frame), 0);
      f_recv[j-frame_id] = f_recv1;
      int k = 0;
      if(f_recv[j-frame_id].seq_no == 0)
      {
        printf("Ack number %d received\n", f_recv[j-frame_id].ack);
        // fprintf(fp_1,"%d|", f_recv[j-frame_id].ack);     
        if(f_recv[j-frame_id].ack == prev[0] && fcount < 3 && k < 3)
        {
          prev[k] = f_recv[j-frame_id].ack;
          k++;
          fcount = k;
          flag1 = 1;
        }
        else
        {
          flag1 = 0;
          k = 0;
        }
        prev[k] = f_recv[j-frame_id].ack;
      }
      else
      {
        printf("Ack not received\n");
        a_recv = 0;
      }
      bzero(buf, sizeof(buf));
      if(flag1 == 1)
      {
        dropNo = prev[0];
        flag2 = 1;
        x1 = i/2;
        if(fcount == 3)
        {
          for(int m = 0 ; m < 10; m++)
          {
            prev[m] = 0;
          }
        }
        fcount = 0;
      }
    }
    printf("\n");
    if (flag2 == 1)
    {
      f_send1 = f_send[dropNo-frame_id];
      send(s, (void *)&f_send1, sizeof(struct frame), 0);
      printf("Frame number %d Sent\n", dropNo);
      printf("\n");
      for(int j = dropNo; j<frame_id+i;j++)
      {
        int len = recv(s, (void *)&f_recv1, sizeof(struct frame), 0);
        f_recv[j-frame_id] = f_recv1;
        if(f_recv[j-frame_id].seq_no == 0)
        {
          printf("Ack number %d received\n", f_recv[j-frame_id].ack);
        } 
      }
      printf("\n");
      flag2 = 0;
      frame_id = frame_id + i;
      i = 1;
      for(int m = 0; m < 10; m++)
      {
        prev[m] = 0;
      }
      dropNo = 0;
    }
    else
    {
      frame_id = frame_id + i;
    }
    bzero((char *)f_send, sizeof(f_send));
    bzero((char *)f_recv, sizeof(f_recv));
    rtt++;
		// fprintf(fp,"%d\n", rtt);
    rtts[r_cnt++] = rtt;
  }
  printf("---OUTPUT---\n");
  printf("RTTs\tCWND\n");
  for(int i = 0; i < r_cnt; i++)
  {
    printf("%d\t%d\n", rtts[i], cwnd[i]);
  }
  // fclose(fp);
  close(s);
}