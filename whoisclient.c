
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

/* --------------------------------------------------------------
 * Prgram  :   whoisclient
 *
 * Purpose :   UNIX application program that becomes a client
 *             for the Inetnet "whois" service.
 *
 * Use     :   whois hostname username
 *
 * Author  :   Hao Wang
 *
 * Date    :   September, 2015
 *--------------------------------------------------------------
 */

u_short portbase = 0;

void main(int argc,char *argv[])             
{
   int s;                   /* socket descriptor                   */
   int len;                 /* length of received data             */
   struct sockaddr_in  sa;  /* socket addr. structure              */
   struct hostent * hp;     /* result of host name lookup          */
   struct servent * sp;     /* result of service lookup            */
   char buf[BUFSIZ + 1];    /* buffer to read whois infomation     */
   char *myname;            /* pointer to name of this program     */
   char *host;              /* pointer to remote host name         */
   char *user;              /* pointer to remote user name         */

   myname = argv[0];
   portbase = 8000;

   /*
    * Check that there are two command line arguments
    */
   if (argc != 3) {
       fprintf(stderr, "Usage: %s host username\n",myname);
       exit(1);
   }

   host = argv[1];
   user = argv[2];

   /* 
    * Look up the specified hostname
    */
   if ((hp = gethostbyname(host)) == NULL) {
       fprintf(stderr, "%s: %s: no such host?\n",myname,host);
       exit(1);
   }

   /*
    * Put host's address and address type into socket structure
    */
   bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
   sa.sin_family = hp->h_addrtype;
   
   /*
    * Put the whois socket number into the socket structure.
    */
    if((sp = getservbyname("whois","tcp")) == NULL){
      fprintf(stderr, "%s: No whois service on this host\n", myname);
      exit(1);
    }

   /*
    * Put the whois socket number into the socket structure.
    */
   //sa.sin_port = sp->s_port;
   sa.sin_port = htons(ntohs((u_short)sp->s_port)+portbase);


   /*
    * Allocate an open socket.
    */
   if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0){
      perror("socket");
      exit(1);
   }

   /*
    * Connect to the remote server.
    */
   if (connect(s, (struct sockaddr *)(&sa), sizeof sa) < 0)
   {
      perror("connect");
      exit(1);
   }

   /* 
    * Send the request
    */
   if (write(s,user,strlen(user)) != strlen(user)){
      fprintf(stderr, "%s: write error\n", myname);
      exit(1);
   }

   /*
    * Read the reply and put to user's output
    */
   while ( (len = read(s, buf, BUFSIZ)) > 0)
      write(1,buf,len);
   close(s);
   exit(0);
}