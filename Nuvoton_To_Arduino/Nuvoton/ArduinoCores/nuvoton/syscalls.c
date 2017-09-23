/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
  * \file syscalls_mini51.c
  *
  * Implementation of newlib syscall.
  *
  */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "Arduino.h"

#include "syscalls.h"

#include <stdio.h>
#include <stdarg.h>

#if defined (  __GNUC__  ) /* GCC CS3 */
  #include <sys/types.h>
  #include <sys/stat.h>
#endif

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

#undef errno
extern int errno ;
extern int  _end ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void _exit( int status ) ;
extern void _kill( int pid, int sig ) ;
extern int _getpid ( void ) ;

extern void* _sbrk ( int incr )
{
    static unsigned char *heap = NULL ;
    unsigned char *prev_heap ;

    if ( heap == NULL )
    {
        heap = (unsigned char *)&_end ;
    }
    prev_heap = heap;

    heap += incr ;

    return (void*) prev_heap ;
}

extern int link( char *cOld, char *cNew )
{
    return -1 ;
}

extern int _close( int file )
{
    return -1 ;
}

extern int _fstat( int file, struct stat *st )
{
    st->st_mode = S_IFCHR ;

    return 0 ;
}

extern int _isatty( int file )
{
    return 1 ;
}

extern int _lseek( int file, int ptr, int dir )
{
    return 0 ;
}

extern int _read(int file, char *ptr, int len)
{
    return 0 ;
}

extern int _write( int file, char *ptr, int len )
{
    int iIndex ;
    for ( iIndex=0 ; iIndex < len ; iIndex++, ptr++ )
    {
#if defined(__M451__)			  
				// Check if the transmitter is ready
				while(UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
				// Send character
				UART0->DAT = *ptr;		  		  
#elif defined(__NUC240__)
				// Check if the transmitter is ready
				while(UART0->FSR & UART_FSR_TX_FULL_Msk);
				// Send character
				UART0->THR = *ptr;		  		  
#elif defined(__NANO100__)
				// Check if the transmitter is ready
				while(UART0->FSR & UART_FSR_TX_FULL_F_Msk);
				// Send character
				UART0->THR = *ptr;		  
#elif defined(__NUC131__)
				// Check if the transmitter is ready
				while(UART0->FSR & UART_FSR_TX_FULL_Msk);
				// Send character
				UART0->THR = *ptr;		  		  
#elif defined(__NANO1X2__)
				// Check if the transmitter is ready
				while(UART_IS_TX_FULL(UART0));
				
				// Send character
				UART0->THR = *ptr;		  
#endif
    }

    return iIndex ;
}

extern void _exit( int status )
{
    printf( "Exiting with status %d.\n", status ) ;

    for ( ; ; ) ;
}

extern void _kill( int pid, int sig )
{
    return ;
}

extern int _getpid ( void )
{
    return -1 ;
}
