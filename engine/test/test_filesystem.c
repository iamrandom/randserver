
#include <io.h>
#include <stdio.h>



int search_path(const char* path, const char* mode)
{
	long handle;
	if((handle=_findfirst(path,&file_info))==-1L)
		printf("没有找到匹配的项目\n");
	else{
		printf("%s\n",file_info.name);
		while(_findnext(handle,&file_info)==0)
			printf("%s\n",file_info.name);
		_findclose(handle);
	}
}


int main()
{
	long handle;
	struct _finddata_t file_info;
	const char* path = "E:/git_workspace/randserver/luacode/*";
	if((handle=_findfirst(path,&file_info))==-1L)
		printf("没有找到匹配的项目\n");
	else{
		printf("%s\n",file_info.name);
		while(_findnext(handle,&file_info)==0)
			printf("%s\n",file_info.name);
		_findclose(handle);
	}
	return 0;
} 
