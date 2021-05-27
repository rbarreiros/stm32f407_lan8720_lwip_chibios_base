/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <ch.h>
#include <hal.h>
#include <strings.h>
#include <lwipthread.h>
#include <lwip/udp.h>
#include <lwip/netif.h>

#include "debug.h"

// Just a blinker
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  palSetPadMode(GPIOE, 0, PAL_MODE_OUTPUT_PUSHPULL);
  chRegSetThreadName("blinker");
  while (true) {
    palTogglePad(GPIOE, 0);
    chThdSleepMilliseconds(500);
  }
}

void echoRecv(void *arg,
              struct udp_pcb *pcb,
              struct pbuf *p,
              const struct ip4_addr *addr,
              short unsigned int port)
{
  (void)arg;
  DBG("Received a packet on UDP Echo server from: %s:%d\r\n", ip4addr_ntoa(addr), port);
  udp_sendto(pcb, p, addr, port);
}

void startUdpEcho(void)
{
  struct udp_pcb *pcb = udp_new();
  udp_bind(pcb, IP_ADDR_ANY, 7);
  udp_recv(pcb, echoRecv, NULL);
}

/*
 * Application entry point.
 */
int main(void) 
{
  bool linkUp = false;
  struct netif *thisif = NULL;

  halInit();
  chSysInit();
  debugInit();

  DBG("... Initializing ethernet\r\n");

  lwipInit(NULL);

  DBG("... Starting the blinking thread\r\n");
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  // Chibios default network link name is ms0
  thisif = netif_find("ms0");
  if(!thisif)
  {
    DBG("Unable to find default network interface\r\n");
  }
  else 
  {
    // Wait for the link to be up. we stay here until timeout....
    uint8_t cnt = 0;
    while(!linkUp && cnt < 10)
    {
      linkUp = netif_is_up(thisif);
      cnt++;
      chThdSleepMilliseconds(500);
    }

    if(linkUp)
    {
      DBG("Network Config:\r\n\r\n");
#if LWIP_DHCP
      DBG("DHCP: Enabled\r\n");
#endif
#if LWIP_AUTOIP
      DBG("AUTO IP: Enabled\r\n");
#endif

      DBG("MAC: %x:%x:%x:%x:%x:%x\r\n", thisif->hwaddr[0],
                                        thisif->hwaddr[1],
                                        thisif->hwaddr[2],
                                        thisif->hwaddr[3],
                                        thisif->hwaddr[4],
                                        thisif->hwaddr[5]);
      DBG("\r\nIP: %s\r\n", ip4addr_ntoa(netif_ip4_addr(thisif)));
      DBG("Subnet: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(thisif)));
      DBG("Gateway: %s\r\n\r\n", ip4addr_ntoa(netif_ip4_gw(thisif)));
    }
    else 
      DBG("Link didn't get up in time.");
  }

  if(linkUp)
    startUdpEcho();

  while (true) 
  {
    chThdSleepMilliseconds(500);
  }
}
