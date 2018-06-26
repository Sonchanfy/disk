#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<memory.h>
#include<fcntl.h> 
#include<string.h>

#define FILESIZE 1024000 //文件大小
#define BUFSIZE 1024 //自定义缓冲区大小
#define DEVSIZE 29 //驱动生成数据的大小
#define SYSIZE 28 //

int main()
{

	int fd,fd_dev,fd_move;
	int buf_size;
	int move=0;
	int j,z;
	static int len=0;
	int sy_size=0;
	int sy_space;
	int flag;
	
	char sy_buf[SYSIZE];
	char buf[BUFSIZE];
	char dev_data[DEVSIZE];
	char buff[]="qwertyuiop";

	memset(buf,0,sizeof(buf));
	memset(dev_data,0,sizeof(dev_data));
	memset(sy_buf,0,sizeof(sy_buf));

	fd=open("./write_file.txt",O_RDWR|O_CREAT,0777);
	fd_move=open("./move.txt",O_RDWR|O_CREAT,0777);//存放文件指针偏移量
	fd_dev=open("/dev/testdev",O_RDWR);

	if(fd_dev<0)
	{
		printf("open dev file faile!\n");
	}
	write(fd_dev,buff,strlen(buff));
	while(1)
	{
		while(strlen(buf)<BUFSIZE)//判断缓冲区空间
		{
			flag=0;//表示缓冲区还有空间
			read(fd_dev,dev_data,sizeof(dev_data));//读取驱动数据
			//sleep(1);
			printf("%s",buf);
			if((sizeof(buf)-strlen(buf))>=sizeof(dev_data))//缓冲区空间是否够装驱动数据
			{
				sprintf(buf,"%s%s",buf,dev_data);//把数据追加到缓冲区
				memset(dev_data,0,sizeof(dev_data));//把存放驱动数据的数组清零
				if(sizeof(buf)==strlen(buf))//如果缓冲区已满，标志位置1
				{
					flag=1;
				}
			}
			else//若缓冲区的空间小于读到的驱动数据的大小
			{
				sy_space=sizeof(buf)-strlen(buf);//缓冲区剩余的空间大小
				for(j=0;j<sy_space;j++)
				{
					sy_buf[j]=dev_data[j];//把驱动数据存放一部分
				}
				sprintf(buf,"%s%s",buf,sy_buf);//追加到剩下的缓冲区空间内
				memset(sy_buf,0,sizeof(sy_buf));//数组清零
				for(z=0;j<28;j++,z++)//把多余的驱动数据放到另外一个数组
				{
					sy_buf[z]=dev_data[j];
				}
				memset(dev_data,0,sizeof(dev_data));
				flag=1;//缓冲区已满，标志位置1

			}
		}
		if(flag==1)
		{
			flag=0;
			lseek(fd_move,0,SEEK_SET);//指向文件头
			read(fd_move,&move,sizeof(move));//读取最新的偏移量
			if(move<FILESIZE)//偏移量少于文件大小
			{
				len=move;//当前写入位置就是读到的偏移量
			}
			else
			{
				len=0;//表示文件已满，从头写入
			}
			if((FILESIZE-len)>=sizeof(buf))//判断文件剩余大小能否存放buf读取到的所有数据
			{
				lseek(fd,len,SEEK_SET);//文件指针偏移到上次写入的位置
				write(fd,buf,sizeof(buf));//写入数据
				memset(buf,0,sizeof(buf));
				len +=sizeof(buf);//偏移量更新
				
				lseek(fd_move,0,SEEK_SET);//指向文件头
				write(fd_move,&len,sizeof(len));//重新写入最新的偏移量
			}
			else
			{
				lseek(fd,len,SEEK_SET);//文件指针偏移到上次写入位置
				write(fd,buf,(FILESIZE-len));//把buf前（文件剩余大小）多个数据写入文件
				memset(buf,0,sizeof(buf));
				len=0;
				
				lseek(fd_move,0,SEEK_SET);
				write(fd_move,&len,sizeof(len));//从头写入
			}
		}
		if(strlen(sy_buf)>0)//若多出文件大小的数据
		{
			sprintf(buf,"%s%s",buf,sy_buf);//存放到buf，回到最开始循环判断写入
			memset(sy_buf,0,sizeof(sy_buf));
		}
		else
		{
			memset(sy_buf,0,sizeof(sy_buf));
		}
	
	}
	close(fd);
	close(fd_dev);
	close(fd_move);
}



