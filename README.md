# Terminal

* I have implemented semi colon seperated execution of commands.
* I have taken care of null inputs in shell.
* In echo imlementation I have removed double qoutes and printed the string. If
there was no string I have printed error statement.
* I have used /proc/pid/stat for process states ,/proc/pid/statm for prorcess virtual
memory and /proc/pid/exec for getting location of process.
* I have used signals to track children death.And I am killing all children when I
type ctrl + c except for parent.
* I have used sleep and echo to implement remindme.
* To exit clock you have to type ctrl + c.

## Installation

Clone the project from [this](https://github.com/NishanthRachakonda/terminal) repository.
```bash
$ git clone https://github.com/NishanthRachakonda/terminal
```

## Setup

Setup using make.
```bash
$ make docker_build
```

## Usage

For starting terminal:
```bash
$ make docker_run
```

For cleaning:
```bash
$ make docker_clean
```

## Commands

bash commands work fine here too.

## License 

License via Apache License