#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "libft.h"

# define BUFSIZE 1023

int create_server(int port)
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
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		printf("Bind error\n");
		exit(-1);
	}
	listen(sock, 42);
	return (sock);
}

void	get_client_request(int cs, char *request)
{
	char	*tmp;

	if (ft_strcmp(request, "LIST\r") == 0)
	{
		if (getcwd(tmp, 1024) == NULL)
		{
			ft_putstr_fd("BBBBBBBBBBBBBB", cs);
			ft_putstr_fd("getcwd error\n", cs);
		}
		else
		{
			ft_putstr_fd("AAAAAAAAAAAA", cs);
			ft_putstr_fd(tmp, cs);
		}
	}
	printf("Client request: '%s'\n", request);
}

void	get_connection(int sock)
{
	int					cs;
	unsigned int		cslen;
	struct sockaddr_in	csin;
	char				*buf;
	int					pid;

	if ((cs = accept(sock, (struct sockaddr *)&csin, &cslen)) < 0)
	{
		printf("Accept error.\n");
		exit(-1);
	}
	if ((pid = fork()) == 0)
	{
		close(sock);
		while (get_next_line(cs, &buf))
			get_client_request(cs, buf);
		exit(0);
	}
	close(cs);
}

int		transfer_file(char *file_name, int dest_fd)
{
	int			fd;
	char		*buf;
	ssize_t		len;
	size_t		buf_len;
	ssize_t		rd_len;
	ssize_t		w_len;
	struct stat	file_stat;

	if ((fd = open(file_name, O_RDONLY)) == -1)
	{
		printf("Cannot open file. \n");
		exit(-1);
	}
	if (fstat(fd, &file_stat) == -1)
	{
		printf("fstat error.\n");
		exit(-1);
	}
	len = file_stat.st_size;
	while (len > 0)
	{
		buf_len = (BUFSIZE < len) ? BUFSIZE : len;
		if ((rd_len = read(fd, buf, buf_len)) == -1)
			return (-1);
		else if (rd_len == 0)
			break;
		if ((w_len = write(dest_fd, buf, rd_len)) == -1)
			return (-1);
		len -= w_len;
	}
	return (0);
}

int		main(int argc, char **argv)
{
	int					port;
	int					sock;

	if (argc != 2)
		return (-1);
	port = atoi(argv[1]);
	sock = create_server(port);
	while (42)
		get_connection(sock);
	close(sock);
	return (0);
}
