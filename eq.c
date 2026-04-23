#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
	int length;
	int value;
} Word;

static int getvalue(char ch);

static int mysval;
static int buf;

int
getvalue(char ch)
{
	int i;
	int val;
	int order = 11;
	int n = 26;

	for (val=1, i=0; val < n; i += order, val++) {
		i %= n;
		if ('a' + i == ch)
			break;
	}

	return val;
}

int
main(void)
{
	const char *errstr;
	int ch;
	int cw = 0;
	int wait = 0;
	int tc = 0;
	char *part = NULL;
	FILE *fp = stdin;
	Word first;
	Word second;

	if (pledge("stdio", NULL) == -1)
		err(1, "pledge");

	while ((ch = getc(fp)) != EOF) {
		if (ch != '%' && !isblank((unsigned char)ch) && 
		    !iscntrl((unsigned char)ch))
			tc++;

		if (isdigit((unsigned char)ch)) {
			if ((part = reallocarray(part, wait + 1,
			    sizeof(ch))) == NULL)
				err(1, "malloc");
			part[wait] = (unsigned char)ch;
			part[wait + 1] = '\0';

			buf = strtonum(part, 1, INT_MAX, &errstr);
			if (errstr)
				errx(EXIT_FAILURE, "number is %s: %s",
				     errstr, part);

			wait++;

			continue;
		}

		if (wait) {
			mysval += buf;

			wait = 0;
			buf = 0;

			free(part);
			if ((part = malloc(sizeof(ch))) == NULL)
				err(1, "malloc");
		}

		if (isalpha((unsigned char)ch))
			mysval += getvalue(tolower((unsigned char)ch));
		else if (ch == '%' && !cw) { /* TODO: add a way to escape % */
			cw = 1;

			first.length = tc;
			first.value = mysval;

			tc = 0;
			mysval = 0;
		}

	}

	mysval += buf;
	free(part);

	if (cw) {
		second.length = tc;
		second.value = mysval;

		mysval = first.value * second.length +
		         second.value * first.length;
	}

	printf("%d\n", mysval);

	exit(EXIT_SUCCESS);
}
