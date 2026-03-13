#include "codexion.h"

long long ft_atol(const char *str)
{
	long long res;

	int sign;
	int i;
	i = 0;
	res = 0;
	sign = 1;
	while (str[i] == ' ' ||str[i] <= 13 && str[i] >= 9)
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return (res * sign);

}
int ft_strlen(const char *s) {
	int len = 0;
	while (s[len])
		len++;
	return len;
}

void error_exit(const char *msg) {
	write(2, msg, ft_strlen(msg));
	exit(1);
}

int ft_strcmp(const char *s1, const char *s2) {
	int i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return (unsigned char)s1[i] - (unsigned char)s2[i];
}
