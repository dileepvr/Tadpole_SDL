/*
  The files ipaddr.c and ipaddr.h are slightly modified versions of
  files from a lightweight webserver project called File-and-Froget.
  They were written by Tyler Montbriand. The code is hosted on:
  http://burningsmell.org/faf/ He didn't release it under a license,
  so I hope this attribution is enough!
*/
#include "ipaddr.h"

#ifdef WIN32

#include <winsock2.h>
#include <stdio.h>

/* Retrieve IP address via winsock2    */
/* TODO:  Retrieve MAC address as well */
int GetIP(const char *ignore, char *ip) 
{
  int error;
  char hostNameBuf[1000]; // should do...
  WORD requestedVersion = 2;
  WSADATA wsaData;
  int retval=0;
  struct hostent* hostInfo;

  if (error=WSAStartup(requestedVersion, &wsaData))
  {
	fprintf(stderr,"Unable to initialize winsock\n");
	fprintf(stderr,"Error code: %s\n",WSAGetLastError());
    return(-1);
  }

  gethostname(hostNameBuf,sizeof(hostNameBuf));
  hostInfo = gethostbyname(hostNameBuf);

  if (hostInfo)
  {
    struct sockaddr_in ipAddr;
    unsigned long lAddr;
    u_long addr  = inet_addr(hostInfo->h_addr);
    char* addrPtr = (char *)(&lAddr);

	memcpy(addrPtr,(char *)(hostInfo->h_addr),hostInfo->h_length);
    ipAddr.sin_addr.s_addr = lAddr;	
    
	sprintf(ip,"%s",inet_ntoa(ipAddr.sin_addr));
  }
  else
    retval=-1;

  WSACleanup();

  return retval;
}

#elif defined(__LINUX__)	/* Linux	*/
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <string.h>


/* retrieve IP address via ioctl interface */
int GetIP(const char *Interface, char *ip)
{
  int                  s;
  struct ifreq         ifr;
  int                  err;
  struct sockaddr_in * addr_in;

  s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
  if ( -1 == s )
  {
    perror("Getting interface socket");
    close(s);
    return(-1);
  }

  if(Interface == NULL) Interface="eth0";

  strncpy( ifr.ifr_name, Interface, IFNAMSIZ );
  err = ioctl( s, SIOCGIFADDR, &ifr );
  if ( -1 == err )
  {
    perror("Getting IP address");
    close(s);
    return(-1);
  }
  addr_in = (struct sockaddr_in *)&ifr.ifr_addr;

  if(ip != NULL)
  {
    sprintf(ip,"%s",inet_ntoa(addr_in->sin_addr));
  }

  err = ioctl(s, SIOCGIFHWADDR, &ifr );

  strncpy( ifr.ifr_name, Interface, IFNAMSIZ );
  if ( -1 == err )
  {
    perror("Getting HW address");
    close(s);
    return(-1);
  }


  close(s);
  return(0);
}

#elif defined(__APPLE__)
// #error "Someone fill in this stuff for OSX kthxbye"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_dl.h>

#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>

int GetIP(const char *iface, char *ip) 
{
  int have_mac=0,have_ip=0;
  struct ifaddrs *ifap,*ifnext;

  union
  {
    void               *vp;
    struct sockaddr_dl *sdl;
    struct sockaddr_in *sin;
  } addr;

  getifaddrs(&ifap);
  if(ifap==NULL)
    return(-1);

  if(iface==NULL)
    iface="en0";

  ifnext=ifap;
  while(ifnext!=NULL)
  {
    if(strcmp(iface,ifnext->ifa_name)==0)
    {
      addr.vp=(ifnext->ifa_addr);

      switch(ifnext->ifa_addr->sa_family)
      {
      case AF_INET:
        if(ip!=NULL)
        {
          char *tmp=inet_ntoa(addr.sin->sin_addr);
          strcpy(ip,tmp);
          have_ip=1;
        }
        break;

      case AF_LINK:
        break;
      }
    }

    ifnext=ifnext->ifa_next;
  }

  freeifaddrs(ifap);


  return(0);
}

#else
// #error "Not defined for this platform"
#endif
