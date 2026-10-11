#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	char filename[256];
	int pipefd[2];
	pid_t pid;
	
	char input_msg[] = "Введите имя файла:\n";
	write(STDOUT_FILENO, input_msg, sizeof(input_msg) - 1);
	
	int pos = 0;
	char ch;
	while (read(STDIN_FILENO, &ch, 1) > 0 && ch != '\n') {
		filename[pos++] = ch;
	}
	filename[pos] = '\0';
	
	int file_fd = open(filename, O_RDONLY);
	if (file_fd < 0) {
		write(STDOUT_FILENO, "Ошибка открытия файла\n", 23);
		return 1;
	}
	
	if (pipe(pipefd) < 0) {
		write(STDOUT_FILENO, "Ошибка создания pipe\n", 22);
		close(file_fd);
		return 1;
	}
	
	pid = fork();
	
	if (pid < 0) {
		write(STDOUT_FILENO, "Ошибка fork\n", 13);
		close(file_fd);
		close(pipefd[0]);
		close(pipefd[1]);
		return 1;
	}
	
	if (pid == 0) {
		close(pipefd[0]);
		
		dup2(file_fd, STDIN_FILENO);
		close(file_fd);
		
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		
		execl("./child", "child", NULL);
		
		write(STDOUT_FILENO, "Ошибка exec\n", 13);
		return 1;
	} else {
		close(pipefd[1]);
		close(file_fd);
		
		char buffer[1024];
		int bytes_read;
		
		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			write(STDOUT_FILENO, buffer, bytes_read);
		}
		
		close(pipefd[0]);
		
		int status;
		wait(&status);
	}
	
	return 0;
}
