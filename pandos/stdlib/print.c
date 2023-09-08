#include "print.h"
#include "string.h"

#include <pandos_const.h>
#include <pandos_types.h>
#include <umps/cp0.h>
#include <umps/libumps.h>
#include <umps/arch.h>

char okbuf[2048]; /* sequence of progress messages */
char errbuf[128]; /* contains reason for failing */
HIDDEN char msgbuf[256]; /* nonrecoverable error message before shut down */
char *mp = okbuf;

#define TRANSMITTED 5
#define ACK 1
#define PRINTCHR 2
#define CHAROFFSET 8
#define STATUSMASK 0xFF
#define TERM0ADDR 0x10000254

typedef unsigned int devreg;

/* This function returns the terminal transmitter status value given its address */
devreg termstat(memaddr *stataddr)
{
    return ((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns TRUE if
 * print was successful, FALSE if not   */
unsigned int termprint(char *str, unsigned int term)
{
    memaddr *statusp;
    memaddr *commandp;
    devreg stat;
    devreg cmd;
    unsigned int error = FALSE;

    if (term < DEVPERINT)
    {
        /* terminal is correct */
        /* compute device register field addresses */
        statusp = (devreg *)(TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
        commandp = (devreg *)(TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));

        /* test device status */
        stat = termstat(statusp);
        if (stat == READY || stat == TRANSMITTED)
        {
            /* device is available */

            /* print cycle */
            while (*str != EOS && !error)
            {
                cmd = (*str << CHAROFFSET) | PRINTCHR;
                *commandp = cmd;

                /* busy waiting */
                stat = termstat(statusp);
                while (stat == BUSY)
                    stat = termstat(statusp);

                /* end of wait */
                if (stat != TRANSMITTED)
                    error = TRUE;
                else
                    /* move to next char */
                    *commandp = ACK;
                    str++;
            }
        }
        else
            /* device is not available */
            error = TRUE;

        *commandp = ACK;
    }
    else
        /* wrong terminal device number */
        error = TRUE;

    // termreg_t* base = (termreg_t*)TERM0ADDR;
    // base->transm_command = ACK;
    return (!error);
}

/* This function placess the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp)
{
    char *tstrp = strp;
    while ((*mp++ = *strp++) != '\0')
        ;
    mp--;
    termprint(tstrp, 1);
}

int printf(char *str, ...)
{
    va_list vl;
    int i = 0, j = 0;
    char buff[200], tmp[20];
    va_start(vl, str);
    while (str && str[i])
    {
        if (str[i] == '%')
        {
            i++;
            switch (str[i])
            {
            case 'c':
            {
                buff[j] = (char)va_arg(vl, int);
                j++;
                break;
            }
            case 'd':
            {
                itoa(va_arg(vl, int), tmp, 10);
                strcpy(&buff[j], tmp);
                j += strlen(tmp);
                break;
            }
            case 'x':
            {
                itoa(va_arg(vl, int), tmp, 16);
                strcpy(&buff[j], tmp);
                j += strlen(tmp);
                break;
            }
            case 'b':
            {
                //add padding
                itoa(va_arg(vl, int), tmp, 2);
                for(int i = 0; i < 16-strlen(tmp); i++)
                    strcpy(&buff[j++], "0");
                
                strcpy(&buff[j], tmp);
                j += strlen(tmp);
                break;
            }
            case 's':
            {
                const char* str_ = va_arg(vl, const char*);
                strcpy(&buff[j], str_);
                j += strlen(str_);
                break;
            }
            case 'p': {
                itoa(va_arg(vl, int), tmp, 16);
                strcpy(&buff[j], "0x");
                j += 2;
                strcpy(&buff[j], tmp);
                j += strlen(tmp);
                break;
            }
            }
        }
        else
        {
            buff[j] = str[i];
            j++;
        }
        i++;
    }
    buff[j] = '\0';

    addokbuf(buff);
    va_end(vl);
    return j;
}

inline int breakpoint()
{
    return 0;
}