#include "user_thread.h"
#include "MQTT/MQTTS7G2.h"
#include "MQTT/MQTTS7G2_TLS.h"
#include "MQTT/MQTTClient.h".
#include "oneM2M/SKTtpDebug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "oneM2M/oneM2M.h"
#include "oneM2M/Versions/oneM2M_V1_14.h"
#include "Configuration.h"
#include "sample/MA/MA.h"

ULONG error_counter;
UINT status;
ULONG ip0_ip_address = 0;
ULONG ip0_mask = 0;

/* User Thread entry function */
void user_thread_entry(void)
{
    status = nx_ip_fragment_enable (&g_ip0);
    if (status != NX_SUCCESS)
        error_counter++;
    status = nx_dhcp_start (&g_dhcp_client0);
    /* Check for DHCP start error. */
    if (status)
        error_counter++;
    while (status != NX_IP_ADDRESS_RESOLVED)
    {
        /* Wait for IP address to be resolved through DHCP. */
        nx_ip_status_check (&g_ip0, NX_IP_ADDRESS_RESOLVED, (ULONG *) &status, 10);
    }
    status = nx_ip_interface_address_get (&g_ip0, 0, &ip0_ip_address, &ip0_mask);
    if (status != NX_SUCCESS)
        error_counter++;

    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);

    Timer t;
    InitTimer (&t);

    MARun();

    return 0;
}
