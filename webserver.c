#include <ulfius.h>
#include <rmsutils.h>
#include <jansson.h>

#define PORT 8000
#define STARTDIR "/home/matthew/Documents/cweb/templates"
#define NAME "TRIPLER"
#define VERSION "0.1"

struct _u_instance instance;

int route_catchall(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    FILE *fp;
    puts(request->http_url);
    fp = fopen(strs_cat((const char *[]){STARTDIR, request->http_url, ""}), "r");
    if (fp == NULL)
    {
        ulfius_set_string_body_response(response, 404, "Resource Not Found");
        return U_CALLBACK_COMPLETE;
    }

    if (fseek(fp, 0L, SEEK_END) != 0)
    {
        ulfius_set_string_body_response(response, 500, "Server Error");
        return U_CALLBACK_COMPLETE;
    };
    int sz = ftell(fp);
    char ch[sz];
    rewind(fp);
    fread(ch, 1, sz, (FILE *)fp);
    u_map_put(response->map_header, "Content-Type", "text/html");
    ulfius_set_binary_body_response(response, 200, ch, sz);
    return U_CALLBACK_CONTINUE;
}

int route_index(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    json_t *root;
    json_error_t error;
    FILE *fp;
    fp = fopen("wisdoms.json", "r");
    if (fp == NULL)
    {
        ulfius_set_string_body_response(response, 404, "Resource Not Found");
        return U_CALLBACK_COMPLETE;
    }
    if (fseek(fp, 0L, SEEK_END) != 0)
    {
        puts("e");
        ulfius_set_string_body_response(response, 500, "Server Error");
        return U_CALLBACK_COMPLETE;
    };
    int sz = ftell(fp);
    char ch[sz];
    rewind(fp);
    fread(ch, 1, sz, (FILE *)fp);
    ch[sz] = '\0';
    root = json_loads(ch, 0, &error);
    if (!root)
    {
        ulfius_set_string_body_response(response, 500, "Server Error");
        return U_CALLBACK_COMPLETE;
    }
    if (!json_is_array(root))
    {
        fprintf(stderr, "error: root is not an array\n");
        json_decref(root);
        ulfius_set_string_body_response(response, 500, "Server Error");
        return U_CALLBACK_COMPLETE;
    }

    int index = rand() % json_array_size(root);
    const char *selected = json_string_value(json_array_get(root, index));

    char *responsetext = strs_cat((const char *[]){"You have reached the 😀", NAME, " API, version ", VERSION, ". ", "It is currently ", get_time(), " wherever this server is running.", " Your Selected Wisom is: ", selected, ""});
    u_map_put(response->map_header, "Content-Type", "text/plain; charset=utf-8");
    ulfius_set_string_body_response(response, 200, responsetext);
    free(responsetext);
    return U_CALLBACK_CONTINUE;
}

int main(void)
{
    // intialize randm seed
    srand(time(0));

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK)
    {
        fprintf(stderr, "Error ulfius_init_instance, abort\n");
        return (1);
    }

    // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", "*", NULL, 0, &route_catchall, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/", NULL, 0, &route_index, NULL);
    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK)
    {
        printf("Start framework on port %d\n", instance.port);

        // Wait for the user to press <enter> on the console to quit the application
        getchar();
    }
    else
    {
        fprintf(stderr, "Error starting framework\n");
    }
    printf("End framework\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}
