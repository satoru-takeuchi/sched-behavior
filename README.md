# sched-behavior

Observe process sheduler's behavior

# How it works

- Run multiple processes simultaneously in a CPU (or some CPUs)
- There are the following tree arguments

 - n: the number of processes run simultaneously
 - total: the number of time [usec] for each process consume
 - resol: capture data once per this time [usec]

- Each process works as follows

 - Exit after consuming "total."
 - Capture the following information once per "resol"

    - The ID corresponding to each process: from 0 to (n-1)
    - lapse time from the beginning of the process creation [usec]
    - progress(from 0 to 1) once per using "resol".

 - Show whole captured information at exit

# How to use

Run `make sched && ./capture` and analyze it as you like.

# An example of the result

Here is an example of the result captured in the author's environment.

## Hardware environment

- CPU: Intel(R) Core(TM) i5-2400 CPU @ 3.10GHz x 1 (4 core 1 thread)
- RAM: 8GB

## Software environment

- OS: The newest Debian GNU/Linux stretch at Feb 2 2017
- kernel: 4.8.0-1-amd64

## Test Pattern

Run the test program with the following arguments in a CPU
- n: 1, 2, 3, and 4
- total: 100000 (100ms)
- resol: 1000 (1ms)

It's just the ones writtein in [capture](https://github.com/satoru-takeuchi/sched-behavior/blob/master/capture)

## Graphs

There are two kinds of graphs for each concurrency.

- The ID of running process in a CPU

 - x-axis: lapse time from the process creation [us]
 - ID of the each processes (0-3)
 
- Progress of each processes

 - x-axis: the same as the above mentioned graph
 - y-axis: progress. 0 means not work at all yet. 1 means finished to work
 
### Concurrency 1 (just one process)

![p1-who-is-running](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p1-who-is-running.png)

---

![p1-progress](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p1-progress.png)

### Concurrency 2

![p2-who-is-running](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p2-who-is-running.png)

---

![p2-progress](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p2-progress.png)

### Concurrency 3

![p3-who-is-running](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p3-who-is-running.png)

---

![p3-progress](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p3-progress.png)

### Concurrency 4

![p4-who-is-running](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p4-who-is-running.png)

---

![p4-progress](https://github.com/satoru-takeuchi/sched-behavior/blob/master/images/p4-progress.png)
