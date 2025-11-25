
// scheduler.cpp
#include <iostream>
#include <pthread.h>
#include <thread>
#include <vector>
#include <queue>
#include <chrono>
#include <mutex>
#include <fstream>
#include <cmath>

using namespace std;

#pragma pack(push, 1)
struct Process {
    char processName[32];   
    int processId;          
    char activityStatus;    
    int cpuBurst;           
    int baseRegister;       
    int64_t limitRegister;  
    char processType;       
    int numberFiles;        
    char priority;          
    int checksum;           
};
#pragma pack(pop)

struct CompareByCpuBurst {
    bool operator()(const Process& p1, const Process& p2) const {
        return p1.cpuBurst > p2.cpuBurst;
    }
};

struct CompareByPriority {
    bool operator()(const Process& p1, const Process& p2) const {
        return p1.priority > p2.priority;
    }
};

struct ProcessorInfo {
    int processorIndex;
    int algorithm; // 0=FCFS,1=RoundRobin,2=SJF,3=Priority
    queue<Process> readyQueue;
};

// Global Variables
vector<Process> processes;
vector<ProcessorInfo> processors;
pthread_mutex_t readyQueueMutex;
int globalProcess = 0;
bool terminateFlag = false;

// Thread-safe execution function with neat output
Process executeProcess(Process process, int algorithm, int processorIndex, int timeQuant) {
    int executed = (process.cpuBurst > timeQuant) ? timeQuant : process.cpuBurst;
    process.cpuBurst -= executed;
    this_thread::sleep_for(chrono::milliseconds(10));

    const char* algoName = "";
    switch(algorithm){
        case 0: algoName = "FCFS"; break;
        case 1: algoName = "RR"; break;
        case 2: algoName = "SJF"; break;
        case 3: algoName = "Priority"; break;
    }

    if (algorithm == 3) { // Priority scheduling
        printf("Processor_%-2d | %-8s | PID: %-3d | %-10s | Exec: %-3d | Left: %-3d | Pri: %-3d\n",
               processorIndex, algoName, process.processId, process.processName,
               executed, process.cpuBurst, process.priority);
    } else {
        printf("Processor_%-2d | %-8s | PID: %-3d | %-10s | Exec: %-3d | Left: %-3d\n",
               processorIndex, algoName, process.processId, process.processName,
               executed, process.cpuBurst);
    }

    return process;
}

// SJF sort
void sortSJF(queue<Process> &q){
    priority_queue<Process, vector<Process>, CompareByCpuBurst> pq;
    while(!q.empty()){ pq.push(q.front()); q.pop(); }
    while(!pq.empty()){ q.push(pq.top()); pq.pop(); }
}

// Priority sort
void sortPriority(queue<Process> &q){
    priority_queue<Process, vector<Process>, CompareByPriority> pq;
    while(!q.empty()){ pq.push(q.front()); q.pop(); }
    while(!pq.empty()){ q.push(pq.top()); pq.pop(); }
}

// FCFS
void* fcfsScheduling(void* arg){
    ProcessorInfo* proc = (ProcessorInfo*)arg;
    int processorIndex = proc->processorIndex;
    int algorithm = 0, timeQuant = 2;

    while(!terminateFlag){
        pthread_mutex_lock(&readyQueueMutex);
        if(proc->readyQueue.empty()){ pthread_mutex_unlock(&readyQueueMutex); this_thread::sleep_for(chrono::milliseconds(500)); continue; }
        Process process = proc->readyQueue.front(); proc->readyQueue.pop();
        pthread_mutex_unlock(&readyQueueMutex);

        process = executeProcess(process, algorithm, processorIndex, process.cpuBurst);
        pthread_mutex_lock(&readyQueueMutex); globalProcess--; pthread_mutex_unlock(&readyQueueMutex);
    }
    return nullptr;
}

// Round Robin
void* roundRobinScheduling(void* arg){
    ProcessorInfo* proc = (ProcessorInfo*)arg;
    int processorIndex = proc->processorIndex;
    int algorithm = 1, timeQuant = 2;

    while(!terminateFlag){
        pthread_mutex_lock(&readyQueueMutex);
        if(proc->readyQueue.empty()){ pthread_mutex_unlock(&readyQueueMutex); this_thread::sleep_for(chrono::milliseconds(500)); continue; }
        Process process = proc->readyQueue.front(); proc->readyQueue.pop();
        pthread_mutex_unlock(&readyQueueMutex);

        if(process.cpuBurst > timeQuant){
            process = executeProcess(process, algorithm, processorIndex, timeQuant);
            pthread_mutex_lock(&readyQueueMutex); proc->readyQueue.push(process); pthread_mutex_unlock(&readyQueueMutex);
        } else {
            process = executeProcess(process, algorithm, processorIndex, process.cpuBurst);
            pthread_mutex_lock(&readyQueueMutex); globalProcess--; pthread_mutex_unlock(&readyQueueMutex);
        }
    }
    return nullptr;
}

// SJF
void* sjfScheduling(void* arg){
    ProcessorInfo* proc = (ProcessorInfo*)arg;
    int processorIndex = proc->processorIndex;
    int algorithm = 2, timeQuant = 2;

    while(!terminateFlag){
        pthread_mutex_lock(&readyQueueMutex);
        if(proc->readyQueue.empty()){ pthread_mutex_unlock(&readyQueueMutex); this_thread::sleep_for(chrono::milliseconds(500)); continue; }
        sortSJF(proc->readyQueue);
        Process process = proc->readyQueue.front(); proc->readyQueue.pop();
        pthread_mutex_unlock(&readyQueueMutex);

        while(process.cpuBurst > timeQuant){ process = executeProcess(process, algorithm, processorIndex, timeQuant); }
        process = executeProcess(process, algorithm, processorIndex, process.cpuBurst);
        pthread_mutex_lock(&readyQueueMutex); globalProcess--; pthread_mutex_unlock(&readyQueueMutex);
    }
    return nullptr;
}

// Priority Scheduling
void* priorityScheduling(void* arg){
    ProcessorInfo* proc = (ProcessorInfo*)arg;
    int processorIndex = proc->processorIndex;
    int algorithm = 3, timeQuant = 2;

    while(!terminateFlag){
        pthread_mutex_lock(&readyQueueMutex);
        if(proc->readyQueue.empty()){ pthread_mutex_unlock(&readyQueueMutex); this_thread::sleep_for(chrono::milliseconds(500)); continue; }
        sortPriority(proc->readyQueue);
        Process process = proc->readyQueue.front(); proc->readyQueue.pop();
        pthread_mutex_unlock(&readyQueueMutex);

        while(process.cpuBurst > timeQuant){ process = executeProcess(process, algorithm, processorIndex, timeQuant); }
        process = executeProcess(process, algorithm, processorIndex, process.cpuBurst);
        pthread_mutex_lock(&readyQueueMutex); globalProcess--; pthread_mutex_unlock(&readyQueueMutex);
    }
    return nullptr;
}

// Controller Thread
void* controllerThread(void* arg){
    vector<pthread_t> threads(processors.size());
    for(size_t i=0;i<processors.size();i++){
        ProcessorInfo* proc = &processors[i];
        int algo = proc->algorithm;
        if(algo==0) pthread_create(&threads[i], nullptr, fcfsScheduling, proc);
        else if(algo==1) pthread_create(&threads[i], nullptr, roundRobinScheduling, proc);
        else if(algo==2) pthread_create(&threads[i], nullptr, sjfScheduling, proc);
        else if(algo==3) pthread_create(&threads[i], nullptr, priorityScheduling, proc);
    }

    while(globalProcess > 0){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    terminateFlag = true;

    for(auto &t: threads) pthread_join(t,nullptr);
    return nullptr;
}

// Read binary process file
vector<Process> readProcesses(const string& filename){
    ifstream file(filename, ios::binary | ios::ate);
    vector<Process> data;
    if(!file){ cerr<<"Cannot open file "<<filename<<endl; exit(-1); }
    streamsize size = file.tellg();
    if(size % sizeof(Process) != 0){ cerr<<"Invalid file size\n"; exit(-1); }
    file.seekg(0, ios::beg);

    data.resize(size / sizeof(Process));
    if(!file.read(reinterpret_cast<char*>(data.data()), size)){ cerr<<"Failed to read file\n"; exit(-1); }
    return data;
}

int main(int argc,char* argv[]){
    if(argc < 4){ cout<<"Usage: "<<argv[0]<<" <dataFile> <algorithm load pairs>\n"; return -1; }

    vector<double> loads;
    int processorCount = 1;
    for(int i=2;i<argc;i+=2){
        ProcessorInfo p;
        p.processorIndex = processorCount++;
        p.algorithm = stoi(argv[i]);
        loads.push_back(stod(argv[i+1]));
        processors.push_back(p);
    }

    processes = readProcesses(argv[1]);
    globalProcess = (int)processes.back().processId + 1;

    // Assign processes to ready queues
    vector<int> counts(processors.size());
    int remaining = globalProcess;
    for(size_t i=0;i<processors.size();i++){
        counts[i] = int(globalProcess * loads[i]);
        remaining -= counts[i];
    }
    for(int i=0;i<remaining;i++) counts[i % processors.size()]++;

    int idx = 0;
    for(size_t i=0;i<processors.size();i++){
        for(int j=0;j<counts[i];j++) processors[i].readyQueue.push(processes[idx++]);
    }

    pthread_t ctrlThread;
    pthread_create(&ctrlThread, nullptr, controllerThread, nullptr);
    pthread_join(ctrlThread, nullptr);

    cout<<"All processes completed.\n";
    return 0;
}

