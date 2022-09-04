#include <stdio.h>  //inputs and outputs
#include <ulfius.h> //server library
#include <string.h> //string functions like strcat() and strcpy()
#include <malloc.h> //memory allocation
#include <time.h>   //self explanatory...
#include <dirent.h> //crawl directories

// gcc -I /home/matthew/Documents/cweb/ webserver.c -lulfius -ljansson
char *strs_cat(const char **strs);
char *randpath(char *start);
json_t *open_json(char *filepath);
char *get_time(void);
char *sanitize(char *haystack, char *needle);
char *construct_one(char *filepath);

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
        if (strstr(word, needle))
        {

            idxToDel = strstr(word, needle) - word;
            memmove(&word[idxToDel], &word[idxToDel + 1], strlen(word) - idxToDel);
        }
    }
    return strdup((char *)word);
}

char *construct_one(char *filepath)
{
    json_t *filejson = open_json(filepath);
    if (!filejson)
    {
        return NULL;
    }
    if (!json_is_object(filejson))
    {
        json_decref(filejson);
        return NULL;
    }
    //*One-Liner Olympics
    return strs_cat((const char *[]){"<html> <h1>", sanitize(sanitize((char *)json_string_value(json_object_get(filejson, "Title")), ">"), "<"), "</h1> <hr> <p>", sanitize(sanitize((char *)json_string_value(json_object_get(filejson, "Body")), ">"), "<"), "</p> </html>", ""});
}

char *construct_all()
{
    struct dirent *files;
    char *returned = "<html><h1>Rants,Ravings and Ramblings.</h1><hr>";
    DIR *dir = opendir("/home/matthew/Documents/cweb/tripleR");
    if (!dir)
    {
        return NULL;
    }
    while ((files = readdir(dir)) != NULL)
    {
        json_t *filejson = open_json(strs_cat((const char *[]){"/home/matthew/Documents/cweb/tripleR/", files->d_name, ""}));
        puts(files->d_name);
        if (!filejson)
        {
            continue;
        }
        if (!json_is_object(filejson) || !json_object_get(filejson, "Title") || !json_object_get(filejson, "Body") || !json_object_get(filejson, "Link"))
        {
            json_decref(filejson);
            continue;
        }
        returned = strs_cat((const char *[]){returned, "<h2><a href='", sanitize(sanitize((char *)json_string_value(json_object_get(filejson, "Link")), ">"), "<"), "'>", sanitize(sanitize((char *)json_string_value(json_object_get(filejson, "Title")), ">"), "<"), "</a></h2><p>", sanitize(sanitize((char *)json_string_value(json_object_get(filejson, "Body")), ">"), "<"), "</p><hr>", ""});
    }
    closedir(dir);
    return returned;
}

json_t *open_json(char *filepath)
{
    json_t *root;
    json_error_t error;
    FILE *fp;
    fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        return NULL;
    }
    if (fseek(fp, 0L, SEEK_END) != 0)
    {
        return NULL;
    };
    int sz = ftell(fp);
    char ch[sz];
    rewind(fp);
    fread(ch, 1, sz, (FILE *)fp);
    ch[sz] = '\0';
    root = json_loads(ch, 0, &error);
    if (!root)
    {
        return NULL;
    }
    return root;
}