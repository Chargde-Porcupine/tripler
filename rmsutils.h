#include <stdio.h>  //inputs and outputs
#include <ulfius.h> //server library
#include <string.h> //string functions like strcat() and strcpy()
#include <malloc.h> //memory allocation
#include <time.h>   //self explanatory...

// gcc -I /home/matthew/Documents/cweb/ webserver.c -lulfius -ljansson

char *strs_cat(const char **strs)
{
    char *output = malloc(1 * sizeof(char));

    size_t curr_len = 0;

    for (int i = 0; strs[i][0] != '\0'; ++i)
    {
        curr_len += strlen(strs[i]);
        output = realloc(output, curr_len * sizeof(char));
        if (i == 0)
        {
            strcpy(output, strs[i]);
            continue;
        }
        strcat(output, strs[i]);
    }

    return output;
}

char *randpath(char *start)
{
    int num = random();
    char tstr[64];
    sprintf(tstr, "%d", num);
    return strs_cat((const char *[]){"/", start, "/", (char *)strdup(tstr), ""});
}

char *get_time(void)
{
    time_t mytime = time(NULL);
    char *time_str = ctime(&mytime);
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

/*
 * @brief Santizes a string input, removes a charachter of choice
 * @param haystack A string that you wish to sanitize
 * @param needle string that should be removed
 * @return A sanitized char *

*/
char *sanitize(char *haystack, char *needle)
{
  char word[strlen(haystack) + 1];
  strcpy(word, haystack);
    int idxToDel;
    while (strstr(word, needle))
    {
        if(strstr(word, needle)){
            
        idxToDel = strstr(word, needle) - word;
        memmove(&word[idxToDel], &word[idxToDel + 1], strlen(word) - idxToDel);
        }
    }
    return strdup((char *)word);
}