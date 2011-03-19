/* 
 * File:   circlepack.c
 * Author: sasha
 *
 * Created on March 16, 2011, 10:14 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/*
 * 
 */

/*
 *
 */
double noise(){
    
    double number;
    //float average,stddev,deviation,avedev;

    
    number = sin(6.28*(double)rand()/(double)RAND_MAX);
     


    return number;
}

float ptdist(float x1,float y1,float z1,float x2,float y2,float z2){

    return (sqrt(pow(x1-x2,2)+pow(y1-y2,2)+pow(z1-z2,2)));
}


int main() {
    float D = 40; //pipe diameter
    float d = 3; //sphere diameter
    float Hmax, HDratio = 3, H;// fill height and ratio
    float theta; //sweep angle for sphere packing
    float step, stepH;
    FILE *storefile;
    float radialpass;
    float xtemp,ytemp,ztemp;
    float xfile,yfile,zfile;
    const float pi = 3.14159265;
    int pnumtemp = 1,pnumfile, Hpass=1, Hpasstotal,thetapass;
    int interference,scanlines,scan=1;
    double gap, mingap;
    fpos_t storeposA,storeposB;
    float totalvolume,particlevolume,occupiedvolume,packratio,scanvolume;

    printf("--- RANDOMIZED RASTER FILL PROGRAM - CYLINDER ----\n\n");
    printf("(Fills a cylinder of diameter, D, up to a certain height \nmeasured in its diameters with spheres of diameter, d.)\n");
    printf("OUTPUT FILE:\tpacking.pts\nSTATS:\t\tsummary.txt\n\n");

    printf("-- INPUT PARAMETERS --\n");
    printf("Cylinder diameter, D (mm):\t\t\t");
    scanf("%f",&D);
    printf("\nFill height in cylinder diameters, H/D:\t\t");
    scanf("%f",&HDratio);
    printf("\nSphere diameter, d (mm):\t\t\t");
    scanf("%f",&d);

    printf("\nThrowing beads into a pipe, I'll let you know how it goes.\n");


    // TIMIMG BIT
    time_t start, stop;
    int diff;
    int elapsed[3];


    start=time(NULL);

    srand((unsigned)(time(0)));

    Hmax = D*HDratio;


    scanvolume = d * pi * pow(D,2) / 4;
    particlevolume = pi * pow (d,3) / 6;
    scanlines = 0.6 * scanvolume / particlevolume;

    step = d / 3;
    stepH = d / 3;

    Hpasstotal = Hmax / stepH;

    storefile = fopen("packing.pts","w+");

    for (H = d/2;H<Hmax;H+=stepH){

        stop = time(NULL);
        diff = difftime(stop,start);
        elapsed[2]=diff%60;
        elapsed[1]=((diff-elapsed[2])/60)%60;
        elapsed[0]=(((diff-elapsed[2])/60)-elapsed[1])/60;

        printf("\n( Elapsed time: %02dh:%02dm:%02ds )\tSlice %d out of %d\n",elapsed[0],elapsed[1],elapsed[2], Hpass++, Hpasstotal);

        for (radialpass=((D-d)/2);radialpass>=0;radialpass-=step){

            //printf("\n");
            //thetapass = 0;
            //printf("|");
        //printf("current rad pass is %f\n",radialpass);

        for (theta=0;theta<=360;theta+=(.5+.25*noise())){
            //thetapass++;

            if ((theta)==0){
                printf("|");
            }





            xtemp = radialpass * cos(theta * pi / 180)+step*noise()/2;
            ytemp = radialpass * sin(theta * pi / 180)+step*noise()/2;
            ztemp=H+stepH*noise()/2;

            while (ztemp<d/2){
            ztemp=H+stepH*noise();
            }

            if (radialpass==((D-d)/2)){
                if (sqrt(pow(xtemp,2)+pow(xtemp,2))>((D-d)/2)){
                    xtemp = radialpass * cos(theta * pi / 180);
                    ytemp = radialpass * sin(theta * pi / 180);
                }
            }

            interference = 0;
            mingap = 0;
            int gapreset = 0;
            float ei=0, ej=0, ek=0;


            rewind(storefile);

            if (pnumtemp>(scanlines*2)){
                if ((scan%2)==1){
                    fsetpos(storefile,&storeposA);
                }
                if ((scan%2)==0){
                    fsetpos(storefile,&storeposB);
                }
                
            }


            while ((fscanf(storefile,"%f %f %f",&xfile, &yfile, &zfile) != EOF)&&(interference==0)){
                //scanf(storefile,"%d%f%f%f",&pnumfile, &xfile, &yfile, &zfile);

                //printf("current file line is %d %f %f %f\n",pnumfile, xfile, yfile, zfile);

                if (ptdist(xtemp,ytemp,ztemp,xfile,yfile,zfile)<d){
                    interference++;
                    //printf("interference! \n");
                }
                else {
                    float Scenter;
                    gap = ptdist(xtemp,ytemp,ztemp,xfile,yfile,zfile) - d;
                    //printf("\ngap: %f",gap);
                    Scenter = ptdist(xtemp,ytemp,ztemp,xfile,yfile,zfile);
                    if (gap<mingap){
                        mingap=gap;
                        //printf("\nmingap: %f",mingap);
                        ei=(xfile-xtemp)/Scenter;
                        ej=(yfile-ytemp)/Scenter;
                        ek=(zfile-ztemp)/Scenter;
                    }
                    if ((mingap==0)&&(gapreset==0)){
                        gapreset++;
                        //printf("\ngapreset: %f",gapreset);
                        mingap=gap;
                        ei=(xfile-xtemp)/Scenter;
                        ej=(yfile-ytemp)/Scenter;
                        ek=(zfile-ztemp)/Scenter;
                    }

                }
            }

            if (ptdist(xtemp,ytemp,ztemp,xfile,yfile,zfile)<d){
                    interference++;
                    //printf("interference! \n");
                }

            fseek(storefile,0, SEEK_END);


            if ((interference == 0)){

                xtemp += ei * mingap;
                ytemp += ej * mingap;
                ztemp += ek * mingap;

                //printf("\nsample offset: \t%f \t%f \t%f", ei*mingap,ej*mingap,ek*mingap);

                fprintf(storefile,"%f %f %f\n",xtemp,ytemp,ztemp);
                // printf("%d\t%f\t%f\t%f\n",pnumtemp,xtemp,ytemp,ztemp);
                if (pnumtemp%scanlines==0){
                    if ((scan%2)==1){
                        fgetpos(storefile,&storeposA);
                    }
                    if ((scan%2)==0){
                        fgetpos(storefile,&storeposB);
                    }
                    scan++;
                }
                pnumtemp++;
            }


        }
    }
    }

    

    

    fclose(storefile);


    // DIAGNOSTICS FILE

    

    FILE *diagfile;
    diagfile = fopen("summary.txt","w");

    totalvolume = Hmax * pi * pow(D,2) / 4;
    particlevolume = pi * pow (d,3) / 6;
    occupiedvolume = (double)pnumtemp * particlevolume;
    packratio = occupiedvolume / totalvolume;

    stop = time(NULL);
    diff = difftime(stop,start);
    elapsed[2]=diff%60;
    elapsed[1]=((diff-elapsed[2])/60)%60;
    elapsed[0]=(((diff-elapsed[2])/60)-elapsed[1])/60;

    fprintf(diagfile,"--- SPHERE PACKING SUMMARY ---\n");
    fprintf(diagfile,"\t TOTAL TIME FOR PACKING: %6d seconds. \t ( %02dh:%02dm:%02ds )\n\n", diff,elapsed[0],elapsed[1],elapsed[2]);

    fprintf(diagfile,"--- PARTICLE INFO\n");
    fprintf(diagfile,"\tDiameter: \t%5f mm\n", d);
    fprintf(diagfile,"\tTotal count: \t%5d\n\n", pnumtemp);

    fprintf(diagfile,"--- VOLUME INFO (Cylinder)\n");
    fprintf(diagfile,"\tDiameter: \t%5f mm\n", D);
    fprintf(diagfile,"\tFill Height: \t%5f mm\n", Hmax);
    fprintf(diagfile,"\t(H/D ratio): \t%5f\n", HDratio);
    fprintf(diagfile,"\tPacking: \t%2.2f%%\n", packratio*100);

    fclose(diagfile);

    printf("\ntotal number of particles for this run: %d\n",pnumtemp);



    return (EXIT_SUCCESS);
}
