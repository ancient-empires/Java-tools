#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LARGE_SPACE_SIZE 2048
#define BYTE_CAP 256

#define CR 0x0D
#define LF 0x0A
#define DBQUOTE '"' // 0x22
#define BACKSLASH '\\' // 0x5C

char* strrev(char* str) {
	// reverse the input string in-place
	char buffer[LARGE_SPACE_SIZE];
	strcpy(buffer, str);
	int buffer_len = strlen(buffer);
	for (int i = 0; i < buffer_len; ++i) {
		buffer[buffer_len-1-i] = str[i];
	}
	strcpy(str, buffer);
	return str;
}

char* GetFilename(char* str) {
	// get filename from str, and save it in-place
	char buffer[LARGE_SPACE_SIZE];
	strcpy(buffer, str);
	strrev(buffer);
	int buffer_len = strlen(buffer);
	for (int i = 0; i < buffer_len; ++i) {
		if (buffer[i] == BACKSLASH) {
			buffer[i] = 0;
			break;
		}
	}
	strrev(buffer);
	strcpy(str, buffer);
	return str;
}

void help(void) {
	printf(" byblo - 200x - http://go.to/byblo - byblo@hotmail.com\n");
	printf("\n please use the following syntax :\n");
	printf(" AE2pak.exe filename.pak -e (extract) c:\\extract_path (optional)\n");
	printf("  or\n");
	printf(" AE2pak.exe filename.pak -p (pack) filelist.txt\n\n");
	exit(0);
}

char extract(char* fo2s,char* fn2s,unsigned long int filepos,unsigned long int filesize) {
 FILE *fo2, *fn2;
 unsigned int i,j;
 unsigned char o1;

 fo2 = fopen(fo2s,"rb");
 if (!fo2)
	{
	printf(" Error, could not open %s for extraction !\n",fo2s);
	return 0;
	}
 rewind(fo2);
 fseek(fo2, filepos, 0);

 fn2 = fopen(fn2s,"wb");
	if (!fn2) {
		printf(" Error, could not open %s for writing !\n",fn2s);
		fclose(fo2);
		return 0;
	}
 for (i=0;i<filesize;i++) {
	o1 = getc(fo2);
	if (feof(fo2)) {
		printf(" Error, eof reached when extracting!\n Check your file !");
		fclose(fo2);
		fclose(fn2);
		return 0;
	}
	putc(o1,fn2);
 }

 fclose(fo2);
 fclose(fn2);
 return 1;
}


int main(int argc, char *argv[]) {
	int idx;
	FILE *fo, *fo3, *fn, *fl;
	unsigned long int i, j, k, l, m, headpos, totalfiles=0, totalextracted=0, filepos=0, filesize, totalerrors=0;
	unsigned char o1, o2, o3, o4;
	char sdata[LARGE_SPACE_SIZE], sdata3[LARGE_SPACE_SIZE];
	char sdata2[LARGE_SPACE_SIZE][BYTE_CAP];
	unsigned int sdata2s[LARGE_SPACE_SIZE];
	char newfile[LARGE_SPACE_SIZE];


	for (idx = 0; idx < argc; idx++); //ne pas effacer, sinon �a foire.
	printf("\n Ancient Empires II packer-unpacker v0.11b\n\n"); //titre du programme

	if(idx<2) help(); //CHECK1=v�rifie qu'il y a bien au moins 3 parametres

	if (!strcmp(argv[2],"-e")) {
		goto extract;
		exit(0);
	}

	if (!strcmp(argv[2],"-p")) {
		goto pack;
		exit(0);
	}

	help();

extract:
	printf("Extracting...\n");
	fo = fopen(argv[1],"rb");
	if (!fo) {
		printf("error:file %s not found.\n",argv[1]);
		fclose(fo);
		exit(1);
		}
	rewind(fo);
	o1=getc(fo);
	o2=getc(fo);
	headpos = o1 * BYTE_CAP + o2;
	printf("Header position : %d\n",headpos);
	o1=getc(fo);
	o2=getc(fo);
	headpos = o1 * BYTE_CAP + o2;
	printf("Total Files announced : %d\n",totalfiles);

	if (feof(fo)) {
		fclose(fo);
		printf("error:unexpected end of file \n : %s.\n",argv[1]);
		exit(1);
	}

	if (argv[3]) {
		strcpy(sdata3,argv[3]);
		k = strlen(sdata3);
		if (sdata3[k-1] == DBQUOTE) {
			sdata3[k-1] = 0;
		}
		if (sdata3[k-1] == BACKSLASH) {
			sdata3[k-1] = 0;
		}
		strcat(sdata3,"\\_filelist.txt");
		strcpy(sdata,sdata3);
		//printf("%s\n",sdata);
	}
	else
		strcpy(sdata,"_filelist.txt");

	printf(" Filelist is '%s'\n", sdata);

	fl = fopen(sdata,"wb");

	if (!fl) {
		printf("error:logging file %s cannot be created for writing.\n",sdata);
		fclose(fo);
		fclose(fl);
		exit(1);
		}

	for (i=0;i<totalfiles;i++) {
		o1=getc(fo);
		o2=getc(fo);
		k = (o1 * BYTE_CAP+o2;
		for (j=0;j<k;j++) {
			sdata[j]=getc(fo);
		}
		sdata[j]=0;
		o1=getc(fo);
		o2=getc(fo);
		o3=getc(fo);
		o4=getc(fo);
		filepos = ((o1*16777216)+(o2*65536)+(o3*256)+o4+headpos);
		o1=getc(fo);
		o2=getc(fo);
		filesize = ((o1*BYTE_CAP)+o2);
		if (argv[3]) {
		strcpy(sdata3,argv[3]);
		k = strlen(sdata3);
		if (sdata3[k-1] == DBQUOTE) {
			sdata3[k-1]=0;
		}
		if (sdata3[k-1] == BACKSLASH) {
			sdata3[k-1]=0;
		}
		strcat(sdata3,"\\");
		strcat(sdata3,sdata);
		strcpy(sdata,sdata3);
		//printf("%s\n",sdata);
		}
		//printf("extracting file %d at %d : %s, %d byte(s)\n",i,filepos,sdata,filesize);
		fprintf(fl,"%s",sdata);
		putc(0x0d,fl);
		putc(0x0a,fl);

		if (!extract(argv[1],sdata,filepos,filesize))
			totalerrors++;
		totalextracted++;
	}
	fclose(fl);
	fclose(fo);


	printf("\n Uh yeah, its done! %d errors for %d/%d (extracted/announced)\n",totalerrors, totalextracted,totalfiles);
	exit(0);

pack:
	if (!argv[3]) help();

	printf("Packing...\n");
	fo = fopen(argv[3],"r");
	if (!fo) {
		printf(" Error, file %s not found.\n",argv[3]);
		help();
		}
	rewind(fo);
	printf("Checking filelist...\n");
	while(!feof(fo)) {
		sdata[0]=0;
		fgets(sdata, 512, fo);
		k = strlen(sdata);
		if ( (k>0) && ((sdata[k-1]==0x0A) || (sdata[k-1]==0x0D)) )sdata[k-1]=0;
		if ( (strcmp(sdata2[totalfiles],sdata)) && (k>1) ) {
			strcpy(sdata2[totalfiles],sdata);
			fo3 = fopen(sdata2[totalfiles],"rb");
			if (!fo3) {
				printf("Error, could not found : %s\n",sdata2[totalfiles]);
				totalerrors++;
			}
			else{
				rewind(fo3);
				fseek (fo3, 0, SEEK_END);
				sdata2s[totalfiles] = ftell(fo3);
				fclose(fo3);
				//printf("'%s' %d bytes\n",sdata2[totalfiles],sdata2s[totalfiles]);
			}
			totalfiles++;
			if (totalfiles>512) break;
		}
	}
	fclose(fo);

	if (totalerrors>0) {
		printf("Sorry, could not found %d files, fix the problem before retrying.\n",totalerrors);
		exit(0);
	}

	if (totalfiles == 0) {
		printf("Nothing to pack. Check your files!\n");
		exit(1);
	}

	if (totalfiles > LARGE_SPACE_SIZE) {
		printf("Sorry, this crappy exe cannot pack more than %d files!\n", LARGE_SPACE_SIZE);
		exit(1);
	}

	fn = fopen(argv[1],"wb");
	if (!fn)
		{
		printf("Error, could not open file %s for writing\n",argv[1]);
		exit(1);
		}


	rewind(fn);
	//r�serv�
	putc(0xFF,fn);
	putc(0xFF,fn);

	o3 = totalfiles / BYTE_CAP;
	o4 = totalfiles % BYTE_CAP;
	//printf("\n\nDEBUG %x %x %x %x\n\n",o1,o2,o3,o4);
	putc(o3,fn);
	putc(o4,fn);


	for (i=0;i<totalfiles;i++) {
		strcpy(sdata,sdata2[i]);
		GetFilename(sdata);
		k = strlen(sdata);
		o3 = (k/256);
		j = (o3*256);
		k = (k-j);
		o4 = k;

		putc(o3,fn);
		putc(o4,fn);

		fputs(sdata,fn);


		//position relative
		k = filepos;
		o1 = (k/16777216);
		j = (o1*16777216);

		k = (k-j);
		o2 = (k/65536);
		j = (o2*65536);

		k = (k-j);
		o3 = (k/256);
		j = (o3*256);

		k = (k-j);
		o4 = k;
		//printf("\n\nDEBUG %x %x %x %x\n\n",o1,o2,o3,o4);
		//printf("\n\nDEBUG %d\n\n",sdata2s[i]);
		putc(o1,fn);
		putc(o2,fn);
		putc(o3,fn);
		putc(o4,fn);


		//taille du fichier
		k = sdata2s[i];
		filepos = (filepos+k);

		o3 = (k/256);
		j = (o3*256);

		k = (k-j);
		o4 = k;

		putc(o3,fn);
		putc(o4,fn);

	}
	//�criture du header de la position de fin du header
	fseek (fn, 0, SEEK_END);
	filepos = ftell(fn);
	k = filepos;

	o3 = (k/256);
	j = (o3*256);

	k = (k-j);
	o4 = k;

	rewind(fn);
	putc(o3,fn);
	putc(o4,fn);

	//paquetage des fichiers
	rewind(fn);
	fseek (fn, filepos, 0);

	for (i=0;i<totalfiles;i++) {
		fo = fopen(sdata2[i],"rb");
		if (!fo) {
			printf("Error, could not open file %s for reading\n",sdata2[i]);
			fclose(fn);
			exit(1);
			}
		//printf("packing file %d from : %s, %d byte(s)\n",i,sdata2[i],sdata2s[i]);
		j=0;
		while(!feof(fo)) {
			o1=getc(fo);
			if (!feof(fo)) putc(o1,fn);
			j++;
		}
		if ((j-1)!=sdata2s[i]) printf("Error, could not match size of file ! j = %d\n",j);
		fclose(fo);
	}
	fclose(fn);


	printf("\n Uh yeah, its done! %d errors for %d (announced)\n",totalerrors, totalfiles);
	exit(0);
}

