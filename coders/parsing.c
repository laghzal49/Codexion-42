/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:09:20 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 13:53:44 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	validate_numeric_args(char **argv)
{
	int			index;
	long long	val;

	index = 1;
	while (index <= 7)
	{
		if (valdite_number(argv[index]) != SUCCESS)
			return (error_exit(ERR_NOT_NUMBER));
		val = ft_atol(argv[index]);
		if (val == LLONG_MAX)
			return (error_exit(ERR_OVERFLOW));
		if (val > (long long)INT_MAX)
			return (error_exit(ERR_TOO_LARGE));
		if (val < 0)
			return (error_exit(ERR_NON_NEG));
		index++;
	}
	return (SUCCESS);
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
	return (SUCCESS);
}

static int	parse_scheduler(t_config *params, char **argv)
{
	if (strcmp(argv[8], "fifo") == 0)
		params->is_edf = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		params->is_edf = EDF;
	else
		return (error_exit(ERR_SCHEDULER));
	return (SUCCESS);
}

int	parsing(t_config *params, char **argv)
{
	if (validate_numeric_args(argv) != SUCCESS)
		return (FAIL);
	if (parse_values(params, argv) != SUCCESS)
		return (FAIL);
	if (parse_scheduler(params, argv) != SUCCESS)
		return (FAIL);
	return (SUCCESS);
}
