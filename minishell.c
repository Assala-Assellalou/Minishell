#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include <stdbool.h>
#include <string.h>
//TP 1 : Processus
#include <sys/types.h>  
#include <unistd.h>    
#include <sys/wait.h> 
//TP 2 et 3: Signaux 
#include <signal.h>  
//TP 4 : Fichiers
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

//------------------------------------------------------------------------------------
/*Etape 5 : Traitement du signal SIGCHILD*/
/*
void traitement_sigchld(int sig) {
    //SIGCHLD indique qu'un processus fils vient de ce terminer
    printf("Reception du signal SIGCHLD (%d) => processus fils terminé\n", sig);
}
*/
//------------------------------------------------------------------------------------
/*Etape 6 : Utilisation de SIGCHLD pour traiter la terminaison des processus fils*/
void traitement_sigchld(int sig) {
    (void)sig;
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        /*printf("Reception du signal SIGCHLD (%d): processus fils %d terminé\n",sig, pid);*/
        /*Etape 9 : Affichage d’un message indiquant le signal reçu*/
         if (WIFEXITED(status)) {
            printf("\nProcessus fils %d terminé normalement (%d)\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("\nProcessus fils %d terminé par un signal (%d)\n", pid, WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("\nProcessus fils %d suspendu (%d)\n", pid, WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("\nProcessus fils %d repris\n", pid);
        }
    }
}
//---------------------------------------------------------------------------------------
/* Etape 11.1 : Gestion de la frappe au clavier de ctrl-C et ctrl-Z */
/*
void traitement_sigint(int sig) {
    (void)sig;
    printf("\nCtrl-C ignoré \n> ");
    fflush(stdout);
}

void traitement_sigtstp(int sig) {
    (void)sig;
    printf("\nCtrl-Z ignoré \n> ");
    fflush(stdout);
}
*/
//---------------------------------------------------------------------------------------
/* Etape 16 : Tubes simples */
void exec_tube(char **cmd1, char **cmd2 ) {
    int tube[2];
    //Creation du tube 
    if (pipe(tube) == -1 ) { 
        perror("erreur pipe");
        exit(EXIT_FAILURE);
    }
    // Creation du 1er fils
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    //Execution de la 1ere commande par le 1er fils 
    if (pid1 == 0) { // Fils 1 
        //redirection 
        close(tube[0]);
        dup2(tube[1], 1); 
        close(tube[1]);
        //execution
        execvp(cmd1[0], cmd1);
        perror("execvp cmd1");
        exit(EXIT_FAILURE);
    }
    // Creation du deuxieme fils 
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    //Execution de la 2eme commande par le 2eme fils 
    if (pid2 == 0) {  //Fils 2
        //redicrection 
        close(tube[1]);
        dup2(tube[0], 0);
        close(tube[0]);
        //execution
        execvp(cmd2[0], cmd2);
        perror("execvp cmd2");
        exit(EXIT_FAILURE);
    }
    //Pere 
    //fermer les tubes
    close(tube[0]);
    close(tube[1]);
    //Attendre les fils avec waitpid
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

}
//===================== Programme principal ===================================

int main(void) {
    
    //Initialisation des traitements de signaux
    // SIGCHLD : 
    struct sigaction sa;
    sa.sa_handler = traitement_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
    //--------------------------------------------------------------------------
    /*Etape 11.1 : Changer le traitement des signaux SIGINT et SIGTSTP
    // SIGINT (Ctrl-C)
    struct sigaction sa_int;
    sa_int.sa_handler = traitement_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    // SIGTSTP (Ctrl-Z)
    struct sigaction sa_tstp;
    sa_tstp.sa_handler = traitement_sigtstp;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_tstp, NULL);
    */
    //---------------------------------------------------------------------------
    /* Etape 11.2 : Ignorer les signaux SIGINT et SIGTSTP
    //SIGINT (Ctrl-C)
    signal(SIGINT, SIG_IGN);
    //SIGTSTP (Ctrl-Z)
    signal(SIGTSTP, SIG_IGN);
    */
    //-----------------------------------------------------------------------------
    // Etape 11.3 : Masquer SIGINT et SIGTSTP
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTSTP);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    
    bool fini= false;

    while (!fini) {
        printf("> ");
        struct cmdline *commande= readcmd();

        if (commande == NULL) {
            // commande == NULL -> erreur readcmd()
            perror("erreur lecture commande \n");
            exit(EXIT_FAILURE);
    
        } else {

            if (commande->err) {
                // commande->err != NULL -> commande->seq == NULL
                printf("erreur saisie de la commande : %s\n", commande->err);
        
            } else {
                /*Etape 16 : Tube simple */
                if (commande->seq[0] && commande->seq[1] && !commande->seq[2]) {
                    exec_tube(commande->seq[0], commande->seq[1]);
                    continue; 
                }
                //-----------------------------------------------------------
                int indexseq= 0;
                char **cmd;
                while ((cmd= commande->seq[indexseq])) {
                    if (cmd[0]) {
                        if (strcmp(cmd[0], "exit") == 0) {
                            fini= true;
                            printf("Au revoir ...\n");
                        }
                        //====================== Modifications ===============================
                        /*Etape 14 : Deplacement dans l’arborescence*/
                        else if (strcmp(cmd[0], "cd") == 0) {
                            // Commande interne cd
                            char *repertoire_cible = NULL;
                            if (cmd[1] == NULL) {
                                // Pas d'argument : aller dans $HOME
                                repertoire_cible = getenv("HOME");
                                if (repertoire_cible == NULL) {
                                    fprintf(stderr, "cd: variable d'environnement HOME non définie\n");
                                    continue;
                                }
                            } else {
                                // cd repertoire_cible
                                repertoire_cible = cmd[1];
                            }
                            if (chdir(repertoire_cible) != 0) {
                                perror("erreur cd");
                            }
                        }
                        //----------------------------------------------------------------
                        /*Etape 15 : Afficher le contenu d’un repertoire*/
                        else if (strcmp(cmd[0], "dir") == 0) {
                            // Commande interne dir
                            const char *repertoire = (cmd[1] == NULL) ? "." : cmd[1];
                            DIR *dir = opendir(repertoire);
                            if (dir == NULL) {
                                perror("erreur dir");
                                break;
                            }
                            struct dirent *entree;
                            while ((entree = readdir(dir)) != NULL) {
                                printf("%s\n", entree->d_name);
                            }
                            closedir(dir);
                        }

                        else {
                          pid_t pid = fork();
                          if (pid == -1) {
                            perror("Echec du fork");
                            exit(EXIT_FAILURE);
                          }

                          if (pid == 0) { /* fils */
                            //-------------------------------------------------------------
                            /* Etape 12 : Detacher les processus fils en arriere plan*/
                            if (commande->backgrounded != NULL) {
                                setpgrp(); //on associe un nouveau groupe
                            }
                            // Demasquer SIGINT et SIGTSTP pour le fils
                            sigset_t mask;
                            sigemptyset(&mask);
                            sigaddset(&mask, SIGINT);
                            sigaddset(&mask, SIGTSTP);
                            sigprocmask(SIG_UNBLOCK, &mask, NULL);
                            //-------------------------------------------------------------------------
                            /* Etape 13 : Redirections*/
                            // Redirection entree (<)
                            if (commande->in != NULL) {
                                int desc_in = open(commande->in, O_RDONLY);
                                if (desc_in < 0) {
                                    perror("Erreur ouverture fichier en entrée");
                                    exit(EXIT_FAILURE);
                                }
                                dup2(desc_in, 0); //redirection de l'entree standard (0)
                                close(desc_in); //fermer le descripteur car il est duppliqué par dup2
                            }
                            // Redirection sortie (>)
                             if (commande->out != NULL) {
                                int desc_out = open(commande->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                if (desc_out < 0) {
                                    perror("Erreur ouverture fichier en sortie");
                                    exit(EXIT_FAILURE);
                                }
                                dup2(desc_out, 1); //redirection de la sortie standard (1)
                                close(desc_out); //fermer le descripteur car il est duppliqué par dup2
                            }
                            //-------------------------------------------------------------------------
                            /*Etape 2 : Execution de la commande*/
                            // On utilise execvp
                            execvp(cmd[0], cmd);
                            perror("Echec de execvp");
                            exit(EXIT_FAILURE);
                          }
                           //---------------------------------------------------------------------------
                          else { /* père */
                            /*Etape 3 (Modifiée) : Attendre la terminaison de la commande en cours, si elle est en avant-plan */
                            if (commande->backgrounded == NULL){
                                /*
                                waitpid(pid, NULL, 0);
                                */
                                //-----------------------------------------------------------------------
                                // Etape 7 : Attendre un signal : pause 
                                 pause(); //Attend SIGCHILD 
                            }
                            //---------------------------------------------------------------------------
                            /*Etape 4 : Ne pas attendre la terminaison de la commande en cours, si elle est en arriere-plan*/
                            else {
                                printf("La commande s'execute en tache de fond de pid =%d\n", pid);
                            }
                          }
                        }

                        indexseq++;
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
