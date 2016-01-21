#include <stdio.h>
#include <stdint.h>
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
