#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>




typedef struct
{
    double r;
    double g;
    double b;
} pixel_rgb;


int main(int argc, char *argv[])
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

    FILE* fp1;
    fp1 = fopen(input,"r");
    
    if(fp1==NULL){
    
    printf("\nIt can not open the file \n");
    exit(EXIT_FAILURE);
    
    }
    
    char format;
    int format_2;
    int x;
    int y;
    int max;

    unsigned int xt[2];
    unsigned int yt[2];

    pixel_rgb** img = NULL;

    fscanf(fp1, "%c%d%d%d%d",&format,&format_2,&x,&y,&max);

    img = calloc(1,(y+2)*sizeof(pixel_rgb*));
    for (int i = 0; i<(y+2); i++)
    {
        img[i]=calloc(1,(x+2)*sizeof(pixel_rgb));
    }

    for(int i = 0; i<y; i++)
    {
        for(int k = 0; k<x; k++)
        {
            fscanf(fp1,"%lf",&img[i][k].r);
            fscanf(fp1,"%lf",&img[i][k].g);
            fscanf(fp1,"%lf",&img[i][k].b);
        }
    }
    FILE* fp2;
    fp2 = fopen(output,"w");
    
    if (fp2==NULL){
    	printf("Can not write \n");
    	exit(EXIT_FAILURE);
    
    }
    
    pixel_rgb* img2 = NULL;

    if (size>=1)
    {

        img2 = calloc(1,ceil(x*size)*ceil(y*size)*sizeof(pixel_rgb));
        fprintf(fp2,"%c%d\n%.0lf %.0lf\n%d\n",format,format_2,ceil(x*size),ceil(y*size),max);

        double old_x = 0;
        double old_y = 0;

        double width_ratio = (double)(x)/(double)(x*size);
        double height_ratio = (double) (y)/(double)(y*size);

        double A_r, A_g, A_b;
        double B_r, B_g, B_b;

        printf("\nwith_ratio is %lf\n", width_ratio);
        printf("\nheight_ratio is %lf\n", height_ratio);

        int index=0;
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

                A_r = img[xt[0]][yt[0]].r * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[0]].r * (old_x-xt[0])/(xt[1]-xt[0]);

                B_r = img[xt[0]][yt[1]].r * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[1]].r * (old_x-xt[0])/(xt[1]-xt[0]);

                A_g = img[xt[0]][yt[0]].g * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[0]].g * (old_x-xt[0])/(xt[1]-xt[0]);

                B_g = img[xt[0]][yt[1]].g * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[1]].g * (old_x-xt[0])/(xt[1]-xt[0]);

                A_b = img[xt[0]][yt[0]].b * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[0]].b * (old_x-xt[0])/(xt[1]-xt[0]);

                B_b = img[xt[0]][yt[1]].b * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[1]].b * (old_x-xt[0])/(xt[1]-xt[0]);




                img2[index].r = (A_r * (yt[1]-old_y)/(yt[1]-yt[0]) + B_r * (old_y-yt[0])/(yt[1]-yt[0]));
                img2[index].g = (A_g * (yt[1]-old_y)/(yt[1]-yt[0]) + B_g * (old_y-yt[0])/(yt[1]-yt[0]));
                img2[index].b = (A_b * (yt[1]-old_y)/(yt[1]-yt[0]) + B_b * (old_y-yt[0])/(yt[1]-yt[0]));
                index++;
            }

        }

        for(int i = 0; i<(x*size)*(y*size); i++)
        {
            
            fprintf(fp2,"%.0lf ",img2[i].r);
            fprintf(fp2,"%.0lf ",img2[i].g);
            fprintf(fp2,"%.0lf ",img2[i].b);
            
        }

    }

    else
    {



        img2 = calloc(1,(x)*(y)*size*sizeof(pixel_rgb));



        fprintf(fp2,"%c%d\n%.0lf %.0lf\n%d\n",format,format_2,ceil(size*x),ceil(y*size),max);



        double old_x = 0;
        double old_y = 0;

        double width_ratio = (double)(x)/(double)(x*size);
        double height_ratio = (double) (y)/(double) (y*size);

        double A_r, A_g, A_b;
        double B_r, B_g, B_b;

        printf("\nwith_ratio is %lf\n", width_ratio);
        printf("\nheight_ratio is %lf\n", height_ratio);

        int index=0;
        for(int i = 0; i<y*size; i++)
        {
            for(int k = 0; k<x*size; k++)
            {

                old_x = (double)i * (double)width_ratio;

                old_y = (double)k * (double)height_ratio;


                xt[0]=floor(old_x);
                xt[1]=ceil(old_x);

                yt[0]=floor(old_y);
                yt[1]=ceil(old_y);

                if (xt[0]==xt[1]) xt[1]++;
                if (yt[0]==yt[1]) yt[1]++;

                A_r = img[xt[0]][yt[0]].r * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[0]].r * (old_x-xt[0])/(xt[1]-xt[0]);

                B_r = img[xt[0]][yt[1]].r * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[1]].r * (old_x-xt[0])/(xt[1]-xt[0]);

                A_g = img[xt[0]][yt[0]].g * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[0]].g * (old_x-xt[0])/(xt[1]-xt[0]);

                B_g = img[xt[0]][yt[1]].g * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[1]].g * (old_x-xt[0])/(xt[1]-xt[0]);

                A_b = img[xt[0]][yt[0]].b * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[0]].b * (old_x-xt[0])/(xt[1]-xt[0]);

                B_b = img[xt[0]][yt[1]].b * (xt[1]-old_x)/(xt[1]-xt[0]) + img[xt[1]][yt[1]].b * (old_x-xt[0])/(xt[1]-xt[0]);




                img2[index].r = (A_r * (yt[1]-old_y)/(yt[1]-yt[0]) + B_r * (old_y-yt[0])/(yt[1]-yt[0]));
                img2[index].g = (A_g * (yt[1]-old_y)/(yt[1]-yt[0]) + B_g * (old_y-yt[0])/(yt[1]-yt[0]));
                img2[index].b = (A_b * (yt[1]-old_y)/(yt[1]-yt[0]) + B_b * (old_y-yt[0])/(yt[1]-yt[0]));
                index++;
            }

        }

        for(int i = 0; i<x*y*size; i++)
        {
            
            fprintf(fp2,"%.0lf ",img2[i].r);
            fprintf(fp2,"%.0lf ",img2[i].g);
            fprintf(fp2,"%.0lf ",img2[i].b);
            
        }

    }

    
    //shared memory
    //das Selbe Spiel für P6
  

    //removing some comments from the files and add other

    fclose(fp1);
    fclose(fp2);
    free(img);
    free(img2);


    return 0;
}

