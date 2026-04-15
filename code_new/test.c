/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 10:29:38 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/14 16:52:54 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

typedef struct s_fuel
{
  int fuel;
  pthread_mutex_t   mutex;
  pthread_cond_t    cond;
}   t_fuel;

void* fuel(void* g)
{
  t_fuel *f = (t_fuel *)g;
  int   i = 0;
  while (i < 5)
  {
    pthread_mutex_lock(&f->mutex);
    f->fuel += 15;
    pthread_mutex_unlock(&f->mutex);
    printf("Filled fuel...%d\n", f->fuel);
    pthread_cond_broadcast(&f->cond);
    i++;
  }
  return (NULL);
}

void* car(void* g) 
{

  t_fuel *f = (t_fuel *)g;
  pthread_mutex_lock(&f->mutex);
  while (f->fuel < 40)
  {
    printf("no fuel wait\n");
    pthread_cond_wait(&f->cond, &f->mutex);
  } 
  f->fuel -= 40;
  printf("fuck fuel...%d\n", f->fuel);
  pthread_mutex_unlock(&f->mutex);
  return (NULL);
}

int main()
{
  t_fuel    *f;
  pthread_t t[2];
  int   i = (1-1)/2;
  f = malloc(sizeof(t_fuel));
  printf("%d\n", i);
  f->fuel = 0;
  if (pthread_mutex_init(&f->mutex, NULL))
    return 1;
  pthread_cond_init(&f->cond, NULL);
  while (i < 2)
  {
    if (i == 1)
    {
      if (pthread_create(&t[i], NULL, fuel, f))
        perror("Failed");
    }
    else
    {
      if (pthread_create(&t[i], NULL, car, f))
          perror("Faild 2");
    }

    i++;
  }
    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    free(f);
  return 0;
}
