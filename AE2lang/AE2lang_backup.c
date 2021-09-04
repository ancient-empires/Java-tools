#include <stdio.h>

strrev(char* sdata) {
    int i, k;
    char sdata2[512];
    strcpy(sdata2, sdata);
    k = strlen(sdata2);
    for (i = 0;i < k;i++) {
        sdata2[k - i - 1] = sdata[i];
    }
    strcpy(sdata, sdata2);
}

void GetFilename(char* sdata) {
    int i, k;
    char sdata2[512];
    strcpy(sdata2, sdata);
    strrev(sdata2);
    k = strlen(sdata2);
    for (i = 0;i < k;i++) {
        if (sdata2[i] == '\\') {
            sdata2[i] = 0;
            break;
        }
    }
    strrev(sdata2);
    strcpy(sdata, sdata2);
    return;
}

void help(void) {
    printf(" byblo - 200x - http://go.to/byblo - byblo@hotmail.com\n");
    printf("\n please use the following syntax :\n");
    printf(" AE2lang lang.dat lang.txt (dat2txt)\n");
    printf("  or\n");
    printf(" AE2lang lang.txt lang.dat (txt2dat)\n\n");
    printf(" Note that the appropriate function is selected\n");
    printf("  looking at the files extensions (minuscule only)\n\n");
    exit(0);
}

void main(int argc, char* argv[])
{
    int idx;
    FILE* fo, * fn;
    unsigned long int i, j, k, l, m, totalstrings = 0, totalstringscount = 0;
    unsigned char o1, o2, o3, o4, ignoreline;
    char sdata[2048], sdata2[2048];


    for (idx = 0; idx < argc; idx++); //ne pas effacer, sinon �a foire.
    printf("\n Ancient Empires II language file text converter v0.1b\n\n"); //titre du programme

    if (idx < 2) help(); //CHECK1=v�rifie qu'il y a bien au moins 3 parametres

    strcpy(sdata, argv[1]);
    k = strlen(sdata);
    strcpy(sdata2, argv[2]);
    l = strlen(sdata2);
    if (k < 1) help();
    if (l < 1) help();

    if ((sdata[k - 1] == 't') && (sdata[k - 2] == 'a') && (sdata[k - 3] == 'd') && (sdata2[l - 1] == 't') && (sdata2[l - 2] == 'x') && (sdata2[l - 3] == 't')) goto dat2txt;
    if ((sdata[k - 1] == 't') && (sdata[k - 2] == 'x') && (sdata[k - 3] == 't') && (sdata2[l - 1] == 't') && (sdata2[l - 2] == 'a') && (sdata2[l - 3] == 'd')) goto txt2dat;

    help();



    //******************************************************************************************
dat2txt:
    printf("\n Converting DAT to TXT...\n\n");

    fo = fopen(sdata, "rb");
    if (!fo) {
        printf(" Error, could not open %s for reading !\n", sdata);
        exit(0);
    }

    o1 = getc(fo);
    o2 = getc(fo);
    o3 = getc(fo);
    o4 = getc(fo);
    totalstrings = ((o1 * 16777216) + (o2 * 65536) + (o3 * 256) + o4);
    //printf("\n %d\n\n",totalstrings);
    if (1 > totalstrings) {
        fclose(fo);
        printf("\nError, incorrect format ? : %d (totalstrings announced in 4 first bytes)\n\n", totalstrings);
        exit(0);
    }

    fn = fopen(sdata2, "wb");
    if (!fn) {
        fclose(fo);
        printf(" Error, could not open %s for writing !\n", sdata2);
        exit(0);
    }

    for (i = 0;i < totalstrings;i++) {
        o3 = getc(fo);
        o4 = getc(fo);
        k = ((o3 * 256) + o4);
        if ((1 > k) && (k != 0)) {
            m = ftell(fo);
            printf("Error when getting length for text n. %d at offset : %xh\n", i, m);
            fclose(fo);
            fclose(fn);
            exit(0);
        }
        for (j = 0;j < k;j++) {
            o1 = getc(fo);
            if (o1 == 0x0A)
                o1 = 0x7C;
            //printf("\n %d\n\n",totalstrings);
            if (feof(fo) && ((k + 1) != totalstrings)) {
                m = ftell(fo);
                printf("Reached end of file unexpected when reading text n. %d at offset : %xh\n", i, m);
                fclose(fo);
                fclose(fn);
                exit(0);
            }
            sdata[j] = o1;
        }
        sdata[j] = 0;
        /*
            itoa(totalstringscount,sdata2,10);
            fputs("#string#->",fn);
            putc(0x09,fn);
            fputs(sdata2,fn);
            putc(0x0D,fn);
            putc(0x0A,fn);
        */
        fputs(sdata, fn);
        putc(0x0D, fn);
        putc(0x0A, fn);
        totalstringscount++;
        //printf("'%s'\n",sdata);
        //if (k==0) {printf("'%s'",sdata); fclose(fo); fclose(fn); exit(0); }
        //break;
    }


    printf(" Uh yeah, its done! %d/%d (anounced/extracted)\n", totalstrings, totalstringscount);
    fclose(fo);
    fclose(fn);
    exit(0);



    //******************************************************************************************
txt2dat:
    printf("\n Converting TXT to DAT...\n\n");

    fo = fopen(sdata, "rb");
    if (!fo) {
        printf(" Error, could not open %s for reading !\n", sdata);
        exit(0);
    }

    fn = fopen(sdata2, "wb");
    if (!fn) {
        printf(" Error, could not open %s for writing !\n", sdata2);
        fclose(fo);
        exit(0);
    }

    //�criture en-tete temporaire
    putc(0xff, fn);
    putc(0xff, fn);
    putc(0xff, fn);
    putc(0xff, fn);


    //Evincer �ventuellement le foutu EFBB BF en en-t�te qui sort d'on se sait o�...
    o1 = getc(fo);
    o2 = getc(fo);
    o3 = getc(fo);

    if ((o1 == 0xEF) && (o2 == 0xBB) && (o3 == 0xBF))
        printf("Detected and avoided the header EFBB BF from the text file.\n");
    else
        rewind(fo);

    //inits
    sdata[0] = 0;
    l = 0;
    ignoreline = 0;

    while (!feof(fo)) {
        o1 = getc(fo);
        if ((l == 0) && (o1 == 0x5E)) ignoreline = 1;
        //restaurer les "|" en 0x0a
        if (o1 == 0x7C) o1 = 0x0a;
        if (o1 == 0x0D) {
            o1 = getc(fo);
            if (o1 == 0x7C) o1 = 0x0a;
            if (o1 == 0x0A) {
                sdata[l] = 0;
                k = strlen(sdata);
                o3 = (k / 256);
                j = (o3 * 256);
                k = (k - j);
                o4 = k;
                if (ignoreline != 1) {
                    putc(o3, fn);
                    putc(o4, fn);
                    fputs(sdata, fn);
                    //printf("'%s'\n",sdata);
                    totalstringscount++;
                }
                l = 0;
                ignoreline = 0;
            }
            else {
                sdata[l] = o1;
                l++;
            }
        }
        else {
            sdata[l] = o1;
            l++;
        }
    }

    rewind(fn);
    //on va pas se faire chier, hop!
    putc(0x0, fn);
    putc(0x0, fn);

    k = totalstringscount;
    o3 = (k / 256);
    j = (o3 * 256);
    k = (k - j);
    o4 = k;
    putc(o3, fn);
    putc(o4, fn);


    printf(" Uh yeah, its done! %d (strings count)\n", totalstringscount);
    fclose(fo);
    fclose(fn);
    exit(0);



    //******************************************************************************************
str2dat:
    printf("\n Inserting string to dat... TODO\n\n");
    exit(0);



}//FIN void main




