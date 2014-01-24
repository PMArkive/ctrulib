#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctr/types.h>
#include <ctr/FS.h>
#include <ctr/svc.h>

Result FSUSER_Initialize(Handle handle)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x08010002; //request header code
	cmdbuf[1]=32;
	
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;
	
	return cmdbuf[1];
}

Result FSUSER_OpenFileDirectly(Handle handle, Handle* out, FS_archive archive, FS_path fileLowPath, u32 openflags, u32 attributes)
{
	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x08030204;
	cmdbuf[1]=0;
	cmdbuf[2]=archive.id;
	cmdbuf[3]=archive.lowPath.type;
	cmdbuf[4]=archive.lowPath.size;
	cmdbuf[5]=fileLowPath.type;
	cmdbuf[6]=fileLowPath.size;
	cmdbuf[7]=openflags;
	cmdbuf[8]=attributes;
	cmdbuf[9]=(archive.lowPath.size<<14)|0x802;
	cmdbuf[10]=(u32)archive.lowPath.data;
	cmdbuf[11]=(fileLowPath.size<<14)|2;
	cmdbuf[12]=(u32)fileLowPath.data;
 
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;
 
	if(out)*out=cmdbuf[3];
 
	return cmdbuf[1];
}

Result FSUSER_OpenArchive(Handle handle, FS_archive* archive)
{
	if(!archive)return -2;
	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x080C00C2;
	cmdbuf[1]=archive->id;
	cmdbuf[2]=archive->lowPath.type;
	cmdbuf[3]=archive->lowPath.size;
	cmdbuf[4]=(archive->lowPath.size<<14)|0x2;
	cmdbuf[5]=(u32)archive->lowPath.data;
 
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;
 
	archive->handleLow=cmdbuf[2];
	archive->handleHigh=cmdbuf[3];
 
	return cmdbuf[1];
}

Result FSFILE_Close(Handle handle)
{
	u32* cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x08080000;

	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result FSFILE_Read(Handle handle, u32 *bytesRead, u64 offset, u32 *buffer, u32 size)
{
	u32 *cmdbuf=getThreadCommandBuffer();
 
	cmdbuf[0]=0x080200C2;
	cmdbuf[1]=(u32)offset;
	cmdbuf[2]=(u32)(offset>>32);
	cmdbuf[3]=size;
	cmdbuf[4]=(size<<4)|12;
	cmdbuf[5]=(u32)buffer;
 
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	if(bytesRead)*bytesRead=cmdbuf[2];

	return cmdbuf[1];
}

//WARNING : using wrong flushFlags CAN corrupt the archive you're writing to.
//another warning : data should *not* be in RO memory
Result FSFILE_Write(Handle handle, u32 *bytesWritten, u64 offset, u32 *data, u32 size, u32 flushFlags)
{
	u32 *cmdbuf=getThreadCommandBuffer();

	cmdbuf[0]=0x08030102;
	cmdbuf[1]=(u32)offset;
	cmdbuf[2]=(u32)(offset>>32);
	cmdbuf[3]=size;
	cmdbuf[4]=flushFlags;
	cmdbuf[5]=(size<<4)|10;
	cmdbuf[6]=(u32)data;

	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	if(bytesWritten)*bytesWritten=cmdbuf[2];

	return cmdbuf[1];
}

Result FSFILE_GetSize(Handle handle, u64 *size)
{
	u32 *cmdbuf=getThreadCommandBuffer();
 
	cmdbuf[0] = 0x08040000;
 
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;
 
	if(size)*size = *((u64*)&cmdbuf[2]);
 
	return cmdbuf[1];
}