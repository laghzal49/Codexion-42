/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 15:21:54 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 18:38:42 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void take_dongles(t_coder *coder, t_dongle *dongle)
{
    t_request req;

    req.coder_id = coder;
    if (coder->sim->params.is_edf == 1)
        req.key = coder->last_compile + coder->sim->params.time_to_burnout;
    else
        req.key = coder->last_compile;
        
    pthread_mutex_lock(&dongle->mutex);
    pq_push(&dongle->queue, req);
    
    while (dongle->available_flag == 0 || pq_peek(&dongle->queue)->coder_id->id != coder->id) 
        pthread_cond_wait(&dongle->cond, &dongle->mutex);
        
    while (get_time_in_ms() < dongle->cooldown_timestamp)
    {
        pthread_mutex_unlock(&dongle->mutex);
        usleep(50);
        pthread_mutex_lock(&dongle->mutex);
    }
    
    dongle->available_flag = 0;
    pq_pop(&dongle->queue);
    print_state(coder, "has taken a dongle");
    
    pthread_mutex_unlock(&dongle->mutex);
}

void    put_dongles(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available_flag = 1;
	dongle->cooldown_timestamp = get_time_in_ms() + coder->sim->params.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
