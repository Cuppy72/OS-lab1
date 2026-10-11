#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>

#define BUFF_SIZE 256
#define INT_SIZE 16

void print_err(void)
{
	char err_message[] = "Ошибка, сумма чисел выходит за границу числовых значений INT\n";

	write(STDOUT_FILENO, err_message, sizeof(err_message));
}

int main(void)
{
	int total = 0;
	int current = 0;

	int bytes_read;

	bool negative = false;
	bool has_digits = false;

	int pos = 0;
	char buffer[BUFF_SIZE];

	while((bytes_read = read(STDIN_FILENO, buffer + pos, BUFF_SIZE - pos - 1)) > 0) {
		bytes_read += pos;

		for(int i = 0; i < bytes_read; i++) {
			if(isspace(buffer[i])) {
				if(has_digits) {
					if(negative) current = -current;
					
					if(current > 0 && total > INT_MAX - current) {
						print_err();
						return 1;
					} else if(current < 0 && total < INT_MIN - current) {
						print_err();
						return 1;
					}

					total += current;
					current = 0;

					negative = false;
					has_digits = false;
				}

				continue;
			}

			if(buffer[i] == '-') {
				negative = true;
				continue;
			}

			if(buffer[i] == '+') continue;

			if(isdigit(buffer[i])) {
				current *= 10;
				current += buffer[i] - '0';

				has_digits = true;
				continue;
			}
		}

		if(has_digits) {
			int tail_start = bytes_read - 1;

			while(tail_start > 0 && isdigit(buffer[tail_start]))
				tail_start--;

			if(tail_start > 0 && buffer[tail_start] == '-')
				tail_start--;

			pos = bytes_read - tail_start;
			for(int j = 0; j < pos; j++)
				buffer[j] = buffer[tail_start + j];

			current = 0;
			negative = false;
			has_digits = false;
		} else
			pos = 0;
	}

	if(has_digits) {
		if(negative) current = -current;
					
		if(current > 0 && total > INT_MAX - current) {
			print_err();
			return 1;
		} else if(current < 0 && total < INT_MIN - current) {
			print_err();
			return 1;
		}

		total += current;
	}

	int count_nums = 0;

	char tmp[INT_SIZE];
	char *ptr_tmp = tmp;

	negative = (total < 0) ? true : false;
	total = (total < 0) ? -total : total;

	if(total == 0) {
		*ptr_tmp++ = '0';
		count_nums = 1;
	} else {
		while(total > 0) {
			*ptr_tmp++ = '0' + total % 10;
			total /= 10;

			count_nums++;
		}
	}

	ptr_tmp--;
	
	char result[INT_SIZE];
	char *ptr_result = result;

	if(negative)
		*ptr_result++ = '-';

	for(int i = 0; i < count_nums; i++)
		*ptr_result++ = *ptr_tmp--;

	*ptr_result = '\n';

	write(STDOUT_FILENO, result, count_nums + 1);
	return 0;
}
