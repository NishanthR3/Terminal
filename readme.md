run shell: ./shell

1) I have implemented semi colon seperated execution of commands.
2) I have taken care of null inputs in shell.
3) In echo imlementation I have removed double qoutes and printed the string. If
there was no string I have printed error statement.
4)I have used /proc/pid/stat for process states ,/proc/pid/statm for prorcess virtual
memory and /proc/pid/exec for getting location of process.
5) I have used signals to track children death.And I am killing all children when I
type ctrl + c except for parent.
6) I have used sleep and echo to implement remindme.
7) To exit clock you have to type ctrl + c.
