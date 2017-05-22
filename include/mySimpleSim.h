/*

The project is developed as part of Computer Architecture class
Project Name: Debugger for simpleRISC.

Developer's Name:Srinadh and Kesav.
Developer's Email id:2015csb1015@iitrpr.ac.in and 2015csb1016@iitrpr.ac.in
Date:Nov5-Nov6.

*/

/* mySimpleSim.h
   Purpose of this file: header file for mySimpleSim
*/

void run_simplesim();
void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();
void reset_break();
void reset_proc_without_instr();
void run_once();
void Print_Register(int);  
void Setting_Break(int);
void Print_Memory(unsigned int);


//reads from the instruction memory and updates the instruction register
void fetch();
//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode();
//executes the ALU operation based on ALUop
void execute();
//perform the memory operation
void mem();
//writes the results back to register file
void write_back();

int read_word(unsigned char *mem, unsigned int address);
void write_word(unsigned char *mem, unsigned int address, unsigned int data);
