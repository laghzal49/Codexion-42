/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:05:32 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/07 15:20:03 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_strlen(const char *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

void	error_exit(const char *msg)
{
	write(2, msg, ft_strlen(msg));
	exit(1);
}

long long	ft_atol(const char *str)
{
	long long	res;
	int			sign;
	int			i;

	i = 0;
	res = 0;
	sign = 1;
	while (str[i] == ' ' || (str[i] <= 13 && str[i] >= 9))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		res = res * 10 + (str[i++] - '0');
	return (res * sign);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;
	long long		ms;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return (ms);
}
