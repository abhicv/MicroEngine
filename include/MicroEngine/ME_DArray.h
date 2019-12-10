#ifndef ME_DARRAY_H
#define ME_DARRAY_H

typedef struct
{
    void *data;
    int numOfElements;

}ME_DArray;

void ME_CreateDArray();
void ME_DestroyDArray(ME_DArray *dArray);

#endif // ME_DARRAY_H
