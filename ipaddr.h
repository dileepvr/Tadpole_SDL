/*
  The files ipaddr.c and ipaddr.h are slightly modified versions of
  files from a lightweight webserver project called File-and-Froget.
  They were written by Tyler Montbriand. The code is hosted on:
  http://burningsmell.org/faf/ He didn't release it under a license,
  so I hope this attribution is enough!
*/


#ifndef __IPADDR_H__
#define __IPADDR_H__

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/*Retrieves IP address for specified interface.
  Places IP into ip as a character string into
  mac as a 6-byte binary code.

  Example:  GetIP("eth0",buf); */
int GetIP( const char *iface, char *ip);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*__IPADDR_H__*/
