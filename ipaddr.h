#ifndef __IPADDR_H__
#define __IPADDR_H__

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/*Retrieves IP address and MAC address for specified interface.
  Places IP into ip as a character string, and MAC address into
  mac as a 6-byte binary code.

  Example:  GetIP("eth0",buf,mac); */
int GetIP( const char *iface, char *ip);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*__IPADDR_H__*/
