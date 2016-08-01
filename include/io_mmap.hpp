/*
* using mmap for intensive io
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

class mmap_reader{
    public:
        int fd;
        char *start;
        struct stat sb;
        long end;
        
        mmap_reader():end(0){}

bool open_mmap(char* filepath){
    end = 0;
  fd = open(filepath, O_RDONLY);
  fstat(fd, &sb);
//  printf("filesize=%ld\n", sb.st_size);
  start = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(start == MAP_FAILED)
      return false;
  return true;
}

void close_mmap(){
  munmap(start, sb.st_size);
  close(fd);
}

long filesize(){
    return sb.st_size;
}

long linecount(){
    long cnt = 0;
    while (end < sb.st_size){
        while(start[end++] !='\n');
        cnt++;
    }
    return cnt;
}

void skipfirstline(){
  end = 0; 
  while (start[end++]!='\n');
}

void skipline(){
  while (start[end++]!='\n');
}

long skiplinefrom(long& offset){
  long old = offset;
  while (start[offset++]!='\n');
  return offset - old;
}

void skiprecord(){
  while(start[end] != '|' && start[end] != '\n') 
    ++end;
  ++end;
}


bool hasNext(){
    return end < sb.st_size;
}

/* read a integer */
int getInt() {
  int x = 0;
  while (start[end]>='0' && start[end]<='9') {
    x = x*10+(start[end]-'0');
    ++end;
  }
  ++end; /* skip '|' */
  return x;
}

long getLong() {
  long x = 0;
  while (start[end]>='0' && start[end]<='9') {
    x = x*10+(start[end]-'0');
    ++end;
  }
  ++end; /* skip '|' */
  return x;
}

/*return the read bytes. */
long getLong(long& offset, long& p1){
  long x = 0;
  long old = offset;
  while (start[offset]>='0' && start[offset]<='9') {
    x = x*10+(start[offset]-'0');
    ++offset;
  }
  ++offset; /* skip '\n' */
  p1 = x;
  return offset-old;
}

void getString(char* name){
    int len = 0;
    while(start[end] != '|'){
        name[len] = start[end];
        ++len;
        ++end;
    }
    name[len] = 0;
    ++end; /* skip '|' */
}
};
