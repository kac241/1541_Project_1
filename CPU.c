/**************************************************************/
/* CS/COE 1541				 			
   just compile with gcc -o pipeline pipeline.c			
   and execute using							
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr	0  
***************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 

#define PREDICTION_TABLE_SIZE 64

struct trace_item buffered[9];

int main(int argc, char **argv)
{
  struct trace_item *tr_entry;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int predictionMethod = 0;
  
  unsigned char t_type = 0;
  unsigned char t_sReg_a= 0;
  unsigned char t_sReg_b= 0;
  unsigned char t_dReg= 0;
  unsigned int t_PC = 0;
  unsigned int t_Addr = 0;

  unsigned int cycle_number = 0;

  int branchPredictionTable[PREDICTION_TABLE_SIZE];  //branch prediction hashtable

  if (argc == 1) {
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n\n");
    exit(0);
  }
    
  trace_file_name = argv[1];
  if (argc == 2){
    trace_view_on = 0;
    predictionMethod = 0;
    printf("\n Prediction Method is: %d \n",predictionMethod);
    printf("\n Trace View is: %d  \n", trace_view_on);
  }
  
  if (argc == 3) {
    predictionMethod = atoi(argv[2]);
    trace_view_on = 0;
    printf("\n Prediction Method is: %d \n",predictionMethod);
     printf("\n Trace View is: %d  \n", trace_view_on);
  }

  if (argc == 4) {
    trace_view_on = atoi(argv[3]);
    predictionMethod = atoi(argv[2]);
    printf("\n Prediction Method is: %d \n",predictionMethod);
    printf("\n Trace View is: %d  \n", trace_view_on);
  }

  if ((predictionMethod != 0) && (predictionMethod != 1) && (predictionMethod != 2)) {
    printf("\n Invalid Prediction Method. (0 = Always predict not taken");
    printf("\n 1 = 1 bit predictor\n");
    printf(" 2 = 2 bit predictor)\n");
    exit(0); 
  }

  if ((trace_view_on != 0) && (trace_view_on != 1)) {
    printf("\n Traceview selector invalid. [0 = off, 1 = on]\n");
    exit(0);
  }

  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();

  //structs for storing the instructions in each stage
  struct trace_item if1;
  struct trace_item if2;
  struct trace_item id;
  struct trace_item ex1;
  struct trace_item ex2;
  struct trace_item mem1;
  struct trace_item mem2;
  struct trace_item wb;
  struct trace_item nextInstruction;
  struct trace_item noOP;

//setting up the noOP 
  noOp.type = 0;
  noOP.sReg_a = 0;
  noOp.sReg_b = 0;
  noOp.dReg = 0;
  noOp.PC = 0;
  noOp.Addr = 0;

  while(1) {
    size = trace_get_item(&tr_entry);
    
    if (!size) {       /* no more instructions (trace_items) to simulate */
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      break;
    }
    else{              /* parse the next instruction to simulate */

      //logic for detecting hazards
      if ((wb->dReg == id->sReg_a) || (wb->dReg == id->sReg_b)) {
        //stall IF1/IF2/ID
        
      } 
      cycle_number++;
      t_type = tr_entry->type;
      t_sReg_a = tr_entry->sReg_a;
      t_sReg_b = tr_entry->sReg_b;
      t_dReg = tr_entry->dReg;
      t_PC = tr_entry->PC;
      t_Addr = tr_entry->Addr;
    }      

// SIMULATION OF A SINGLE CYCLE cpu IS TRIVIAL - EACH INSTRUCTION IS EXECUTED
// IN ONE CYCLE

    if (trace_view_on) {/* print the executed instruction if trace_view_on=1 */
      switch(tr_entry->type) {
        case ti_NOP:
          printf("[cycle %d] NOP:",cycle_number) ;
          break;
        case ti_RTYPE:
          printf("[cycle %d] RTYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
          break;
        case ti_ITYPE:
          printf("[cycle %d] ITYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
          break;
        case ti_LOAD:
          printf("[cycle %d] LOAD:",cycle_number) ;      
          printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
          break;
        case ti_STORE:
          printf("[cycle %d] STORE:",cycle_number) ;      
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
        case ti_BRANCH:
          printf("[cycle %d] BRANCH:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
        case ti_JTYPE:
          printf("[cycle %d] JTYPE:",cycle_number) ;
          printf(" (PC: %x)(addr: %x)\n", tr_entry->PC,tr_entry->Addr);
          break;
        case ti_SPECIAL:
          printf("[cycle %d] SPECIAL:",cycle_number) ;      	
          break;
        case ti_JRTYPE:
          printf("[cycle %d] JRTYPE:",cycle_number) ;
          printf(" (PC: %x) (sReg_a: %d)(addr: %x)\n", tr_entry->PC, tr_entry->dReg, tr_entry->Addr);
          break;
      }
    }
  }

  trace_uninit();

  exit(0);
}

