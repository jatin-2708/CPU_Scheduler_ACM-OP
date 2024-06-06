#include <bits/stdc++.h>
using namespace std;

struct Task
{
    int id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
};

void FCFS_scheduling(vector<Task> &tasks, const int &num_tasks);

void SJF_scheduling(vector<Task> &tasks, const int &num_tasks);

void P_scheduling(vector<Task> &tasks, const int &num_tasks);

void RR_scheduling(vector<Task> &tasks, const int &num_tasks, const int &time_quantum);

int main()
{
    int num_tasks;
    cout << "Enter the number of tasks: ";
    cin >> num_tasks;

    vector<Task> tasks(num_tasks);
    cout << "Considering high no. represent highest Priority\n";
    cout << "Enter task details (arrival time, burst time, priority):\n";
    for (int i = 0; i < num_tasks; ++i)
    {
        tasks[i].id = i + 1;
        cout << "Task " << tasks[i].id << ": ";
        cin >> tasks[i].arrival_time >> tasks[i].burst_time >> tasks[i].priority;
        tasks[i].remaining_time = tasks[i].burst_time;
    }

    cout << "Select the scheduling algorithm:\n"
         << "1. First Come First Served Scheduling Algorithm\n"
         << "2. Shortest Job First Preemptive Scheduling Algorithm\n"
         << "3. Priority Preemptive Scheduling Algorithm\n"
         << "4. Round Robin Scheduling Algorithm\n\n";

    int algo_select;
    cin >> algo_select;
    if (algo_select == 1)
    {
        FCFS_scheduling(tasks, num_tasks);
    }
    else if (algo_select == 2)
    {
        SJF_scheduling(tasks, num_tasks);
    }
    else if (algo_select == 3)
    {
        P_scheduling(tasks, num_tasks);
    }
    else if (algo_select == 4)
    {
        int time_quantum;
        cout << "\nEnter the time quantum for Round Robin scheduling: ";
        cin >> time_quantum;
        RR_scheduling(tasks, num_tasks, time_quantum);
    }
    else
    {
        cout << "Invalid Input\n";
    }
    cout << "Press any key to continue...";
    cin.get(); // Wait for user to press a key

    return 0;
}

void FCFS_scheduling(vector<Task> &tasks, const int &num_tasks)
{
    // Sort tasks by arrival time
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         { return a.arrival_time < b.arrival_time; });

    int execution_lower[num_tasks], execution_higher[num_tasks], waiting[num_tasks], turnaround[num_tasks];

    cout << "\nFCFS Scheduling:\n\n";
    int current_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;

    for (const Task &task : tasks)
    {
        // Wait if the task hasn't arrived yet
        if (current_time < task.arrival_time)
        {
            current_time = task.arrival_time;
        }

        // Execute the task
        execution_lower[task.id - 1] = current_time;
        current_time += task.burst_time;
        execution_higher[task.id - 1] = current_time;
        turnaround[task.id - 1] = current_time - task.arrival_time;
        waiting[task.id - 1] = turnaround[task.id - 1] - task.burst_time;
        total_turnaround_time += turnaround[task.id - 1];
        total_waiting_time += waiting[task.id - 1];
    }

    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         { return a.id < b.id; });

    for (const Task &task : tasks)
    {
        cout << "Task " << task.id << " executed during time " << execution_lower[task.id - 1];
        cout << " to " << execution_higher[task.id - 1] << " unit" << endl;
        cout << "Turn Around Time for Task " << task.id << " is " << turnaround[task.id - 1] << " unit" << endl;
        cout << "Waiting Time for Task " << task.id << " is " << waiting[task.id - 1] << " unit\n"
             << endl;
    }

    double average_waiting_time = static_cast<double>(total_waiting_time) / num_tasks;
    double average_turnaround_time = static_cast<double>(total_turnaround_time) / num_tasks;

    cout << "Average Turn Around Time: " << average_turnaround_time << " unit" << endl;
    cout << "Average Waiting Time: " << average_waiting_time << " unit" << endl;
}

void SJF_scheduling(vector<Task> &tasks, const int &num_tasks)
{
    //Considering this algo to be Preemptive  
    int bursttime[num_tasks];
    for (int i = 0; i < num_tasks; i++)
    {
        bursttime[tasks[i].id - 1] = tasks[i].burst_time;
    }
    // Sort tasks primarily by arrival time, secondarily by burst time
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         {
        if (a.arrival_time == b.arrival_time) {
            return a.burst_time < b.burst_time;
        }
        return a.arrival_time < b.arrival_time; });

    vector<vector<int>> execution(num_tasks);
    cout << "\nSJF Scheduling:\n"
         << endl;
    int current_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int tasks_completed = 0;

    while (tasks_completed < num_tasks)
    {
        int shortest_job_index = -1;
        for (int i = 0; i < num_tasks; ++i)
        {
            if (tasks[i].arrival_time <= current_time && tasks[i].burst_time > 0)
            {
                if (shortest_job_index == -1 || tasks[i].burst_time < tasks[shortest_job_index].burst_time)
                {
                    shortest_job_index = i;
                }
            }
        }

        if (shortest_job_index == -1)
        {
            // No task is ready to execute, move to the next available task's arrival time
            current_time++;
            continue;
        }

        Task &task = tasks[shortest_job_index];
        task.burst_time--;
        execution[task.id - 1].push_back(current_time);

        if (task.burst_time == 0)
        {
            tasks_completed++;
        }

        current_time++;
    }

    // Sort tasks back by their IDs for output consistency
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         { return a.id < b.id; });

    // Print the execution times for each task
    for (const Task &task : tasks)
    {
        cout << "Task " << task.id << " executed during time ";
        if (execution[task.id - 1].empty())
        {
            cout << "no time";
        }
        else
        {
            int start = execution[task.id - 1][0], end = execution[task.id - 1][0];
            for (size_t i = 1; i < execution[task.id - 1].size(); ++i)
            {
                if (execution[task.id - 1][i] == execution[task.id - 1][i - 1] + 1)
                {
                    end = execution[task.id - 1][i];
                }
                else
                {
                    if (start == end)
                    {
                        cout << start << " to " << start + 1;
                    }
                    else
                    {
                        cout << start << " to " << end + 1;
                    }
                    cout << ", ";
                    start = end = execution[task.id - 1][i];
                }
            }
            int completion_time = 0;
            if (start == end)
            {
                cout << start << " to " << start + 1 << " unit";
                completion_time = start + 1;
            }
            else
            {
                cout << start << " to " << end + 1 << " unit";
                completion_time = end + 1;
            }
            int turnaroundtime = completion_time - task.arrival_time;
            int waitingtime = turnaroundtime - bursttime[task.id - 1];
            cout << "\nTurn Around Time for Task " << task.id << " is " << turnaroundtime << " unit" << endl;
            cout << "Waiting Time for Task " << task.id << " is " << waitingtime << " unit"
                 << endl;
            total_waiting_time += waitingtime;
            total_turnaround_time += turnaroundtime;
        }
        cout << endl;
    }

    double average_waiting_time = static_cast<double>(total_waiting_time) / num_tasks;
    double average_turnaround_time = static_cast<double>(total_turnaround_time) / num_tasks;

    cout << "Average Turnaround Time: " << average_turnaround_time << " units" << endl;
    cout << "Average Waiting Time: " << average_waiting_time << " units" << endl;
}

void P_scheduling(vector<Task> &tasks, const int &num_tasks)
{
    // Considering this algo to be Preemptive
    int bursttime[num_tasks];
    for (int i = 0; i < num_tasks; i++)
    {
        bursttime[tasks[i].id - 1] = tasks[i].burst_time;
    }
    // Sort tasks primarily by arrival time, secondarily by burst time
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         {
        if (a.arrival_time == b.arrival_time) {
            return a.priority > b.priority;
        }
        return a.arrival_time < b.arrival_time; });

    vector<vector<int>> execution(num_tasks);
    cout << "\nPriority Scheduling:\n"
         << endl;
    int current_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int tasks_completed = 0;

    while (tasks_completed < num_tasks)
    {
        int highest_priority_index = -1;
        for (int i = 0; i < num_tasks; ++i)
        {
            if (tasks[i].arrival_time <= current_time && tasks[i].burst_time > 0)
            {
                if (highest_priority_index == -1 || tasks[i].priority > tasks[highest_priority_index].priority)
                {
                    highest_priority_index = i;
                }
            }
        }

        if (highest_priority_index == -1)
        {
            // No task is ready to execute, move to the next available task's arrival time
            current_time++;
            continue;
        }

        Task &task = tasks[highest_priority_index];
        task.burst_time--;
        execution[task.id - 1].push_back(current_time);

        if (task.burst_time == 0)
        {
            tasks_completed++;
        }

        current_time++;
    }

    // Sort tasks back by their IDs for output consistency
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         { return a.id < b.id; });

    // Print the execution times for each task
    for (const Task &task : tasks)
    {
        cout << "Task " << task.id << " executed during time ";
        if (execution[task.id - 1].empty())
        {
            cout << "no time";
        }
        else
        {
            int start = execution[task.id - 1][0], end = execution[task.id - 1][0];
            for (size_t i = 1; i < execution[task.id - 1].size(); ++i)
            {
                if (execution[task.id - 1][i] == execution[task.id - 1][i - 1] + 1)
                {
                    end = execution[task.id - 1][i];
                }
                else
                {
                    if (start == end)
                    {
                        cout << start << " to " << start + 1;
                    }
                    else
                    {
                        cout << start << " to " << end + 1;
                    }
                    cout << ", ";
                    start = end = execution[task.id - 1][i];
                }
            }
            int completion_time = 0;
            if (start == end)
            {
                cout << start << " to " << start + 1 << " unit";
                completion_time = start + 1;
            }
            else
            {
                cout << start << " to " << end + 1 << " unit";
                completion_time = end + 1;
            }
            int turnaroundtime = completion_time - task.arrival_time;
            int waitingtime = turnaroundtime - bursttime[task.id - 1];
            cout << "\nTurn Around Time for Task " << task.id << " is " << turnaroundtime << " unit" << endl;
            cout << "Waiting Time for Task " << task.id << " is " << waitingtime << " unit"
                 << endl;
            total_waiting_time += waitingtime;
            total_turnaround_time += turnaroundtime;
        }
        cout << endl;
    }

    double average_waiting_time = static_cast<double>(total_waiting_time) / num_tasks;
    double average_turnaround_time = static_cast<double>(total_turnaround_time) / num_tasks;

    cout << "Average Turnaround Time: " << average_turnaround_time << " units" << endl;
    cout << "Average Waiting Time: " << average_waiting_time << " units" << endl;
}

void RR_scheduling(vector<Task> &tasks, const int &num_tasks, const int &time_quantum)
{
    queue<Task *> task_queue;
    int current_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int tasks_completed = 0;
    cout << "\nRound Robin Scheduling:\n"
         << endl;

    vector<pair<int, int>> execution[num_tasks];

    // Sort tasks by arrival time
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         { return a.arrival_time < b.arrival_time; });

    // Add initial tasks to the queue
    auto task_it = tasks.begin();
    while (task_it != tasks.end() && task_it->arrival_time <= current_time)
    {
        task_queue.push(&(*task_it));
        task_it++;
    }

    while (tasks_completed < num_tasks)
    {
        if (!task_queue.empty())
        {
            Task *task = task_queue.front();
            task_queue.pop();

            int exec_time = min(time_quantum, task->remaining_time);

            execution[task->id - 1].emplace_back(current_time, current_time + exec_time);
            task->remaining_time -= exec_time;
            current_time += exec_time;

            // Add new tasks to the queue that have arrived
            while (task_it != tasks.end() && task_it->arrival_time <= current_time)
            {
                task_queue.push(&(*task_it));
                task_it++;
            }

            if (task->remaining_time > 0)
            {
                task_queue.push(task);
            }
            else
            {
                int turnaround_time = current_time - task->arrival_time;
                int waiting_time = turnaround_time - task->burst_time;

                total_turnaround_time += turnaround_time;
                total_waiting_time += waiting_time;
                tasks_completed++;
            }
        }
        else
        {
            current_time++;
        }
    }
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b)
         { return a.id < b.id; });

    for (const Task &task : tasks)
    {
        cout << "Task " << task.id << " executed during time ";
        for (const auto &period : execution[task.id - 1])
        {
            cout << period.first << " to " << period.second << (period == execution[task.id - 1].back() ? " unit" : ", ");
        }
        cout << endl;
        int turnaround_time = execution[task.id - 1].back().second - task.arrival_time;
        int waiting_time = turnaround_time - task.burst_time;
        cout << "Turn Around Time for Task " << task.id << " is " << turnaround_time << " unit" << endl;
        cout << "Waiting Time for Task " << task.id << " is " << waiting_time << " unit\n"
             << endl;
    }

    double average_waiting_time = static_cast<double>(total_waiting_time) / num_tasks;
    double average_turnaround_time = static_cast<double>(total_turnaround_time) / num_tasks;
    cout << "\nAverage Turnaround Time: " << average_turnaround_time << " unit" << endl;
    cout << "Average Waiting Time: " << average_waiting_time << " unit" << endl;
}