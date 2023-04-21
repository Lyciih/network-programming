#include <network-programming.h>

int command_need_fork(char * command, size_t command_len, char * arg[], char * buffer, char * save_p, dllNode_t * count_list)
{
	char command_string[command_len];
	for(int i = 0; i < command_len; i++)
	{
		command_string[i] = *(command + i);
	}


		
	int pipe_p_c[2];
	pipe(pipe_p_c);

	int pipe_c_p[2];
	pipe(pipe_c_p);
	
	int child_pid;
	child_pid = fork();

	if(child_pid == -1)
	{
		perror("command fork error : ");
	}
	else if(child_pid == 0)
	{
		if(*save_p != '\0')
		{
			close(pipe_p_c[1]);
			dup2(pipe_p_c[0], STDIN_FILENO);
			close(pipe_p_c[0]);

			close(pipe_c_p[0]);
			dup2(pipe_c_p[1], STDOUT_FILENO);
			close(pipe_c_p[1]);

			execvp(command_string, arg);	
		}
		else
		{
			close(pipe_p_c[1]);
			dup2(pipe_p_c[0], STDIN_FILENO);
			close(pipe_p_c[0]);

			close(pipe_c_p[0]);
			close(pipe_c_p[1]);

			execvp(command_string, arg);	
		}
	}
	else
	{
		close(pipe_p_c[0]);

		number_temp * is_zero = count_list_check(count_list);
		if(is_zero != NULL)
		{
			write(pipe_p_c[1], is_zero->temp, strlen(is_zero->temp));

			DLL_delete(&(is_zero->node));
			free(is_zero->temp);
			free(is_zero);
		}
		else
		{
			write(pipe_p_c[1], buffer, strlen(buffer));
		}
		close(pipe_p_c[1]);

		wait(&child_pid);

		close(pipe_c_p[1]);


		if(*save_p != '\0')
		{
			read(pipe_c_p[0], buffer, 5000);
		}
		close(pipe_c_p[0]);
	}
	return 0;
}
				

