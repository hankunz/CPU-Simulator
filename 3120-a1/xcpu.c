/*
 *  Xcpu can fetch, decode, execute the instructions for a 16-bit xcpu.
 *
 *  Author: Hankun Zhang
 *  Course: CSCI3120
 *
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define X_INSTRUCTIONS_NOT_NEEDED

#include "xis.h"
#include "xcpu.h"

void xcpu_print( xcpu *c ) {
    int i;

    fprintf( stdout, "PC: %4.4x, State: %4.4x: Registers:\n", c->pc, c->state );
    for( i = 0; i < X_MAX_REGS; i++ ) {
        fprintf( stdout, " %4.4x", c->regs[i] );
    }
    fprintf( stdout, "\n" );
}

extern int xcpu_execute( xcpu *c ) {
    
    //variables declaration
    unsigned short instr = c->memory[(c->pc)];
    unsigned short H;
    unsigned short L,A,B,M;
    unsigned short D = 0;

    //For extended instr, first two bytes are instruction, second two bytes are other data
    if(XIS_IS_EXT_OP(instr))
    {
        H=instr;
        L=c->memory[(c->pc)+1];
        
        //combine two 8 bits to a 16 bits (label)
        A=c->memory[(c->pc)+2];
        B=c->memory[(c->pc)+3];
        M=A<<8;
        D=M|B;
        c->pc=c->pc+4;
        
    }
    //non-extended instructions
    else
    {
        H=instr;
        L=c->memory[(c->pc)+1];
        c->pc=c->pc+2;

    }
    

    
    switch (H) {
            
        //0-Operand Instructions
        case I_RET:
            c->pc=c->memory[c->regs[15]];
            c->regs[15]=c->regs[15]+2;
            break;
        case I_CLD:
            c->state = c->state &0xFFFD;
            break;
        case I_STD:
            c->state = c->state|0x0002;
            break;
            
        //1-Operand Instructions
        case I_NEG:
            c->regs[XIS_REG1(L)] = - c->regs[XIS_REG1(L)];
            break;
        case I_NOT:
            c->regs[XIS_REG1(L)] = ! c->regs[XIS_REG1(L)];
            break;
        case I_INC:
            c->regs[XIS_REG1(L)] = c->regs[XIS_REG1(L)]+1;
            break;
        case I_DEC:
            c->regs[XIS_REG1(L)] = c->regs[XIS_REG1(L)]-1;
            break;
        case I_PUSH:
            c->regs[15] = c->regs[15]-2;
            c->memory[c->regs[15]] = c->regs[XIS_REG1(L)]>>8;
            c->memory[c->regs[15]+1] = c->regs[XIS_REG1(L)]&0x00FF;
            break;
        case I_POP:
           c->regs[XIS_REG1(L)]=  (c->memory[c->regs[15]+1]);
            c->regs[15] = c->regs[15]+2;
            break;
        case I_JMPR:
            c->pc=c->regs[XIS_REG1(L)];
            break;
            
        case I_CALLR:
            c->regs[15] = c->regs[15]-2;
            c->memory[c->regs[15]]=c->pc;
            c->pc=c->regs[XIS_REG1(L)];
            break;
            
        case I_OUT:
            fprintf( stdout, "%c", c->regs[XIS_REG1(L)] );
            break;
            
        //pc go back to the current position
        case I_BR:
            if ((c->state & 0x0001) == 0x0001) {
                c->pc=(c->pc-2)+L;
            }
            break;
            
        //pc go back to the current position
        case I_JR:
            c->pc=(c->pc-2)+L;
            break;
            
        //2-Operand Instructions
        case I_ADD:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)]+c->regs[XIS_REG1(L)];
            break;
        case I_SUB:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)]-c->regs[XIS_REG1(L)];
            break;
        case I_MUL:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)]*c->regs[XIS_REG1(L)];
            break;
        case I_DIV:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)]/c->regs[XIS_REG1(L)];
            break;
        case I_AND:
            c->regs[XIS_REG2(L)]=(c->regs[XIS_REG1(L)]) & (c->regs[XIS_REG2(L)]);
            break;
        case I_OR:
            c->regs[XIS_REG2(L)]=(c->regs[XIS_REG1(L)])|(c->regs[XIS_REG2(L)]);
            break;
        case I_XOR:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)]^c->regs[XIS_REG1(L)];
            break;
        case I_SHR:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)]>>c->regs[XIS_REG1(L)];
            break;
        case I_SHL:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG2(L)] << c->regs[XIS_REG1(L)];
            break;
        case I_TEST:
            if ((c->regs[XIS_REG2(L)] & c->regs[XIS_REG1(L)])!= 0)
                c->state = c->state|0x0001;
            else
                c->state=c->state & 0xfffe;
            break;
        case I_CMP:
            if (c->regs[XIS_REG1(L)] < c->regs[XIS_REG2(L)])
                c->state = c->state|0x0001;
            else
                c->state=c->state & 0xfffe;
            break;
        case I_EQU:
            if (c->regs[XIS_REG1(L)] == c->regs[XIS_REG2(L)])
                c->state = c->state|0x0001;
            else
                c->state=c->state & 0xfffe;
            break;
        case I_MOV:
            c->regs[XIS_REG2(L)]=c->regs[XIS_REG1(L)];
            break;
            
        //combine two 8 bits memory chunks into a 16 bits register
        case I_LOAD:
            c->regs[XIS_REG2(L)]=c->memory[c->regs[XIS_REG1(L)]]<<8 | c->memory[c->regs[XIS_REG1(L)]+1];
            break;
            
        //divide 16 bits register into two 8 bits and store into memory
        case I_STOR:
            c->memory[c->regs[XIS_REG2(L)]] = c->regs[XIS_REG1(L)]>>8;
            c->memory[c->regs[XIS_REG2(L)]+1] = c->regs[XIS_REG1(L)]&0x00FF;
            break;
            
        case I_LOADB:
            c->regs[XIS_REG2(L)]=c->memory[c->regs[XIS_REG1(L)]];
            break;
            
        case I_STORB:
            c->memory[c->regs[XIS_REG2(L)]] = c->regs[XIS_REG1(L)]&0x00FF;
            break;
            
        //Exteded Instructions
        case I_JMP:
            c->pc = D;
            break;
            
        //call a label
        case I_CALL:

            c->regs[15] = c->regs[15]-2;
            c->memory[c->regs[15]]=c->pc;
            c->pc = D;
            break;
            
        case I_LOADI:
            c->regs[XIS_REG1(L)]=D;
            break;
            
        default:
            return 0;
    }
    
    //print the states if the debug bit is on
    if(c->state == (c->state|0x0002))
        xcpu_print(c);
    
    return 1;
}


/* Not needed for assignment 1 */
int xcpu_exception( xcpu *c, unsigned int ex ) {
    return 0;
}
