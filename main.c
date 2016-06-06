#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<math.h>

#define MIZOGIN 0
#define KOLEJKA 1
#define OGLOSZENIE 2
#define AKCJA 3

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
    int MAX_LICZBA_UCZESTNIKOW = atoi(argv[1]);
    
    //MIZOGIN
    if(rank==0){
        srand(time(NULL));
        sleep(2);
        int prelekcja[2]; // 0 to kolejna lokacja, 1 to liczba uczestnikow
        int czas_oczekiwania = rand()%30 + 15;
        printf("\n## Czas do nastepnej prelekcji: %ds.\n", czas_oczekiwania);
        int nr_prelekcji=0;
        while(1==1){
            prelekcja[0] = rand()%3;
            prelekcja[1] = rand()%MAX_LICZBA_UCZESTNIKOW + 1;
            nr_prelekcji++;
            sleep(czas_oczekiwania);
            czas_oczekiwania = rand()%30 + 15;
            printf("\n## Prelekcja nr %d. Miejsce: %d. Ilosc uczestnikow: %d. Czas do nastepnej prelekcji: %ds.\n\n", nr_prelekcji, prelekcja[0], prelekcja[1], czas_oczekiwania);
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
        int liczba_uczestniczek = 0;
        feministka[1] = 0;
        feministka[0] = rand()%3;
        printf(">> %d:%d Narodziny. Ustawie sie w miejscu numer %d. \n", rank, feministka[1], feministka[0]);
        
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
        
        while(1==1){
            srand(time(NULL) + rank);
            miejsce_w_kolejce=1;
            for(i=1; i<size; i++){
                if(rank == i) continue; 
                if(lokalizacje_siostr[rank] == lokalizacje_siostr[i]){

                    if(zegary_siostr[rank] < zegary_siostr[i]){ }
                    else if (zegary_siostr[rank] > zegary_siostr[i]) miejsce_w_kolejce++;
                    else if (zegary_siostr[rank] == zegary_siostr[i]){
                        if(rank < i){ }
                        else if (rank > i ) miejsce_w_kolejce++;
                    }
                }
            }
            
            //odebranie broadcastu mizoginow
            MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, OGLOSZENIE, MPI_COMM_WORLD, &status);
            
            kolejka = 0;            
            for(i = 1; i< size; i++){
                if(lokalizacje_siostr[i]==buf[0]) kolejka++;
            }
            liczba_uczestniczek = min(kolejka, buf[1]);
                    
            printf(">> %d:%d Stoje jako %d. w lokalizacji %d.\n", rank, feministka[1], miejsce_w_kolejce, feministka[0]);  
            
            if(feministka[0] == buf[0]){
                if(miejsce_w_kolejce <= buf[1]){
                    printf(">> %d:%d Ide siac spustoszenie. Szowinistyczne swinie!\n", rank, feministka[1]);
                    sleep(2);
                    
                    srand(time(NULL) + rank);
                    feministka[0]=rand()%3;
                    
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
                    
                    printf(">> %d:%d Wracam do gry. Bede w lokalizacji %d.\n", rank, feministka[1], feministka[0]);
                }
                else{
                    printf(">> %d:%d Czekam w kolejce na %d.\n", rank, feministka[1], feministka[0]);
                    
                    for(i=0; i< liczba_uczestniczek; i++){
                        MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                        lokalizacje_siostr[status.MPI_SOURCE] = buf[0];
                        zegary_siostr[status.MPI_SOURCE] = buf[1]; 
                        feministka[1] = max(feministka[1], buf[1]);
                    }
                }
            }
            else{
                printf(">> %d:%d Czekam w kolejce na %d.\n", rank, feministka[1], feministka[0]);
                
                for(i=0; i< liczba_uczestniczek; i++){
                    MPI_Recv(buf, 2, MPI_INT, MPI_ANY_SOURCE, KOLEJKA, MPI_COMM_WORLD, &status);
                    lokalizacje_siostr[status.MPI_SOURCE] = buf[0];
                    zegary_siostr[status.MPI_SOURCE] = buf[1];                    
                    feministka[1] = max(feministka[1], buf[1]);
                }
            }
        }        
    }
    MPI_Finalize();
    return 0;
}
