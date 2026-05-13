/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:56:33 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 13:43:23 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_parsing(int argc)
{
	if (argc != 9)
	{
		error_exit(ERR_USAGE_1);
		error_exit(ERR_USAGE_2);
		return (error_exit(ERR_USAGE_3));
	}
	return (SUCCESS);
}

static int	prepare_app(t_app *all, int argc, char **argv)
{
	if (check_parsing(argc) != SUCCESS)
		return (FAIL);
	if (parsing(&all->parms, argv) != SUCCESS)
		return (FAIL);
	if (init_all(all) != SUCCESS)
		return (FAIL);
	return (SUCCESS);
}

static int	run_app(int argc, char **argv)
{
	t_app	*all;

	all = malloc(sizeof(t_app));
	if (!all)
		return (FAIL);
	memset(all, 0, sizeof(t_app));
	if (prepare_app(all, argc, argv) != SUCCESS)
	{
		cleanup_all(all);
		return (FAIL);
	}
	if (start_and_join_coders(all) != SUCCESS)
	{
		cleanup_all(all);
		return (FAIL);
	}
	cleanup_all(all);
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	if (run_app(argc, argv) == 0)
		return (SUCCESS);
	return (FAIL);
}
