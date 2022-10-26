#include <cstdint>
#include <cstdio>

int main(void) {
  float A[4] = {1, 2, 3, 4};
  float B[4] = {5, 6, 7, 8};

  union Whatever {
    int32_t* int32_type;
    float* float_type;
  } wa, wb;

  wa.float_type = A;
  wb.float_type = B;

  for (int i = 0; i < 4; i++) wb.int32_type[i] = wa.int32_type[i];

  for (int i = 0; i < 4; i++) printf("%f\n", B[i]);

  return 0;
}
