/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:56:33 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/03/13 09:02:42 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "codexion.h"


t_params *parsing(t_params *parse, char **av)
{
	parse->num_coders = ft_atol(av[1]);
    parse->time_to_burnout = ft_atol(av[2]);
    parse->time_to_compile = ft_atol(av[3]);
    parse->time_to_debug = ft_atol(av[4]);
    parse->time_to_refactor = ft_atol(av[5]);
    parse->compiles_required = ft_atol(av[6]);
    parse->dongle_cooldown = ft_atol(av[7]);

    if (parse->num_coders <= 0 || parse->time_to_burnout <= 0 || 
        parse->time_to_compile < 0 || parse->time_to_debug < 0 || 
        parse->time_to_refactor < 0 || parse->compiles_required < 0 || 
        parse->dongle_cooldown < 0)
    {
        error_exit("Error: Invalid numeric arguments. Must be positive.\n");
    }

    if (ft_strcmp(av[8], "fifo") == 0)
        parse->is_edf = 0;
    else if (ft_strcmp(av[8], "edf") == 0)
        parse->is_edf = 1;
    else
        error_exit("Error: Invalid scheduler. Must be 'fifo' or 'edf'.\n");

    printf("Parsed: %d coders, scheduler enum: %d (0=FIFO, 1=EDF)\n", 
            parse->num_coders, parse->is_edf);
	return parse;
}


int main(int ac, char **av) {
	t_params	*parse;
	parse = malloc(sizeof(t_params));
	if (!parse)
		return 1;
	if (ac != 9)
		error_exit("Usage: ./main num_coders time_to_burnout time_to_compile time_to_debug time_to_refactor num_compiles_req dongle_cooldown scheduler\n");
	parse = parsing(parse, av);
    
	free(parse);
    return 0;

}