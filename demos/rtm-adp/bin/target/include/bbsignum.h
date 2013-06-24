/* Signal number definitions.  Linux version.
   Copyright (C) 1995,1996,1997,1998,1999,2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef	_BBSIGNUM_H
#define _BBSIGNUM_H
/* Signals.  */
#define	BBSIGHUP		1	/* Hangup (POSIX).  */
#define	BBSIGINT		2	/* Interrupt (ANSI).  */
#define	BBSIGQUIT		3	/* Quit (POSIX).  */
#define	BBSIGILL		4	/* Illegal instruction (ANSI).  */
#define	BBSIGTRAP		5	/* Trace trap (POSIX).  */
#define	BBSIGABRT		6	/* Abort (ANSI).  */
#define	BBSIGIOT		6	/* IOT trap (4.2 BSD).  */
#define	BBSIGBUS		7	/* BUS error (4.2 BSD).  */
#define	BBSIGFPE		8	/* Floating-point exception (ANSI).  */
#define	BBSIGKILL		9	/* Kill, unblockable (POSIX).  */
#define	BBSIGUSR1		10	/* User-defined signal 1 (POSIX).  */
#define	BBSIGSEGV		11	/* Segmentation violation (ANSI).  */
#define	BBSIGUSR2		12	/* User-defined signal 2 (POSIX).  */
#define	BBSIGPIPE		13	/* Broken pipe (POSIX).  */
#define	BBSIGALRM		14	/* Alarm clock (POSIX).  */
#define	BBSIGTERM		15	/* Termination (ANSI).  */
#define	BBSIGSTKFLT	16	/* Stack fault.  */
#define	BBSIGCLD		BBSIGCHLD	/* Same as BBSIGCHLD (System V).  */
#define	BBSIGCHLD		17	/* Child status has changed (POSIX).  */
#define	BBSIGCONT		18	/* Continue (POSIX).  */
#define	BBSIGSTOP		19	/* Stop, unblockable (POSIX).  */
#define	BBSIGTSTP		20	/* Keyboard stop (POSIX).  */
#define	BBSIGTTIN		21	/* Background read from tty (POSIX).  */
#define	BBSIGTTOU		22	/* Background write to tty (POSIX).  */
#define	BBSIGURG		23	/* Urgent condition on socket (4.2 BSD).  */
#define	BBSIGXCPU		24	/* CPU limit exceeded (4.2 BSD).  */
#define	BBSIGXFSZ		25	/* File size limit exceeded (4.2 BSD).  */
#define	BBSIGVTALRM	26	/* Virtual alarm clock (4.2 BSD).  */
#define	BBSIGPROF		27	/* Profiling alarm clock (4.2 BSD).  */
#define	BBSIGWINCH	28	/* Window size change (4.3 BSD, Sun).  */
#define	BBSIGPOLL		BBSIGIO	/* Pollable event occurred (System V).  */
#define	BBSIGIO		29	/* I/O now possible (4.2 BSD).  */
#define	BBSIGPWR		30	/* Power failure restart (System V).  */
#define BBSIGSYS		31	/* Bad system call.  */
#define BBSIGUNUSED	31

#define	_NBBSIG		65	/* Biggest signal number + 1
				   (including real-time signals).  */


#define BBSIGMUTEX0_WAIT 100
#define BBSIGMUTEX15_WAIT 115
#define BBSIGEOT0 116
#define BBSIGEOT1 117
#define BBSIGEOT2 118
#define BBSIGINT0 120
#define BBSIGINT1 124
#define BBSIGINT2 128
#define BBSIGINT3 129
#define BBSIGINT4 130
#define BBSIGINT5 131
#define BBSIGINT6 132
#define BBSIGINT7 133

#define BBSIGLINKIN00 140
#define BBSIGLINKIN01 141

#define BBSIGLINKIN10 142
#define BBSIGLINKIN11 143

#define BBSIGLINKIN20 144
#define BBSIGLINKIN21 145

#define BBSIGLINKIN30 146
#define BBSIGLINKIN31 147


#define BBSIGLINKIN40 148
#define BBSIGLINKIN41 149

#define BBSIGLINKIN50 150
#define BBSIGLINKIN51 151

#define BBSIGLINKIN60 152
#define BBSIGLINKIN61 153

#define BBSIGLINKIN70 154
#define BBSIGLINKIN71 155

#define BBSIGALL 0x0fff
/**
   \brief signal sent by ssc handler
 */
#define BBSIGSSC 160

#endif	/* <signal.h> included.  */
