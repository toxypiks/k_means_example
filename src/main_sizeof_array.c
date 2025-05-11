#include <stdio.h>

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

size_t al(float xs[]){
    return sizeof(xs)/sizeof(xs[0]);
}

void max_fct(float* myarray, size_t my_array_size) {
    size_t array_size = ARRAY_LEN(myarray);
    printf("array_size: %d\n", array_size);
    size_t array_size2 = al(myarray);
    printf("array_size2: %d\n", array_size2);

    printf("array_size_richtig: %d\n", my_array_size);
}

void main(void){
    float my_new_array[5] = {1.0f, 2.2f, 3.3f, 4.4f, 5.5f};
    printf("array_size00: %d\n", ARRAY_LEN(my_new_array));
    printf("array_size01: %d\n", al(my_new_array));

    max_fct(my_new_array, ARRAY_LEN(my_new_array));
}
