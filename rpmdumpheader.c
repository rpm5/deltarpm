/*
 * Copyright (c) 2004 Michael Schroeder (mls@suse.de)
 *
 * This program is licensed under the BSD license, read LICENSE.BSD
 * for further information
 */

#include <fcntl.h>
#include <string.h>

#ifdef	RPM5
#include <rpmio.h>
#include <rpmtag.h>
#include <rpmdb.h>
#include <pkgio.h>
#include <rpmrc.h>
#include <rpmts.h>
#else	/* RPM5 */
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmdb.h>
#endif	/* RPM5 */

int main(int argc, char **argv)
{
  rpmts ts = NULL;
  Header h;
  int ret = 0;

  char *n;
  char *e;
  char *v;
  char *r;
  char *a = NULL;

  if (argc > 2 && !strcmp(argv[1], "-a"))
    {
      a = argv[2];
      argc -= 2;
      argv += 2;
    }
  if (argc != 2)
    {
      fprintf(stderr, "usage: rpmdumpheader package\n");
      exit(1);
    }
  n = argv[1];
  r = strrchr(n, '-');
  if (r == 0 || r == n)
    {
      fprintf(stderr, "package name must be in n-(e:)v-r format\n");
      exit(1);
    }
  *r++ = 0;
  v = strrchr(n, '-');
  if (v == 0 || v == n)
    {
      fprintf(stderr, "package name must be in n-(e:)v-r format\n");
      exit(1);
    }
  *v++ = 0;
  e = strchr(v, ':');
  if (e)
    {
      char *t;
      *e++ = 0;
      t = e;
      e = v;
      v = t;
    }

  rpmReadConfigFiles(NULL, NULL);

  ts = rpmtsCreate();
  
#ifdef	RPM5
  rpmmi mi = rpmtsInitIterator(ts, RPMTAG_NAME, n, 0);
  rpmmiAddPattern(mi, RPMTAG_EPOCH, RPMMIRE_STRCMP, e);
  rpmmiAddPattern(mi, RPMTAG_VERSION, RPMMIRE_STRCMP, v);
  rpmmiAddPattern(mi, RPMTAG_RELEASE, RPMMIRE_STRCMP, r);
  if (a)
    rpmmiAddPattern(mi, RPMTAG_ARCH, RPMMIRE_STRCMP, a);

  if ((h = rpmmiNext(mi)) != NULL)
    {   const char item[] = "Header";
	const char * msg = NULL;
	FD_t fdo = Fopen("-", "w.ufdio");
        (void) rpmpkgWrite(item, fdo, h, &msg);
        msg = _free(msg);
        (void) Fflush(fdo);
    }
  else
    {
      if (e)
	fprintf(stderr, "%s-%s:%s-%s%s%s is not installed\n", n, e, v, r, a ? "." : "", a ? a : "");
      else
	fprintf(stderr, "%s-%s-%s%s%s is not installed\n", n, v, r, a ? "." : "", a ? a : "");
      ret = 1;
    }
  mi = rpmmiFree(mi);
#else	/* RPM5 */
  rpmdbMatchIterator mi = rpmtsInitIterator(ts, RPMTAG_NAME, n, 0);
  rpmdbSetIteratorRE(mi, RPMTAG_EPOCH, RPMMIRE_STRCMP, e);
  rpmdbSetIteratorRE(mi, RPMTAG_VERSION, RPMMIRE_STRCMP, v);
  rpmdbSetIteratorRE(mi, RPMTAG_RELEASE, RPMMIRE_STRCMP, r);
  if (a)
    rpmdbSetIteratorRE(mi, RPMTAG_ARCH, RPMMIRE_STRCMP, a);

  if ((h = rpmdbNextIterator(mi)) != NULL)
    {
      FD_t fdo = Fopen("-", "w.ufdio");
      headerWrite(fdo, h, HEADER_MAGIC_YES);
    }
  else
    {
      if (e)
	fprintf(stderr, "%s-%s:%s-%s%s%s is not installed\n", n, e, v, r, a ? "." : "", a ? a : "");
      else
	fprintf(stderr, "%s-%s-%s%s%s is not installed\n", n, v, r, a ? "." : "", a ? a : "");
      ret = 1;
    }
  mi = rpmdbFreeIterator(mi);
#endif	/* RPM5 */

  ts = rpmtsFree(ts);
  exit(ret);
}
