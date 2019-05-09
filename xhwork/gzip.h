#ifndef GZIP_H
#define GZIP_H
#include "zip/zlib.h"

int gzcompress(unsigned char* data,unsigned long ndata,unsigned char* zdata,unsigned long* nzdata)
{
    z_stream c_stream;
    int err = 0;
 
    if(data && ndata > 0)
	{
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
        if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
		{
			return -1;
		}
        c_stream.next_in  = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
        while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata) 
		{
            if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) 
			{
				return -1;
			}
        }
        if(c_stream.avail_in != 0) 
		{
			return c_stream.avail_in;
		}
        for(;;) 
		{
            if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END)
			{
				break;
			}
            if(err != Z_OK)
			{
				return -1;
			}
        }
        if(deflateEnd(&c_stream) != Z_OK)
		{
			return -1;
		}
        *nzdata = c_stream.total_out;
        return 0;
    }
    return -1;
}


#endif