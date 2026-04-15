/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 19:16:10 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/13 17:48:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H 

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>
# include <unistd.h>
typedef struct s_parmas {
  long long num_coders;
  long long time_to_burnout;
  long long time_to_compile;
  long long time_to_debug;
  long long time_to_refactor;
  long long compiles_required;
  long long dongle_cooldown;  
  int is_edf;
}   t_params;

typedef struct   s_heap {
  long long key;
  long long size;
}   t_heap;

typedef struct s_dongle
{
  int    in_use;
  pthread_mutex_t   mutex;
  int id;
  long long cooldown;
}   t_dongle;


typedef struct s_coder
{
  long long coder_id;
  long long time_to_die;
  t_dongle  *left_dongle;
  t_dongle  *right_dongle;
  pthread_t thread;
  t_heap    heap[2];
  int       complie_count;
  long long last_compile;
  ///
} t_coder;


typedef struct s_all {
  t_params  parms;
  t_dongle *dongles;
  t_coder   *coder;

} t_all;
void    parsing(t_params *parse, char **av);
int ft_strlen(const char *s);
void    error_exit(const char *msg);
long long   ft_atol(const char *str);
long long   get_time_in_ms(void);



#endif
