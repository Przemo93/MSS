#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<math.h>

#define MIZOGIN 0
#define MAX_LICZBA_UCZESTNIKOW 2
#define KOLEJKA 1
#define OGLOSZENIE 2
#define AKCJA 3

//#define max(x,y) (((x) < (y)) ? (y) : (x))
//#define min(x,y) (((x) < (y)) ? (x) : (y))

int max(int a, int b) 
{
    if (a>b) return a;
    return b;
}

int min(int a, int b){
    if(a>b) return b;
    return a;
}

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
        int nr_prelekcji=0;
        while(1==1){
            prelekcja[0] = rand()%3;
            prelekcja[1] = rand()%MAX_LICZBA_UCZESTNIKOW + 1;
            nr_prelekcji++;
            //czas_oczekiwania = rand()%30 + 15; 
            czas_oczekiwania = 15;
            //printf("## Czekam %d sekund.\n", czas_oczekiwania);
            sleep(czas_oczekiwania);
            printf("\n\n## Prelekcja %d. Miejsce: %d. Liczebnosc: %d.\n", nr_prelekcji, prelekcja[0], prelekcja[1]);
            for(i=1; i<size; i++){
                MPI_Send(prelekcja, 2, MPI_INT, i, OGLOSZENIE, MPI_COMM_WORLD);
            }
        }
    }


    //FEMINISTKI
    else{
        srand(time(NULL) + rank);
        int feministka[2]; //0 to lokacja, 1 to wartosc zegara Lamporta
        int lokalizacje_siostr[size]; 
        int zegary_siostr[size]; 
        for(i=0; i<size; i++) {lokalizacje_siostr[i]=0; zegary_siostr[i]=0;}
        int odebrane = 0;
        int kolejka = 0;
        int buf[2];
        int miejsce_w_kolejce = 1;
        int dlugosc_mojej_kolejki=1;
        int liczba_uczestniczek = 0;
        //int poziom_znudzenia=0;
        //int ile_poszlo=0;
        feministka[1] = 0;
        feministka[0] = rand()%3;
        printf(">> %d:%d Narodziny. Ustawie sie w miejscu numer %d. \n", rank, feministka[1], feministka[0]);
        //poziom_znudzenia = 0;
        
        feministka[1]++;
        for(i=1; i<size; i++){
            if(i==rank) continue;
            MPI_Send(feministka, 2, MPI_INT, i, KOLEJKA, MPI_COMM_WORLD);
        }
        
        while(odebrane < size-2){
                MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                odebrane++;
                lokalizacje_siostr[status.MPI_SOURCE]=buf[0];
                zegary_siostr[status.MPI_SOURCE]=buf[1];
                feministka[1] = max(buf[1], feministka[1]); // aktualizacja zegara Lamporta
        }
        
        lokalizacje_siostr[rank]=feministka[0];
        zegary_siostr[rank]=feministka[1];
        
        /* WHILE */
        while(1==1){
            srand(time(NULL) + rank);
            miejsce_w_kolejce=1;
            dlugosc_mojej_kolejki = 1;
            for(i=1; i<size; i++){
                if(rank == i) continue; 
                if(lokalizacje_siostr[rank] == lokalizacje_siostr[i]){
                    dlugosc_mojej_kolejki++;

                    if(zegary_siostr[rank] < zegary_siostr[i]){
                        //printf(">> %d:%d moj Lamport jest mniejszy niz jej- bede przed nia\n", rank, feministka[1]);
                    }
                    else if (zegary_siostr[rank] > zegary_siostr[i]){
                        printf(">> %d:%d moj Lamport jest wiekszy niz %d- bede za nia\n", rank, feministka[1], zegary_siostr[i]);
                        miejsce_w_kolejce++;
                    }
                    else if (zegary_siostr[rank] == zegary_siostr[i]){
                        //printf(">> %d: nasze Lamporty sa rowne, zdecyduje rank\n", rank);
                        if(rank < i){
                            //printf(">> %d: moj rank mniejszy niz %d, stane przed nia\n", rank, status.MPI_SOURCE);
                        }
                        else if (rank > i){
                            printf(">> %d:%d moj rank wiekszy niz %d, stane za nia\n", rank, feministka[1], i);
                            miejsce_w_kolejce++;
                        }
                    }
                }
            }
            //tu już jest pełne info
            //for(i = 1; i< size; i++) feministka[1] = max(feministka[1], zegary_siostr[i]);
            
            //printf(">> %d:%d Moje poprawne miejsce w kolejce: %d\n", rank, feministka[1], miejsce_w_kolejce);
            //for(i=1; i<size; i++) printf("> %d: lok_siostr[%d]=%d\n", rank, i, lokalizacje_siostr[i]);
            
            /*if(poziom_znudzenia >= 3){
                feministka[1]++;
                feministka[0]=rand()%3;
                printf(">> %d:%d Umieram z nudow. Moje nowe miejsce: %d.\n", rank, feministka[1], feministka[0]);
                poziom_znudzenia=0;
                continue;
            }*/
            /*if(rank == 1)
                for(i = 1; i< size; i++){
                
                    printf("Lokalizacja %d siostry: %d\n", i, lokalizacje_siostr[i]);
                    printf("      Zegar %d siostry: %d\n", i, zegary_siostr[i]);
                }*/
            
            //odebranie broadcastu mizoginow
            MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, OGLOSZENIE, MPI_COMM_WORLD, &status);
            // printf(">> %d: wiem ze mizoginow bedzie %d w miejscu %d\n", rank, buf[1], buf[0]);
            
            kolejka = 0;
            
            for(i = 1; i< size; i++){
                if(lokalizacje_siostr[i]==buf[0]) kolejka++;
            }
            
            liczba_uczestniczek = min(kolejka, buf[1]);
            
            //ile_poszlo=0;
                    
            printf(">> %d:%d Stoje jako %d. w lokalizacji %d.\n", rank, feministka[1], miejsce_w_kolejce, feministka[0]);            
            if(feministka[0] == buf[0]){
                //printf(">> %d: Jestem tam, gdzie bedzie prelekcja!\n", rank);
                if(miejsce_w_kolejce <= buf[1]){
                    sleep(1);
                    printf(">> %d:%d Ide siac spustoszenie.\n", rank, feministka[1]);
                    sleep(1);
                    printf(">> %d:%d Szowinistyczne swinie!\n", rank, feministka[1]);
                    srand(time(NULL) + rank);
                    feministka[0]=rand()%3;
                    //poziom_znudzenia=0;
                    feministka[1]++;
                    for(i=1; i<size; i++){
                        if(i==rank) continue;
                        MPI_Send(feministka, 2, MPI_INT, i, KOLEJKA, MPI_COMM_WORLD);
                    }
                    for(i=0; i < liczba_uczestniczek-1; i++){
                        MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                        lokalizacje_siostr[status.MPI_SOURCE] = buf[0];
                        zegary_siostr[status.MPI_SOURCE] = buf[1];
                        feministka[1] = max(feministka[1], buf[1]);
                    }
                    lokalizacje_siostr[rank] = feministka[0];
                    zegary_siostr[rank] = feministka[1];
                    //dlugosc_mojej_kolejki = 1;
                    //miejsce_w_kolejce = 1;
                    printf(">> %d:%d Wracam do gry. Bede w lokalizacji %d.\n", rank, feministka[1], feministka[0]);
                }
                else{
                    //for(i=0; i<size; i++) if(lokalizacje_siostr[i] == feministka[0]) ile_poszlo++;
                    //if(ile_poszlo > buf[1]) ile_poszlo = buf[1];
                    //miejsce_w_kolejce-=ile_poszlo;
                    printf(">> %d:%d Czekam w kolejce na %d\n", rank, feministka[1], feministka[0]);
                    for(i=0; i< liczba_uczestniczek; i++){
                        MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                        lokalizacje_siostr[status.MPI_SOURCE] = buf[0];
                        zegary_siostr[status.MPI_SOURCE] = buf[1]; 
                        feministka[1] = max(feministka[1], buf[1]);
                    }
                    printf(">> %d:%d Zostaje w kolejce na %d.\n", rank, feministka[1], feministka[0]);
                }
            }
            else{
                //poziom_znudzenia++;
                //printf(">> %d:%d Tu sie nic nie dzieje... nuda %d/3.\n", rank, feministka[1], poziom_znudzenia);
                printf(">> %d:%d Zaczynam sie nudzic. Liczba ucz: %d\n", rank, feministka[1], liczba_uczestniczek);
                for(i=0; i< liczba_uczestniczek; i++){
                    MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                    lokalizacje_siostr[status.MPI_SOURCE] = buf[0];
                    zegary_siostr[status.MPI_SOURCE] = buf[1];                    
                    feministka[1] = max(feministka[1], buf[1]);
                }
                printf(">> %d:%d Nudze sie.\n", rank, feministka[1]);                
            }
        }        
    }
    MPI_Finalize();
    return 0;
}
