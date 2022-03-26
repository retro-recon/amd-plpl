/* June 11, 1987 PLPL V2.2 Simulator Source module
READMAP.C reads the JEDEC file into a temporary buffer which
is used to hold the fuse/cell states. The simulator accesses
these states to determine the logic equations and architecture
set up of the device to simulate.
*/
#define DEF_MAIN 1
#include "simdef.h"
/* ------------------- */
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
/* this routine will always return the next character in the file;
if not, then this is an error */
one_char(xfile)
FILE **xfile;
{
nullfile_chk(*xfile,"ONE CHAR file");
return(fgetc(*xfile));
} /* ONE_CHAR */
/* ------------------- */
/* look for the KEY character in FILE1 */
get_key(file1,key)
FILE **file1;
char key;
{ char chrget;
nullfile_chk(*file1,"JEDEC file");
for (;!feof(*file1);) {
  chrget = fgetc(*file1);
  if (chrget == key)
     return(YES);
} /* FOR */
return(NO);
} /* GET_KEY */
/* ------------------- */
/*
Get the next few characters (must be between 0 and 9) until white
spaces are seen. Then convert the characters read into a number
*/
extract_number(nfile)
FILE **nfile;
{ char chr_val[20],echr;
  int val,digit_count;

nullfile_chk(*nfile,"ONE CHAR file");
for (chr_val[0] = '-',digit_count = 0;!feof(*nfile);) {
  echr = fgetc(*nfile);
  switch(echr) {
    case ' ' :
    case '\n':
    case '\t':
    case '\r': if (chr_val[0] == '-')  /* no digits were read */
                 inform("Specify link location",BAD_JEDEC);
               if (digit_count == 0)
                 inform("No fuse/link # after L",BAD_JEDEC);
               chr_val[digit_count] = '\0';
               val = atoi(chr_val);
               if (val >= fuse_count)
                 inform("Exceeded fuse array size",BAD_JEDEC);
               return(val);
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': chr_val[digit_count++] = echr;
              break;
    default : inform("Invalid link location",BAD_JEDEC);
  } /* SWITCH */
} /* FOR */
} /* EXTRACT_NUMBER */
/* ------------------- */
/*
read the fuse number, white space, and then a binary string
corresponding to the fuse states starting from the fuse number,
until a * is encountered
*/
read_fuse_states(rfile)
FILE **rfile;
{ char xchr;
  int fuse_location;
fuse_location = extract_number(rfile);
nullfile_chk(*rfile,"ONE CHAR file");
for (;!feof(*rfile);) {
  xchr = fgetc(*rfile);
  switch(xchr) {
    case ' ' :
    case '\n':
    case '\t':
    case '\r': break;
    case '0' :
    case '1' : jedec_map[fuse_location++] = xchr;
               break;
    case '*' : return;
    case ETX :
    default  : inform(SEARCH_FOR_STAR,BAD_JEDEC);
               break;
  } /* SWITCH on XCHR */
} /* FOR */
} /* READ_FUSE_STATES */
/* ------------------- */
/* fill the buffer with the vector values */
stuff_buffer(sfile,sbuffer,buflimit)
FILE **sfile;
char sbuffer[];
int buflimit;
{ char xchr;
  int item_cnt;

for (item_cnt = strlen(sbuffer); !feof(*sfile);) {
  xchr = fgetc(*sfile);
  if (isalpha(xchr)) {
    if (islower(xchr))
      xchr = toupper(xchr);
  }
  switch(xchr) {
    case ' ' : /* disregard white spaces */
    case '\n':
    case '\t':
    case '\r': break;
    case 'C' : /* load clocks */
    case 'K' :
    case '0' : /* load 0 to 9 */
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
    case 'L' : /* Low and High */
    case 'H' :
    case 'F' : /* Floats, Zeners, and Don't cares */
    case 'Z' :
    case 'X' :
    case 'P' : /* register preload and internal preload */
    case 'B' :
    case 'N' : /* N = don't check at all */
               if (item_cnt >= buflimit)
                 inform("Pin count-vector element mismatch",STOP);
               sbuffer[item_cnt++] = xchr;
               break;
    case '*' : sbuffer[item_cnt] = '\0';
               return;
    case ETX :
    default  : if (feof(*sfile)) {
                 sbuffer[item_cnt] = '\0';
                 return;
               }
               inform(SEARCH_FOR_STAR,BAD_JEDEC);
               break;
  } /* SWITCH on XCHR */
} /* FOR */
} /* STUFF_BUFFER */
/* ------------------- */
/* internal register check: check if test vector elements have been
   written for the internal registers. If a Note field is followed by
   the $ symbol, then the next characters until the * must be included
   in the PARTIAL_BUFFER.
*/
int_reg_check(ifile,partial_buffer,i_limits)
FILE **ifile;
char partial_buffer[];
int i_limits;
{ char chr1,chr2;
for (chr1 = fgetc(*ifile);
     isspace(chr1) && !feof(*ifile);
     chr1 = fgetc(*ifile))
  ;
switch(chr1) {
  case 'N' :
    for (chr2 = fgetc(*ifile);
         isspace(chr2) && !feof(*ifile);
         chr2 = fgetc(*ifile))
      ;
    if (chr2 == '$') {
      stuff_buffer(ifile,partial_buffer,i_limits);
      return;
    }
    else {
      if (get_key(ifile,'*') == NO)
        inform(SEARCH_FOR_STAR,BAD_JEDEC);
    } /* ELSE */
    break;
  default  :
    ungetc(chr1,*ifile);
    return;
  } /* SWITCH */
} /* INT_REG_CHECK */
/* ------------------- */
/*
This function reads the file (which is assumed to be a JEDEC 3-A
standard fuse map. The fields F (default fuse), L (link location),
and N (note field) are interpreted. If RD_TYPE = 1, then read the test
vectors and return them in the VBUFFER. Check that the vector limit
XLIMIT is not exceeded
*/
read_map(rd_file,vbuffer,rd_type,xlimits)
FILE **rd_file;
char vbuffer[];
int rd_type,xlimits; /* xlimits = max number of vector elemnts */
{ char item;
  int a1,field_specified; /* flag to mark if F field already specified once */

if (*rd_file == NULL)
  inform("JEDEC/Vector file NULL",STOP);
if (rd_type == 0)
  fprintf(stderr,"\nReading JEDEC map [%s]\n",jmapfile);
if (rd_type == 0) {
  if (get_key(rd_file,STX) == NO)
    inform("No STX",BAD_JEDEC);
  if (get_key(rd_file,'*') == NO)
    inform(SEARCH_FOR_STAR,BAD_JEDEC);
}
for (field_specified = NO;!feof(*rd_file);) {
  item = fgetc(*rd_file);
  switch(item) {
    case 'F'  : if (field_specified == YES && rd_type == 0) {
                  inform("Specify F field only once",BAD_JEDEC);
                }
                else
                  field_specified = YES;
                item = (char)one_char(rd_file);
                /* initialize JEDEC_MAP buffer to all 0s */
                switch(item) {
                  case '0':  /* default link value must be 0 or 1 */
                  case '1': if (rd_type == 0) {
                              for (a1 = 0; a1 < fuse_count; ++a1)
                                jedec_map[a1] = item;
                            }
                            break;
                  default : inform("F0/1 only",BAD_JEDEC);
                            break;
                } /* SWITCH */
                if (get_key(rd_file,'*') == NO)
                  inform(SEARCH_FOR_STAR,BAD_JEDEC);
                break;
    case 'L'  : if (rd_type == 0)
                  read_fuse_states(rd_file);
                else {
                  if (get_key(rd_file,'*') == NO)
                    inform(SEARCH_FOR_STAR,BAD_JEDEC);
                }
                break;
    case ETX  : return;
    case ' '  :
    case '\t' :
    case '\n' :
    case '\r' : break;
    case 'N'  : /* skip all note fields */
                if (get_key(rd_file,'*') == NO)
                  inform(SEARCH_FOR_STAR,BAD_JEDEC);
                break;
    case 'V'  : /* and vector fields for now */
                if (rd_type == 1) {
                  a1 = extract_number(rd_file); /* get # then discard */
                  stuff_buffer(rd_file,vbuffer,xlimits);
                  if (strlen(vbuffer) < num_pins)
                    int_reg_check(rd_file,vbuffer,xlimits);
                  return;
                }
                else {
                  if (get_key(rd_file,'*') == NO)
                    inform(SEARCH_FOR_STAR,BAD_JEDEC);
                } /* ELSE */
                break; /* skip all white space */
    default   : if (feof(*rd_file)) {
                  if (rd_type == 1)
                    return;
                  else
                    inform("No ETX",BAD_JEDEC);
                }
                if (get_key(rd_file,'*') == NO)
                   inform(SEARCH_FOR_STAR,BAD_JEDEC);
                break; /* skip all white space */
  } /* SWITCH */
} /* FOR */
inform("No ETX",BAD_JEDEC);
} /* READ_MAP */
/* ------------------- */
print_map()
{ int b,c;
for (b = 0, c = 0 ; b < fuse_count; ++b,++c) {
  if (b%input_array == 0) {
    fprintf(m29file,"\n[%5d]",b);
    c = 0;
  }
  if (c%4 == 0)
    fprintf(m29file," ");
  fprintf(m29file,"%c",jedec_map[b]);
} /* FOR */
} /* PRINT_MAP */
/* ------------------- */
calculate_checksum()
{
unsigned int checksum;
int x;
for (x = 0,checksum = 0; x < fuse_count; ++x) {
  if (jedec_map[x] == '1')
     checksum = checksum + (1 << (x%8));
} /* FOR */
fprintf(m29file,"\nChecksum [%04x]",checksum);
} /* CALCULATE_CHECKSUM */
/* ------------------- */
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
/* ------------------- */
findpart(part_path,part_only)
char part_path[],part_only[];
{ FILE *ftmp;
  char findbuf[80];
ftmp = fopen(CONFIG_FILE,"r");
if (ftmp != NULL) {
  fscanf(ftmp,"%s",findbuf);
  strcpy(part_path,findbuf);
  fclose(ftmp);
  ftmp = NULL;
}
ftmp = fopen(NAME_FILE,"r");
if (ftmp == NULL)
  return;
fscanf(ftmp,"%s",findbuf);
strcpy(part_only,findbuf);
switch(part_only[0]) {
  case 'p':
  case 'P': part_only[0] = 's';
            break;
} /* SWITCH */
strcat(part_path,part_only);
fclose(ftmp);
ftmp = NULL;
} /* FINDPART */
/* ------------------- */
valid_part(a_part)
char a_part[];
{ FILE *fptr;
fptr = fopen(a_part,"r");
if (fptr == NULL)
  return(NO);
else
  fclose(fptr);
fptr = NULL;
return(YES);
} /* VALID_PART */
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
/* ------------------- */
FILE *partsearch(partdbase)
char partdbase[];
{ char answer[80],tmp2[80];
  FILE *pathptr;
  int newpath;

tmp2[0] = '\0';
if (partdbase[0] == '\0')
  findpart(partdbase,tmp2);
else
  strcpy(tmp2,partdbase);
for (newpath = NO;valid_part(partdbase) == NO;) {
  fprintf(stderr,"Part [%s] not found; specify part and path",partdbase);
  fprintf(stderr,"\nPart [default = %s]: ",tmp2);
  get_string(answer,20);
  if (answer[0] != '\0')
    strcpy(tmp2,answer);
  fprintf(stderr,"Path: ");
  get_string(answer,60);  /* path cannot be more than 60 char */
  strcpy(partdbase,answer);
  strcat(partdbase,tmp2);
  newpath = YES;
} /* FOR */
if (newpath == YES) {
  if (answer[0] != '\0') {
    pathptr = fopen(CONFIG_FILE,"w");
    if (pathptr != NULL) {
      fprintf(pathptr,"%s",answer);
      fclose(pathptr);
      pathptr = NULL;
    }
    else
      inform("Cannot open config file",STOP);
  }
  pathptr = fopen(NAME_FILE,"w");
  if (pathptr != NULL) {
    fprintf(pathptr,"%s",tmp2);
    fclose(pathptr);
    pathptr = NULL;
  }
  else
    inform("Cannot open part file",STOP);
}
return(exist(partdbase,"r"));
} /* PARTSEARCH */
/* ------------------- */
read_mem_check(bufptr,rmsg)
char *bufptr,rmsg[];
{
if (bufptr == NULL) {
  fprintf(stderr,"\nInsufficient memory for [%s]",rmsg);
  exit(BAD_EXIT);
}
} /* READ_MEM_CHECK */
/* ------------------- */
read_JEDEC_map()
{ int i;

m29file = exist(MAP_FILE,"x");
if (debug_flag == YES)
  fprintf(stderr,"\nDevice link/fuse count: [%d]",fuse_count);
jedec_map = malloc(fuse_count);
read_mem_check(jedec_map,"JEDEC map");
/* initialize JEDEC_MAP buffer to all 0s */
for (i = 0; i < fuse_count; ++i)
  jedec_map[i] = '0';
read_map(&jedecfile,"-",0,0);

print_map();  /* for debugging */
calculate_checksum();

fclose(jedecfile);
fclose(m29file);
jedecfile = NULL;
m29file = NULL;
} /* READ_JEDEC_MAP */
