#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

int server_id = 0;
int sequence_id = 0;
long last_timestamp = 0;

const long INIT_ID      = 4611686018427387904; /*pow(2, 62)*/
const long EPOCH_TIME   = 1476148258123; /* 时间基数 */
const int BITS_FULL     = 64;
const int BITS_PRE      = 2;  /* 固定位01 */
const int BITS_TIME     = 41; /* 可支持69年 */
const int BITS_SERVER   = 5;  /* 可支持31台集群服务 */
const int BITS_WORKER   = 7;  /* 可支持业务数127个 */
const int BITS_SEQUENCE = 9;  /* 一毫秒内支持生成511个id */

/**
 * 生成毫秒级时间
 * @Author DENGDAOCHENG
 * @return [description]
 */
long timeGen()
{
	int wait_next_ms = 0;
	long time;
	struct timeval tv;
	
	do {
		if(wait_next_ms > 0) {
			usleep(1000);
		}
		gettimeofday(&tv, NULL);
		time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

		if(time > last_timestamp) {
			sequence_id = 0;
		}

		wait_next_ms++;

	} while(time == last_timestamp && sequence_id >= (pow(2, BITS_SEQUENCE) - 1) || time < last_timestamp);

	last_timestamp = time;

	return time;
}

/**
 * 生成id
 * @Author DENGDAOCHENG
 * @param  worker_id    [description]
 * @return              [description]
 */
unsigned long getID(int worker_id)
{
	unsigned long id;
	long time;
	long diff_time;
	int shift;

	int max_server_id = pow(2, BITS_SERVER) - 1;
	int max_worker_id = pow(2, BITS_WORKER) - 1;
	int max_sequence_id = pow(2, BITS_SEQUENCE) - 1;
	
	time = timeGen();
	diff_time = time - EPOCH_TIME;
	
	shift = BITS_FULL - BITS_PRE - BITS_TIME;
	id = INIT_ID | diff_time << shift;

	shift -= BITS_SERVER;
	id |= (server_id & max_server_id) << shift;

	shift -= BITS_WORKER;
	id |= (worker_id & max_worker_id) << shift;

	id |= sequence_id & max_sequence_id;

	sequence_id++;

	return time;
}

/**
 * 二进制转换
 * @Author DENGDAOCHENG
 * @param  id           [description]
 * @return              [description]
 */
char * convert(long id)
{
	static char buffer[64];
	int i = 0;

	while(id > 0) {
		buffer[i] = id % 2;
		id = id / 2;
		i++;
	}

	buffer[i] = 0;

	return buffer;
}

/**
 * 解析id信息
 * @Author DENGDAOCHENG
 * @param  id           [description]
 * @return              [description]
 */
char parse(long id)
{
	char *buffer;
	int p_sequence_id = 0;
	int p_worker_id = 0;
	int p_server_id = 0;
	long p_timestamp = 0;

	buffer = convert(id);
	int i;
	for (i = 0; i < 64; ++i)
	{
		printf("%d", buffer[i]);
		if (i < BITS_SEQUENCE)
		{
			p_sequence_id += buffer[i] == 0 ? 0 : pow(2, i);
		}
		else if(i >= BITS_SEQUENCE && i < BITS_SEQUENCE + BITS_WORKER) {
			p_worker_id += buffer[i] == 0 ? 0 : pow(2, i - BITS_SEQUENCE);
		}
		else if (i >= BITS_SEQUENCE + BITS_WORKER && i < BITS_SEQUENCE + BITS_WORKER + BITS_SERVER)
		{
			p_server_id += buffer[i] == 0 ? 0 : pow(2, i - BITS_WORKER - BITS_SEQUENCE);
		}
		else if (i >= BITS_SEQUENCE + BITS_WORKER + BITS_SERVER && i < BITS_SEQUENCE + BITS_WORKER + BITS_SERVER + BITS_TIME)
		{
			p_timestamp += buffer[i] == 0 ? 0 : pow(2, i - BITS_SERVER - BITS_WORKER - BITS_SEQUENCE);
		}
	}

	p_timestamp += EPOCH_TIME;
	p_timestamp /= 1000;

	printf("\n%d %d %d %ld\n", p_sequence_id, p_worker_id, p_server_id, p_timestamp);

}

void main (int argc, char **argv)
{
	long id;
	int worker_id = 1;
	long start_time = 0;

	while(1) {
		id = getID(worker_id);
		printf("%ld\n", id);

		if(start_time == 0) {
			start_time = last_timestamp;
		}
		else if (start_time < last_timestamp - 10)
		{
			break;
		}
	}
	
	return;
}