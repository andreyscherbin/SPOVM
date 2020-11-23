#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

#define SEMAPHORE_CLIENT_NOTIFICATION "/semaphore_for_client_notification"
#define SEMAPHORE_SERVER_NOTIFICATION "/semaphore_for_server_notification"
extern int errno;
using namespace std;

  int main(){

  sem_t *sem_client,*sem_server; //идентификатор семафора
  int channel;                   // идентификатор канала
  char input[]="Ввод пользователя";

  /* Создаем семафор для уведомления сервера */
  if ( (sem_server = sem_open(SEMAPHORE_SERVER_NOTIFICATION, O_CREAT, 0777, 0)) == SEM_FAILED ) {
        perror("sem_open");
        return 1;
    }

  /* Открываем уже существующий семафор для уведомления клиента */
  if ( (sem_client = sem_open(SEMAPHORE_CLIENT_NOTIFICATION, 0)) == SEM_FAILED ) {
        perror("sem_open");
        return 1;
        }

  if (( channel = open("/tmp/fifo0001.1", O_RDONLY))<0)

  { perror("fifo open"); exit(0); }

while(1){  // бесконечный цикл получения строки и её выводом клиентским процессом

  printf("Ожидание уведомления о том, что серверный процесс готов начать передачу данных.\n");
      if (sem_wait(sem_client) < 0 ) // ожидание уведомления о том, что серверный процесс готов начать передачу данных
          perror("sem_wait");
  printf("Уведомление о том, что серверный процесс готов начать передачу данных получено.\n");
  int bytes = read(channel, &input, sizeof(input)); // читаем из канала ввод пользователя
  input[bytes] = '\0'; // ноль-символ в строку, чтобы избавиться от мусора при выводе.
  printf("%s\n",input);
  sem_post(sem_server);                 // уведомление серверного процесса об успешном получении строки
  printf("Клиент готов для получения очередной строки.\n");
}

  close(channel);
  return 0;
  }
