Don't panic!

lamboot

kompilacja programów w MPI ($ oznacza znak prompta; nie wpisuj tego znaku)
    
    $ mpicc main.c -o main

Uruchamiania programów w MPI, pięć procesów
    
    $ mpirun -np 5 ./main
	$ mpirun --hostfile machines -np 5 ./main

Najczęstsze błędy:

1) MPI pyszczy o to, że nie ma "default-hostfile"

    $ mpirun -default-hostfile none -np 5 ./program.exe

2) MPI pyszczy, że brak bibliotek współdzielonych

    w pliku ~/.bashrc dopisz w dowolnym miejscu linijkę

    export LD_LIBRARY_PATH=/usr/lib64/mpi/gcc/openmpi/lib64

