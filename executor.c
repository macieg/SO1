//Maciej Andrearczyk, ma333856
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "err.h"
#define MAX_LINE_SIZE 1000000

/*
 * String z linjką wejściową.
 */
char in[MAX_LINE_SIZE];

/*
 * String z linijką wyjściową.
 */
char res[MAX_LINE_SIZE];

/*
 * Pomocnicza zmienna trzymająca potęgi 10.
 */
int p10;

/*
 * Sprawdza, czy znak jest operatorem.
 */
bool is_operand(int ind)
{
	return (in[ind] == '+' || 
			(in[ind] == '-' && (in[ind+1] == ' ' || in[ind+1] == '\n'))|| 
			in[ind] == '*' || 
			in[ind] == '/');
}

/*
 * Dodaje cyfrę do liczby.
 */
int add_digit(int j, int x)
{
	if (in[j] == '-')
		x *= -1;
	else
	{
		x += p10 * (in[j] -'0');
		p10 *= 10;
	}
	return x;
}

/*
 * Wykonuje odpowiednią operację wskazaną w in[j]
 * na a i b.
 */
int do_operation(int a, int b, int j)
{
	if (in[j] == '+')
		return a += b;
	if (in[j] == '-')
		return a -= b;
	if (in[j] == '*')
		return a *= b;	
	return a /= b; //in[i] == '/'
}

void clear_res(int n)
{
	for (int i = 0; i < n; i++)
		res[i] = '\0';
}

/*
 * Robi jeden krok obliczeń i zapisuje wynik w tabeli res.
 */
void one_step()
{
	int a = 0, b = 0;
	for (int i = 0; in[i] != '\0'; i++)
	{
		if (is_operand(i))
		{
			int j;

			p10 = 1;
			for (j = i-2; in[j] != ' '; j--)
				b = add_digit(j, b);

			p10 = 1;
			for (j--; in[j] != ' '; j--)
				a = add_digit(j, a);

			j--;

			int k;
			for (k = 0; k <= j; k++)
				res[k] = in[k];
			res[k] = '\0';
			k++;

			sprintf(res, "%s %d", res, do_operation(a, b, i));
			while (res[k] != '\0') k++;

			i++;
			for (; in[i] != '\0'; i++, k++)
				res[k] = in[i];
			res[k] = '\n';
			res[k+1] = '\0';
			return;	
		}
	}
	strcpy(res, in);
}

/*
 * Sprawdzam, czy nie powinienem się zakończyć. (# na wejściu)
 */
int check_if_finished()
{
	return (in[0] == '#') ? 1 : 0;
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	while(1)
	{
		fgets(in, MAX_LINE_SIZE, stdin);
		if (check_if_finished() == 1)
		{
			printf("#\n");
			break;
		}
		one_step();
		printf("%s", res);
	}

	wait(0);
	exit(0);
}
