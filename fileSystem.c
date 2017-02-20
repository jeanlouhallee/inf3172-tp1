#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc, char *argv[]){

    FILE *file = fopen("test", "wb");
    ftruncate(fileno(file), 512 * 1000);
    fclose(file);
}

void creationFicher(){

}

void suppressionFichier(){

}

void creationRepertoire(){

}

void suppressionRepertoire(){

}

void lireFichier(){

}
