/*
 *  Xsim is a cpu simulator which can allocate and read the program into memory,
 *  and then initialize a xcpu strcture to execute the instructions.
 *
 *  Author: Hankun Zhang
 *  Course: CSCI3120
 *
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "xcpu.h"
#include "xis.h"
#include "xreloc.h"

int main(int argc, const char * argv[]) {
    
    //variables and objects declaration
    FILE *fp;
    int cycle;
    xcpu cpu;
    int i,j;
    int cpuReturn=1;
    
    //warn if wrong number of command line arguements were entered
    if( argc < 3 ) {
        printf( "usage: xsim cycle file.xo\n" );
        return 1;
    }
    
    //convert char to int
    cycle =  atoi(argv[1]) ;
    
    /* allocate memory for memory file */
    cpu.memory = malloc( XIS_MEM_SIZE + 85 ); /* + 85 for overflow */
    if( !cpu.memory ) {
        printf( "error: memory allocation (%d) failed\n", XIS_MEM_SIZE );
        return 1;
    }
    
    /* open input file */
    fp = fopen( argv[2], "rb" );
    if( !fp ) {
        printf( "error: could not open input file %s\n", argv[2] );
        return 1;
    }

    //load the binary file into memory so that xcpu.c can read the instructions from memory
    //every 8 bits in binary file should be a byte, which stored into a memory spot such as memory[1]
    fread( cpu.memory, XIS_MEM_SIZE, 1, fp);
    
    //initialize the xcpu strcture
    cpu.pc=0;
    cpu.state=0;
    for (j=0; j<16; j++)
        cpu.regs[j]=0;
    
    //run the instrctions for number of cycles
    for (i = 0; i < cycle; i++)
    {
        cpuReturn=xcpu_execute(&cpu);
        if(cpuReturn==0)
        {
            //if a wrong instrction is readed
            printf("CPU has halted.\n");
            break;
        }
       
    }
    
    //if all cycles are excuted withought error,print
    if(cpuReturn==1)
        printf("CPU ran out of time.\n");
    
    //close file
    fclose(fp);
    return 0;
    
   
    
    
    
    
    
}


