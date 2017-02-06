/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef SOCKET_H_
#define SOCKET_H_

#include "wiznet.h"

#define htons(x) __REV16(x)
#define ntohs(x) __REV16(x)
#define htonl(x) __REV(x)
#define ntohl(x) __REV(x)

/** Socket file descriptor and select wrapper
  */
class Socket {
public:
    /** Socket
     */
    Socket();
    
    /** Set blocking or non-blocking mode of the socket and a timeout on
        blocking socket operations
    \param blocking  true for blocking mode, false for non-blocking mode.
    \param timeout   timeout in ms [Default: (1500)ms].
    */
    void set_blocking(bool blocking, unsigned int timeout=1500);
    
    /** Close the socket file descriptor
     */
    int close();
    
    ~Socket();
    
protected:
    int _sock_fd;
    bool _blocking;
    int _timeout;

    WIZnet_Chip* eth;
};


#endif /* SOCKET_H_ */

