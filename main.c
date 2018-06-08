// NGINX reverse proxy conf generator
// Written by @nepgeargo

/* Generates NGINX reverse proxy conf file in the following format:
 
upstream [src_fqdn]
{
    server [dest_fqdn]:[dest_port];
}

server
{
    listen *:[src_port];
    server_name [src_fqdn];

    location /
    {
        proxy_pass https://[dest_fqdn];
        proxy_set_header        Host            $host;
        proxy_set_header        X-Real-IP       $remote_addr;
        proxy_set_header        X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_connect_timeout 180;
        proxy_send_timeout 180;
        proxy_read_timeout 180;
    }
}
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Colored output definitions
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define BLU "\x1B[34m"
#define RESET "\x1B[0m"

// Variable size definitions
#define NUM_CHAR 100
#define NUM_PORT 65536

// Error definitions
#define ERR_OVERFLOW "overflow"
#define ERR_NO_FQDN "no FQDN input"
#define ERR_NO_PORT "no port input"

// Site type definitions
#define SRC "source"
#define DEST "destination"

typedef struct Site
{
    char fqdn[NUM_CHAR];
    int fqdnLen;
    int port;
} Site_t;

void GetSite(Site_t *site, char *type);
int GetLine(char *line, int maxLen);

int IsNewlineOrReturn(char c);

void PrintConf(Site_t *src, Site_t *dest);

void PrintNewline();
void PrintWarning(const char *funcName, char *errType);
void PrintOverflow(const char *funcName, int num);

int main(int argc, char *argv[])
{
    Site_t src;
    Site_t dest;

    printf("Welcome to NGINX reverse proxy configuration generator\n");
    printf("Written by @nepgeargo\n");
    PrintNewline();
    GetSite(&src, SRC);
    GetSite(&dest, DEST);
    PrintConf(&src, &dest);
}

// Fills a Site_t struct with FQDN and port number
void GetSite(Site_t *site, char *type)
{
    char fqdn[NUM_CHAR];
    int fqdnLen;
    int port;

    // Reads the FQDN
    printf("Please enter the FQDN of the %s: ", type);
    fqdnLen = GetLine(fqdn, NUM_CHAR);
    if (fqdnLen != 0)
    {
        // Read successful
        strcpy(site->fqdn, fqdn);
        site->fqdnLen = fqdnLen;
    }
    else
    {
        // Read failed
        PrintWarning(__func__, ERR_NO_FQDN);
        exit(EXIT_FAILURE);
    }

    // Reads the port number
    printf("Please enter the port number of the %s: ", type);
    if ((scanf("%d", &port) == 1) && (port > 0) && (port <= NUM_PORT))
        // Read successful
        site->port = port;
    else
    {
        // Read failed
        PrintWarning(__func__, ERR_NO_PORT);
        exit(EXIT_FAILURE);
    }

    // Prints the inputs
    PrintNewline();
    printf(GRN "FQDN of the %s: %s\n", type, fqdn);
    printf("Port number of the %s: %d\n" RESET, type, port);
    PrintNewline();
}

// Gets a line of input
int GetLine(char *line, int maxLen)
{
    char c;
    int len = 0;
    int numOverflow = 0;

    while (((c = getchar()) != EOF) && !isalpha(c))
    {
        // Do nothing
    }

    if (c == EOF)
        return 0;

    line[len++] = c;

    while (!IsNewlineOrReturn(c = getchar()))
    {
        if (len < (maxLen - 1))
            line[len++] = c;
        else
            numOverflow++;
    }

    PrintOverflow(__func__, numOverflow);

    line[len] = '\0';
    return len;
}

// Returns if a char is a newline or EOF
int IsNewlineOrReturn(char c)
{
    if ((c == '\n') || (c == '\r'))
        return 1;
    return 0;
}

void PrintConf(Site_t *src, Site_t *dest)
{
    printf(GRN "Configuration generated!\n" RESET);
    PrintNewline();

    printf("upstream %s\n", src->fqdn);
    printf("{\n");
    printf("    server %s:%d\n", dest->fqdn, dest->port);
    printf("}\n");
    PrintNewline();

    printf("server\n");
    printf("{\n");
    printf("    listen *:%d;\n", src->port);
    printf("    server_name %s;\n", src->fqdn);
    PrintNewline();
    printf("    location /\n");
    printf("    {\n");
    printf("        proxy_pass https://%s;\n", dest->fqdn);
    printf("        proxy_set_header        Host            $host;\n");
    printf("        proxy_set_header        X-Real-IP       $remote_addr;\n");
    printf("        proxy_set_header        X-Forwarded-For $proxy_add_x_forwarded_for;\n");
    PrintNewline();
    printf("        proxy_connect_timeout 180;\n");
    printf("        proxy_send_timeout 180;\n");
    printf("        proxy_read_timeout 180;\n");
    printf("    }\n");
    printf("}\n");
}

// Prints a newline
void PrintNewline()
{
    printf("\n");
}

// Prints a warning
void PrintWarning(const char *funcName, char *errType)
{
    printf(RED "Warning! Error: %s in function %s\n" RESET, errType, funcName);
}

// Prints the number of overflows, if any
void PrintOverflow(const char *funcName, int num)
{
    if (num > 0)
    {
        PrintWarning(funcName, ERR_OVERFLOW);
        printf(RED "Number of overflows: %d\n" RESET, num);
    }
}
