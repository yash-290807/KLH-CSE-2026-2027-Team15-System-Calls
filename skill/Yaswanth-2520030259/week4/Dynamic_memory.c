#include <stdio.h>
#include <stdlib.h>

int main()
{
    int size;

    printf("Enter initial array size: ");
    scanf("%d", &size);

    if (size <= 0)
    {
        printf("Invalid size\n");
        return 1;
    }

    int *arr = malloc(size * sizeof(int));

    if (arr == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    printf("Enter %d numbers:\n", size);

    for (int i = 0; i < size; i++)
    {
        scanf("%d", &arr[i]);
    }

    printf("\nOriginal array:\n");

    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }

    // Increase array size
    int new_size;

    printf("\n\nEnter new size: ");
    scanf("%d", &new_size);

    if (new_size <= 0)
    {
        free(arr);
        printf("Invalid size\n");
        return 1;
    }

    int *temp = realloc(arr, new_size * sizeof(int));

    if (temp == NULL)
    {
        free(arr);
        printf("Memory resizing failed\n");
        return 1;
    }

    arr = temp;

    if (new_size > size)
    {
        printf("Enter %d additional numbers:\n",
               new_size - size);

        for (int i = size; i < new_size; i++)
        {
            scanf("%d", &arr[i]);
        }
    }

    printf("\nFinal array:\n");

    for (int i = 0; i < new_size; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");

    // Release memory
    free(arr);

    printf("Memory released successfully.\n");

    return 0;
}
