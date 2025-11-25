#ifndef MYHEADER_H
#define MYHEADER_H
struct Process;
std::queue<Process> fcfsSchedule(queue < Process > *q){
	queue < Process > * que =q;
    int processorNum = (int) que -> size(), selfEx = 0;;
    //printf("\nshortest received %d processes\n", processorNum);
    std::priority_queue<Process, std::vector<Process>, CompareByCpuBurst> priorityQ;
    std::vector<Process> scheduledQueueSJF;
    queue<Process> orderedQ;
    Process shortestP=que->front();
//SORTING LOGIC
     while (!que->empty()) {
        Process currentProcess = que->front();
        que->pop();
        priorityQ.push(currentProcess);
    }

    // Dequeue processes from the readyQueue and enqueue them into the scheduledQueue
    while (!priorityQ.empty()) {
        Process currentProcess = priorityQ.top();
        priorityQ.pop();
        orderedQ->push(currentProcess);
        //printf("\n\tpushed process%d with %d", currentProcess.processId,currentProcess.cpuBurst);
        fflush(stdout);
    }
    return orderedQ;
}
#endif  // MYHEADER_H