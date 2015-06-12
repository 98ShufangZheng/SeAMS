#include "types_communs.hh"

void sauvegardeRect(regionC** &bm, int Wi, int Le, int lux, int luy, int rdx, int rdy, int nummod, int iter);

int saveRGBtiffRect(const char *filename, const char *soft, const char *desc,
		    unsigned int *data, short photometric, int width, int height);
