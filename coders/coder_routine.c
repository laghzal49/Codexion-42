/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 10:41:53 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 19:04:13 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

void    print_state(t_coder *coder, const char *state)
{
  pthread_mutex_lock(&coder->sim->log_mutex);
  pthread_mutex_lock(&coder->sim->stop_mutex);
  if (coder->sim->stop_flag == 0)
    printf("%lld %d %s\n", get_time_in_ms() - coder->sim->start_time_ms, coder->id, state);
  pthread_mutex_unlock(&coder->sim->stop_mutex);
  pthread_mutex_unlock(&coder->sim->log_mutex);
}
void action_compile(t_coder *coder)
{
	// 1. Grab Dongles safely
	if (coder->id % 2 == 0)
	{
		take_dongles(coder, coder->right_dongle);
		take_dongles(coder, coder->left_dongle);
	}
	else
	{
		take_dongles(coder, coder->left_dongle);
		take_dongles(coder, coder->right_dongle);
	}
	
	// 2. Do the work
	print_state(coder, "is compiling");
	
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->last_compile = get_time_in_ms();
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	
	ft_usleep(coder->sim->params.time_to_compile);
	
	// PROTECT THE COUNTER!
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	
	// 3. Drop Dongles safely (Reverse order)
	if (coder->id % 2 == 0)
	{
		put_dongles(coder, coder->left_dongle);
		put_dongles(coder, coder->right_dongle);
	}
	else
	{
		put_dongles(coder, coder->right_dongle);
		put_dongles(coder, coder->left_dongle);
	}
}
void    action_rest(t_coder *coder)
{
	print_state(coder, "is debugging");
	ft_usleep(coder->sim->params.time_to_debug);
	
	print_state(coder, "is refactoring");
	ft_usleep(coder->sim->params.time_to_refactor);
}
void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		ft_usleep(10);
		
	while (1)
	{
		// 1. Stop check
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop_flag == 1)
		{
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			break;
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);

		// 2. Do actions
		action_compile(coder);
		action_rest(coder);
		
		// MAKE SURE THERE IS NOTHING ELSE HERE!
	}
	return (NULL);
}
