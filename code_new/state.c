/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 12:32:46 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:04:47 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

void	coder_compile(t_coder *coder)
{
	if (should_stop(coder->all))
		return ;
	pthread_mutex_lock(&coder->cv_mu);
	coder->compile_count++;
	coder->time_to_die = get_time_in_ms() + \
		coder->all->parms.time_to_burnout;
	pthread_mutex_unlock(&coder->cv_mu);
	log_print(coder, "is compiling");
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
