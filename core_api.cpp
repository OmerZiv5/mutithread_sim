/* 046267 Computer Architecture - HW #4 */

#include "core_api.h"
#include "sim_api.h"
#include <iostream>
#include <vector>
#include <stdio.h>

#define BLOCKED_VEC 1
#define FINEGRAINED_VEC 2

using namespace std;

double blocked_count = 0;
double fine_count = 0;
double blocked_cycles = 0;
double fine_cycles = 0;

class Thread{
public:
    tcontext reg_file;
    bool HALT;
    int thread_id;
    uint32_t line;
    int mem_cycles;

    Thread(int tid){
        for(int i = 0; i < REGS_COUNT; i++){
            reg_file.reg[i] = 0;
        }
        HALT = false;
        thread_id = tid;
        line = 0;
        mem_cycles = 0;
    }

    ~Thread(){}
};

vector<Thread> blocked_threads;
vector<Thread> fine_threads;

/*int load_latency = SIM_GetLoadLat();
int store_latency = SIM_GetStoreLat();
*/

void do_instruction(Instruction operation, int tid, int vec_type, int load_latency, int store_latency){
    if(vec_type == BLOCKED_VEC){
        if(operation.opcode == 0){
            // NOP
        }
        else if(operation.opcode == 1){
            // ADD
            int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
            int src2 = blocked_threads[tid].reg_file.reg[operation.src2_index_imm];
            blocked_threads[tid].reg_file.reg[operation.dst_index] = src1 + src2;
        }
        else if(operation.opcode == 2){
            // SUB
            int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
            int src2 = blocked_threads[tid].reg_file.reg[operation.src2_index_imm];
            blocked_threads[tid].reg_file.reg[operation.dst_index] = src1 - src2;
        }
        else if(operation.opcode == 3){
            // ADDI
            int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
            int imm = operation.src2_index_imm;
            blocked_threads[tid].reg_file.reg[operation.dst_index] = src1 + imm;
        }
        else if(operation.opcode == 4){
            // SUBI
            int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
            int imm = operation.src2_index_imm;
            blocked_threads[tid].reg_file.reg[operation.dst_index] = src1 - imm;
        }
        else if(operation.opcode == 5){
            // LOAD
            blocked_threads[tid].mem_cycles += load_latency;
            if(operation.isSrc2Imm){
                // 2nd argument is immediate
                uint32_t addr;
                int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
                int imm = operation.src2_index_imm;
                addr = src1 + imm;
                int32_t dst;

                SIM_MemDataRead(addr, &dst);
                blocked_threads[tid].reg_file.reg[operation.dst_index] = dst;
            }
            else{
                // 2nd argument is not immediate
                uint32_t addr;
                int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
                int src2 = blocked_threads[tid].reg_file.reg[operation.src2_index_imm];
                addr = src1 + src2;
                int32_t dst;

                SIM_MemDataRead(addr, &dst);
                blocked_threads[tid].reg_file.reg[operation.dst_index] = dst;
            }
        }
        else if(operation.opcode == 6){
            // STORE
            blocked_threads[tid].mem_cycles += store_latency;
            int src1 = blocked_threads[tid].reg_file.reg[operation.src1_index];
            int32_t dst = blocked_threads[tid].reg_file.reg[operation.dst_index];
            if(operation.isSrc2Imm){
                // 2nd argument is an immediate
                int imm = operation.src2_index_imm;
                uint32_t addr = dst + imm;
                SIM_MemDataWrite(addr, src1);
            }
            else{
                // 2nd argument is not an immediate
                int src2 = blocked_threads[tid].reg_file.reg[operation.src2_index_imm];
                uint32_t addr = dst + src2;
                SIM_MemDataWrite(addr, src1);
            }
        }
        else if(operation.opcode == 7){
            // HALT
            blocked_threads[tid].HALT = true;
        }
    }

    else{
        // vec_type = fine-grained

        if(operation.opcode == 0){
            // NOP
        }
        else if(operation.opcode == 1){
            // ADD
            int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
            int src2 = fine_threads[tid].reg_file.reg[operation.src2_index_imm];
            fine_threads[tid].reg_file.reg[operation.dst_index] = src1 + src2;
        }
        else if(operation.opcode == 2){
            // SUB
            int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
            int src2 = fine_threads[tid].reg_file.reg[operation.src2_index_imm];
            fine_threads[tid].reg_file.reg[operation.dst_index] = src1 - src2;
        }
        else if(operation.opcode == 3){
            // ADDI
            int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
            int imm = operation.src2_index_imm;
            fine_threads[tid].reg_file.reg[operation.dst_index] = src1 + imm;
        }
        else if(operation.opcode == 4){
            // SUBI
            int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
            int imm = operation.src2_index_imm;
            fine_threads[tid].reg_file.reg[operation.dst_index] = src1 - imm;
        }
        else if(operation.opcode == 5){
            // LOAD
            fine_threads[tid].mem_cycles += load_latency;
            if(operation.isSrc2Imm){
                // 2nd argument is immediate
                uint32_t addr;
                int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
                int imm = operation.src2_index_imm;
                addr = src1 + imm;
                int32_t dst;

                SIM_MemDataRead(addr, &dst);
                fine_threads[tid].reg_file.reg[operation.dst_index] = dst;
            }
            else{
                // 2nd argument is not immediate
                uint32_t addr;
                int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
                int src2 = fine_threads[tid].reg_file.reg[operation.src2_index_imm];
                addr = src1 + src2;
                int32_t dst;

                SIM_MemDataRead(addr, &dst);
                fine_threads[tid].reg_file.reg[operation.dst_index] = dst;
            }
        }
        else if(operation.opcode == 6){
            // STORE
            fine_threads[tid].mem_cycles += store_latency;
            int src1 = fine_threads[tid].reg_file.reg[operation.src1_index];
            int32_t dst = fine_threads[tid].reg_file.reg[operation.dst_index];
            if(operation.isSrc2Imm){
                // 2nd argument is an immediate
                int imm = operation.src2_index_imm;
                uint32_t addr = dst + imm;
                SIM_MemDataWrite(addr, src1);
            }
            else{
                // 2nd argument is not an immediate
                int src2 = fine_threads[tid].reg_file.reg[operation.src2_index_imm];
                uint32_t addr = dst + src2;
                SIM_MemDataWrite(addr, src1);
            }
        }
        else if(operation.opcode == 7){
            // HALT
            fine_threads[tid].HALT = true;
        }
    }
}


void CORE_BlockedMT(){
    Instruction* operation = (Instruction*)malloc(sizeof(Instruction));
    int halt_counter = 0;
    int curr_tid = 0;
    int num_threads = SIM_GetThreadsNum();
    int load_latency = SIM_GetLoadLat();
    int store_latency = SIM_GetStoreLat();
    // initializing threads vector
    for(int i = 0; i < num_threads; i++){
        Thread thread(i);
        blocked_threads.push_back(thread);
    }
    //get penalty for context switch
    int switch_penalty = SIM_GetSwitchCycles();

    while(1){
        if(halt_counter == num_threads){
            // all the threads are done
            break;
        }
        curr_tid = curr_tid % num_threads;
        int idle = 0;
        for(int k = 0; k < num_threads; k++){
            if(blocked_threads[k].HALT || blocked_threads[k].mem_cycles > 0){
                idle++;
            }
        }
        if((blocked_threads[curr_tid].HALT || blocked_threads[curr_tid].mem_cycles > 0) && idle != num_threads){
            // Need context switch
            // current thread is busy and not all the threads are busy, so you can CS to a non-busy thread
            blocked_cycles += switch_penalty;
            //cout << "CONTEXT SWITCH - blocked cycles += " << switch_penalty << endl;
            while(blocked_threads[curr_tid].HALT || blocked_threads[curr_tid].mem_cycles > 0){
                //this thread is done or still in memory - skip threads until we find one that is not busy
                // context switch
                curr_tid++;
                curr_tid = curr_tid % num_threads;
            }
            // Updating memory cycles
            for(int j = 0; j < num_threads; j++){
                if(blocked_threads[j].mem_cycles > 0){
                    blocked_threads[j].mem_cycles -= switch_penalty;
                }
                if(blocked_threads[j].mem_cycles < 0){
                    blocked_threads[j].mem_cycles = 0;
                }
            }
        }
        else{
            blocked_cycles++;
            if(idle != num_threads){
                while(blocked_threads[curr_tid].HALT || blocked_threads[curr_tid].mem_cycles > 0){
                    //this thread is done or still in memory - skip threads until we find one that is not busy
                    // context switch
                    curr_tid++;
                    curr_tid = curr_tid % num_threads;
                }

                uint32_t line = blocked_threads[curr_tid].line;
                SIM_MemInstRead(line, operation, curr_tid);
                do_instruction(*operation, curr_tid, BLOCKED_VEC, load_latency, store_latency);
                if(operation->opcode == 7){
                    halt_counter++;
                }
                blocked_count++;
                blocked_threads[curr_tid].line++;
                //cout << "COMMAND - " << operation->opcode << " blocked cycles ++"  << endl;
                // Updating memory cycles
                for(int j = 0; j < num_threads; j++){
                    if(blocked_threads[j].mem_cycles > 0 && j != curr_tid){
                        blocked_threads[j].mem_cycles--;
                    }
                }
            }
            else{
                // IDLE
                //cout << "IDLE - blocked cycles ++" << endl;
                // Updating memory cycles
                for(int j = 0; j < num_threads; j++){
                    if(blocked_threads[j].mem_cycles > 0){
                        blocked_threads[j].mem_cycles--;
                    }
                }
            }
        }
    }
    free(operation);
}

void CORE_FinegrainedMT(){
    Instruction* operation = (Instruction*) malloc(sizeof(Instruction));
    int halt_counter = 0;
    int curr_tid = 0;
    int num_threads = SIM_GetThreadsNum();
    int load_latency = SIM_GetLoadLat();
    int store_latency = SIM_GetStoreLat();
    // initializing threads vector
    for(int i = 0; i < num_threads; i++){
        Thread thread(i);
        fine_threads.push_back(thread);
    }

    while(1){
        if(halt_counter == num_threads){
            // all the threads are done
            break;
        }
        curr_tid = curr_tid % num_threads;
        int idle = 0;
        for(int k = 0; k < num_threads; k++){
            if(fine_threads[k].HALT || fine_threads[k].mem_cycles > 0){
                idle++;
            }
        }

        if(idle != num_threads){
            while(fine_threads[curr_tid].HALT || fine_threads[curr_tid].mem_cycles > 0){
                //this thread is done or still in memory - skip to the next thread
                curr_tid++;
                curr_tid = curr_tid % num_threads;
            }

            uint32_t line = fine_threads[curr_tid].line;
            SIM_MemInstRead(line, operation, curr_tid);
            do_instruction(*operation, curr_tid, FINEGRAINED_VEC, load_latency, store_latency);
            // Updating memory cycles
            for(int j = 0; j < num_threads; j++){
                if(fine_threads[j].mem_cycles > 0 && j != curr_tid){
                    fine_threads[j].mem_cycles--;
                }
            }
            if(operation->opcode == 7){
                halt_counter++;
            }
            fine_count++;
            fine_threads[curr_tid].line++;
            curr_tid++;
        }
        else{
            // Updating memory cycles
            for(int j = 0; j < num_threads; j++){
                if(fine_threads[j].mem_cycles > 0){
                    fine_threads[j].mem_cycles--;
                }
            }
        }
        fine_cycles++;
    }

    free(operation);
}

double CORE_BlockedMT_CPI(){
    double CPI = blocked_cycles/blocked_count;
    // Cleaning the vector
    blocked_threads.clear();
    return CPI;
}

double CORE_FinegrainedMT_CPI(){
    double CPI = fine_cycles/fine_count;
    // Cleaning the vector
    fine_threads.clear();
    return CPI;
}

void CORE_BlockedMT_CTX(tcontext* context, int threadid) {
    for(int i = 0; i < REGS_COUNT; i++){
        context[threadid].reg[i] = blocked_threads[threadid].reg_file.reg[i];
    }
}

void CORE_FinegrainedMT_CTX(tcontext* context, int threadid) {
    for(int i = 0; i < REGS_COUNT; i++){
        context[threadid].reg[i] = fine_threads[threadid].reg_file.reg[i];
    }
}
