/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:09:20 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:17:57 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	validate_numeric_args(char **argv)
{
	int			index;
	long long	val;
	int			error_check;

	index = 1;
	error_check = 0;
	while (index <= 7)
	{
		if (valdite_number(argv[index]))
			error_check += error_exit(ERR_NOT_NUMBER);
		val = ft_atol(argv[index]);
		if (error_check == 0 && val == LLONG_MAX)
			error_check += error_exit(ERR_OVERFLOW);
		if (error_check == 0 && val > (long long)INT_MAX)
			error_check += error_exit(ERR_TOO_LARGE);
		if (error_check == 0 && index < 8 && val < 0)
			error_check += error_exit(ERR_NON_NEG);
		index++;
	}
	if (error_check != 0)
		return (1);
	return (0);
}

static int	parse_values(t_config *params, char **argv)
{
	params->num_coders = ft_atol(argv[1]);
	params->time_to_burnout = ft_atol(argv[2]);
	params->time_to_compile = ft_atol(argv[3]);
	params->time_to_debug = ft_atol(argv[4]);
	params->time_to_refactor = ft_atol(argv[5]);
	params->compiles_required = ft_atol(argv[6]);
	params->dongle_cooldown = ft_atol(argv[7]);
	return (0);
}

static int	parse_scheduler(t_config *params, char **argv)
{
	if (strcmp(argv[8], "fifo") == 0)
		params->is_edf = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		params->is_edf = EDF;
	else
		return (error_exit(ERR_SCHEDULER));
	return (0);
}

int	parsing(t_config *params, char **argv)
{
	if (validate_numeric_args(argv) != 0)
		return (1);
	if (parse_values(params, argv) != 0)
		return (1);
	if (parse_scheduler(params, argv) != 0)
		return (1);
	return (0);
}
