#include <stdio.h>                                                                  
#include <stdlib.h>                                                                 
#include <dirent.h>                                                                 
#include <string.h>                                                                 
#include <math.h>                                                                   
#include <assert.h>                                                                 
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define BYTES_PER_READ 2                                                            
#define NIFTI_HEADER_SIZE 352

struct Nifti{
  char *h_buffer;
  unsigned short *buffer;
  size_t img_size;
};

struct AppArgs{
  char *in_file;
  //char *out_dir;
  //long int iterations;
};

struct Nifti read_img(char* filename){

  time_t start = time(NULL);

  long size;
  size_t number_of_ints;
  size_t number_of_elements;
  size_t h_read;
  size_t read;
  char *h_buffer;
  unsigned short *buffer;

  FILE *file = fopen(filename, "r");

  if (!file) {
      fprintf(stderr, "Unable to open/create file\n");
      exit( 1 );
  }

  /* Size of file */
  fseek(file, 0, SEEK_END);
  size = ftell(file) - NIFTI_HEADER_SIZE;
  fseek(file, 0, SEEK_SET);

  /* 'size' is the number of bytes, not the number of ints,
     so we need to adjust accordingly */
  number_of_ints = (size % sizeof(unsigned short) ? 1 : 0) + (size / sizeof(unsigned short));

  number_of_elements = (size % BYTES_PER_READ ? 1 : 0) +
      (size / BYTES_PER_READ);


  if(!(h_buffer = malloc(NIFTI_HEADER_SIZE))) {
      fprintf(stderr, "Failed to allocate header memory\n");
      exit( 1 );
  }
  if(!(buffer = malloc(number_of_ints * sizeof(unsigned short)))) {
      fprintf(stderr, "Failed to allocate buffer memory\n");
      exit( 1 );
  }

  h_read = fread(h_buffer, 1, NIFTI_HEADER_SIZE, file);
  read = fread(buffer, BYTES_PER_READ, number_of_elements, file);

  struct Nifti img = {h_buffer, buffer, read};
  fclose(file);

  time_t end = time(NULL);

  char hostname[HOST_NAME_MAX + 1];
  gethostname(hostname, HOST_NAME_MAX + 1);

  printf("read_start,%s,%lu,%lu,%s\n",filename,start,getpid(),hostname);
  printf("read_end,%s,%lu,%lu,%s\n",filename,end,getpid(),hostname);
  
  return img;

}

void *start_app(void *args){                                                     
                                                                               
  struct AppArgs* aargs = args;                                                
  char *in_file = aargs->in_file;                                                
  //char *out_dir = aargs->out_dir;                                              
  //long int iterations = aargs->iterations;                                     
                                                                               
  //char outfn[strlen(out_dir) + strlen(in_file) + 2];                     
  //sprintf(outfn, "%s/%s", out_dir, in_file);                             
                                                                               
  struct Nifti img = read_img(in_file);                                       
                                                                               
  //printf("number of iterations: %d\n", iterations);                      
  //for (int j=0; j < iterations; j++){                                      
                                                                               
  //    img.buffer = increment( thread_idx, img.buffer, img.img_size );      
  //}                                                                        
                                                                               
  //write_image( img, outfn );                                               
                                                                               
  return 0;                                                                    
                                                                               
}                              

int main(int argc, char* argv[]){
    struct AppArgs aargs = { argv[1] };
    start_app(&aargs);

    return 0;
}
