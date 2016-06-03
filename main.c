#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<math.h>

#define MIZOGIN 0
#define MAX_LICZBA_UCZESTNIKOW 10
#define KOLEJKA 1
#define OGLOSZENIE 2
#define AKCJA 3

#define max(x,y) (((x) < (y)) ? (y) : (x))

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int size, rank, i;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //MIZOGIN
	if(rank==0){
        srand(time(NULL));
        sleep(3);
        int prelekcja[2]; // 0 to kolejna lokacja, 1 to liczba uczestnikow
        int czas_oczekiwania = rand()%30 + 15;
        while(1==1){
            prelekcja[0] = rand()%3;
            prelekcja[1] = rand()%MAX_LICZBA_UCZESTNIKOW + 1;
            printf("## Lokacja prelekcji to: %d. Bedzie nas %d.\n", prelekcja[0], prelekcja[1]);
            //czas_oczekiwania = rand()%30 + 15; 
            czas_oczekiwania = 10;
            //printf("## Czekam %d sekund.\n", czas_oczekiwania);
            sleep(czas_oczekiwania);
            for(i=1; i<size; i++){
                MPI_Send(prelekcja, 2, MPI_INT, i, OGLOSZENIE, MPI_COMM_WORLD);
            }
        }
	}


	//FEMINISTKI
	else{
        int feministka[2]; //0 to lokacja, 1 to wartosc zegara Lamporta
	int lokalizacje_siostr[size]; for(i=0; i<size; i++) lokalizacje_siostr[i]=0;
	int odebrane = 0;
	int buf[2];
        int miejsce_w_kolejce = 1;
        int poziom_znudzenia=0;
        int ile_poszlo=0;
        feministka[1] = 0; 
        while(1==1){
            srand(time(NULL) + rank);
            feministka[0] = rand()%3;
            printf(">> %d: Urodzilam sie. Ustawie sie w miejscu numer %d! \n", rank, feministka[0]);
            poziom_znudzenia = 0;
            feministka[1]++;
            for(i=1; i<size; i++){
                if(i==rank) continue;
                feministka[1]++;
                MPI_Send(feministka, 2, MPI_INT, i, KOLEJKA, MPI_COMM_WORLD);
            }
            
            lokalizacje_siostr[rank]=feministka[0];
            while(odebrane < size-2){
                MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                feministka[1] = max(buf[1], feministka[1]);
                odebrane++;
                lokalizacje_siostr[status.MPI_SOURCE]=buf[0];
                if(feministka[0] == buf[0]){
                    if(feministka[1] < buf[1]){
                        //printf("moj Lamport jest mniejszy niz jej- bede przed nia\n");
                    }
                    else if (feministka[1] > buf[1]){
                        //printf("moj Lamport jest wiekszy niz jej- bede za nia\n");
                        miejsce_w_kolejce++;
                    }
                    else{
                        //printf("nasze Lamporty sa rowne, zdecyduje rank\n");
                        if(rank < status.MPI_SOURCE){
                            //printf(">> %d: moj rank mniejszy niz %d, stane przed nia\n", rank, status.MPI_SOURCE);
                        }
                        else{
                            //printf(">> %d: moj rank wiekszy niz %d, stane za nia\n", rank, status.MPI_SOURCE);
                            miejsce_w_kolejce++;
                        }
                    }
                }
            }
            //printf(">> %d: moje miejsce w kolejce: %d\n", rank, miejsce_w_kolejce);
            //for(i=1; i<size; i++) printf("> %d: lok_siostr[%d]=%d\n", rank, i, lokalizacje_siostr[i]);
            
            if(poziom_znudzenia >= 3){
                printf(">> %d: Znudzilam sie i umre.\n", rank);
                continue;
            }
            
            //odebranie broadcastu mizoginow
            MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, OGLOSZENIE, MPI_COMM_WORLD, &status);
            // printf(">> %d: wiem ze mizoginow bedzie %d w miejscu %d\n", rank, buf[1], buf[0]);
                    
            odebrane = 0;
            ile_poszlo=0;
                    
            printf(">> %d: stoje jako %d. w lokalizacji %d.\n   Najblizsze miejsce:   %d,  czlonkow: %d.\n", rank, miejsce_w_kolejce, feministka[0], buf[0], buf[1]);            
            if(feministka[0] == buf[0]){
                //printf(">> %d: Jestem tam, gdzie bedzie prelekcja!\n", rank);
                if(miejsce_w_kolejce <= buf[1]){
                    sleep(1);
                    printf(">> %d: Ide siac spustoszenie.\n", rank);
                    poziom_znudzenia=0;
                    sleep(1);
                    //ta idzie do domu i sie odradza, i spoko
                }
                else{
                    for(i=0; i<size; i++) if(lokalizacje_siostr[i] == feministka[0]) ile_poszlo++;
                    if(ile_poszlo > buf[1]) ile_poszlo = buf[1];
                    miejsce_w_kolejce-=ile_poszlo;
                    printf(">> %d: Zostaje w kolejce na %d. Moje obecne miejsce: %d.\n", rank, feministka[0], miejsce_w_kolejce);
                    //ta tez sie teraz odradza, a powinna kurde czekac
                    sleep(3);
                }
            }
            else{
                poziom_znudzenia++;
                printf(">> %d: Tu sie nic nie dzieje... nudno na: %d\n", rank, poziom_znudzenia);
            }
        }
    }
    MPI_Finalize();
    return 0;
}
