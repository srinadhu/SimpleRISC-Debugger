/*

The project is developed as part of Computer Architecture class
Project Name: Debugger for simpleRISC Programs.

Developer's Name:Srinadh and Keshav.
Developer's Email id:2015csb1015@iitrpr.ac.in and 2015csb1016@iitrpr.ac.in
Date: Oct 14-17 and Nov5-6.

*/

/* mySimpleSim.cpp
   Purpose of this file: implementation file for mySimpleSim
*/

#include "mySimpleSim.h"
#include <stdlib.h>
#include <stdio.h>

//Register file
static unsigned int R[16];
static int PC;
//flags
static int gt,eq;
//Branches
static unsigned int branchPc,branchtgt;
//memory
static unsigned char MEM[4000];

//intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
static unsigned	int immediate;
static unsigned int AluResult;
static unsigned int Ldresult;
//for Debugger.
static unsigned int STEP=0;//for running once.
unsigned int Brk[4000];//for storing break points.

void reset_proc_without_instr(){
	int i;
	for(i=0;i<16;i++){                        // Initialising registers with value 0 and giving stack pointer some value.
		if(i!=14){
			R[i]=0;
		}
		else{
			R[14]=3000;	
		}
	}
	PC=0;                      //initializing Pc,branchPc,Step.
	branchPc=0;
	STEP=0;
}

void run_once(){//for running once.
	STEP=1;
}

void Print_Register(int res){//for printing required register.
	printf("Register %d:%d\n",res,R[res]);
} 

void Setting_Break(int address){//for setting break point.
	Brk[address%4000]=1;
}

void Print_Memory(unsigned int res){//for pinting memory instruction.
	printf("Instruction at memory 0x%x:%x\n",res,read_word(MEM,res));
}

void reset_break(){//resetting all breaks.
	int i;
	for(i=0;i<4000;i++){
		Brk[i]=0;
	}
}

void run_simplesim() {
  int i;
  while((read_word(MEM,PC)!=0) ) {
    fetch();
    decode();
    execute();
    //If memory acess goes out of assumed system(i.e here 4000) we get segmentation fault,so dealing with it.
    if(((instruction_word>>27) == 14)||((instruction_word>>27) == 15)){
      if(AluResult>4000){
	printf("Memory out of bounds for assumed system. \n");
	break;
      }
    }

    mem();
    write_back();
    if(STEP==1 ){
 	STEP=0;   	
 	break;
    }
    if( Brk[PC%4000]==1)
    	break;
  }
}

// it is used to set the reset values
//reset all registers and memory content to 0
void reset_proc() {
	int i;
	for(i=0;i<16;i++){                        // Initialising registers with value 0 and giving stack pointer some value.
		if(i!=14){
			R[i]=0;
		}
		else{
			R[14]=3000;	
		}
	}
	PC=0;
	for(i=0;i<4000;i=i+4){                                       // Initialising memory elements with '\0'.
		MEM[i]='\0';
	}
	branchPc=0;   
	STEP=0;                                               // Setting BranchPc to zero.
}

//load_program_memory reads the input memory, and pupulates the instruction
// memory
void load_program_memory(char *file_name) {
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    write_word(MEM, address, instruction);
  }
 
  fclose(fp);
  reset_break();//resetting break.
}

//writes the data memory in "data_out.mem" file
void write_data_memory() {
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }

  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, read_word(MEM, i));
  }
  fclose(fp);
}

//reads from the instruction memory and updates the instruction register
void fetch() {
	//Instruction Fetch.
	instruction_word=read_word(MEM,PC);
	PC+=4;
}

//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode() {
	//branchtgt.
	unsigned int bt= ((instruction_word<<5)>>5)<<2;
	if((instruction_word<<5)>>31){
		bt+=(0x7<<29);//extending sign.
	}
	branchtgt=bt;

	//operand 1.
	if((instruction_word>>27) == 20 ){
		operand1=R[15];                       // If isRet is true then storing the address in operand 1.
	}
	else{
		operand1=R[(instruction_word<<10)>>28];          // else extracting the source 1.
	}

	//operand 2
	if((instruction_word>>27) == 15 ){                  // checking if isSt is 1 or 0 and extracting operand accordingly.
		operand2=R[(instruction_word<<6)>>28];
	}
	else{
		operand2=R[(instruction_word<<14)>>28];
	}

	//immediate
	unsigned int immx=(instruction_word<<16)>>16;
	if((instruction_word <<14)>>30 == 0 ){                      // checking if modifier is 0 and extending accordingly.
		if(immx>>16){
			immx=immx|0xffff0000;
		}

	}
	else if((instruction_word <<14)>>30 == 1){                  // checking if modifier is 1 and extending accordingly.
		immx=immx>>16;
	} 
	else if((instruction_word <<14)>>30 == 2){                  // checking if modifier is 2 and extending accordingly.
		;
	}
	immediate=immx;
}

//executes the ALU operation based on ALUop
void execute() {
	//Branch pc.
	if((instruction_word>>27) == 20 ){
		branchPc=operand1;
	}
	else{
		branchPc=branchtgt;
	}

	//Alu result.
	//Add Instruction.
	if((instruction_word>>27) == 0){
		if((instruction_word<<5)>>31 == 1){
			AluResult=operand1+immediate;
		}
		else{
			AluResult=operand1+operand2;
		}
		
	}

	//Subtract Instruction.
	if((instruction_word>>27) == 1){
		if((instruction_word<<5)>>31){
			AluResult=operand1-immediate;
		}
		else{
			AluResult=operand1-operand2;
		}
	}

	//Multiply Instruction.
	if((instruction_word>>27) == 2){
		if((instruction_word<<5)>>31){
			AluResult=operand1*immediate;
		}
		else{
			AluResult=operand1*operand2;
		}
	}

	//Divide Instruction.
	if((instruction_word>>27) == 3){
		if((instruction_word<<5)>>31){
			AluResult=operand1/immediate;
		}
		else{
			AluResult=operand1/operand2;
		}
	}

	//Mod Instruction.
	if((instruction_word>>27) == 4){
		if((instruction_word<<5)>>31){
			AluResult=operand1%immediate;
		}
		else{
			AluResult=operand1%operand2;
		}
	}

	//cmp Instruction.
	if((instruction_word>>27) == 5){
		if((instruction_word<<5)>>31){
			if(operand1==immediate){//gt flag should be initialized to zero or else infinite loop will occur similarly others.
				eq=1;
				gt=0;
			}
			if(operand1>immediate){
				gt=1;
				eq=0;
			}
		}
		else{
			if(operand1==operand2){
				eq=1;
				gt=0;
			}
			if(operand1>operand2){
				gt=1;
				eq=0;
			}
		}
	}

	//and Instruction.
	if((instruction_word>>27) == 6){
		if((instruction_word<<5)>>31){
			AluResult=operand1 &immediate;
		}
		else{
			AluResult=operand1&operand2;
		}
	}

	//or Instruction.
	if((instruction_word>>27) == 7){
		if((instruction_word<<5)>>31){
			AluResult=operand1|immediate;
		}
		else{
			AluResult=operand1|operand2;
		}
	}

	//not Instruction.
	if((instruction_word>>27) == 8){
		if((instruction_word<<5)>>31){
			AluResult=~immediate;
		}
		else{
			AluResult=~operand2;
		}
	}

	//Mov Instruction.
	if((instruction_word>>27) == 9){
		if((instruction_word<<5)>>31){
			AluResult=immediate;
		}
		else{
			AluResult=operand2;
		}
	}

	//Lsl Instruction.
	if((instruction_word>>27) == 10){
		if((instruction_word<<5)>>31){
			AluResult=operand1<<immediate;
		}
		else{
			AluResult=operand1<<operand2;
		}
	}

	//Lsr Instruction.
	if((instruction_word>>27) == 11){
		if((instruction_word<<5)>>31){
			AluResult=operand1>>immediate;
		}
		else{
			AluResult=operand1>>operand2;
		}
	}

	//asr Instruction.
	if((instruction_word>>27) == 12){
		if((instruction_word<<5)>>31){
			AluResult=operand1>>immediate;
			if((AluResult!=0) && (operand1>>31) ){//extending sign.
				unsigned int i;
				for(i=32;i<(32-immediate);i--){
					AluResult=AluResult|(1<<i);
				}
			}
		}
		else{
			AluResult=operand1>>operand2;
			if((AluResult!=0) && (operand1>>31) ){//extending sign.
				unsigned int i;
				for(i=32;i<(32-operand2);i--){
					AluResult=AluResult|(1<<i);
				}
			}
		}
	}

	//nop Instruction.
	if((instruction_word>>27) == 13){
		;
	}

	//Ld Instruction.
	if((instruction_word>>27) == 14){
			AluResult=operand1+immediate;
	}

	//st Instruction.
	if((instruction_word>>27) == 15){
			AluResult=operand1+immediate;
	}

	//beq Instruction.
	if((instruction_word>>27) == 16){
		if(eq){
			PC+=(branchtgt-4);
		}
	}

	//bgt Instruction.
	if((instruction_word>>27) == 17){
		if(gt){
			PC+=(branchtgt-4);
		}
	}

	//b Instruction.
	if((instruction_word>>27) == 18){
		PC+=(branchtgt-4);
	}

	//call Instruction.
	if((instruction_word>>27) == 19){
		R[15]=PC;
		PC+=(branchtgt-4);
		
	}
	//ret Instruction.
	if((instruction_word>>27) == 20){
		PC=R[15];
	}
}

//perform the memory operation
void mem() {
	//Ld Instruction.
	if((instruction_word>>27) == 14){             // checking if isLd is 1 or 0.
		Ldresult=MEM[4000-AluResult];
	}
	//st Instruction.
	if((instruction_word>>27) == 15){             // checking if isSt is 1 or 0.
		MEM[4000-AluResult]=operand2;
	}
}
//writes the results back to register file
void write_back() {
	unsigned int temp=(instruction_word>>27),reg;
	if((temp!=5)&&(temp!=13)&&(temp!=15)&&(temp!=16)&&(temp!=17)&&(temp!=18)&&(temp!=20)){           // Checking if isWb is 1 or 0.
		if((instruction_word>>27) == 19){
			reg=15;
		}
		else{
			reg=(instruction_word<<6)>>28;
		}
		if((instruction_word>>27) != 19 && (instruction_word>>27) != 14){                    // checking if isLd and isCall is 1 or 0 and act accordingly.
			R[reg]=AluResult;
		}
		else if((instruction_word>>27) != 19 && (instruction_word>>27) == 14){              //Last case is taken care before.
			R[reg]=Ldresult;
		}
	}
}

int read_word(unsigned char *mem, unsigned int address) {
  int *data;
  data =  (int*) (mem + address);
  return *data;
}

void write_word(unsigned char *mem, unsigned int address, unsigned int data) {
  int *data_p;
  data_p = (int*) (mem + address);
  *data_p = data;
}
