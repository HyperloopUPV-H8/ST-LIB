release: minor
summary: fix remaining scheduler race conditions and add warning when tasks are not ran in time

The mechanism for checking if tasks are not ran in time is very simple but that also means the scheduler can only know if a task has not been called when the waiting time runs out for the second time.
This means you will know if you're too slow to execute the task in less than 2x its period but not if you're between 1 and 2x its period.
