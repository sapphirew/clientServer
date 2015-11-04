#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

/*-----------------------------------------------------------------
 * Program      : whoisserver
 * Purpose      : UNIX application program that acts as a server
 *                for 'whois' service on the local machine. It listens
 *                on well-known WHOIS port (43) and answers queries from
 *                clients. This program requires super-user privilege to
 *                run.
 * Use          : whois hostname username
 * Author       : Hao Wang
 * Date         : September, 2015
 *-----------------------------------------------------------------
 */

#define BACKLOG          5          /* # of requests to queue               */
#define MAXHOSTNAME      32         /* maximum host name length we tolerate */

void whois (int sock);
u_short portbase = 0;
int main(int argc,char *argv[])
{
   int s, t;                        /* socket descriptor                   */
   int i;                           /* general purpose integer             */
   struct sockaddr_in sa, isa;      /* internet socket structure           */
   struct hostent *hp;              /* host name structure                 */
   char *myname;                    /* pointer to the name of this program */
   struct servent *sp;              /* pointer to srevice entry            */
   char localhost[MAXHOSTNAME+1];   /* local host name as character string */

   myname = argv[0];
   portbase = 8000;
    
   /*
    * Look up the WHOIS service entry
    */
    if ((sp = getservbyname("whois","tcp")) == NULL){
      fprintf(stderr, "%s: No whois service on this host\n", myname);
      exit(1);
    }

   /*
    * Get our own host information
    */
   gethostname(localhost,MAXHOSTNAME);
   if ((hp = gethostbyname(localhost)) == NULL)
   {
      fprintf(stderr,"%s: Cannnot get local host info.\n",myname);
      exit(1);
   }
   
   /*
    * Put the WHOIS socket number and our address info
    * into the socket structure
    */
   //sa.sin_port = sp->s_port;

   sa.sin_port = htons(ntohs((u_short)sp->s_port)+portbase);

   bcopy((char *)hp->h_addr,(char *)&sa.sin_addr,hp->h_length);
   sa.sin_family = hp->h_addrtype;

   /*
    * Allocate an open socket for the incoming connections
    */
   if ((s = socket(hp->h_addrtype,SOCK_STREAM,0)) < 0)
   {
      perror("socket");
      exit(1);
   }

   /*
    * Bind the socket to the service port 
    * so we can hear incoming connection
    */
    if (bind(s,(struct sockaddr *)(&sa),sizeof sa) < 0)
    {
       perror("bind");
       exit(1);
    }
    /*
     * Set maximum connections we will fall behind
     */
    listen(s, BACKLOG);

    /*
     * Go into an infinite loop waiting for new connections
     */
   while (1)
   {
      i = sizeof isa;
      /*
       * We hang in accept() while waiting for new customers
       */
      if ((t = accept(s, (struct sockaddr *)&isa, &i)) < 0)
      {
         perror("accept");
         exit(1);
      }
      whois(t);      /* perform the actual WHOIS service */
      close(t);
   }
}

/*
 * Get the WHOIS request from remote host and format a reply.
 */
void whois(int sock){
   struct passwd *p;
   char buf[BUFSIZ+1];
   int i;

   /*
    * Get one line request
    */
   if ((i = read(sock,buf,BUFSIZ)) <= 0)
      return;
   buf[i] = '\0';
   /*
    * Look up the requested user and format a reply
    */
   if ((p = getpwnam(buf)) == NULL)
       strcpy(buf,"User not found\n");
   else
       sprintf(buf,"%s: %s\n",p->pw_name,p->pw_gecos);
   /*
    * Return reply
    */
   write(sock,buf,strlen(buf));
   return;
}







/*

./whoisclient.exe unix.cssd.pitt.edu haw69
*---------------------------------------------*
cc -o whoisclient.exe whoisclient.c
*/



/*
char *alias;
 alias = &(hp->h_alias);
 if(alias == null)
    printf("no alias-null\n");
 else
    printf("alias ame\n");
 
 int i = 0;
 while(hp-> h_alias[i]!=null){
    printf("h_alias[%d]=%s\n",i, hp->h_alias[i]);
    i++;
 }
 
// P67
 printf("h_address = %d\n",hp->h_addrtype);
 printf("h_addr_length=%d\n".hp->h_length);
 
 
 char *aliadaddress;
 aliasaddress = &(hp->h_address_list);
 if(aliasaddress == null)
    printf("no addresses\n");
 else
    printf("address list is\n");
 
 i=0;
 while(hp->h_addr_list[i]!=null){
    printf("h_addr_list[%d]=%s\n",i,inet_ntoa(hp->h_addr_list[i]));
    i++;
 }
 
 
*/