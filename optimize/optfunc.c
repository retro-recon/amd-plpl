/*
Oct 24, 1987: OPTFUNC.C contains miscellaneous
functions used by the optimizer.
*/
#define OPT_FILE 1
#include "opt2def.h"
/*  - - - - - - - - - - - - */
/* check if the FILE pointer is NULL */
nullfile_chk(a_ptr,chk_file)
FILE *a_ptr;
char chk_file[];
{
if (a_ptr == NULL) {
  fprintf(stderr,"\nCould not access [%s]\n",chk_file);
  exit(BAD_EXIT);
}
} /* FILE_CHECK */
/*  - - - - - - - - - - - - */
/*
Get a string; to fit into the buffer up to a limit. If a
space is encountered before the <CR>, then return the
char string and read the rest of the line (until '\n')
*/
get_string(a_buf,limit)
char a_buf[];
int limit;
{ int i;
  char gchr;
for (i = 0,gchr = 'x',a_buf[0] = '\0'; gchr != '\0' && i < limit-1; ++i) {
  a_buf[i] = getc(stdin);
  switch(a_buf[i]) {
  case '\n': a_buf[i] = '\0';
             return;
  case ' ' :
  case '\t': a_buf[i] = '\0';
             gchr = '\0'; /* break out of the FOR loop */
             break;
  }
} /* FOR */
a_buf[i] = '\0';
/* Cancel the rest of the line */
for (; gchr != '\n'; gchr = getc(stdin))
  ;
} /* GET_STRING */
/* ------------------- */
/* If mode is X, then always overwrite the file; if C, then
   check if the file exists; if it does not return NULL else
   return the pointer to it
*/
FILE *exist(filename,mode)
char *filename,*mode;
{ FILE *ptr;
  char answer[4],msg_exist[128];

switch(mode[0]) {
  case 'x':
  case 'X': ptr = fopen(filename,"w");
            nullfile_chk(ptr,filename);
            return(ptr);
  default: break;
} /* SWITCH */
ptr = NULL;
ptr = fopen(filename,"r");
switch(mode[0]) {
  case 'c':
  case 'C':
  case 'r':
  case 'R': if (ptr == NULL) {
              sprintf(msg_exist,"[%s] nonexistent",filename);
              if (mode[0] == 'c' || mode[0] == 'C')
                return(NULL);
              inform(msg_exist,NO_FILE);
            }
            return(ptr);
  case 'w': /* FILE does not exist; create it */
  case 'W': if (ptr == NULL) {
              ptr = fopen(filename,mode);
              nullfile_chk(ptr,filename);
              return(ptr);
            }
  default : break;
} /* SWITCH on mode */
fprintf(stderr,"\n[%s] exists\nDelete it, Create new file, or Exit (d/c/e)? ",
        filename);
get_string(answer,3);
if (isascii(answer[0])) {
  answer[0] = toupper(answer[0]);
}
ptr = NULL; /* reset the pointer */
switch(answer[0]) {
  case 'D' : ptr = fopen(filename,mode);
             nullfile_chk(ptr,filename);
             return(ptr);
  case 'C' : fprintf(stderr,"\nNew Filename: ");
             get_string(filename,LONGNAME-1);
             return(exist(filename,mode));
  default  : exit(GOOD_EXIT);
} /* SWITCH */
} /* EXIST */
/*  -   -   -   -   -  */
convert(base,nmbr,pattern)
/* convert using BASE the number NMBR and put the result in PATTERN */
char base;  /* can take the values B,O,D,H for Bin, Octal, Dec, and Hex */
char nmbr[],pattern[];
{ int i,j,k,value,num_type;
  char buf[MAX_NAME];

/* convert everything to decimal first */

num_type = DECIMAL;
if (nmbr[0] == '#')  /* take out the radices */
 { switch(nmbr[1])
    { case 'b' :
      case 'B' : num_type = BINARY;
                 break;
      case 'o' :
      case 'O' : num_type = OCTAL;
                 break;
      case 'd' :
      case 'D' : num_type = DECIMAL;
                 break;
      case 'h' :
      case 'H' : num_type = HEXADECIMAL;
                 break;
      default  : err(FATAL,"unknown number base",0,0,0);
    } /* SWITCH */
   for (i=2, j=0; nmbr[i] != '\0'; ++i)
     buf[j++] = nmbr[i];
   buf[j] = '\0';
 }
else /* NMBR was decimal */
  strcpy(buf,nmbr);
switch(num_type)
 { case OCTAL       : sscanf(buf,"%o",&value); break;
   case HEXADECIMAL : sscanf(buf,"%x",&value); break;
   case DECIMAL     : sscanf(buf,"%d",&value); break;
   case BINARY
       : i = strlen(buf);
         for (j=i-1,k=1,value=0; j >= 0; --j)
          { if (j!= i-1)
              k = k*2;
            if (buf[j] == '1')
              value = value+k;
          }
         break;
   default : value = (int)atoi(nmbr);
             break;
 } /* SWITCH */
switch(base)
 { case 'b' :
   case 'B' : if (num_type == BINARY)
               { strcpy(pattern,buf);
                 return;
               }
              for (i=0 ; i<16 ; ++i)
               { if (((value>>i)&(0x1)) == 1)
                   buf[i] = '1';
                 else
                   buf[i] = '0';
               }
              buf[i] = '\0';
              j = strlen(buf)-1;  /* reverse the string */
              for (i = j,pattern[j+1] = '\0';i >= 0; --i)
                pattern[j-i] = buf[i];
              break;
   case 'd' :
   case 'D' : sprintf(pattern,"%d",value);
              break;
   case 'o' :
   case 'O' : sprintf(pattern,"%o",value);
              break;
   case 'h' :
   case 'H' : sprintf(pattern,"%x",value);
              break;
 } /* SWITCH */
} /* CONVERT */
/* - - - - - - - - - - */
/* display the CSTRING to the output file pointer OUT_FILE and
   to the screen if SUPPRESS_MSG is NO
*/
display(cstring)
char cstring[];
{
if (suppress_msg == NO)
  fprintf(stderr,"%s",cstring);
if (out_file != NULL)
  fprintf(out_file,"%s",cstring);
} /* DISPLAY */
/* - - - - - - - - - - */
