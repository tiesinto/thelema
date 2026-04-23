#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#define MYSTIFY(X) ((X) * ((X) + 1) / 2)

static int snum;
static char mbuf[BUFSIZ];

int
main(void)
{
	const char *errstr;
	char *p, *endp;

	if (pledge("stdio", NULL) == -1)
		err(1, "pledge");

	p = mbuf;
	endp = mbuf + sizeof(mbuf) - 1;
	for (;;) {
		if (!fgets(p, endp - p, stdin))
			break;
		for (; *p && p < endp; ++p)
			;
		if (*(p - 1) == '\n') { /* strip newline */
			*(p - 1) = '\0';
			break;
		}
	}

	snum = strtonum(mbuf, 1, INT_MAX, &errstr);
	if (errstr)
		errx(EXIT_FAILURE, "input is %s: %s", errstr, mbuf);

	printf("%d\n", MYSTIFY(snum));

	exit(EXIT_SUCCESS);
}
