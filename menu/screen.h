/* this file contains a set of subroutines to position the cursor
   and clear parts of the screen.
   the assumption is made that install has been run to create the
   file CODES.CRT on the current logged disk.
*/

#include "Ascii.h"
#include "msdos.h"

#define video 16

static struct regval sreg, rreg;


crtinit()
{
/* no intit necessary for IBM-PC  */
}

char crtpos(colx,rowy)
char colx,rowy;
{
sreg.ax = 0x0200;
sreg.bx = 0;
sreg.dx = colx + (rowy<<8);
sysint(video, &sreg, &rreg);
}

unsigned int crtwhere()
{
sreg.ax = 0x0300;
sreg.bx = 0;
sysint(video, &sreg, &rreg);
return rreg.dx;
}


char crthome()
{
crtpos(0,0);
}

crtblank(col, row, count)
unsigned int col, row, count;
{
sreg.cx = count;
sreg.ax = 0x0A20;
sreg.bx = 0;
sysint(video, &sreg, &rreg);
crtpos( col, row );
}

char crtceol()
{
unsigned int count, col, row;
col = crtwhere();
row = col>>8;
col = col & 255;
if (col<79) count = 80-col;
else count = 1;
crtblank( col, row, count);
}

char crtceos()
{
unsigned int count, col, row;

count = crtwhere();
row = count>>8;
col = count & 255;
if (col<79) count = 80 - col;
else count = 1;
if (row<24) count += (24 - row) * 80;
crtblank( col, row, count);
}

char crtclr()
{
crthome();
crtceos();
}

char crtleft()
{
putchar(BSpace);
}
