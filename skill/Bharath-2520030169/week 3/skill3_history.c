#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HISTORY 10
#define MAX_LENGTH 100

int main()
{
    char *history[MAX_HISTORY];
    char input[MAX_LENGTH];
    int count = 0;

    printf("Simple Command History\n");
    printf("Type 'history' to display commands\n");
    printf("Type 'exit' to quit\n\n");

    while (1)
    {
        printf("myshell> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
            break;

        if (strcmp(input, "history") == 0)
        {
            printf("\nCommand History:\n");

            for (int i = 0; i < count; i++)
                printf("%d: %s\n", i + 1, history[i]);

            printf("\n");
            continue;
        }

        if (strlen(input) > 0)
        {
            if (count < MAX_HISTORY)
            {
                history[count] = malloc(strlen(input) + 1);

                if (history[count] == NULL)
                {
                    printf("Memory allocation failed\n");
                    break;
                }

                strcpy(history[count], input);
                count++;
            }
            else
            {
                free(history[0]);

                for (int i = 1; i < MAX_HISTORY; i++)
                    history[i - 1] = history[i];

                history[MAX_HISTORY - 1] = malloc(strlen(input) + 1);
                strcpy(history[MAX_HISTORY - 1], input);
            }
        }
    }

    // Release memory
    for (int i = 0; i < count; i++)
        free(history[i]);

    printf("Shell terminated.\n");

    return 0;
}
