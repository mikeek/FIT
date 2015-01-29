/* santa.c
 * Reseni IOS-DU2, 30.3.2013
 * Autor: Michal Kozubik, FIT, xkozub03@stud.fit.vutbr.cz
 * Prelozeno: gcc 4.7.2
 * popis
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include "error.h"

#define FILE_OUT "santa.out"
#define PATH "/tmp"
#define ID 'X'
#define SEM_MEM    0
#define SEM_WORK   1
#define SEM_END    2
#define SEM_GOT    3
#define SEM_ELF    4
#define SEM_CHECK  5

#define NUM_SEMS   6

#define GROUP      3

#define MICROSEC   1000

enum {
	ERROR = -1, EOK, ERRIN, ERRSYS
};

// union pro semafory

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

// struktura, ktera bude sdilena napric procesy

typedef struct sharem {
	int A, C, H, I, NE, S;
	int sem_id;
	int at_once;
	int queue;
	FILE *f;
} sharemem;

int semap_ctl(int sem_id, int nsem, int elves);
int semap_wait(int sem_id, int nsem, int cnt);
int semap_post(int sem_id, int nsem, int cnt);
int elfJob(sharemem *sh);
int santaJob(sharemem *sem_id);
int str_tol(char *str);
void killMain(int x);
void killOthers(int x);

// funkce pro praci se semafory

int semap_ctl(int sem_id, int nsem, int elves)
{
	union semun semap;
	unsigned short values[nsem];
	values[SEM_MEM] = 1;
	values[SEM_WORK] = 0;
	values[SEM_END] = 1;
	values[SEM_GOT] = 0;
	values[SEM_ELF] = elves;
	values[SEM_CHECK] = 0;
	semap.array = values;
	return semctl(sem_id, 0, SETALL, semap);
}

int semap_wait(int sem_id, int nsem, int cnt)
{
	struct sembuf op[1];
	op[0].sem_num = nsem;
	op[0].sem_op = -cnt;
	op[0].sem_flg = 0;
	int ret;
	ret = semop(sem_id, op, 1);
	if (ret == -1) {
		kill(0, SIGINT);
	}
	return ret;
}

int semap_post(int sem_id, int nsem, int cnt)
{
	struct sembuf op[1];
	op[0].sem_num = nsem;
	op[0].sem_op = cnt;
	op[0].sem_flg = 0;
	int ret;
	ret = semop(sem_id, op, 1);
	if (ret == -1) {
		kill(0, SIGINT);
	}
	return ret;
}

int semap_deal(int sem_id)
{
	union semun ignored;
	return (semctl(sem_id, 1, IPC_RMID, ignored));
}

// ---------------------------------------------------------------------------
// Proces elfu
// ---------------------------------------------------------------------------

int elfJob(sharemem *sh)
{
	int C = sh->C;
	//     Vypsani zpravy o startu cinnosti elfa
	semap_wait(sh->sem_id, SEM_MEM, 1);
	int I = ++(sh->I);
	fprintf(sh->f, "%d: elf: %d: started\n", ++(sh->A), I);
	int H = sh->H;
	semap_post(sh->sem_id, SEM_MEM, 1);
	srand(getpid());

	//     Prace elfa
	do {
		usleep(rand() % (H + 1));
		semap_wait(sh->sem_id, SEM_MEM, 1);
		fprintf(sh->f, "%d: elf: %d: needed help\n", ++(sh->A), I);
		semap_post(sh->sem_id, SEM_MEM, 1);
		//         Zadost o pomoc        
		//         Vstup at_once elfu do zadatelny
		semap_wait(sh->sem_id, SEM_ELF, 1);
		semap_wait(sh->sem_id, SEM_MEM, 1);
		fprintf(sh->f, "%d: elf: %d: asked for help\n", ++(sh->A), I);
		//                 Pokud vsichni uvnitr "askli", tak posledni kontaktuje santu
		//                 (pusti mu semafor SEM_WORK)
		(sh->queue)++;
		if (sh->queue == sh->at_once) {
			sh->queue = 0;
			semap_post(sh->sem_id, SEM_WORK, 1);
		}
		semap_post(sh->sem_id, SEM_MEM, 1);
		//         Cekani na vyrizeni zadosti od santy
		semap_wait(sh->sem_id, SEM_GOT, 1);
		semap_wait(sh->sem_id, SEM_MEM, 1);
		fprintf(sh->f, "%d: elf: %d: got help\n", ++(sh->A), I);
		semap_post(sh->sem_id, SEM_MEM, 1);
		C--;
		if (C <= 0) {
			semap_wait(sh->sem_id, SEM_MEM, 1);
			fprintf(sh->f, "%d: elf: %d: got a vacation\n", ++(sh->A), I);
			(sh->NE)--;
			if (sh->NE <= 0) {
				semap_wait(sh->sem_id, SEM_END, 1);
			}
			semap_post(sh->sem_id, SEM_MEM, 1);
		}
		//         Nyni si santa muze "checknout", kolik elfu zbyva
		semap_wait(sh->sem_id, SEM_MEM, 1);
		(sh->queue)++;
		if (sh->queue == sh->at_once) {
			sh->queue = 0;
			semap_post(sh->sem_id, SEM_CHECK, 1);
		}
		semap_post(sh->sem_id, SEM_MEM, 1);
		//         Pote da vedet, ze si sravne nastavil (SEM_GOT) "at_once" pro dalsi navstevy
	} while (C > 0);
	//     Cekani na uvolneni ostatnich elfu
	//     = cekani az posledni elf, ktery jde na dovolenou nastavi semafor SEM_END na 0
	//            Vsichni jsou na dovolene
	semap_wait(sh->sem_id, SEM_END, 0);

	semap_wait(sh->sem_id, SEM_MEM, 1);
	fprintf(sh->f, "%d: elf: %d: finished\n", ++(sh->A), I);
	semap_post(sh->sem_id, SEM_MEM, 1);
	return 0;
}

// ---------------------------------------------------------------------------
// Proces santy
// ---------------------------------------------------------------------------

int santaJob(sharemem *sh)
{
	//     Zacatek santova procesu
	semap_wait(sh->sem_id, SEM_MEM, 1);
	fprintf(sh->f, "%d: santa: started\n", ++(sh->A));
	semap_post(sh->sem_id, SEM_MEM, 1);
	int S = sh->S;
	srand(getpid());
	int waiting;
	do {
		//         Cekani na pozadavek od elfu
		semap_wait(sh->sem_id, SEM_WORK, 1);
		waiting = sh->at_once;
		semap_wait(sh->sem_id, SEM_MEM, 1);
		fprintf(sh->f, "%d: santa: checked state: %d: %d\n", ++(sh->A), sh->NE, waiting);
		fprintf(sh->f, "%d: santa: can help\n", ++(sh->A));
		semap_post(sh->sem_id, SEM_MEM, 1);
		//         Prace
		usleep(rand() % (S + 1));
		//         Poslani infa o hotove praci
		semap_post(sh->sem_id, SEM_GOT, sh->at_once);
		//         Cekani nez si elfove nastavi svuj pocet (sh->NE)
		semap_wait(sh->sem_id, SEM_CHECK, 1);
		if (sh->NE <= 0) {
			waiting = 0;
		}
		if (sh->NE > GROUP) {
			sh->at_once = GROUP;
		} else {
			sh->at_once = 1;
		}
		semap_post(sh->sem_id, SEM_ELF, sh->at_once);
	} while (waiting != 0);
	//     Ukonceni procesu santy
	semap_wait(sh->sem_id, SEM_MEM, 1);
	fprintf(sh->f, "%d: santa: checked state: %d: %d\n", ++(sh->A), sh->NE, waiting);
	fprintf(sh->f, "%d: santa: finished\n", ++(sh->A));
	semap_post(sh->sem_id, SEM_MEM, 1);
	return 0;
}

int str_tol(char *str)
{
	int num;
	char *endptr = NULL;
	num = strtol(str, &endptr, 10);
	if (*endptr != '\0') {
		return ERROR;
	}
	return num;
}

void killOthers(int x)
{
	(void) x;
	exit(2);
}

/* C = pocet navstev skritka u santy, nez mu zacne dovolena (>0)
 * E = pocet skritku pracujicich ve vyrobe (>0)
 * H = max. doba (ms), nez se vyskytne problem a elf potrebuje pomoc (>=0)
 * S = max. doba (ms) obsluhy santou (delka trvani pomoci) (>=0)
 */
int main(int argc, char *argv[])
{
	if (argc != 5) {
		FatalError("spatny pocet parametru!");
	}
	//     Zpracovani parametru
	int C = str_tol(argv[1]);
	int E = str_tol(argv[2]);
	int H = str_tol(argv[3]);
	int S = str_tol(argv[4]);
	if ((C <= 0) || (E <= 0) || (H < 0) || (S < 0)) {
		FatalError("spatne zadane parametry");
	}
	//     Generovani klicu pro sdilenou pamet a semafory
	key_t addr;
	if ((addr = ftok(PATH, ID)) == (key_t) ERROR) {
		Error("chyba pri generovani klice");
		exit(ERRSYS);
	}
	key_t addr_sem;
	if ((addr_sem = ftok(PATH, ID + 1)) == (key_t) ERROR) {
		Error("chyba pri generovani klice");
		exit(ERRSYS);
	}
	//     Otevreni souboru santa.out
	FILE *f;
	if ((f = fopen(FILE_OUT, "w")) == NULL) {
		Error("chyba pri otevirani souboru \"%s\"", FILE_OUT);
		exit(ERRSYS);
	}
	//     Vytvoreni sdilene pameti
	int seg_id;
	sharemem *sh_m;
	if ((seg_id = shmget(addr, sizeof (sh_m), IPC_CREAT | S_IRUSR | S_IWUSR)) == ERROR) {
		fclose(f);
		Error("main: chyba pri ziskavani pameti");
		exit(ERRSYS);
	}
	if ((sh_m = shmat(seg_id, 0, 0)) == (void *) ERROR) {
		fclose(f);
		Error("main: chyba pri alokaci pameti");
		exit(ERRSYS);
	}
	//     Vytvoreni semaforu pro praci se sdilenou pameti
	int sem_id;
	if ((sem_id = semget(addr_sem, NUM_SEMS, IPC_CREAT | S_IRUSR | S_IWUSR)) == ERROR) {
		fclose(f);
		shmdt(sh_m);
		shmctl(seg_id, IPC_RMID, 0);
		Error("chyba pri tvorbe semaforu");
		exit(ERRSYS);
	}
	if (E > GROUP) {
		sh_m->at_once = GROUP;
	} else {
		sh_m->at_once = 1;
	}
	if (semap_ctl(sem_id, NUM_SEMS, sh_m->at_once) == ERROR) {
		fclose(f);
		shmdt(sh_m);
		shmctl(seg_id, IPC_RMID, 0);
		(void) semap_deal(sem_id);
		Error("chyba pri inicializaci semaforu");
		exit(ERRSYS);
	}
	//     Nastaveni hodnot sdilene pameti
	sh_m->A = 0;
	sh_m->I = 0;
	sh_m->C = C;
	sh_m->H = H*MICROSEC;
	sh_m->S = S*MICROSEC;
	sh_m->NE = E;
	sh_m->f = f;
	sh_m->sem_id = sem_id;
	sh_m->queue = 0;

	//     Vypnuti bufferu pri praci se souborem santa.out - pri bufferovani 
	//     byl prehazen vystup
	setvbuf(f, NULL, _IONBF, 0);
	signal(SIGINT, &killOthers);
	//     Santuv proces
	pid_t pid = fork();
	if (pid == 0) {
		//         Ziskani ukazatele na sdilenou pamet a zavolani f-ce santaJob
		int seg_id_s;
		int ret_santa;
		sharemem *sh_s;
		if ((seg_id_s = shmget(addr, sizeof (sh_s), S_IRUSR | S_IWUSR)) == ERROR) {
			Error("santa: chyba pri praci s pameti");
			exit(ERRSYS);
		}
		if ((sh_s = shmat(seg_id_s, 0, 0)) == (void *) ERROR) {
			Error("santa: chyba pri praci s pameti");
			exit(ERRSYS);
		}
		if ((ret_santa = santaJob(sh_s)) != EOK) {
			shmdt(sh_s);
			Error("chyba pri procesu santy");
			exit(ERRSYS);
		}
		shmdt(sh_s);
		exit(EOK);
	} else if (pid == ERROR) {
		Error("chyba pri fork() procesu santa");
		fclose(f);
		(void) semap_deal(sem_id);
		shmdt(sh_m);
		shmctl(seg_id, IPC_RMID, 0);
		exit(ERRSYS);
	}
	pid = 1;
	int cnt = 0;
	//     Elfi procesy
	while (cnt < E) {
		if (pid > 0) {
			if ((pid = fork()) == ERROR) {
				Error("chyba pri fork() u vytvareni elfu");
				signal(SIGINT, SIG_IGN);
				kill(0, SIGINT);
				break;
			}
		}
		if (pid == 0) {
			int seg_id_e;
			sharemem *sh_e;
			if ((seg_id_e = shmget(addr, sizeof (sh_e), S_IRUSR | S_IWUSR)) == ERROR) {
				Error("elf: chyba pri praci s pameti, shmget()");
				exit(ERRSYS);
			}
			if ((sh_e = shmat(seg_id_e, 0, 0)) == (void *) ERROR) {
				Error("elf: chyba pri praci s pameti, shmat()");
				exit(ERRSYS);
			}
			if (sh_e == sh_m) {
				printf("STEJNY\n\n");
			}
			if (elfJob(sh_e) != EOK) {
				shmdt(sh_e);
				Error("elf: chyba pri vykonavani prace");
				exit(ERRSYS);
			}
			shmdt(sh_e);
			exit(EOK);
		}
		cnt++;
	}
	//     Cekani na ukonceni vsech spustenych procesu
	int status;
	int excode = 0;
	for (int i = 0; i < cnt + 1; i++) {
		wait(&status);
		if (status > excode) {
			excode = status;
		}
	}
	fclose(f);
	(void) semap_deal(sem_id);
	shmdt(sh_m);
	shmctl(seg_id, IPC_RMID, 0);
	return excode;
}