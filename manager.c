//Maciej Andrearczyk, ma333856
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "err.h"

/*
 * Maksymalne ograniczenie na długość nazwy pliku.
 */
#define M 1000

/*
 * Maksymalne ograniczenie na długość jednej linii.
 */
#define MAX_LINE_SIZE 1000000

/*
 * Adres katalogu z plikami in out.
 */
char data_address[] = "./DATA/";

/*
 * Liczba wykonawców.
 */
int N;

/*
 * Nazwa pliku wejściowego.
 */
char input_file_name[M];

/*
 * Nazwa pliku wyjściowego.
 */
char output_file_name[M];

/*
 * Linia z pojedynczym wyrażeniem.
 */
char expr_line[MAX_LINE_SIZE];

/*
 * Plik z danymi.
 */
FILE *input_file;

/*
 * Plik wynikowy.
 */
FILE *output_file;

/*
 * Liczba wyrażeń do obliczenia.
 */
int expr_number;

/*
 * Deskryptory odpowiednio "pisze dziecko czyta ojciec"
 * oraz "pisze ojciec czyta dziecko".
 */
int pipe_ch_2_p[2], pipe_p_2_ch[2];

/*
 * Bufory pomocnicze.
 */
char in_buf[MAX_LINE_SIZE];
char out_buf[MAX_LINE_SIZE];

/*
 * Ilość obliczonych do tej pory wyrażeń.
 */
int calculated_exprs;

int is_calculated()
{
	int space_number = 0;
	for (int i = 0; in_buf[i] != '\0'; i++)
		if (in_buf[i] == ' ')
			space_number++;
	return (space_number == 1) ? 1 : 0;
}

/*
 * Manager forkuje się tworzy ścieżkę n elementową korzystając z fork,
 * w kazdym dziecku wywoluje exec z executorem.
 */
int main(int argc, char *argv[])
{
	if (argc != 4)
		fatal("Error, improper number of arguments\n");

	setvbuf(stdout, NULL, _IONBF, 0);
	N = atoi(argv[1]);

	if (pipe(pipe_ch_2_p) == -1)
		syserr("Error in pipe\n");

	if (pipe(pipe_p_2_ch) == -1)
		syserr("Error in pipe\n");

	if (close(0) == -1)
		syserr("Error, close(0)\n");
	if (close(1) == -1)
		syserr("Error, close(1)\n");
	switch (fork())
	{
		case -1:
			syserr("Error in main fork\n");
		case 0:
			if (close(pipe_ch_2_p[0]) == -1)
				syserr("Error, close pipe_ch_2_p[0]\n");
			if (close(pipe_p_2_ch[1]) == -1)
				syserr("Error, close pipe_p_2_ch[1]\n");
			if (dup(pipe_p_2_ch[0]) == -1)
				syserr("Error, dup pipe_p_2_ch[0]\n");
			if (close(pipe_p_2_ch[0]) == -1)
				syserr("Error, close pipe pipe_p_2_ch[0]\n");
			if (dup(pipe_ch_2_p[1]) == -1)
				syserr("Error, dup pipe_ch_2_p[1]\n");
			if (close(pipe_ch_2_p[1]) == -1)
				syserr("Error, close pipe_ch_2_p[1]\n");

			int pipe_dsc[2];
			N--;
			while(N > 0)
			{
				if (pipe(pipe_dsc) == -1)
					syserr("Error, pipe pipe_dsc");

				switch(fork())
				{
					case -1:
						syserr("Error in chain fork\n");
					case 0:
						if (close(pipe_dsc[1]) == -1)
							syserr("Error in close pipe_dsc[0]\n");
						if (close(0) == -1)
							syserr("Error in close stdin\n");
						if (dup(pipe_dsc[0]) == -1)
							syserr("Error in dup pipe_dsc[1]\n");
						if (close(pipe_dsc[0]) == -1)
							syserr("Error in close pipe_dsc[1]\n");
						N--;
						break;
					default:
						if (close(pipe_dsc[0]) == -1)
							syserr("Error in close pipe_dsc[1]\n");
						if (close(1) == -1)
							syserr("Error in close stdout\n");
						if (dup(pipe_dsc[1]) == -1)
							syserr("Error in dup pipe_dsc[0]\n");
						if (close(pipe_dsc[1]) == -1)
							syserr("Error in close pipe_dsc[0]\n");
						N = 0;
						break;	

				}
			}
			execl("./executor", "executor", 0); 

		default:	
			if (close(pipe_ch_2_p[1]) == -1)
				syserr("Error, close pipe_ch_2_p[1]\n");
			if (close(pipe_p_2_ch[0]) == -1)
				syserr("Error, close pipe_p_2_ch[0]\n");
			if (dup(pipe_ch_2_p[0]) == -1)
				syserr("Error, dup pipe_ch_2_p[0]\n");
			if (close(pipe_ch_2_p[0]) == -1)
				syserr("Error, close pipe_ch_2_p[0]\n");
			if (dup(pipe_p_2_ch[1]) == -1)
				syserr("Error, dup pipe_p_2_ch[1]\n");
			if (close(pipe_p_2_ch[1]) == -1)
				syserr("Error, close pipe_p_2_ch[1]\n");

			strcpy(input_file_name, data_address);
			strcpy(output_file_name, data_address);
			strcat(input_file_name, argv[2]);
			strcat(output_file_name, argv[3]);

			input_file = fopen(input_file_name, "r");

			if (input_file == NULL)
				syserr("Error while opening input file\n");

			output_file = fopen(output_file_name, "w");

			if (fgets(in_buf, MAX_LINE_SIZE, input_file) == NULL)
				syserr("Empty input file\n");
			expr_number = atoi(in_buf);

			int expr_counter = 0;
			for (int i = 0; i < N && fgets(in_buf, MAX_LINE_SIZE, input_file) != NULL; i++)
			{
				printf("%d: %s", expr_counter+1, in_buf);
				expr_counter++;
			}

			while(1)
			{
				fgets(in_buf, MAX_LINE_SIZE, stdin);
				if (is_calculated() == 1)
				{
					calculated_exprs++;
					fputs(in_buf, output_file);
					fflush(output_file);
					if (fgets(in_buf, MAX_LINE_SIZE, input_file) != NULL)
						printf("%d: %s", ++expr_counter, in_buf);
				}
				else
					printf("%s", in_buf);
				if (calculated_exprs == expr_number)
				{
					printf("#\n");
					break;
				}

			}

			fclose(input_file);
			fclose(output_file);
	}

	if (wait(0) == -1)
		syserr("Error in wait\n");

	exit(0);
}
