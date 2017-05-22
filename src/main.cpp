/*

The project is developed as part of Computer Architecture class
Project Name: Debugger for SimpleRISC Programs

Developer's Name:Srinadh and Keshav
Developer's Email id:2015csb1015@iitpr.ac.in and 2015csb1016@iitrpr.ac.in
Date: Oct14-17 and Nov5-6.

*/

/* main.cpp
   Purpose of this file: The file handles the input and output, and
   invokes the Debugger.
*/

#include "mySimpleSim.h"
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char** argv) {

  reset_proc();
  char ip[400],op[400];
  reset_break();//resetting all breaks that are already set.
  while(1){
    cout<<"gdb>";
    cin.getline(ip,sizeof(ip));//it removes all spaces and reads into first argument (stackoverflow)

    if(strcmp(ip,"Quit()")==0){//For exiting.
        break;
    }

    else if(strcmp(ip,"run")==0){//NOTE:For running,the definition of running is taken such that program should start from first instruction.
      reset_proc_without_instr();
      run_simplesim();
    }

    else if(strcmp(ip,"continue")==0){//for continuing from breakpoint.
      run_simplesim();
    }

    else if(strcmp(ip,"step")==0){//for running once.
      run_once();
      run_simplesim();
    }
    //Contains invocations that need Two Arguments or more separated by space.Here 'break','print r -- ','print m --',' load'. 
    else{

      strcpy(op,ip);//For loading it into memory.
      if(ip[4]==' '){
        int j=0;
        op[4]='\0';
        if(strcmp(op,"load")==0){
          for(int i=5;ip[i]!='\0';i++){
            op[j]=ip[i];   
            j++;
          }
          op[j]='\0';
          load_program_memory(op);
        }
      }

      else if(ip[5]==' '){//Printing Required Register.
        if(ip[6]=='r'){
            int res=0,temp=8;
            while(ip[temp]!='\0'){
              res=res*10 +(ip[temp]-'0');
              temp++;
            }
            Print_Register(res);
        }

        else if(ip[6]=='m'){//printing instruction for given memory address in hexadecimal format.
          int res=0,temp=10;
          while(ip[temp]!='\0'){
	    if((ip[temp]=='a')|| (ip[temp]=='b')|| (ip[temp]=='c')|| (ip[temp]=='d')|| (ip[temp]=='e')||(ip[temp]=='f')){
            	res=res*16+(ip[temp]-87);//to give them values for a=10 and so on.
	    }
	    else{
		res=res*16+(ip[temp]-'0');
	    }
            temp++;
          }
           Print_Memory(res);
        }

        else{//Setting Break Points for given address in hexadecimal format.
          int res=0,temp=8;
          while(ip[temp]!='\0'){
            if((ip[temp]=='a')|| (ip[temp]=='b')|| (ip[temp]=='c')|| (ip[temp]=='d')|| (ip[temp]=='e')||(ip[temp]=='f')){
            	res=res*16+(ip[temp]-87);//to give them values for a=10 and so on.
	    }
	    else{
		res=res*16+(ip[temp]-'0');
	    }
            temp++;
          }
           Setting_Break(res);
        }
      }
    }
  }
  return 1;
}

