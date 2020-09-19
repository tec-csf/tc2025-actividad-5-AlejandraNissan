/*
Tarea 5
Alejandra Nissan
A01024682
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <time.h>

typedef struct {
    int * pipeCanal;
}canal;

void turno(canal *);

int main(int argc, char * const * argv){
    int nValue=0, c;
    int esEntero= 1;
    
    opterr = 0;

    while((c=getopt(argc, argv, "hn:")) != -1){
        switch(c){
            case 'n':
                nValue = atoi(optarg);
                //Revisando que el valor sea un número entero
                if(nValue<=0){
                    printf("Se requiere un numero y que sea mayor a 0\n");
                    exit(0);
                }
                break;
            case 'h':
                printf("Debes ingresar la cantidad de procesos que quieres que formen el token ring en el siguiente formato ./a.out -n <numero>\n");
                return 1;
            case '?':
            if (optopt == 'n')
                fprintf (stderr, "Opción -%c requiere un argumento.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Opción desconocida '\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();                
        }
    }

    //En caso de que no se pasen argumentos
    if(nValue <=0){
        printf ("Se debe incluir -n y luego un número mayor a cero\n");
        return(1);
    }

    for (int index = optind; index < argc; index++){
        printf ("El argumento no es una opción válida %s\n", argv[index]);
        return(1);
    }

    //Testigo
    char testigo = 'T';
    //int index= 0, veces=0;

    //Reservando espacio para arreglo de pipes
    canal * tokenRing;
    tokenRing = (canal *)malloc(sizeof(canal)*nValue);
    

    //Creación de pipes
    for(int i= 0; i<nValue; i++){
        //reservando espacio para el pipe
        (tokenRing+i)->pipeCanal = (int *) malloc(sizeof(int)*2);
        //creando el pipe
        pipe((tokenRing +i)->pipeCanal);
    }

    canal * auxToken = tokenRing;
    canal * finToken = tokenRing + nValue;

    pid_t pid;

    //Padre escribe inicialmente
    write(*((auxToken)->pipeCanal+1), &testigo, sizeof(char));
    printf("Soy el proceso padre con PID %d y acabo de enviar el testigo %c para que comience el ciclo\n\n", getpid(), testigo); 

    auxToken++;

    while (auxToken<finToken) {
        pid = fork();

        if (pid == -1)
        {
            printf("Error al crear el proceso hijo \n");
            //Liberando memoria
            for(int i=0; i<nValue; i++){
                free((tokenRing+i)->pipeCanal);
            }
            free(tokenRing);
            exit(1);
        }
        else if (pid == 0)
        {
            // Estamos en el hijo
            //printf("Soy el proceso hijo con PID: %d con padre %d\n", getpid(), getppid());
            while(1){
                turno(auxToken);
            }
            //exit(0);
        }
        auxToken++;
    }  

    //Padre
    while(1){
                
        close(*((tokenRing+nValue-1)->pipeCanal +1));
        read(*((tokenRing+nValue-1)->pipeCanal), &testigo, sizeof(char));
        printf("Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n", getpid(), testigo);   

        sleep(5);

        close(*((tokenRing)->pipeCanal));
        write(*((tokenRing)->pipeCanal+1), &testigo, sizeof(char));
        printf("Soy el proceso con PID %d y acabo de enviar el testigo %c\n\n", getpid(), testigo);
    }

    //Liberando memoria
    for(int i=0; i<nValue; i++){
        free((tokenRing+i)->pipeCanal);
    }
    free(tokenRing);
}

void turno(canal * fd)
{
    char c;

    close(*((fd-1)->pipeCanal+1));
    read((*((fd-1)->pipeCanal)), &c, sizeof(char));
    printf("Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n", getpid(), c);   

    sleep(5);

    close(*((fd)->pipeCanal));
    write(*((fd)->pipeCanal+1), &c, sizeof(char));
    printf("Soy el proceso con PID %d y acabo de enviar el testigo %c\n\n", getpid(), c);
}


