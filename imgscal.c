#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

typedef struct
{
    double r;
    double g;
    double b;
} pixel_rgb;


int main (int argc, char **argv)
{

    int option;
    char input[20]="";
    char output[20]="";
    double size;

    while (( option = getopt(argc, argv, "i:o:s:h")) !=-1)
    {

        switch(option)
        {
        case 'i':


            strncpy(input,optarg,sizeof(input));
            break;

        case 'o':


            strncpy(output,optarg,sizeof(output));
            break;

        case 's':


            sscanf(optarg,"%5lf", &size);
            break;

        case 'h':

            printf("To make an image bigger for example: \n");
            printf("\n./a.out -i inputimag.p3.ppm -o outputimage -s 2\n\n");
            printf("\nTo make an image smaller for example:\n");
            printf("\n./a.out -i inputimag.p3.ppm -o outputimage -s 0.5\n\n");
            exit(EXIT_FAILURE);
            break;

        default:
            printf("no Data input\n");
            exit(EXIT_FAILURE);

        }
    }


    if (size>4)
    {
        printf("It will take some time\n");
        printf("Please use other size which is smaller than 4\n");
        exit(EXIT_FAILURE);

    }

    if(size==0)
    {
        printf("SIZE ZERO!!\nIt will not work\n");
        exit(EXIT_FAILURE);

    }

    FILE* fp1 = NULL;
    fp1 = fopen(input,"r");

    if(fp1==NULL)
    {
        printf("\nIt can not open the file \n");
        exit(EXIT_FAILURE);
    }




    char format;
    int format_2;
    int x;
    int y;
    int max;

    fscanf(fp1, "%c%d%d%d%d",&format,&format_2,&x,&y,&max);



    int shmid = shmget(IPC_PRIVATE, x*y*sizeof(pixel_rgb), IPC_CREAT | 0600 );

    if (shmid < 0)
    {
        printf("EROOR with shmget\n");
        exit(EXIT_FAILURE);
    }

    pixel_rgb* img = shmat(shmid, 0,0);



    if( img < (pixel_rgb *) 0)
    {
        printf("\nERROR while atteching \n");
        exit(EXIT_FAILURE);
    }


    pid_t pid = fork();

    if (pid == -1)
        return EXIT_FAILURE;

    else if( pid == 0)
    {

        // THE CHILD scan the image pixels
        for(int i = 0; i<x*y; i++)
        {

            fscanf(fp1,"%lf",&img[i].r);
            fscanf(fp1,"%lf",&img[i].g);
            fscanf(fp1,"%lf",&img[i].b);

        }
        
        fclose(fp1);
        printf("I am the child process and this is my ID: %d and I finished\n", getpid());

    }





    if (pid > 0)
    {

        // THE PARENT TAKE the pixels and do the rest of the WORK!

        wait(NULL);

        pixel_rgb** img2 = NULL;
        img2 = calloc(1,(y+2)*sizeof(pixel_rgb*));
        for (int i = 0; i<(y+2); i++)
        {
            img2[i]=calloc(1,(x+2)*sizeof(pixel_rgb));
        }

        pixel_rgb* img3=NULL;

        img3 = calloc(1,ceil(x*size)*ceil(y*size)*sizeof(pixel_rgb));

        unsigned int xt[2];
        unsigned int yt[2];


        double old_x = 0;
        double old_y = 0;

        double width_ratio = (double)(x)/(double)(x*size);
        double height_ratio = (double) (y)/(double)(y*size);

        double A_r, A_g, A_b;
        double B_r, B_g, B_b;

        printf("\nwith_ratio is %lf\n", width_ratio);
        printf("\nheight_ratio is %lf\n", height_ratio);



        // 1D image to 2D image to perform the image scaling algorithm
        long int index = 0;
        for(int i = 0; i<y; i++)
        {
            for(int k = 0; k<x; k++)
            {
                img2[i][k].r = img[index].r;
                img2[i][k].g = img[index].g;
                img2[i][k].b = img[index].b;
                index++;
            }
        }







        long int index2=0;
        for(int i = 0; i<ceil(y*size); i++)
        {
            for(int k = 0; k<ceil(x*size); k++)
            {

                old_x = (double)i * (double)width_ratio;

                old_y = (double)k * (double)height_ratio;


                xt[0]=floor(old_x);
                xt[1]=ceil(old_x);

                yt[0]=floor(old_y);
                yt[1]=ceil(old_y);

                if (xt[0]==xt[1]) xt[1]++;
                if (yt[0]==yt[1]) yt[1]++;

                A_r = img2[xt[0]][yt[0]].r * (xt[1]-old_x)/(xt[1]-xt[0]) + img2[xt[1]][yt[0]].r * (old_x-xt[0])/(xt[1]-xt[0]);

                B_r = img2[xt[0]][yt[1]].r * (xt[1]-old_x)/(xt[1]-xt[0]) + img2[xt[1]][yt[1]].r * (old_x-xt[0])/(xt[1]-xt[0]);

                A_g = img2[xt[0]][yt[0]].g * (xt[1]-old_x)/(xt[1]-xt[0]) + img2[xt[1]][yt[0]].g * (old_x-xt[0])/(xt[1]-xt[0]);

                B_g = img2[xt[0]][yt[1]].g * (xt[1]-old_x)/(xt[1]-xt[0]) + img2[xt[1]][yt[1]].g * (old_x-xt[0])/(xt[1]-xt[0]);

                A_b = img2[xt[0]][yt[0]].b * (xt[1]-old_x)/(xt[1]-xt[0]) + img2[xt[1]][yt[0]].b * (old_x-xt[0])/(xt[1]-xt[0]);

                B_b = img2[xt[0]][yt[1]].b * (xt[1]-old_x)/(xt[1]-xt[0]) + img2[xt[1]][yt[1]].b * (old_x-xt[0])/(xt[1]-xt[0]);




                img3[index2].r = (A_r * (yt[1]-old_y)/(yt[1]-yt[0]) + B_r * (old_y-yt[0])/(yt[1]-yt[0]));
                img3[index2].g = (A_g * (yt[1]-old_y)/(yt[1]-yt[0]) + B_g * (old_y-yt[0])/(yt[1]-yt[0]));
                img3[index2].b = (A_b * (yt[1]-old_y)/(yt[1]-yt[0]) + B_b * (old_y-yt[0])/(yt[1]-yt[0]));
                index2++;
            }

        }


        FILE* fp2 = NULL;
        fp2 = fopen(output,"w");

        if (fp2==NULL)
        {
            printf("Can not write \n");
            exit(EXIT_FAILURE);

        }


        fprintf(fp2,"%c%d\n%.0lf %.0lf\n%d\n",format,format_2,ceil(x*size),ceil(y*size),max);


        for(int i = 0; i<(x*size)*(y*size); i++)
        {

            fprintf(fp2,"%.0lf ",img3[i].r);
            fprintf(fp2,"%.0lf ",img3[i].g);
            fprintf(fp2,"%.0lf ",img3[i].b);

        }

        free(img2);
        free(img3);
        fclose(fp2);

        printf("I am the The parent process and this is my ID: %d and I finished\n", getpid());

    }

    shmdt(img);
    shmctl(shmid, IPC_RMID, 0);
    return 0;




}

