#include "main.h"
#include "lwip/dhcp.h"
#include "app_ethernet.h"
#include <stm32f4xx_hal.h>


void User_notification(struct netif *netif)
{
    if (netif_is_up(netif))
    {
//        LOG_WRITE(ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
    }
    else
    {
//        LOG_TRACE();
    }
}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_notify_conn_changed(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    if (netif_is_link_up(netif))
    {
        IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

        netif_set_addr(netif, &ipaddr, &netmask, &gw);

        LOG_WRITE("Static IP address: %s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));

        netif_set_up(netif);
    }
    else
    {
        LOG_WRITE("disconnect");

        /*  When the netif link is down this function must be called.*/
        netif_set_down(netif);
    }
}
