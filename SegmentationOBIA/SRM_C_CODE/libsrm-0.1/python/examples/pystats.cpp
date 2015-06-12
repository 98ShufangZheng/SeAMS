unsigned int i;

unsigned int id = 0;
for(i=0; i < n; i++) {
  if (REGIONS1(i) == i) {
    REGIONS_ID1(i) = id;
    SIZES1(id) = REGIONS_SIZES1(i);
    id++;
  }
  else {
    REGIONS_ID1(i) = 0;
  }
 }

for(i=0; i < n; i++) {
  unsigned int region = REGIONS1(i);
  unsigned int reg = REGIONS_ID1(region);

  double x = i / Nim[0];
  double y = i % Nim[0];
  double r = (double)IM3((unsigned int)x, (unsigned int)y, 2);
  double g = (double)IM3((unsigned int)x, (unsigned int)y, 1);
  double b = (double)IM3((unsigned int)x, (unsigned int)y, 0);

  MU2(reg, 0) += r;
  MU2(reg, 1) += g;
  MU2(reg, 2) += b;
  MU2(reg, 3) += x;
  MU2(reg, 4) += y;

  // it's a symmetric matrix, we fill only the upper half
  SIGMA3(reg, 0, 0) += r*r;
  SIGMA3(reg, 0, 1) += r*g;
  SIGMA3(reg, 0, 2) += r*b;
  SIGMA3(reg, 0, 3) += r*x;
  SIGMA3(reg, 0, 4) += r*y;

  SIGMA3(reg, 1, 1) += g*g;
  SIGMA3(reg, 1, 2) += g*b;
  SIGMA3(reg, 1, 3) += g*x;
  SIGMA3(reg, 1, 4) += g*y;

  SIGMA3(reg, 2, 2) += b*b;
  SIGMA3(reg, 2, 3) += b*x;
  SIGMA3(reg, 2, 4) += b*y;

  SIGMA3(reg, 3, 3) += x*x;
  SIGMA3(reg, 3, 4) += x*y;

  SIGMA3(reg, 4, 4) += y*y;
 }

for(i=0; i < count; i++) {
  MU2(i, 0) /= SIZES1(i);
  MU2(i, 1) /= SIZES1(i);
  MU2(i, 2) /= SIZES1(i);
  MU2(i, 3) /= SIZES1(i);
  MU2(i, 4) /= SIZES1(i);

  SIGMA3(i, 0, 0) /= SIZES1(i);
  SIGMA3(i, 0, 1) /= SIZES1(i);
  SIGMA3(i, 0, 2) /= SIZES1(i);
  SIGMA3(i, 0, 3) /= SIZES1(i);
  SIGMA3(i, 0, 4) /= SIZES1(i);

  SIGMA3(i, 1, 1) /= SIZES1(i);
  SIGMA3(i, 1, 2) /= SIZES1(i);
  SIGMA3(i, 1, 3) /= SIZES1(i);
  SIGMA3(i, 1, 4) /= SIZES1(i);

  SIGMA3(i, 2, 2) /= SIZES1(i);
  SIGMA3(i, 2, 3) /= SIZES1(i);
  SIGMA3(i, 2, 4) /= SIZES1(i);

  SIGMA3(i, 3, 3) /= SIZES1(i);
  SIGMA3(i, 3, 4) /= SIZES1(i);

  SIGMA3(i, 4, 4) /= SIZES1(i);
 }

