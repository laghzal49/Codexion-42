/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 10:41:53 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/12 16:05:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

static void	drop_all_dongles(t_coder *coder)
{
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

void	action_compile(t_coder *coder, t_params params)
{
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
	print_state(coder, "is compiling");
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->last_compile = get_time_in_ms();
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	ft_smart_sleep(coder, params.time_to_compile);
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	drop_all_dongles(coder);
}

void	action_debug(t_coder *coder, t_params params)
{
	print_state(coder, "is debugging");
	ft_smart_sleep(coder, params.time_to_debug);
}

void	action_refactor(t_coder *coder, t_params params)
{
	print_state(coder, "is refactoring");
	ft_smart_sleep(coder, params.time_to_refactor);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		ft_usleep(10);
	while (1)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop_flag == 1)
		{
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			break ;
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		action_compile(coder, coder->sim->params);
		action_debug(coder, coder->sim->params);
		action_refactor(coder, coder->sim->params);
	}
	return (NULL);
}
