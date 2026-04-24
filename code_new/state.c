/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 12:32:46 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 14:24:31 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	coder_compile(t_coder *coder)
{
	take_dongles(coder);
	if (should_stop(coder->all))
		return ;
	log_print(coder, "is compiling");
	pthread_mutex_lock(&coder->cv_mu);
	coder->last_compile = get_time_in_ms();
	coder->compile_count++;
	coder->time_to_die = get_time_in_ms() + \
		coder->all->parms.time_to_burnout;
	coder->has_first_dongle = 0;
	pthread_mutex_unlock(&coder->cv_mu);
	smart_sleep(coder->all->parms.time_to_compile, coder->all);
	put_dongle(coder);
}

void	coder_other(t_coder *coder)
{
	log_print(coder, "is debugging");
	smart_sleep(coder->all->parms.time_to_debug, coder->all);
	if (should_stop(coder->all))
		return ;
	log_print(coder, "is refactoring");
	smart_sleep(coder->all->parms.time_to_refactor, coder->all);
}

void	put_dongle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	long long	now;
	t_all		*all;

	all = coder->all;
	first = coder->right_dongle;
	second = coder->left_dongle;
	now = get_time_in_ms();
	pthread_mutex_lock(&all->req_mu);
	first->in_use = 0;
	first->cooldown = now + coder->all->parms.dongle_cooldown;
	if (second != first)
	{
		second->in_use = 0;
		second->cooldown = now + coder->all->parms.dongle_cooldown;
	}
	pthread_cond_signal(&all->req_cv);
	pthread_mutex_unlock(&all->req_mu);
}
