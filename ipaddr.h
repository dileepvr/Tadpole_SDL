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
