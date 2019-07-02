### CONT: program to execute a command continously.

This simple program executes another (indicated as parameters to the program)
continously (using a delay between executions) until a number of executions is
made or interrupted from the command line.

The program executes the required program and captures the output of that
program, counting the number of lines, and then waits for the timeout to
expire, executes an ANSI escape sequence to move the cursor as many lines
up as the last execution output, and runs again the program to overwrite
the last output it did.

It follows each line with an ANSI escape to erase from that point to the
end of the screen line.  After execution, it emits another ANSI escape to
erase from that line to the end of the screen.

In normal use, you should specify the command and command line arguments
at the end of the `cont` command, e.g.  To execute each half second a `ps`
command, or a clock using the `date(1)` command:

    $ cont -t 0.5 px x

or

    $ cont date "+%H:%M:%S"

In case you want to run a pipeline, run it using a shell, as in the following
example:

    cont -t 0.5 sh -c "ps x | sed '1s/.*/^[[1m&^[[m/'"

(that runs ps throug `sed(1)` to show the header line in bold ---the `^[` is
an ASCII ESC)
