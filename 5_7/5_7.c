#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

struct my_iovec {
	void *iov_base;
	size_t iov_len;
};

ssize_t my_readv(int fd, const struct my_iovec *iov, int iovcnt);
ssize_t my_writev(int fd, const struct my_iovec *iov, int iovcnt);


int
main(int argc, char **argv)
{
	int fd;
	char str[10];
	char str3[5];
	char *str2;
	struct my_iovec i_ar[4];
	struct my_iovec x;
	struct my_iovec y;
	struct my_iovec z;
	struct my_iovec zz;
	x.iov_base = &str;
	x.iov_len = 10;
	i_ar[0] = x;

	zz.iov_len=5;
	zz.iov_base= &str3;
	i_ar[1] = zz;
	i_ar[2] = z;

	str2 = (char*)malloc(50);
	y.iov_base = &str2;
	y.iov_len = 0;
	i_ar[3] = y;

	fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	write(fd, "111111111111111", 15);
	close(fd);
	fd = open("test.txt", O_RDONLY);
	my_readv(fd, i_ar, 4);

	close(fd);
	fd = open("test2.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	my_writev(fd, i_ar, 4);
	close(fd);
}

ssize_t
my_readv(int fd, const struct my_iovec *iov, int iovcnt)
{
	char c[1];
	ssize_t numRead;
	int current_iov;
	ssize_t current_count;
	ssize_t total_read;

	total_read = 0;
	current_count = 0;
	current_iov = 0;
	while((numRead = read(fd, c, 1)) != 0)
	{
		if(numRead == -1)
			return -1;

		total_read++;
		current_count++;
my_readv__ensure_no_zero_length_io:
		if(iov[current_iov].iov_base == NULL )
		{
			current_iov++;
			if(current_iov >= iovcnt)
			{
				total_read--;
				break;
			}
			goto my_readv__ensure_no_zero_length_io;
		}

		if(iov[current_iov].iov_len < current_count)
		{
			current_count = 1;
			current_iov++;
			if(current_iov >= iovcnt)
			{
				total_read--;
				break;
			}
			goto my_readv__ensure_no_zero_length_io;
		}

		((char*)(iov[current_iov].iov_base))[current_count -1] = *c;
	}
	return total_read;
}

ssize_t
my_writev(int fd, const struct my_iovec *iov, int iovcnt)
{
	ssize_t total_written;
	total_written = 0;
	for(int i = 0; i < iovcnt; i++)
	{
		if(iov[i].iov_base == NULL)
			continue;
		printf("%lu\n",&iov[i].iov_base);
		for(int j = 0; j < iov[i].iov_len; j++)
		{
			write(fd,&((char*)iov[i].iov_base)[j],(size_t)1);
			total_written++;
		}
	}
	return total_written;
}
