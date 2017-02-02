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

    - process ID starting from 0
    - lapse time from the beginning of the process creation [usec]
    - progress(0-1) once per using "resol".

 - Show whole captured information at exit

# How to use

Run `make sched && ./capture` and analyze it as you like.
