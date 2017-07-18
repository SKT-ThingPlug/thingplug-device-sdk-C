/*******************************************************************************
* Copyright (c) 2017 SKT Corp.
*******************************************************************************/

#include "MQTTS7G2.h"
#include "nx_api.h"
#include "common_data.h"
#include "nx_dns.h"

static unsigned long MilliTimer = 0;
uint8_t init_flag = 0;
extern NX_DNS g_dns0;

void SysTickIntHandler(timer_callback_args_t * p_args) {
  MilliTimer += 10;
}

char expired(Timer* timer) {
  long left = timer->end_time - MilliTimer;
  return (left < 0);
}

void countdown_ms(Timer* timer, unsigned int timeout) {
  timer->end_time = MilliTimer + timeout;
}

void countdown(Timer* timer, unsigned int timeout) {
  timer->end_time = MilliTimer + (timeout * 1000);
}

int left_ms(Timer* timer) {
  long left = timer->end_time - MilliTimer;
  return (left < 0) ? 0 : left;
}

void InitTimer(Timer* timer) {
  timer->end_time = 0;
}

NX_PACKET *read_packet_ptr = NULL;
ULONG prepend_offset = 0;

// Network Headers & Variables & Functions
int s7g2_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  ULONG copied_idx = 0;
  Timer t;
  countdown_ms(&t , timeout_ms);
 
  while(len != 0){
    ULONG left_len;
    ULONG recv_len = 0;
    ULONG bytes_copied = 0;
    
    if(prepend_offset == 0){
      nx_tcp_socket_receive(&n->my_socket, &read_packet_ptr, timeout_ms);
    }
    nx_packet_length_get(read_packet_ptr, &recv_len);
    left_len = recv_len - prepend_offset;
    if(recv_len == 0){
        if(expired(&t))break;
    }

    
    if( left_len >= len ) {
      nx_packet_data_extract_offset(read_packet_ptr, prepend_offset, &buffer[copied_idx], len, &bytes_copied);
      if( left_len == bytes_copied ){
        prepend_offset = 0;
        nx_packet_release(read_packet_ptr);
        read_packet_ptr = NULL;
      } else {
        prepend_offset += bytes_copied;
      }
    } else {
      nx_packet_data_extract_offset(read_packet_ptr, prepend_offset, &buffer[copied_idx], left_len, &bytes_copied);
      prepend_offset = 0;
      nx_packet_release(read_packet_ptr);
      read_packet_ptr = NULL;
    }
    len -= bytes_copied;
    copied_idx += bytes_copied;
  }
  
  return copied_idx;
}

int s7g2_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  UINT status;
  NX_PACKET       *packet_ptr;
  status =  nx_packet_allocate(n->my_packet_pool, &packet_ptr, NX_TCP_PACKET, timeout_ms);
  if(status) return -1;
  status = nx_packet_data_append(packet_ptr, buffer, len, n->my_packet_pool, timeout_ms);
  if(status) return -1;
  status =  nx_tcp_socket_send(&n->my_socket, packet_ptr, timeout_ms);
  if(status) return -1;
  nx_packet_release(packet_ptr);
  return len;
}

void s7g2_disconnect(Network* n) 
{
  n->byte_ip = 0;
  nx_tcp_socket_disconnect(&n->my_socket, 200);
  nx_tcp_socket_delete(&n->my_socket);
}


void NewNetwork(Network* n) {
  n->my_ip = &g_ip0;
  n->my_packet_pool = &g_packet_pool0;
  n->byte_ip = 0;
  n->mqttread = s7g2_read;
  n->mqttwrite = s7g2_write;
  n->disconnect = s7g2_disconnect;
}

int ConnectNetwork(Network* n, char* addr, unsigned int port)
{
  UINT status;
  UCHAR record_buffer[200];
  UINT record_count;
  UINT error_count = 0;

  status = nx_dns_server_add(&g_dns0, IP_ADDRESS(8,8,8,8));
  if (status)error_count++;
  status = nx_dns_ipv4_address_by_name_get(&g_dns0, (UCHAR*)addr, (VOID *)&record_buffer[0], 200, &record_count, 400);
  if (status)error_count++;
  status = nx_tcp_socket_create(n->my_ip, &n->my_socket, "MqttSocket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 512, NX_NULL, NX_NULL);
  if (status)error_count++;
  n->byte_ip = *((ULONG *)(record_buffer));
  if (status)error_count++;
  status =  nx_tcp_client_socket_bind(&n->my_socket, 5002, NX_WAIT_FOREVER);
  if (status)error_count++;
  status =  nx_tcp_client_socket_connect(&n->my_socket, n->byte_ip, port, NX_WAIT_FOREVER);
  if (status)error_count++;
  if (error_count != 0)
      while (1);
  return 0;
}

int IsNetworkConnected(Network* n)
{
    if(n->byte_ip != 0)
        return 1;
    else
        return 0;
}

void NetworkDisconnect(Network* n)
{
    n->disconnect(n);
}
