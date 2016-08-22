
#ifdef  __APPLE__
typedef off_t off64_t;
#define fseeko64        fseeko
#define ftello64        ftello
#define fopen64         fopen
#endif

struct rpmpay {
  char *name;
  unsigned int x;
  unsigned int lx;
  off64_t o;
  unsigned int l;
  int level;
};

int rpmoffs(FILE *fp, char *isoname, struct rpmpay **retp);

