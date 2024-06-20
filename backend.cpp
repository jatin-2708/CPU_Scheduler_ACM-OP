#include <iostream>
#include <chrono>
#include <cassert>
#include <sys/time.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <sstream>
using namespace std;
struct Process
{
    int id;
    int arrivalTime;
    int burstTime;
    int priority;
    int completionTime = 0;
    int waitingTime = 0;
    int turnAroundTime = 0;
    int startTime = 0;
};
class ComparePriority
{
public:
    bool operator()(const Process &p1, const Process &p2)
    {
        return p1.priority > p2.priority; // Higher priority value means lower actual priority
    }
};
const double TIME_CUTOFF = 1.0;
const int ITERATIONS = 1e6;

vector<Process> parseInput(const string &input)
{
    vector<Process> processes;
    stringstream ss(input);
    string processString;

    while (getline(ss, processString, ';'))
    {
        stringstream ps(processString);
        string token;
        Process process;

        getline(ps, token, ',');
        process.id = stoi(token);
        getline(ps, token, ',');
        process.arrivalTime = stoi(token);
        getline(ps, token, ',');
        process.burstTime = stoi(token);
        getline(ps, token, ',');
        process.priority = stoi(token);
        processes.push_back(process);
    }

    return processes;
}

void FCFS(vector<Process> &processes)
{
    int n = processes.size();
    int currentTime = 0;
    auto cmp = [&](Process P1, Process P2) -> bool
    {
        return P1.arrivalTime < P2.arrivalTime;
    };
    sort(processes.begin(), processes.end(), cmp);
    for (int i = 0; i < n; ++i)
    {
        if (currentTime < processes[i].arrivalTime)
        {
            currentTime = processes[i].arrivalTime;
        }

        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnAroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnAroundTime - processes[i].burstTime;

        currentTime += processes[i].burstTime;
    }
}
void HighestResponseRatioNext(vector<Process> &processes)
{
    int n = processes.size();
    int currentTime = 0;
    int completedCount = 0;

    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrivalTime < b.arrivalTime; });

    while (completedCount < n)
    {
        double maxResponseRatio = -1;
        int currentProcess = -1;

        for (int i = 0; i < n; ++i)
        {
            if (processes[i].arrivalTime <= currentTime && processes[i].completionTime == 0)
            {
                int waitingTime = currentTime - processes[i].arrivalTime;
                double responseRatio = (waitingTime + processes[i].burstTime) / (double)processes[i].burstTime;

                if (responseRatio > maxResponseRatio)
                {
                    maxResponseRatio = responseRatio;
                    currentProcess = i;
                }
            }
        }

        if (currentProcess == -1)
        {
            currentTime++;
            continue;
        }

        processes[currentProcess].startTime = currentTime;
        currentTime += processes[currentProcess].burstTime;
        processes[currentProcess].completionTime = currentTime;
        processes[currentProcess].turnAroundTime = processes[currentProcess].completionTime - processes[currentProcess].arrivalTime;
        processes[currentProcess].waitingTime = processes[currentProcess].turnAroundTime - processes[currentProcess].burstTime;

        completedCount++;
    }
}
const int QUANTUM = 2;
void roundRobin(vector<Process> &processes, int timeQuantum)
{
    queue<int> readyQueue;
    int currentTime = 0;
    vector<int> remainingTime(processes.size());
    vector<int> arrivalTimes(processes.size());

    for (size_t i = 0; i < processes.size(); ++i)
    {
        remainingTime[i] = processes[i].burstTime;
        arrivalTimes[i] = processes[i].arrivalTime;
    }

    int completed = 0;
    int n = processes.size();
    int currentProcess = 0;
    vector<int> lastExecutionTime(processes.size(), 0);

    while (completed != n)
    {
        bool done = true;

        for (size_t i = 0; i < processes.size(); ++i)
        {
            if (arrivalTimes[i] <= currentTime && remainingTime[i] > 0)
            {
                readyQueue.push(i);
                arrivalTimes[i] = 1e9; // Ensure it isn't added again
            }
        }

        if (!readyQueue.empty())
        {
            currentProcess = readyQueue.front();
            readyQueue.pop();

            if (remainingTime[currentProcess] > timeQuantum)
            {
                if (lastExecutionTime[currentProcess] == 0)
                {
                    processes[currentProcess].startTime = currentTime;
                }

                currentTime += timeQuantum;
                remainingTime[currentProcess] -= timeQuantum;
                lastExecutionTime[currentProcess] = currentTime;

                for (size_t i = 0; i < processes.size(); ++i)
                {
                    if (arrivalTimes[i] <= currentTime && remainingTime[i] > 0 && i != currentProcess)
                    {
                        readyQueue.push(i);
                        arrivalTimes[i] = 1e9; // Ensure it isn't added again
                    }
                }

                readyQueue.push(currentProcess);
            }
            else
            {
                if (lastExecutionTime[currentProcess] == 0)
                {
                    processes[currentProcess].startTime = currentTime;
                }

                currentTime += remainingTime[currentProcess];
                processes[currentProcess].completionTime = currentTime;
                processes[currentProcess].turnAroundTime = currentTime - processes[currentProcess].arrivalTime;
                processes[currentProcess].waitingTime = processes[currentProcess].turnAroundTime - processes[currentProcess].burstTime;
                remainingTime[currentProcess] = 0;
                completed++;
            }

            done = false;
        }

        if (done)
        {
            currentTime++;
        }
    }
}

void shortestJobFirst(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrivalTime < b.arrivalTime; });

    int currentTime = 0;
    vector<bool> vis(processes.size(), false);
    processes[0].startTime = processes[0].arrivalTime;
    int cnt = 0;
    size_t i = 0;

    while (cnt < processes.size())
    {
        if (vis[i])
        {
            i++;
            continue;
        }

        if (currentTime < processes[i].arrivalTime)
        {
            currentTime = processes[i].arrivalTime;
        }

        int idx = -1;
        int mn = 1e9;
        for (size_t j = 0; j < processes.size(); j++)
        {
            if (vis[j])
                continue;
            if (processes[j].arrivalTime <= currentTime)
            {
                if (processes[j].burstTime < mn)
                {
                    idx = j;
                    mn = processes[j].burstTime;
                }
            }
        }

        if (idx == -1)
        {
            currentTime++;
            continue;
        }

        processes[idx].startTime = currentTime;
        processes[idx].completionTime = currentTime + processes[idx].burstTime;
        processes[idx].turnAroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
        processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].burstTime;
        currentTime = processes[idx].completionTime;
        vis[idx] = true;
        cnt++;
        i = idx;
    }
}
void longestJobFirst(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrivalTime < b.arrivalTime; });

    int currentTime = 0;
    vector<bool> vis(processes.size(), false);
    processes[0].startTime = processes[0].arrivalTime;
    int cnt = 0;
    size_t i = 0;

    while (cnt < processes.size())
    {
        if (vis[i])
        {
            i++;
            continue;
        }

        if (currentTime < processes[i].arrivalTime)
        {
            currentTime = processes[i].arrivalTime;
        }

        int idx = -1;
        int mx = -1;
        for (size_t j = 0; j < processes.size(); j++)
        {
            if (vis[j])
                continue;
            if (processes[j].arrivalTime <= currentTime)
            {
                if (processes[j].burstTime > mx)
                {
                    idx = j;
                    mx = processes[j].burstTime;
                }
            }
        }

        if (idx == -1)
        {
            currentTime++;
            continue;
        }

        processes[idx].startTime = currentTime;
        processes[idx].completionTime = currentTime + processes[idx].burstTime;
        processes[idx].turnAroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
        processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].burstTime;
        currentTime = processes[idx].completionTime;
        vis[idx] = true;
        cnt++;
        i = idx;
    }
}

void priorityScheduling(vector<Process> &processes)
{
    assert(!processes.empty());

    auto cmpArrival = [](const Process &a, const Process &b)
    {
        return a.arrivalTime < b.arrivalTime;
    };

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), cmpArrival);

    int currentTime = 0;
    vector<Process> result;
    priority_queue<Process, vector<Process>, ComparePriority> pq;
    size_t index = 0;

    while (index < processes.size() || !pq.empty())
    {
        // Add all processes that have arrived by currentTime to the priority queue
        while (index < processes.size() && processes[index].arrivalTime <= currentTime)
        {
            pq.push(processes[index]);
            index++;
        }

        if (!pq.empty())
        { // O(nlogn) --> Fibbonacci Heaps
            Process currentProcess = pq.top();
            pq.pop();

            if (currentProcess.arrivalTime > currentTime)
            {
                currentTime = currentProcess.arrivalTime;
            }

            currentProcess.startTime = currentTime;
            currentProcess.waitingTime = currentProcess.startTime - currentProcess.arrivalTime;
            currentProcess.completionTime = currentTime + currentProcess.burstTime;
            currentProcess.turnAroundTime = currentProcess.completionTime - currentProcess.arrivalTime;

            currentTime = currentProcess.completionTime;
            result.push_back(currentProcess);
        }
        else
        {
            // If the queue is empty, move to the next process arrival time
            if (index < processes.size())
            {
                currentTime = processes[index].arrivalTime;
            }
        }
    }

    processes = result;
}
const int THRESHOLD = 15;
const int LOAD = 5;
void auto_schedule(vector<Process> &processes)
{
    int mx = 0;
    int mn = 1e7;
    for (auto it : processes)
    {
        mx = max(mx, it.priority);
        mn = min(mn, it.priority);
    }
    if (mx - mn > THRESHOLD)
    {
        priorityScheduling(processes);
        return;
    }
    // if arrival times are close or differ by small values
    mx = 0;
    mn = 1e7;
    for (auto it : processes)
    {
        mx = max(mx, it.arrivalTime);
        mn = min(mn, it.arrivalTime);
    }
    if (mx - mn <= 8)
    { // chosen after iterative testing (hbbr)
        shortestJobFirst(processes);
        return;
    }
    if (mx - mn > THRESHOLD + LOAD)
    {
        // avoid starvation use RR
        roundRobin(processes, QUANTUM);
        return;
    }
    // if all other conditions are false the easiest algorithm to implement will be FCFS
    FCFS(processes);
}
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: scheduler.exe <algorithm> <input>" << endl;
        return 1;
    }

    auto chrono_begin = chrono::steady_clock::now();
    string algorithm = argv[1];
    string input = argv[2];

    vector<Process> processes = parseInput(input);

    if (algorithm == "FCFS")
    {
        FCFS(processes);
    }
    else if (algorithm == "RR")
    {
        roundRobin(processes, QUANTUM);
    }
    else if (algorithm == "SJF")
    {
        shortestJobFirst(processes);
    }
    else if (algorithm == "LJF")
    {
        longestJobFirst(processes);
    }
    else if (algorithm == "Priority")
    {
        priorityScheduling(processes);
    }
    else if (algorithm == "HRRN")
    {
        HighestResponseRatioNext(processes);
    }
    else if (algorithm == "Auto")
    {
        auto_schedule(processes);
    }
    else
    {
        cerr << "Invalid algorithm choice\n";
        return 1;
    }

    double totalTAT = 0;
    double totalWT = 0;
    int n = processes.size();

    for (const auto &process : processes)
    {
        totalTAT += process.turnAroundTime;
        totalWT += process.waitingTime;
    }

    double taroundavg = totalTAT / n;
    double twaitavg = totalWT / n;

    cout << "{ \"processes\": [";
    for (size_t i = 0; i < processes.size(); ++i)
    {
        cout << "{"
             << "\"index\":" << processes[i].id << ","
             << "\"Tarrival\":" << processes[i].arrivalTime << ","
             << "\"Tburst\":" << processes[i].burstTime << ","
             << "\"Priority\":" << processes[i].priority << ","
             << "\"Tcompletion\":" << processes[i].completionTime << ","
             << "\"Twaiting\":" << processes[i].waitingTime << ","
             << "\"Tturnaround\":" << processes[i].turnAroundTime
             << "}";
        if (i < processes.size() - 1)
        {
            cout << ",";
        }
    }
    auto chrono_end = chrono::steady_clock::now();
    long double schedulingOverhead = 1e-12 * chrono::duration_cast<chrono::microseconds>(chrono_end - chrono_begin).count();
    cout << "],"
         << "\"taroundavg\": " << taroundavg << ","
         << "\"twaitavg\": " << twaitavg << ","
         << "\"schedulingOverhead\": " << schedulingOverhead << "}";
    cout.flush();
    return 0;
}
