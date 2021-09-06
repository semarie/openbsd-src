/*	$OpenBSD$	*/
/*	Written by Sebasstien Marie, 2021, Public Domain	*/

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <err.h>

#include "main.h"

#define MYID	1234

#define UF1	(1 << 0)
#define UF2	(1 << 1)
#define UF3	(1 << 2)
#define UF4	(1 << 3)

int
do_user(void)
{
	int kq, uflags, n;
	struct kevent ev;
	struct timespec timo = { 1, 0 };
	
	if ((kq = kqueue()) == -1)
		err(1, "kqueue");

	EV_SET(&ev, MYID, EVFILT_USER, EV_ADD, UF1, 0, NULL);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
		err(1, "kevent: EV_ADD");
	uflags = UF1; /* expected fflags associated to MYID */

	EV_SET(&ev, MYID, EVFILT_USER, 0, NOTE_FFAND | UF3|UF4, 0, NULL);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
		err(1, "kevent: NOTE_FFAND");
	uflags &= UF3|UF4; /* expected fflags associated to MYID */

	EV_SET(&ev, MYID, EVFILT_USER, 0, NOTE_FFOR | UF2, 0, NULL);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
		err(1, "kevent: NOTE_FFOR");
	uflags |= UF2; /* expected fflags associated to MYID */

	
	EV_SET(&ev, MYID, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
		err(1, "kevent: NOTE_TRIGGER");

	
	n = kevent(kq, NULL, 0, &ev, 1, &timo);
	ASSX(n == 1);
	ASSX(ev.filter == EVFILT_USER);
	ASSX(ev.ident == MYID);
	ASSX(ev.fflags == uflags);
	
	return 0;
}
