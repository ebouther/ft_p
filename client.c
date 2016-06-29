#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

#include "libft.h"

#define CRLF "\r\n"

int		create_client(char *addr, int port)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	proto = getprotobyname("tcp");
	if (proto == 0)
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(addr);
	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		printf("Connect error\n");
		exit(-1);
	}
	return (sock);
}

void	send_file(char *input, int socket)
{
	
}

/*
USER <SP> <username> <CRLF>
PASS <SP> <password> <CRLF>
ACCT <SP> <account-information> <CRLF>
CWD  <SP> <pathname> <CRLF>
CDUP <CRLF>
SMNT <SP> <pathname> <CRLF>
QUIT <CRLF>
REIN <CRLF>
PORT <SP> <host-port> <CRLF>
PASV <CRLF>
TYPE <SP> <type-code> <CRLF>
STRU <SP> <structure-code> <CRLF>
MODE <SP> <mode-code> <CRLF>
RETR <SP> <pathname> <CRLF>
STOR <SP> <pathname> <CRLF>
STOU <CRLF>
APPE <SP> <pathname> <CRLF>
ALLO <SP> <decimal-integer>
    [<SP> R <SP> <decimal-integer>] <CRLF>
REST <SP> <marker> <CRLF>
RNFR <SP> <pathname> <CRLF>
RNTO <SP> <pathname> <CRLF>
ABOR <CRLF>
DELE <SP> <pathname> <CRLF>
RMD  <SP> <pathname> <CRLF>
MKD  <SP> <pathname> <CRLF>
PWD  <CRLF>
LIST [<SP> <pathname>] <CRLF>
NLST [<SP> <pathname>] <CRLF>
SITE <SP> <string> <CRLF>
SYST <CRLF>
STAT [<SP> <pathname>] <CRLF>
HELP [<SP> <string>] <CRLF>
NOOP <CRLF>
*/
void	exec_input(char *input, int socket)
{
	char	**args;
	size_t	len;

	len = 0;
	args = ft_strsplit(input, ' ');
	while (args && args[len])
		len++;
	if (len == 0)
		return ;
	if (ft_strcmp(args[0], "quit") == 0)
		exit(0);
	else if (ft_strcmp(args[0], "cd") == 0)
	{
		if (len != 2)
			printf("Usage: cd <directory>\n");
		else
		{
			ft_putstr_fd(ft_strjoin_free(ft_strdup("CWD "),
				ft_strjoin(args[1], CRLF)), socket);
		}
	}
	else if (ft_strcmp(args[0], "ls") == 0)
			ft_putstr_fd("LIST\r\n", socket);
	else if (ft_strcmp(args[0], "get") == 0)
		write(socket, input, ft_strlen(input));
	else if (ft_strcmp(args[0], "put") == 0)
		send_file(input, socket);
	else if (ft_strcmp(args[0], "pwd") == 0)
		ft_putstr_fd("PWD\r\n", socket);
	len = 0;
	while (args[len])
		ft_memdel((void **)(args + len++));
	ft_memdel((void **)&args);
}

int		main(int argc, char **argv)
{
	int		port;
	int		sock;
	char	*input;
	char	*buf;
	int		r;

	if (argc != 3)
	{
		printf("Usage: %s <addr> <port>\n", argv[0]);
		return (-1);
	}
	port = atoi(argv[2]);
	sock = create_client(argv[1], port);
	while (42)
	{
		ft_putstr("$> ");
		if (get_next_line(1, &input))
			exec_input(input, sock);
		while ((r = recv(sock, buf, 1023, 0)) > 0)
		{
			buf[r] = '\0';
			ft_putstr_fd(buf, 2);
		}
	}
	close(sock);
	return (0);
}
