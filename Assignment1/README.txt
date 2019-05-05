Yiliang Liu

When kapish startup, it will read the .kapishrc file from home directry. 
Then it will analysis and excute the command in .kapishrc and display it on the console.
Then the prompt will ask user to input their own command. If the user type CTRL+C.
The programm will shut down instantly. If the command is a kapish buit-in, it will
call on of the four built-in functions.
If the command is not a built-in, the program will fork a process and excute the command.
If the command cannot be found while being fork, CTRL+C will terminate the child process,
will keep the parent process running.
If the user use CTRL+C while in parent process, a segementation fault may be caused.
After the user terminate the unfinished child process through CTRL+C, the user can
immediatley type another command, and it will be excuted properly.

This assignment has done by myself. However, during the debug process, I have discussed
some posilibility that might caused the bug, and the potentail fix for it with Mike Zhang.