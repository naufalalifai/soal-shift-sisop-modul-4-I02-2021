# soal-shift-sisop-modul-4-I02-2021

## Member's Name
- Muhammad Naufal Alif Islami (05111942000008)
- Drigo Alexander SIhombing (05111942000020)
- Fitriana Zahirah Tsabit (05111942000011)

## Number 4
### To make it easier to monitor activities on their filesystem, Sin and Sei created a log system with the following specifications.
The system log that will be created is named “SinSeiFS.log” in the user's home directory (/home/[user]/SinSeiFS.log). This system log maintains a list of system call commands that have been executed on the filesystem..
Because Sin and Sei like tidiness, the logs that are made will be divided into two levels,INFO and WARNING.
For the WARNING level log, it is used to log the rmdir and unlink syscalls.
The rest will be recorded at the INFO level.
The format for logging is:


[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

Level : Level logging, dd : 2 digit date, mm : 2 digit month, yyyy : 4 digit year, HH : 2 digit hour (24 Hour format),MM : 2 digit minute, SS : 2 digit second, CMD : Called System Call, DESC : additional information and parameters

INFO::28052021-10:00:00:CREATE::/test.txt
INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt

For number 4 the solution is : 

    void filelog(char *level, char *command, const char *desc[], int descLen)

    {
    FILE *file = fopen(log, "a");
    time_t t;
    struct tm *tmp;
    char timeString[100];


    time(&t);
    tmp = localtime(&t);
    strftime(timeString, sizeof(timeString), "%d%m%y-%H:%M:%S", tmp);


    fprintf(file, "%s::%s:%s", level, timeString, command);
    for (int i = 0; i < descLen; i++)
    {
        fprintf(file, "::%s", desc[i]);
    }
    fprintf(file, "\n");
    fclose(file);
    }


#Kesusahan :
1. Kurang mengertinya terhadap modul fuse
2. terdapat error ketika mengcompile sehingga terdapat error dan blue screen 
